#include "v8.h"

#include "data_structures/destruct_list.h"
#include "test.h"
#include "V8MonkeyTest.h"


using namespace v8::V8Monkey;


#define DESTRUCTARRAYSIZE 10
namespace {
  void intFreer(int* i) {
    if (i) {
      delete i;
    }
  }


  class DeletionObject;


  static bool wasDeleted[DESTRUCTARRAYSIZE];


  static bool nullDeleted = false;


  void resetForDeletion() {
    for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
      wasDeleted[i] = false;
    }
    nullDeleted = false;
  }


  class DeletionObject {
    public:
      DeletionObject(int n) { index = n; }
      ~DeletionObject() { wasDeleted[index] = true; }
      int index;
  };


  void deletionFunction(DeletionObject* d) {
    if (d != nullptr) {
      wasDeleted[d->index] = true;
      delete d;
    } else {
      nullDeleted = true;
    }
  }


  class IterateObject;


  bool objectVisited[DESTRUCTARRAYSIZE];


  static bool nullVisited = false;


  void resetForIteration() {
    for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
      objectVisited[i] = false;
    }
    nullVisited = false;
  }


  class IterateObject {
    public:
      IterateObject(int n) { index = n; }
      ~IterateObject() {}
      int index;
  };


  void objectVisitor(IterateObject* i) {
    if (i != nullptr)  {
      objectVisited[i->index] = true;
    } else {
      nullVisited = true;
    }
  }


  void iterationFreer(IterateObject* i) {
    if (i) {
      delete i;
    }
  }
}


V8MONKEY_TEST(DSList001, "Count initially 0") {
  DestructingList<int> ds(intFreer);
  V8MONKEY_CHECK(ds.Count() == 0, "Count is correct");
}


V8MONKEY_TEST(DSList002, "Count correct after adding one") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  ds.Add(a);
  V8MONKEY_CHECK(ds.Count() == 1, "Count is correct");
}


V8MONKEY_TEST(DSList003, "Count correct after adding multiple") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  int* c = new int(3);
  ds.Add(a);
  ds.Add(b);
  ds.Add(c);
  V8MONKEY_CHECK(ds.Count() == 3, "Count is correct");
}


V8MONKEY_TEST(DSList004, "Doesn't add same pointer twice") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  ds.Add(a);
  ds.Add(a);
  V8MONKEY_CHECK(ds.Count() == 1, "Duplicate not added");
}


V8MONKEY_TEST(DSList005, "Can add null") {
  DestructingList<int> ds(intFreer);
  ds.Add(nullptr);
  V8MONKEY_CHECK(ds.Count() == 1, "Null added");
}


V8MONKEY_TEST(DSList006, "Can't add null twice") {
  DestructingList<int> ds(intFreer);
  ds.Add(nullptr);
  ds.Add(nullptr);
  V8MONKEY_CHECK(ds.Count() == 1, "Null not duplicated");
}


V8MONKEY_TEST(DSList007, "Contains works correctly when empty (1)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  V8MONKEY_CHECK(!ds.Contains(a), "Item not found");
  delete a;
}


V8MONKEY_TEST(DSList008, "Contains works correctly when empty (2)") {
  DestructingList<int> ds(intFreer);
  V8MONKEY_CHECK(!ds.Contains(nullptr), "Null not found");
}


V8MONKEY_TEST(DSList009, "Contains works correctly (1)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  ds.Add(a);
  V8MONKEY_CHECK(ds.Contains(a), "Item found");
}


V8MONKEY_TEST(DSList010, "Contains works correctly (2)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  ds.Add(a);
  ds.Add(b);
  V8MONKEY_CHECK(ds.Contains(a), "Item 1 found");
  V8MONKEY_CHECK(ds.Contains(b), "Item 2 found");
}


V8MONKEY_TEST(DSList011, "Contains works correctly (3)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  ds.Add(a);
  V8MONKEY_CHECK(!ds.Contains(b), "Item not found");
  delete b;
}


V8MONKEY_TEST(DSList012, "Contains works correctly (4)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  ds.Add(a);
  ds.Add(b);
  V8MONKEY_CHECK(!ds.Contains(nullptr), "Null not found");
}


V8MONKEY_TEST(DSList013, "Contains works correctly (5)") {
  DestructingList<int> ds(intFreer);
  ds.Add(nullptr);
  V8MONKEY_CHECK(ds.Contains(nullptr), "Null found");
}


V8MONKEY_TEST(DSList014, "Contains works correctly (6)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  ds.Add(a);
  ds.Add(nullptr);
  V8MONKEY_CHECK(ds.Contains(nullptr), "Null found");
}


