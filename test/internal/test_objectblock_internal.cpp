#include "v8.h"

#include "data_structures/objectblock.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8::V8Monkey;


// ObjectBlock's bookkeeping would be easy to get wrong, hence we poke around testing it's internals as well as API


using namespace v8::V8Monkey;


namespace {
  // Fill up an ObjectBlock<int> with deletable data
  void fill(int** from, int** to) {
    for (; from < to; from++) {
      *from = new int;
    }
  }


  class DeletionObject;


  static bool wasCalled[ObjectBlock<DeletionObject>::EffectiveBlockSize + 2];


  static bool wasDeleted[ObjectBlock<DeletionObject>::EffectiveBlockSize + 2];


  void resetForDeletion() {
    for (int i = 0; i < ObjectBlock<DeletionObject>::EffectiveBlockSize + 2; i++) {
      wasCalled[i] = false;
      wasDeleted[i] = false;
    }
  }


  class DeletionObject {
    public:
      DeletionObject(int n) { index = n; }
      ~DeletionObject() { wasDeleted[index] = true; }
      int index;
  };


  void deletionFunction(DeletionObject* d) {
    wasCalled[d->index] = true;
  }


  class IterateObject;


  bool objectVisited[ObjectBlock<IterateObject>::EffectiveBlockSize * 2];


  void* objectVisitData[ObjectBlock<IterateObject>::EffectiveBlockSize * 2];


  void resetForIteration() {
    for (int i = 0; i < ObjectBlock<IterateObject>::EffectiveBlockSize * 2; i++) {
      objectVisited[i] = false;
      objectVisitData[i] = nullptr;
    }
  }


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


V8MONKEY_TEST(ObjectBlock001, "Number of objects returns 0 when supplied NULL") {
  V8MONKEY_CHECK(TestingBlock::NumberOfItems(NULL) == 0, "Correct value returned");
}


V8MONKEY_TEST(ObjectBlock002, "Number of objects returns 0 when block extended by 1 block") {
  TestingBlock::Limits data = TestingBlock::Extend(NULL);

  V8MONKEY_CHECK(TestingBlock::NumberOfItems(data.limit) == 0, "Correct value returned");

  TestingBlock::Delete(data.limit, data.top, NULL);
}


V8MONKEY_TEST(ObjectBlock003, "Number of objects returns (<blocksize> - 2) when block extended by 2 blocks") {
  TestingBlock::Limits data1 = TestingBlock::Extend(NULL);
  fill(data1.top, data1.limit);
  TestingBlock::Limits data2 = TestingBlock::Extend(data1.limit);

  V8MONKEY_CHECK(TestingBlock::NumberOfItems(data2.limit) == TestingBlock::EffectiveBlockSize, "Correct value returned");

  TestingBlock::Delete(data2.limit, data2.top, data1.top);
  TestingBlock::Delete(data1.limit, data1.top, NULL);
}


V8MONKEY_TEST(ObjectBlock004, "Number of objects returns (<blocksize> - 2) * 2 when block extended by 2 blocks") {
  TestingBlock::Limits data1 = TestingBlock::Extend(NULL);
  fill(data1.top, data1.limit);
  TestingBlock::Limits data2 = TestingBlock::Extend(data1.limit);
  fill(data2.top, data2.limit);
  TestingBlock::Limits data3 = TestingBlock::Extend(data2.limit);

  V8MONKEY_CHECK(TestingBlock::NumberOfItems(data3.limit) == 2 * TestingBlock::EffectiveBlockSize, "Correct value returned");

  TestingBlock::Delete(data3.limit, data3.top, data2.top);
  TestingBlock::Delete(data2.limit, data2.top, data1.top);
  TestingBlock::Delete(data1.limit, data1.top, NULL);
}


V8MONKEY_TEST(ObjectBlock005, "<Internal book-keeping> After 1 extension, previous field points to NULL") {
  TestingBlock::Limits data = TestingBlock::Extend(NULL);
  TestingBlock* prev = reinterpret_cast<TestingBlock*>(*(data.top - 2));

  V8MONKEY_CHECK(prev == NULL, "Prev field in first allocated block points to null");

  TestingBlock::Delete(data.limit, data.top, NULL);
}


V8MONKEY_TEST(ObjectBlock006, "<Internal book-keeping> After 2 extensions, previous field points to limit of first extension") {
  TestingBlock::Limits data1 = TestingBlock::Extend(NULL);
  fill(data1.top, data1.limit);
  TestingBlock::Limits data2 = TestingBlock::Extend(data1.limit);
  int** prev = reinterpret_cast<int**>(*(data2.top - 2));

  V8MONKEY_CHECK(prev == data1.top - 2, "Prev field points to previous block");

  TestingBlock::Delete(data2.limit, data2.top, data1.top);
  TestingBlock::Delete(data1.limit, data1.top, NULL);
}


V8MONKEY_TEST(ObjectBlock007, "Limit and top correspond") {
  TestingBlock::Limits data = TestingBlock::Extend(NULL);
  ptrdiff_t distance = data.limit - data.top;

  V8MONKEY_CHECK(distance == TestingBlock::EffectiveBlockSize, "Address limits correct");

  TestingBlock::Delete(data.limit, data.top, NULL);
}


V8MONKEY_TEST(ObjectBlock008, "Deleting deletes individual members (single block)") {
  class DummyObj;

  static bool objectDeleted[ObjectBlock<DummyObj>::EffectiveBlockSize];
  for (int i = 0; i < ObjectBlock<DummyObj>::EffectiveBlockSize; i++) {
    objectDeleted[i] = false;
  }

  class DummyObj {
    public:
      DummyObj(int n) { index = n; }
      ~DummyObj() { objectDeleted[index] = true; }
    private:
      int index;
  };

  ObjectBlock<DummyObj>::Limits data = ObjectBlock<DummyObj>::Extend(NULL);
  DummyObj** pos = data.top;

  for (int i = 0; i < 10; i++) {
    *(pos++) = new DummyObj(i);
  }

  ObjectBlock<DummyObj>::Delete(data.limit, data.top + 10, NULL);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(objectDeleted[i], "Object deleted");
  }
}


