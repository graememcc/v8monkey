#include "v8.h"

#include "data_structures/objectblock.h"
#include "types/base_types.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8::V8Monkey;


// ObjectBlock's bookkeeping would be easy to get wrong, hence we poke around testing it's internals as well as API


namespace {
  // Fill up an ObjectBlock<int> with deletable data
  void fill(int** from, int** to) {
    for (; from < to; from++) {
      *from = new int;
    }
  }


  // We often create several objects, and delete some subset of them. We use these arrays to track which were deleted
  const int deletionObjectBlockSize = ObjectBlock<DeletionObject>::BlockSize;
  const int deletionObjectMax = deletionObjectBlockSize + 20;
  bool wasCalled[deletionObjectMax];
  bool wasDeleted[deletionObjectMax];


  void resetForDeletion() {
    for (int i = 0; i < deletionObjectMax; i++) {
      wasCalled[i] = false;
      wasDeleted[i] = false;
    }
  }


  void deletionFunction(DeletionObject* d) {
    wasCalled[d->index] = true;
  }


  class IterateObject;


  const int iterationObjectBlockSize = ObjectBlock<IterateObject>::BlockSize;
  const int iterationObjectMax = iterationObjectBlockSize + 20;
  bool objectVisited[iterationObjectMax];
  void* objectVisitData[iterationObjectMax];


  void resetForIteration() {
    for (int i = 0; i < iterationObjectMax; i++) {
      objectVisited[i] = false;
      objectVisitData[i] = nullptr;
    }
  }


  // Like for the DeletionObjects above, the following tests sometimes need to note if objects are iterated over.
  // The approach is similar to the DeletionObjects above
  class IterateObject {
    public:
      IterateObject(int n) { index = n; }
      ~IterateObject() {}
      int index;
  };


  void objectVisitor(IterateObject* i) {
    objectVisited[i->index] = true;
  }


  void objectDataVisitor(IterateObject* i, void* data) {
    objectVisitData[i->index] = data;
  }
}


// Convenience
typedef ObjectBlock<int> TestingBlock;


V8MONKEY_TEST(ObjectBlock001, "Number of objects returns 0 when supplied nullptr") {
  V8MONKEY_CHECK(TestingBlock::NumberOfItems(nullptr) == 0, "Correct value returned");
}


V8MONKEY_TEST(ObjectBlock002, "Number of objects returns 0 when block extended by 1 block") {
  TestingBlock::Limits data = TestingBlock::Extend(nullptr);

  V8MONKEY_CHECK(TestingBlock::NumberOfItems(data.limit) == 0, "Correct value returned");

  TestingBlock::Delete(data.limit, data.top, nullptr);
}


V8MONKEY_TEST(ObjectBlock003, "Number of objects returns <blocksize> when block extended by 2 blocks") {
  TestingBlock::Limits data1 = TestingBlock::Extend(nullptr);
  fill(data1.top, data1.limit);
  TestingBlock::Limits data2 = TestingBlock::Extend(data1.limit);
  fill(data2.top, data2.limit);

  V8MONKEY_CHECK(TestingBlock::NumberOfItems(data2.limit) == TestingBlock::BlockSize, "Correct value returned");

  TestingBlock::Delete(data2.limit, data2.limit, data1.top);
  TestingBlock::Delete(data1.limit, data1.top, nullptr);
}


V8MONKEY_TEST(ObjectBlock004, "Number of objects returns 2 * <blocksize> when block extended by 2 blocks") {
  TestingBlock::Limits data1 = TestingBlock::Extend(nullptr);
  fill(data1.top, data1.limit);
  TestingBlock::Limits data2 = TestingBlock::Extend(data1.limit);
  fill(data2.top, data2.limit);
  TestingBlock::Limits data3 = TestingBlock::Extend(data2.limit);

  V8MONKEY_CHECK(TestingBlock::NumberOfItems(data3.limit) == 2 * TestingBlock::BlockSize, "Correct value returned");

  TestingBlock::Delete(data3.limit, data3.top, data2.top);
  TestingBlock::Delete(data2.limit, data2.top, data1.top);
  TestingBlock::Delete(data1.limit, data1.top, nullptr);
}


