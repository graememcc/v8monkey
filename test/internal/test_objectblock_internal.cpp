/*
// all_of
#include <algorithm>

// Class under test
#include "types/objectblock.h"

// begin, end
#include <iterator>

// shared_ptr
#include <memory>

// DeletionObject, DummyV8MonkeyObject
#include "types/base_types.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8::internal;


namespace {
  class IterateObject;


  const int iterationObjectBlockSize {v8::DataStructures::ObjectBlock<>::slabSize};
  const int iterationObjectMax {iterationObjectBlockSize + 20};
  bool objectVisited[iterationObjectMax];
  void* objectVisitData[iterationObjectMax];


  void resetForIteration() {
    for (int i = 0; i < iterationObjectMax; i++) {
      objectVisited[i] = false;
      objectVisitData[i] = nullptr;
    }
  }


  // The following tests sometimes need to note if objects are iterated over. To that end, we define a class that when
  // constructed takes an index, and define iteration functions that flip the values in the arrays above for each
  // value they are called with. resetForIteration should be called before iterating over the ObjectBlock.
  class IterateObject : public Object {
    public:
      IterateObject(int n) { index = n; }
      ~IterateObject() {}
      int index;
      void DoTrace(JSRuntime*, JSTracer*) {}
  };


  void objectVisitor(Object* o) {
    IterateObject* i {dynamic_cast<IterateObject*>(o)};
    objectVisited[i->index] = true;
  }


  void objectDataVisitor(Object* o, void* data) {
    IterateObject* i {dynamic_cast<IterateObject*>(o)};
    objectVisited[i->index] = true;
    objectVisitData[i->index] = data;
  }
}


using namespace v8::DataStructures;


// Convenience
using TestingBlock = ObjectBlock<>;


V8MONKEY_TEST(ObjectBlock001, "Number of objects initially zero") {
  TestingBlock tb {};
  V8MONKEY_CHECK(tb.NumberOfItems() == 0, "Correct value returned");
}


V8MONKEY_TEST(ObjectBlock002, "Number of objects correct up to slab size") {
  TestingBlock tb {};

  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    tb.Add(new DummyV8MonkeyObject {});
    V8MONKEY_CHECK(tb.NumberOfItems() == i + 1, "Correct value returned");
  }
}


V8MONKEY_TEST(ObjectBlock003, "Number of objects correct adding when adding more than slab size") {
  TestingBlock tb {};

  for (auto i = 1u; i <= TestingBlock::slabSize + 1; i++) {
    tb.Add(new DummyV8MonkeyObject {});
  }

  V8MONKEY_CHECK(tb.NumberOfItems() == TestingBlock::slabSize + 1, "Correct value returned");
}


V8MONKEY_TEST(ObjectBlock004, "Objects are AddRefed on addition") {
  TestingBlock tb {};
  Object* obj {new DummyV8MonkeyObject {}};
  auto refCount  = obj->RefCount();

  tb.Add(obj);
  V8MONKEY_CHECK(obj->RefCount() == refCount + 1, "Object was AddRefed");
}


V8MONKEY_TEST(ObjectBlock005, "Returned free slots not null") {
  TestingBlock tb{};

  TestingBlock::Limits limits = tb.Add(new DummyV8MonkeyObject {});
  V8MONKEY_CHECK(limits.next != nullptr, "Next not null");
}


V8MONKEY_TEST(ObjectBlock006, "Returned free slots not null") {
  TestingBlock tb{};

  TestingBlock::Limits limits = tb.Add(new DummyV8MonkeyObject {});
  V8MONKEY_CHECK(limits.limit != nullptr, "Next not null");
}


V8MONKEY_TEST(ObjectBlock007, "Adding values moves next pointer") {
  TestingBlock tb {};

  TestingBlock::Limits oldLimits = tb.Add(new DummyV8MonkeyObject {});
  for (auto i = 1u; i <= TestingBlock::slabSize + 1; i++) {
    TestingBlock::Limits limits = tb.Add(new DummyV8MonkeyObject {});
    V8MONKEY_CHECK(limits.next != oldLimits.next, "Next pointer moved");
  }
}


V8MONKEY_TEST(ObjectBlock008, "Up to slab size, values are allocated contiguously (1)") {
  TestingBlock tb {};

  Object** prevAddr {tb.Add(new DummyV8MonkeyObject {}).next};

  // Note: don't want to fill a block in this test
  for (auto i = 1u; i < TestingBlock::slabSize; i++) {
    Object** addr {tb.Add(new DummyV8MonkeyObject {}).next};

    V8MONKEY_CHECK(addr == (prevAddr + 1), "Next pointer moved correctly");

    prevAddr = addr;
  }
}


V8MONKEY_TEST(ObjectBlock009, "Up to slab size, values are allocated contiguously (2)") {
  TestingBlock tb {};

  Object** prevAddr {tb.Add(new DummyV8MonkeyObject {}).next};

  // Note: don't want to fill a block in this test
  for (auto i = 1u; i < TestingBlock::slabSize; i++) {
    TestingBlock::Limits limits  = tb.Add(new DummyV8MonkeyObject {});
    Object** addr {limits.objectAddress};

    V8MONKEY_CHECK(addr == prevAddr, "Object was placed at previous 'next' slot");

    prevAddr = limits.next;
  }
}


V8MONKEY_TEST(ObjectBlock010, "Up to slab size, values are allocated contiguously (3)") {
  TestingBlock tb {};

  TestingBlock::Limits oldLimits = tb.Add(new DummyV8MonkeyObject {});

  // Note: don't want to fill a block in this test
  for (auto i = 1u; i < TestingBlock::slabSize; i++) {
    TestingBlock::Limits limits = tb.Add(new DummyV8MonkeyObject {});
    V8MONKEY_CHECK(limits.limit == oldLimits.limit, "Remained in same block");
  }
}


V8MONKEY_TEST(ObjectBlock011, "More than <slabSize> values triggers fresh block allocation when required (1)") {
  TestingBlock tb {};
  TestingBlock::Limits oldLimits {nullptr, nullptr, nullptr};

  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    oldLimits = tb.Add(new DummyV8MonkeyObject {});
  }

  V8MONKEY_CHECK(oldLimits.next == oldLimits.limit, "Limit reached");
  TestingBlock::Limits limits = tb.Add(new DummyV8MonkeyObject {});
  V8MONKEY_CHECK(limits.limit != oldLimits.limit, "Moved to new block");
}


V8MONKEY_TEST(ObjectBlock012, "More than <slabSize> values triggers fresh block allocation when required (2)") {
  TestingBlock tb {};
  TestingBlock::Limits oldLimits {nullptr, nullptr, nullptr};

  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    oldLimits = tb.Add(new DummyV8MonkeyObject {});
  }

  TestingBlock::Limits limits = tb.Add(new DummyV8MonkeyObject {});
  Object** realPrevLimit {oldLimits.limit};
  Object** realLimit {limits.limit};
  // Note: Although unlikely, slabs themselves could be allocated contiguously
  V8MONKEY_CHECK((realLimit == realPrevLimit + TestingBlock::slabSize && limits.objectAddress == oldLimits.next) ||
                 (realLimit != realPrevLimit && limits.objectAddress != oldLimits.next), "Moved to new block");
}


V8MONKEY_TEST(ObjectBlock013, "Full deletion works as expected (1)") {
  TestingBlock tb {};
  tb.Add(new DummyV8MonkeyObject {});
  tb.Delete(nullptr);
  V8MONKEY_CHECK(tb.NumberOfItems() == 0, "Slot count correct");
}


V8MONKEY_TEST(ObjectBlock014, "Full deletion works as expected (2)") {
  TestingBlock tb {};
  bool wasDeleted {false};

  tb.Add(new DeletionObject {&wasDeleted});
  tb.Delete(nullptr);
  V8MONKEY_CHECK(wasDeleted, "Value was deleted");
}


V8MONKEY_TEST(ObjectBlock015, "Full deletion works as expected (3)") {
  TestingBlock tb {};
  tb.Add(new DummyV8MonkeyObject {});

  TestingBlock::Limits limits = tb.Delete(nullptr);
  V8MONKEY_CHECK(limits.next == nullptr, "Next field correct");
  V8MONKEY_CHECK(limits.limit == nullptr, "limit field correct");
}


V8MONKEY_TEST(ObjectBlock016, "Intra-block deletion works as expected (1)") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint = tb.Add(new DummyV8MonkeyObject {});

  static_assert(TestingBlock::slabSize >= 11, "This test makes bad assumptions about the slab size");
  for (auto i = 0u; i < 10u; i++) {
    tb.Add(new DummyV8MonkeyObject {});
  }

  tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(tb.NumberOfItems() == 1, "Slot count correct");
}


V8MONKEY_TEST(ObjectBlock017, "Intra-block deletion works as expected (2)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  TestingBlock tb {};
  bool firstObjectDeleted {false};
  bool wasDeleted[SLOTS];


  TestingBlock::Limits deletionPoint = tb.Add(new DeletionObject {&firstObjectDeleted});
  for (auto i = 0; i < SLOTS; i++) {
    tb.Add(new DeletionObject {&wasDeleted[i]});
    wasDeleted[i] = false;
  }

  tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(std::all_of(std::begin(wasDeleted), std::end(wasDeleted), [](bool& b) { return b; }),
                 "All objects that should have been deleted were");
  V8MONKEY_CHECK(!firstObjectDeleted, "Correct objects left untouched");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock018, "Intra-block deletion works as expected (3)") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint = tb.Add(new DummyV8MonkeyObject {});

  static_assert(TestingBlock::slabSize >= 11, "This test makes bad assumptions about the slab size");
  for (auto i = 0u; i < 10u; i++) {
    tb.Add(new DummyV8MonkeyObject {});
  }

  TestingBlock::Limits newLimits = tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(newLimits.next == deletionPoint.next, "Next field correct");
  V8MONKEY_CHECK(newLimits.limit == deletionPoint.limit, "Next field correct");
}


V8MONKEY_TEST(ObjectBlock019, "Intra-block deletion works as expected (4)") {
  // Tests the edge-case of deleting to the start of a block
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint = tb.Add(new DummyV8MonkeyObject {});

  static_assert(TestingBlock::slabSize >= 11, "This test makes bad assumptions about the slab size");
  for (auto i = 0u; i < 10u; i++) {
    tb.Add(new DummyV8MonkeyObject {});
  }

  TestingBlock::Limits newLimits = tb.Delete(deletionPoint.objectAddress);
  V8MONKEY_CHECK(newLimits.next == deletionPoint.objectAddress, "Next field correct");
  V8MONKEY_CHECK(newLimits.limit == deletionPoint.limit, "Next field correct");
}


V8MONKEY_TEST(ObjectBlock020, "Inter-block deletion works as expected (1)") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint = tb.Add(new DummyV8MonkeyObject {});

  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    tb.Add(new DummyV8MonkeyObject {});
  }

  tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(tb.NumberOfItems() == 1, "Slot count correct");
}


V8MONKEY_TEST(ObjectBlock021, "Inter-block deletion works as expected (2)") {
  TestingBlock tb {};
  bool firstObjectDeleted {false};
  bool wasDeleted[TestingBlock::slabSize];


  TestingBlock::Limits deletionPoint = tb.Add(new DeletionObject {&firstObjectDeleted});
  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    tb.Add(new DeletionObject {&wasDeleted[i]});
    wasDeleted[i] = false;
  }

  tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(std::all_of(std::begin(wasDeleted), std::end(wasDeleted), [](bool& b) { return b; }),
                 "All objects that should have been deleted were");
  V8MONKEY_CHECK(!firstObjectDeleted, "Correct objects left untouched");
}


V8MONKEY_TEST(ObjectBlock022, "Inter-block deletion works as expected (3)") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint = tb.Add(new DummyV8MonkeyObject {});

  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    tb.Add(new DummyV8MonkeyObject {});
  }

  TestingBlock::Limits newLimits = tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(newLimits.next == deletionPoint.next, "Next field correct");
  V8MONKEY_CHECK(newLimits.limit == deletionPoint.limit, "Limit field correct");
}


V8MONKEY_TEST(ObjectBlock023, "Inter-block deletion works as expected (4)") {
  // Tests the edge-case of deleting to the end of the previous block
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint;

  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    deletionPoint = tb.Add(new DummyV8MonkeyObject {});
  }

  tb.Add(new DummyV8MonkeyObject {});
  TestingBlock::Limits newLimits = tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(newLimits.next == deletionPoint.next, "Next field correct");
  V8MONKEY_CHECK(newLimits.limit == deletionPoint.limit, "Limit field correct");
}


V8MONKEY_TEST(ObjectBlock024, "Deleting to most-recent end is a no-op") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint {nullptr, nullptr, nullptr};

  static_assert(TestingBlock::slabSize >= 10, "This test makes bad assumptions about the slab size");
  for (auto i = 0u; i < 10u; i++) {
    deletionPoint = tb.Add(new DummyV8MonkeyObject {});
  }

  auto originalSlotCount = tb.NumberOfItems();
  TestingBlock::Limits newLimits = tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(tb.NumberOfItems() == originalSlotCount, "Slot count unchanged");
  V8MONKEY_CHECK(newLimits.next == deletionPoint.next, "Next field unchanged");
  V8MONKEY_CHECK(newLimits.limit == deletionPoint.limit, "Limit field unchanged");
}


V8MONKEY_TEST(ObjectBlock025, "Deletion copes with nullptrs") {
  TestingBlock tb {};
  DummyV8MonkeyObject* obj {new DummyV8MonkeyObject {}};
  TestingBlock::Limits deletionPoint = tb.Add(obj);
  Object** slotToEmpty = tb.Add(obj).objectAddress;
  // Manually delete the first dummy slot. Note: need to handle the refcount
  (*slotToEmpty)->Release(slotToEmpty);
  *slotToEmpty = nullptr;

  tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(true, "Didn't crash");
}


V8MONKEY_TEST(ObjectBlock026, "Objects are released on destruction (1)") {
  bool wasDeleted {false};

  {
    TestingBlock tb {};
    tb.Add(new DeletionObject {&wasDeleted});
  }

  V8MONKEY_CHECK(wasDeleted, "Object was deleted");
}


V8MONKEY_TEST(ObjectBlock027, "Objects are released on destruction (2)") {
  Object* obj {new DummyV8MonkeyObject {}};
  obj->AddRef();
  auto refCount = 0u;

  {
    TestingBlock tb {};
    tb.Add(obj);
    refCount = obj->RefCount();
  }

  V8MONKEY_CHECK(obj->RefCount() == refCount - 1, "Refcount adjusted correctly");

  for (auto i = obj->RefCount(); i > 0; i--) {
    obj->Release(nullptr);
  }
}


V8MONKEY_TEST(ObjectBlock028, "Iterating over an empty object block works") {
  TestingBlock tb {};

  tb.Iterate(objectVisitor);
  V8MONKEY_CHECK(true, "Didn't crash when iterating over empty");
}


V8MONKEY_TEST(ObjectBlock029, "Intra-block iteration works as expected (1)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  static_assert(TestingBlock::slabSize >= SLOTS, "This test makes bad assumptions about the slab size");
  TestingBlock tb {};

  for (auto i = 0; i < SLOTS; i++) {
    tb.Add(new IterateObject {i});
  }

  resetForIteration();
  tb.Iterate(objectVisitor);

  auto limit = std::begin(objectVisited);
  std::advance(limit, SLOTS);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock030, "Intra-block iteration works as expected (2)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  static_assert(TestingBlock::slabSize >= SLOTS + 1, "This test makes bad assumptions about the slab size");
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint;

  // The game here is to add another object, delete it, and confirm it wasn't iterated, but the survivors were
  for (auto i = 0; i < SLOTS; i++) {
    deletionPoint = tb.Add(new IterateObject {i});
  }

  tb.Add(new IterateObject {SLOTS});
  tb.Delete(deletionPoint.next);

  resetForIteration();
  tb.Iterate(objectVisitor);

  auto limit = std::begin(objectVisited);
  std::advance(limit, SLOTS);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  V8MONKEY_CHECK(!objectVisited[SLOTS], "Deleted object was not iterated");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock031, "Inter-block iteration works as expected (1)") {
  TestingBlock tb {};

  for (auto i = 0; i < iterationObjectMax; i++) {
    tb.Add(new IterateObject {i});
  }

  resetForIteration();
  tb.Iterate(objectVisitor);

  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), std::end(objectVisited), [](bool& b) { return b; }),
                 "All objects that should have been iterated were");
}


V8MONKEY_TEST(ObjectBlock032, "Inter-block iteration works as expected (2)") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint;

  // The game here is to add another object, delete it, and confirm it wasn't iterated, but the survivors were
  for (auto i = 0; i < iterationObjectMax - 1; i++) {
    deletionPoint = tb.Add(new IterateObject {i});
  }

  tb.Add(new IterateObject {iterationObjectMax - 1});
  tb.Delete(deletionPoint.next);

  resetForIteration();
  tb.Iterate(objectVisitor);

  auto limit = std::begin(objectVisited);
  std::advance(limit, iterationObjectMax - 1);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  V8MONKEY_CHECK(!objectVisited[iterationObjectMax - 1], "Deleted object was not iterated");
}


V8MONKEY_TEST(ObjectBlock033, "Iterating with data over an empty object block works") {
  TestingBlock tb {};

  void* objData {nullptr};
  tb.Iterate(objectDataVisitor, objData);
  V8MONKEY_CHECK(true, "Didn't crash when iterating over empty");
}


V8MONKEY_TEST(ObjectBlock034, "Intra-block iteration with data works as expected (1)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  static_assert(TestingBlock::slabSize >= SLOTS, "This test makes bad assumptions about the slab size");
  TestingBlock tb {};

  for (auto i = 0; i < SLOTS; i++) {
    tb.Add(new IterateObject {i});
  }

  resetForIteration();
  void* objData {reinterpret_cast<void*>(0xbeef)};
  tb.Iterate(objectDataVisitor, objData);

  auto limit = std::begin(objectVisited);
  std::advance(limit, SLOTS);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  auto limit2 = std::begin(objectVisitData);
  std::advance(limit2, SLOTS);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisitData), limit2, [&objData](void*& data) { return data == objData; }),
                 "All objects visited with correct data");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock035, "Intra-block iteration with data works as expected (2)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  static_assert(TestingBlock::slabSize >= SLOTS + 1, "This test makes bad assumptions about the slab size");
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint;

  // The game here is to add another object, delete it, and confirm it wasn't iterated, but the survivors were
  for (auto i = 0; i < SLOTS; i++) {
    deletionPoint = tb.Add(new IterateObject {i});
  }

  tb.Add(new IterateObject {SLOTS});
  tb.Delete(deletionPoint.next);

  resetForIteration();
  void* objData {reinterpret_cast<void*>(0xdead)};
  tb.Iterate(objectDataVisitor, objData);

  auto limit = std::begin(objectVisited);
  std::advance(limit, SLOTS);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  auto limit2 = std::begin(objectVisitData);
  std::advance(limit2, SLOTS);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisitData), limit2, [&objData](void*& data) { return data == objData; }),
                 "All objects visited with correct data");

  V8MONKEY_CHECK(!objectVisited[SLOTS], "Deleted object was not iterated");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock036, "Inter-block iteration with data works as expected (1)") {
  TestingBlock tb {};

  for (auto i = 0; i < iterationObjectMax; i++) {
    tb.Add(new IterateObject {i});
  }

  resetForIteration();
  void* objData {reinterpret_cast<void*>(0xcafe)};
  tb.Iterate(objectDataVisitor, objData);

  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), std::end(objectVisited), [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  V8MONKEY_CHECK(std::all_of(std::begin(objectVisitData), std::end(objectVisitData),
                 [&objData](void*& data) { return data == objData; }), "All objects visited with correct data");
}


V8MONKEY_TEST(ObjectBlock037, "Inter-block iteration with data works as expected (2)") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint;

  // The game here is to add another object, delete it, and confirm it wasn't iterated, but the survivors were
  for (auto i = 0; i < iterationObjectMax - 1; i++) {
    deletionPoint = tb.Add(new IterateObject {i});
  }

  tb.Add(new IterateObject {iterationObjectMax - 1});
  tb.Delete(deletionPoint.next);

  resetForIteration();
  void* objData {reinterpret_cast<void*>(0xf00d)};
  tb.Iterate(objectDataVisitor, objData);

  auto limit = std::begin(objectVisited);
  std::advance(limit, iterationObjectMax - 1);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  auto limit2 = std::begin(objectVisitData);
  std::advance(limit2, iterationObjectMax - 1);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisitData), limit2, [&objData](void*& data) { return data == objData; }),
                 "All objects visited with correct data");

  V8MONKEY_CHECK(!objectVisited[iterationObjectMax - 1], "Deleted object was not iterated");
}


V8MONKEY_TEST(ObjectBlock038, "Templated slabSize works as expected") {
  // VS2013 doesn't support constexpr
  #define SLOTS 10
  using Block = v8::DataStructures::ObjectBlock<SLOTS>;
  Block tb {};
  Block::Limits oldLimits {nullptr, nullptr, nullptr};

  for (auto i = 0u; i < SLOTS; i++) {
    oldLimits = tb.Add(new DummyV8MonkeyObject {});
  }

  Block::Limits limits = tb.Add(new DummyV8MonkeyObject {});
  V8MONKEY_CHECK(limits.limit != oldLimits.limit, "Moved to new block");
  #undef SLOTS
}
*/