V8MONKEY_TEST(DSList015, "Delete Works correctly when empty") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  ds.Delete(a);
  V8MONKEY_CHECK(ds.Count() == 0, "List unaffected");
}


V8MONKEY_TEST(DSList016, "Delete works correctly when not there") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  int* c = new int(3);
  ds.Add(a);
  ds.Add(b);
  ds.Delete(c);
  V8MONKEY_CHECK(ds.Contains(a), "List unaffected (1)");
  V8MONKEY_CHECK(ds.Contains(b), "List unaffected (2)");
  V8MONKEY_CHECK(ds.Count() == 2, "List unaffected (3)");
}


V8MONKEY_TEST(DSList017, "Delete works correctly when there (1)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  int* c = new int(3);
  ds.Add(a);
  ds.Add(b);
  ds.Add(c);
  ds.Delete(a);
  V8MONKEY_CHECK(!ds.Contains(a), "Value deleted");
  V8MONKEY_CHECK(ds.Contains(b), "List unaffected (1)");
  V8MONKEY_CHECK(ds.Contains(c), "List unaffected (2)");
  V8MONKEY_CHECK(ds.Count() == 2, "List unaffected (3)");
}


V8MONKEY_TEST(DSList018, "Delete works correctly when there (2)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  int* c = new int(3);
  ds.Add(a);
  ds.Add(b);
  ds.Add(c);
  ds.Delete(b);
  V8MONKEY_CHECK(ds.Contains(a), "List unaffected (1)");
  V8MONKEY_CHECK(!ds.Contains(b), "Value deleted");
  V8MONKEY_CHECK(ds.Contains(c), "List unaffected (2)");
  V8MONKEY_CHECK(ds.Count() == 2, "List unaffected (3)");
}


V8MONKEY_TEST(DSList019, "Delete works correctly when there (3)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  int* c = new int(3);
  ds.Add(a);
  ds.Add(b);
  ds.Add(c);
  ds.Delete(c);
  V8MONKEY_CHECK(ds.Contains(a), "List unaffected (1)");
  V8MONKEY_CHECK(ds.Contains(b), "List unaffected (2)");
  V8MONKEY_CHECK(!ds.Contains(c), "Value deleted");
  V8MONKEY_CHECK(ds.Count() == 2, "List unaffected (3)");
}


V8MONKEY_TEST(DSList020, "Works correctly for null (1)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  ds.Add(nullptr);
  ds.Add(a);
  ds.Add(b);
  ds.Delete(nullptr);
  V8MONKEY_CHECK(ds.Contains(a), "List unaffected (1)");
  V8MONKEY_CHECK(ds.Contains(b), "List unaffected (2)");
  V8MONKEY_CHECK(!ds.Contains(nullptr), "Value deleted");
  V8MONKEY_CHECK(ds.Count() == 2, "List unaffected (3)");
}


V8MONKEY_TEST(DSList021, "Works correctly for null (2)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  ds.Add(a);
  ds.Add(nullptr);
  ds.Add(b);
  ds.Delete(nullptr);
  V8MONKEY_CHECK(ds.Contains(a), "List unaffected (1)");
  V8MONKEY_CHECK(ds.Contains(b), "List unaffected (2)");
  V8MONKEY_CHECK(!ds.Contains(nullptr), "Value deleted");
  V8MONKEY_CHECK(ds.Count() == 2, "List unaffected (3)");
}


V8MONKEY_TEST(DSList022, "Works correctly for null (3)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  ds.Add(a);
  ds.Add(b);
  ds.Add(nullptr);
  ds.Delete(nullptr);
  V8MONKEY_CHECK(ds.Contains(a), "List unaffected (1)");
  V8MONKEY_CHECK(ds.Contains(b), "List unaffected (2)");
  V8MONKEY_CHECK(!ds.Contains(nullptr), "Value deleted");
  V8MONKEY_CHECK(ds.Count() == 2, "List unaffected (3)");
}


V8MONKEY_TEST(DSList023, "Works correctly for null (4)") {
  DestructingList<int> ds(intFreer);
  ds.Add(nullptr);
  ds.Delete(nullptr);
  V8MONKEY_CHECK(!ds.Contains(nullptr), "Value deleted");
  V8MONKEY_CHECK(ds.Count() == 0, "List unaffected (3)");
}