V8MONKEY_TEST(ObjectBlock005, "<Internal book-keeping> After 1 extension, previous field points to nullptr") {
  TestingBlock::Limits data = TestingBlock::Extend(nullptr);
  TestingBlock* prev = reinterpret_cast<TestingBlock*>(*(data.top - 2));

  V8MONKEY_CHECK(prev == nullptr, "Prev field in first allocated block points to null");

  TestingBlock::Delete(data.limit, data.top, nullptr);
}


V8MONKEY_TEST(ObjectBlock006, "<Internal book-keeping> After 2 extensions, previous field points to limit of first extension") {
  TestingBlock::Limits data1 = TestingBlock::Extend(nullptr);
  fill(data1.top, data1.limit);
  TestingBlock::Limits data2 = TestingBlock::Extend(data1.limit);
  int** prev = reinterpret_cast<int**>(*(data2.top - 2));

  V8MONKEY_CHECK(prev == data1.top - 2, "Prev field points to previous block");

  TestingBlock::Delete(data2.limit, data2.top, data1.top);
  TestingBlock::Delete(data1.limit, data1.top, nullptr);
}


V8MONKEY_TEST(ObjectBlock007, "Limit and top correspond") {
  TestingBlock::Limits data = TestingBlock::Extend(nullptr);
  ptrdiff_t distance = data.limit - data.top;

  V8MONKEY_CHECK(distance == TestingBlock::BlockSize, "Address limits correct");

  TestingBlock::Delete(data.limit, data.top, nullptr);
}


V8MONKEY_TEST(ObjectBlock008, "Deleting deletes individual members (single block)") {
  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 10; i++) {
    *(pos++) = new DeletionObject(&wasDeleted[i]);
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, pos, nullptr);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(wasDeleted[i], "Object deleted");
  }
}


V8MONKEY_TEST(ObjectBlock009, "Deleting deletes individual members (partial block)") {
  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 20; i++) {
    *(pos++) = new DeletionObject(&wasDeleted[i]);
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, pos, pos - 10);

  for (int i = 10; i < 20; i++) {
    V8MONKEY_CHECK(wasDeleted[i], "Object deleted");
  }

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  ObjectBlock<DeletionObject>::Delete(data.limit, pos - 10, nullptr);
}


V8MONKEY_TEST(ObjectBlock010, "Deleting deletes individual members (cross block full rollback)") {
  ObjectBlock<DeletionObject>::Limits data1 = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data1.top;

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    *(pos++) = new DeletionObject(&wasDeleted[i]);
  }

  V8MONKEY_CHECK(pos == data1.limit, "Sanity check");

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data1.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < deletionObjectMax - deletionObjectBlockSize; i++) {
    *(pos2++) = new DeletionObject(&wasDeleted[deletionObjectBlockSize + i]);
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, nullptr);

  for (int i = 0; i < deletionObjectMax; i++) {
    V8MONKEY_CHECK(wasDeleted[i], "Object deleted");
  }
}


V8MONKEY_TEST(ObjectBlock011, "Deleting deletes individual members (cross block)") {
  ObjectBlock<DeletionObject>::Limits data1 = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data1.top;

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    *(pos++) = new DeletionObject(&wasDeleted[i]);
  }

  V8MONKEY_CHECK(pos == data1.limit, "Sanity check");

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data1.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < deletionObjectMax - deletionObjectBlockSize; i++) {
    *(pos2++) = new DeletionObject(&wasDeleted[deletionObjectBlockSize + i]);
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, data1.top + 10);

  for (int i = 10; i < deletionObjectMax; i++) {
    V8MONKEY_CHECK(wasDeleted[i], "Object deleted");
  }

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  ObjectBlock<DeletionObject>::Delete(data1.limit, data1.top + 10, nullptr);
}


V8MONKEY_TEST(ObjectBlock012, "Deleting deletes individual members (cross block to full block") {
  ObjectBlock<DeletionObject>::Limits data1 = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data1.top;

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    *(pos++) = new DeletionObject(&wasDeleted[i]);
  }

  V8MONKEY_CHECK(pos == data1.limit, "Sanity check");

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data1.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < deletionObjectMax - deletionObjectBlockSize; i++) {
    *(pos2++) = new DeletionObject(&wasDeleted[deletionObjectBlockSize + i]);
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, data1.limit);

  for (int i = deletionObjectBlockSize; i < deletionObjectMax; i++) {
    V8MONKEY_CHECK(wasDeleted[i], "Object deleted");
  }

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  ObjectBlock<DeletionObject>::Delete(data1.limit, data1.limit, nullptr);
}