V8MONKEY_TEST(ObjectBlock009, "Deleting deletes individual members (partial block)") {
  class DummyObj;

  static bool objectDeleted[ObjectBlock<DummyObj>::EffectiveBlockSize];
  for (int i = 0; i < ObjectBlock<DummyObj>::EffectiveBlockSize; i++) {
    objectDeleted[i] = false;
  }

  class DummyObj {
    public:
      DummyObj(int n) { index = n; }
      ~DummyObj() { objectDeleted[index] = true; }
    private:
      int index;
  };

  ObjectBlock<DummyObj>::Limits data = ObjectBlock<DummyObj>::Extend(NULL);
  DummyObj** pos = data.top;

  for (int i = 0; i < 20; i++) {
    *(pos++) = new DummyObj(i);
  }

  ObjectBlock<DummyObj>::Delete(data.limit, data.top + 20, data.top + 10);

  for (int i = 10; i < 20; i++) {
    V8MONKEY_CHECK(objectDeleted[i], "Object deleted");
  }

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(!objectDeleted[i], "Object not deleted");
  }

  ObjectBlock<DummyObj>::Delete(data.limit, data.top + 10, NULL);
}


V8MONKEY_TEST(ObjectBlock010, "Deleting deletes individual members (cross block full rollback)") {
  class DummyObj;

  static bool objectDeleted[ObjectBlock<DummyObj>::EffectiveBlockSize * 2];
  for (int i = 0; i < ObjectBlock<DummyObj>::EffectiveBlockSize * 2; i++) {
    objectDeleted[i] = false;
  }

  class DummyObj {
    public:
      DummyObj(int n) { index = n; }
      ~DummyObj() { objectDeleted[index] = true; }
    private:
      int index;
  };

  ObjectBlock<DummyObj>::Limits data1 = ObjectBlock<DummyObj>::Extend(NULL);
  DummyObj** pos = data1.top;

  for (int i = 0; i < ObjectBlock<DummyObj>::EffectiveBlockSize; i++) {
    *(pos++) = new DummyObj(i);
  }

  ObjectBlock<DummyObj>::Limits data2 = ObjectBlock<DummyObj>::Extend(data1.limit);
  DummyObj** pos2 = data2.top;

  for (int i = 0; i < 20; i++) {
    *(pos2++) = new DummyObj(ObjectBlock<DummyObj>::EffectiveBlockSize + i);
  }

  ObjectBlock<DummyObj>::Delete(data2.limit, data2.top + 20, NULL);

  for (int i = 0; i < ObjectBlock<DummyObj>::EffectiveBlockSize + 20; i++) {
    V8MONKEY_CHECK(objectDeleted[i], "Object deleted");
  }
}