V8MONKEY_TEST(DSList024, "Works correctly for null (5)") {
  DestructingList<int> ds(intFreer);
  int* a = new int(1);
  int* b = new int(2);
  ds.Add(a);
  ds.Add(b);
  ds.Delete(nullptr);
  V8MONKEY_CHECK(ds.Contains(a), "List unaffected (1)");
  V8MONKEY_CHECK(ds.Contains(b), "List unaffected (2)");
  V8MONKEY_CHECK(ds.Count() == 2, "List unaffected (3)");
}


V8MONKEY_TEST(DSList025, "Iteration works when empty") {
  DestructingList<IterateObject>* ds = new DestructingList<IterateObject>(iterationFreer);

  resetForIteration();
  ds->Iterate(objectVisitor);

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    V8MONKEY_CHECK(!objectVisited[i], "Object not iterated");
  }
  V8MONKEY_CHECK(!nullVisited, "Null not iterated");

  delete ds;
}


V8MONKEY_TEST(DSList026, "Iteration works correctly") {
  DestructingList<IterateObject>* ds = new DestructingList<IterateObject>(iterationFreer);
  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    ds->Add(new IterateObject(i));
  }

  resetForIteration();
  ds->Iterate(objectVisitor);

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    V8MONKEY_CHECK(objectVisited[i], "Object iterated");
  }
  V8MONKEY_CHECK(!nullVisited, "Null not iterated");

  delete ds;
}


V8MONKEY_TEST(DSList027, "Iteration works correctly when null present (1)") {
  DestructingList<IterateObject>* ds = new DestructingList<IterateObject>(iterationFreer);
  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    ds->Add(new IterateObject(i));
  }
  ds->Add(nullptr);

  resetForIteration();
  ds->Iterate(objectVisitor);

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    V8MONKEY_CHECK(objectVisited[i], "Object iterated");
  }
  V8MONKEY_CHECK(nullVisited, "Null iterated");

  delete ds;
}


V8MONKEY_TEST(DSList028, "Iteration works correctly when null present (2)") {
  DestructingList<IterateObject>* ds = new DestructingList<IterateObject>(iterationFreer);
  ds->Add(nullptr);

  resetForIteration();
  ds->Iterate(objectVisitor);

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    V8MONKEY_CHECK(!objectVisited[i], "Object not iterated");
  }
  V8MONKEY_CHECK(nullVisited, "Null iterated");

  delete ds;
}


V8MONKEY_TEST(DSList029, "List deletion works when empty") {
  DestructingList<DeletionObject>* ds = new DestructingList<DeletionObject>(deletionFunction);

  resetForDeletion();
  delete ds;

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }
  V8MONKEY_CHECK(!nullDeleted, "Null not deleted");
}


V8MONKEY_TEST(DSList030, "List deletion works correctly") {
  DestructingList<DeletionObject>* ds = new DestructingList<DeletionObject>(deletionFunction);
  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    ds->Add(new DeletionObject(i));
  }

  resetForDeletion();
  delete ds;

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    V8MONKEY_CHECK(wasDeleted[i], "Object deleted");
  }
  V8MONKEY_CHECK(!nullDeleted, "Null not deleted");
}


V8MONKEY_TEST(DSList031, "List deletion works correctly when null present (1)") {
  DestructingList<DeletionObject>* ds = new DestructingList<DeletionObject>(deletionFunction);
  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    ds->Add(new DeletionObject(i));
  }
  ds->Add(nullptr);

  resetForDeletion();
  delete ds;

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    V8MONKEY_CHECK(wasDeleted[i], "Object deleted");
  }
  V8MONKEY_CHECK(nullDeleted, "Null deleted");
}


V8MONKEY_TEST(DSList032, "List deletion works correctly when null present (2)") {
  DestructingList<DeletionObject>* ds = new DestructingList<DeletionObject>(deletionFunction);
  ds->Add(nullptr);

  resetForDeletion();
  delete ds;

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
  }
  V8MONKEY_CHECK(nullDeleted, "Null deleted");
}


V8MONKEY_TEST(DSList033, "Deletion function called when individual items deleted") {
  DestructingList<DeletionObject>* ds = new DestructingList<DeletionObject>(deletionFunction);
  DeletionObject* ptrs[DESTRUCTARRAYSIZE];

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    ptrs[i] = new DeletionObject(i);
    ds->Add(ptrs[i]);
  }

  resetForDeletion();
  ds->Delete(ptrs[5]);

  for (int i = 0; i < DESTRUCTARRAYSIZE; i++) {
    if (i != 5) {
      V8MONKEY_CHECK(!wasDeleted[i], "Object not deleted");
    } else {
      V8MONKEY_CHECK(wasDeleted[i], "Object deleted");
    }
  }
  V8MONKEY_CHECK(!nullDeleted, "Null deleted");

  delete ds;
}