V8MONKEY_TEST(ObjectBlock013, "Deleting calls given function if supplied (single block)") {
  DeletionObject* pointers[10];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 10; i++) {
    pointers[i] = new DeletionObject(&wasDeleted[i], i);
    *(pos++) = pointers[i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, pos, nullptr, deletionFunction);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called");
  }

  for (int i = 0; i < 10; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock014, "Deleting calls given function if supplied (intra block)") {
  DeletionObject* pointers[20];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 20; i++) {
    pointers[i] = new DeletionObject(&wasDeleted[i], i);
    *(pos++) = pointers[i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, data.top + 20, data.top + 10, deletionFunction);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(!wasCalled[i], "Given function not called for surviving values");
  }

  for (int i = 10; i < 20; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called for correct values");
  }

  ObjectBlock<DeletionObject>::Delete(data.limit, data.top + 10, nullptr, deletionFunction);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called for remaining values");
  }

  for (int i = 0; i < 20; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock015, "Deleting calls given function if supplied (cross block)") {
  DeletionObject* pointers[deletionObjectMax];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    pointers[i] = new DeletionObject(&wasDeleted[i], i);
    *(pos++) = pointers[i];
  }

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < deletionObjectMax - deletionObjectBlockSize; i++) {
    int index = i + deletionObjectBlockSize;
    pointers[index] = new DeletionObject(&wasDeleted[index], index);
    *(pos2++) = pointers[index];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, nullptr, deletionFunction);

  for (int i = 0; i < deletionObjectMax; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called for values");
  }

  for (int i = 0; i < deletionObjectMax; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock016, "Deleting calls given function if supplied (cross block to full block") {
  DeletionObject* pointers[deletionObjectMax];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    pointers[i] = new DeletionObject(&wasDeleted[i], i);
    *(pos++) = pointers[i];
  }

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < deletionObjectMax - deletionObjectBlockSize; i++) {
    int index = i + deletionObjectBlockSize;
    pointers[index] = new DeletionObject(&wasDeleted[index], index);
    *(pos2++) = pointers[index];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, data.limit, deletionFunction);

  for (int i = deletionObjectBlockSize; i < deletionObjectMax; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called for values");
  }

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    V8MONKEY_CHECK(!wasCalled[i], "Given function called for remaining values");
  }

  ObjectBlock<DeletionObject>::Delete(data.limit, data.limit, nullptr, deletionFunction);

  for (int i = 0; i < deletionObjectMax; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock017, "If supplied function, object not deleted by ObjectBlock") {
  DeletionObject* pointers[10];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 10; i++) {
    pointers[i] = new DeletionObject(&wasDeleted[i], i);
    *(pos++) = pointers[i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, pos, nullptr, deletionFunction);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  for (int i = 0; i < 10; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock018, "If supplied function, object not deleted by ObjectBlock (intra block)") {
  DeletionObject* pointers[20];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 20; i++) {
    pointers[i] = new DeletionObject(&wasDeleted[i], i);
    *(pos++) = pointers[i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, data.top + 20, data.top + 10, deletionFunction);

  for (int i = 0; i < 20; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  ObjectBlock<DeletionObject>::Delete(data.limit, data.top + 10, nullptr, deletionFunction);

  for (int i = 0; i < 20; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock019, "If supplied function, object not deleted by ObjectBlock (cross block)") {
  DeletionObject* pointers[deletionObjectMax];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    pointers[i] = new DeletionObject(&wasDeleted[i], i);
    *(pos++) = pointers[i];
  }

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < deletionObjectMax - deletionObjectBlockSize; i++) {
    int index = i + deletionObjectBlockSize;
    pointers[index] = new DeletionObject(&wasDeleted[index], index);
    *(pos2++) = pointers[index];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, nullptr, deletionFunction);

  for (int i = 0; i < deletionObjectMax; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  for (int i = 0; i < deletionObjectMax; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock020, "If supplied function, object not deleted by ObjectBlock (cross block to full block)") {
  DeletionObject* pointers[deletionObjectMax];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(nullptr);
  DeletionObject** pos = data.top;

  for (int i = 0; i < deletionObjectBlockSize; i++) {
    pointers[i] = new DeletionObject(&wasDeleted[i], i);
    *(pos++) = pointers[i];
  }

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < deletionObjectMax - deletionObjectBlockSize; i++) {
    int index = i + deletionObjectBlockSize;
    pointers[index] = new DeletionObject(&wasDeleted[index], index);
    *(pos2++) = pointers[index];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, data.limit, deletionFunction);

  for (int i = 0; i < deletionObjectMax; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  ObjectBlock<DeletionObject>::Delete(data.limit, data.limit, nullptr, deletionFunction);

  for (int i = 0; i < deletionObjectMax; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock021, "Iteration works correctly (single block)") {
  ObjectBlock<IterateObject>::Limits data = ObjectBlock<IterateObject>::Extend(nullptr);
  IterateObject** pos = data.top;

  for (int i = 0; i < 10; i++) {
    *(pos++) = new IterateObject(i);
  }

  resetForIteration();
  ObjectBlock<IterateObject>::Iterate(data.limit, pos, objectVisitor);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(objectVisited[i], "Object visited");
  }

  ObjectBlock<IterateObject>::Delete(data.limit, data.top + 10, nullptr);
}


V8MONKEY_TEST(ObjectBlock022, "Iteration works correctly (cross block)") {
  ObjectBlock<IterateObject>::Limits data1 = ObjectBlock<IterateObject>::Extend(nullptr);
  IterateObject** pos = data1.top;
  ObjectBlock<IterateObject>::Limits data2 = ObjectBlock<IterateObject>::Extend(data1.limit);
  IterateObject** pos2 = data2.top;

  for (int i = 0; i < iterationObjectBlockSize; i++) {
    *(pos++) = new IterateObject(i);
  }

  for (int i = 0; i < iterationObjectMax - iterationObjectBlockSize; i++) {
    *(pos2++) = new IterateObject(iterationObjectBlockSize + i);
  }

  resetForIteration();
  ObjectBlock<IterateObject>::Iterate(data2.limit, pos2, objectVisitor);

  for (int i = 0; i < iterationObjectMax; i++) {
    V8MONKEY_CHECK(objectVisited[i], "Object visited");
  }

  ObjectBlock<IterateObject>::Delete(data2.limit, pos2, nullptr);
}


V8MONKEY_TEST(ObjectBlock023, "Iteration with data works correctly (single block)") {
  ObjectBlock<IterateObject>::Limits data = ObjectBlock<IterateObject>::Extend(nullptr);
  IterateObject** pos = data.top;

  for (int i = 0; i < 10; i++) {
    *(pos++) = new IterateObject(i);
  }

  resetForIteration();
  void* objData = reinterpret_cast<void*>(0xbeef);
  ObjectBlock<IterateObject>::Iterate(data.limit, pos, objectDataVisitor, objData);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(objectVisitData[i] == objData, "Object visited with correct data");
  }

  ObjectBlock<IterateObject>::Delete(data.limit, data.top + 10, nullptr);
}


V8MONKEY_TEST(ObjectBlock024, "Iteration with data works correctly (cross block)") {
  ObjectBlock<IterateObject>::Limits data1 = ObjectBlock<IterateObject>::Extend(nullptr);
  IterateObject** pos = data1.top;
  ObjectBlock<IterateObject>::Limits data2 = ObjectBlock<IterateObject>::Extend(data1.limit);
  IterateObject** pos2 = data2.top;

  for (int i = 0; i < iterationObjectBlockSize; i++) {
    *(pos++) = new IterateObject(i);
  }

  for (int i = 0; i < iterationObjectMax - iterationObjectBlockSize; i++) {
    *(pos2++) = new IterateObject(iterationObjectBlockSize+ i);
  }

  resetForIteration();
  void* objData = reinterpret_cast<void*>(0xbeef);
  ObjectBlock<IterateObject>::Iterate(data2.limit, pos2, objectDataVisitor, objData);

  for (int i = 0; i < iterationObjectMax; i++) {
    V8MONKEY_CHECK(objectVisitData[i] == objData, "Object visited with correct data");
  }


  ObjectBlock<IterateObject>::Delete(data2.limit, pos2, nullptr);
}