V8MONKEY_TEST(ObjectBlock011, "Deleting deletes individual members (cross block)") {
  class DummyObj;

  static bool objectDeleted[ObjectBlock<DummyObj>::EffectiveBlockSize + 20];
  for (int i = 0; i < ObjectBlock<DummyObj>::EffectiveBlockSize + 20; i++) {
    objectDeleted[i] = false;
  }

  class DummyObj {
    public:
      DummyObj(int n) { index = n; }
      ~DummyObj() { objectDeleted[index] = true; }
    private:
      int index;
  };

  ObjectBlock<DummyObj>::Limits data1 = ObjectBlock<DummyObj>::Extend(NULL);
  DummyObj** pos = data1.top;

  for (int i = 0; i < ObjectBlock<DummyObj>::EffectiveBlockSize; i++) {
    *(pos++) = new DummyObj(i);
  }

  ObjectBlock<DummyObj>::Limits data2 = ObjectBlock<DummyObj>::Extend(data1.limit);
  DummyObj** pos2 = data2.top;

  for (int i = 0; i < 20; i++) {
    *(pos2++) = new DummyObj(ObjectBlock<DummyObj>::EffectiveBlockSize + i);
  }

  ObjectBlock<DummyObj>::Delete(data2.limit, pos2, data1.top + 10);

  for (int i = 10; i < ObjectBlock<DummyObj>::EffectiveBlockSize + 20; i++) {
    V8MONKEY_CHECK(objectDeleted[i], "Object deleted");
  }

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(!objectDeleted[i], "Object not deleted");
  }

  ObjectBlock<DummyObj>::Delete(data1.limit, data1.top + 10, NULL);
}


V8MONKEY_TEST(ObjectBlock012, "Deleting calls given function if supplied (single block)") {
  DeletionObject* pointers[10];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(NULL);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 10; i++) {
    pointers[i] = new DeletionObject(i);
    *(pos++) = pointers[i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, pos, NULL, deletionFunction);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called");
  }

  for (int i = 0; i < 10; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock013, "Deleting calls given function if supplied (intra block)") {
  DeletionObject* pointers[20];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(NULL);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 20; i++) {
    pointers[i] = new DeletionObject(i);
    *(pos++) = pointers[i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, data.top + 20, data.top + 10, deletionFunction);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(!wasCalled[i], "Given function not called for remaining values");
  }

  for (int i = 10; i < 20; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called for correct values");
  }

  ObjectBlock<DeletionObject>::Delete(data.limit, data.top + 10, NULL, deletionFunction);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called for remaining values");
  }

  for (int i = 0; i < 20; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock014, "Deleting calls given function if supplied (cross block)") {
  const int blockSize = ObjectBlock<DeletionObject>::EffectiveBlockSize;
  DeletionObject* pointers[blockSize + 2];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(NULL);
  DeletionObject** pos = data.top;

  for (int i = 0; i < blockSize; i++) {
    pointers[i] = new DeletionObject(i);
    *(pos++) = pointers[i];
  }

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < 2; i++) {
    pointers[i + blockSize] = new DeletionObject(i + blockSize);
    *(pos2++) = pointers[i + blockSize];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, NULL, deletionFunction);

  for (int i = 0; i < blockSize + 2; i++) {
    V8MONKEY_CHECK(wasCalled[i], "Given function called for values");
  }

  for (int i = 0; i < blockSize + 2; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock015, "If supplied function, object not deleted by ObjectBlock") {
  DeletionObject* pointers[10];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(NULL);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 10; i++) {
    pointers[i] = new DeletionObject(i);
    *(pos++) = pointers[i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, pos, NULL, deletionFunction);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  for (int i = 0; i < 10; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock016, "If supplied function, object not deleted by ObjectBlock (intra block)") {
  DeletionObject* pointers[20];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(NULL);
  DeletionObject** pos = data.top;

  for (int i = 0; i < 20; i++) {
    pointers[i] = new DeletionObject(i);
    *(pos++) = pointers[i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data.limit, data.top + 20, data.top + 10, deletionFunction);

  for (int i = 0; i < 20; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  ObjectBlock<DeletionObject>::Delete(data.limit, data.top + 10, NULL, deletionFunction);

  for (int i = 0; i < 20; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock017, "If supplied function, object not deleted by ObjectBlock (cross block)") {
  const int blockSize = ObjectBlock<DeletionObject>::EffectiveBlockSize;
  DeletionObject* pointers[blockSize + 2];

  ObjectBlock<DeletionObject>::Limits data = ObjectBlock<DeletionObject>::Extend(NULL);
  DeletionObject** pos = data.top;

  for (int i = 0; i < blockSize; i++) {
    pointers[i] = new DeletionObject(i);
    *(pos++) = pointers[i];
  }

  ObjectBlock<DeletionObject>::Limits data2 = ObjectBlock<DeletionObject>::Extend(data.limit);
  DeletionObject** pos2 = data2.top;

  for (int i = 0; i < 2; i++) {
    pointers[blockSize + i] = new DeletionObject(blockSize + i);
    *(pos2++) = pointers[blockSize + i];
  }

  resetForDeletion();
  ObjectBlock<DeletionObject>::Delete(data2.limit, pos2, NULL, deletionFunction);

  for (int i = 0; i < blockSize + 2; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }

  for (int i = 0; i < blockSize + 2; i++) {
    delete pointers[i];
  }
}


V8MONKEY_TEST(ObjectBlock018, "Iteration works correctly (single block)") {
  ObjectBlock<IterateObject>::Limits data = ObjectBlock<IterateObject>::Extend(NULL);
  IterateObject** pos = data.top;

  for (int i = 0; i < 10; i++) {
    *(pos++) = new IterateObject(i);
  }

  resetForIteration();
  ObjectBlock<IterateObject>::Iterate(data.limit, pos, objectVisitor);

  for (int i = 0; i < 10; i++) {
    V8MONKEY_CHECK(objectVisited[i], "Object visited");
  }

  ObjectBlock<IterateObject>::Delete(data.limit, data.top + 10, NULL);
}


V8MONKEY_TEST(ObjectBlock019, "Iteration works correctly (cross block)") {
  ObjectBlock<IterateObject>::Limits data1 = ObjectBlock<IterateObject>::Extend(NULL);
  IterateObject** pos = data1.top;
  ObjectBlock<IterateObject>::Limits data2 = ObjectBlock<IterateObject>::Extend(data1.limit);
  IterateObject** pos2 = data2.top;

  for (int i = 0; i < ObjectBlock<IterateObject>::EffectiveBlockSize; i++) {
    *(pos++) = new IterateObject(i);
  }

  for (int i = 0; i < 20; i++) {
    *(pos2++) = new IterateObject(ObjectBlock<IterateObject>::EffectiveBlockSize + i);
  }

  resetForIteration();
  ObjectBlock<IterateObject>::Iterate(data2.limit, data2.top + 20, objectVisitor);

  for (int i = 0; i < ObjectBlock<IterateObject>::EffectiveBlockSize + 20; i++) {
    V8MONKEY_CHECK(objectVisited[i], "Object visited");
  }


  ObjectBlock<IterateObject>::Delete(data2.limit, data2.top + 20, NULL);
}


V8MONKEY_TEST(ObjectBlock020, "Iteration with data works correctly (single block)") {
  ObjectBlock<IterateObject>::Limits data = ObjectBlock<IterateObject>::Extend(NULL);
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

  ObjectBlock<IterateObject>::Delete(data.limit, data.top + 10, NULL);
}


V8MONKEY_TEST(ObjectBlock021, "Iteration with data works correctly (cross block)") {
  ObjectBlock<IterateObject>::Limits data1 = ObjectBlock<IterateObject>::Extend(NULL);
  IterateObject** pos = data1.top;
  ObjectBlock<IterateObject>::Limits data2 = ObjectBlock<IterateObject>::Extend(data1.limit);
  IterateObject** pos2 = data2.top;

  for (int i = 0; i < ObjectBlock<IterateObject>::EffectiveBlockSize; i++) {
    *(pos++) = new IterateObject(i);
  }

  for (int i = 0; i < 20; i++) {
    *(pos2++) = new IterateObject(ObjectBlock<IterateObject>::EffectiveBlockSize + i);
  }

  resetForIteration();
  void* objData = reinterpret_cast<void*>(0xbeef);
  ObjectBlock<IterateObject>::Iterate(data2.limit, data2.top + 20, objectDataVisitor, objData);

  for (int i = 0; i < ObjectBlock<IterateObject>::EffectiveBlockSize + 20; i++) {
    V8MONKEY_CHECK(objectVisitData[i] == objData, "Object visited with correct data");
  }


  ObjectBlock<IterateObject>::Delete(data2.limit, data2.top + 20, NULL);
}
