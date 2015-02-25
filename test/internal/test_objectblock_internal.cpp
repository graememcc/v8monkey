// all_of
#include <algorithm>

// Class under test
#include "data_structures/objectblock.h"

// begin, end
#include <iterator>

// shared_ptr
#include <memory>

// DeletionObject
#include "types/base_types.h"

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace v8::internal;


namespace {
  class IterateObject;


  const int iterationObjectBlockSize {v8::DataStructures::ObjectBlock<IterateObject>::slabSize};
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
  class IterateObject {
    public:
      IterateObject(int n) { index = n; }
      ~IterateObject() {}
      int index;
  };


  void objectVisitor(std::shared_ptr<IterateObject> i) {
    objectVisited[i->index] = true;
  }


  void objectDataVisitor(std::shared_ptr<IterateObject> i, void* data) {
    objectVisited[i->index] = true;
    objectVisitData[i->index] = data;
  }
}


using namespace v8::DataStructures;


// Convenience
using TestingBlock = ObjectBlock<unsigned int>;


V8MONKEY_TEST(ObjectBlock001, "Number of objects initially zero") {
  TestingBlock tb {};
  V8MONKEY_CHECK(tb.NumberOfItems() == 0, "Correct value returned");
}


V8MONKEY_TEST(ObjectBlock002, "Number of objects correct up to slab size") {
  TestingBlock tb {};

  for (auto i = 1u; i <= TestingBlock::slabSize; i++) {
    tb.Add(new unsigned int {});
    V8MONKEY_CHECK(tb.NumberOfItems() == i, "Correct value returned");
  }
}


V8MONKEY_TEST(ObjectBlock003, "Number of objects correct adding when adding more than slab size") {
  TestingBlock tb {};

  for (auto i = 1u; i <= TestingBlock::slabSize + 1; i++) {
    tb.Add(new unsigned int {});
  }

  V8MONKEY_CHECK(tb.NumberOfItems() == TestingBlock::slabSize + 1, "Correct value returned");
}


V8MONKEY_TEST(ObjectBlock004, "Returned free slots not null") {
  TestingBlock tb{};

  TestingBlock::Limits limits = tb.Add(new unsigned int {});
  V8MONKEY_CHECK(limits.next != nullptr, "Next not null");
}


V8MONKEY_TEST(ObjectBlock005, "Returned free slots not null") {
  TestingBlock tb{};

  TestingBlock::Limits limits = tb.Add(new unsigned int {});
  V8MONKEY_CHECK(limits.limit != nullptr, "Next not null");
}


V8MONKEY_TEST(ObjectBlock006, "Adding values moves next pointer") {
  TestingBlock tb {};

  TestingBlock::Limits oldLimits = tb.Add(new unsigned int {});
  for (auto i = 1u; i <= TestingBlock::slabSize + 1; i++) {
    TestingBlock::Limits limits = tb.Add(new unsigned int {});
    V8MONKEY_CHECK(limits.next != oldLimits.next, "Next pointer moved");

    // I'm conflicted as to whether we should test next == oldLimits.next + 1 for i up to slabSize, as while it's true
    // in the default case, it won't be when a LimitPtr is specified
  }
}


V8MONKEY_TEST(ObjectBlock007, "<slabSize> values allocated within same block") {
  TestingBlock tb {};

  TestingBlock::Limits oldLimits = tb.Add(new unsigned int {});
  // Note: don't want to fill a block in this test
  for (auto i = 1u; i < TestingBlock::slabSize; i++) {
    TestingBlock::Limits limits = tb.Add(new unsigned int {});
    V8MONKEY_CHECK(limits.limit == oldLimits.limit, "Remained in same block");
  }
}


V8MONKEY_TEST(ObjectBlock008, "More than <slabSize> values allocated in fresh blocks") {
  TestingBlock tb {};
  TestingBlock::Limits oldLimits {nullptr, nullptr};

  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    oldLimits = tb.Add(new unsigned int {});
  }

  TestingBlock::Limits limits = tb.Add(new unsigned int {});
  V8MONKEY_CHECK(limits.limit != oldLimits.limit, "Moved to new block");
}


V8MONKEY_TEST(ObjectBlock009, "Full deletion works as expected (1)") {
  TestingBlock tb {};
  tb.Add(new unsigned int {});
  tb.Delete(nullptr);
  V8MONKEY_CHECK(tb.NumberOfItems() == 0, "Slot count correct");
}


V8MONKEY_TEST(ObjectBlock010, "Full deletion works as expected (2)") {
  using Block = ObjectBlock<DeletionObject>;
  Block block {};
  bool wasDeleted {false};

  block.Add(new DeletionObject {&wasDeleted});
  block.Delete(nullptr);
  V8MONKEY_CHECK(wasDeleted, "Value was deleted");
}


V8MONKEY_TEST(ObjectBlock011, "Intra-block deletion works as expected (1)") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint = tb.Add(new unsigned int {});

  static_assert(TestingBlock::slabSize >= 11, "This test makes bad assumptions about the slab size");
  for (auto i = 0u; i < 10u; i++) {
    tb.Add(new unsigned int {});
  }

  tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(tb.NumberOfItems() == 1, "Slot count correct");
}


V8MONKEY_TEST(ObjectBlock012, "Intra-block deletion works as expected (2)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  using Block = ObjectBlock<DeletionObject>;
  Block block {};
  bool firstObjectDeleted {false};
  bool wasDeleted[SLOTS];


  Block::Limits deletionPoint = block.Add(new DeletionObject {&firstObjectDeleted});
  for (auto i = 0; i < SLOTS; i++) {
    block.Add(new DeletionObject {&wasDeleted[i]});
    wasDeleted[i] = false;
  }

  block.Delete(deletionPoint.next);
  V8MONKEY_CHECK(std::all_of(std::begin(wasDeleted), std::end(wasDeleted), [](bool& b) { return b; }),
                 "All objects that should have been deleted were");
  V8MONKEY_CHECK(!firstObjectDeleted, "Correct objects left untouched");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock013, "Inter-block deletion works as expected (1)") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint = tb.Add(new unsigned int {});

  for (auto i = 0u; i < TestingBlock::slabSize; i++) {
    tb.Add(new unsigned int {});
  }

  tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(tb.NumberOfItems() == 1, "Slot count correct");
}


V8MONKEY_TEST(ObjectBlock014, "Inter-block deletion works as expected (2)") {
  using Block = ObjectBlock<DeletionObject>;
  Block block {};
  bool firstObjectDeleted {false};
  bool wasDeleted[Block::slabSize];


  Block::Limits deletionPoint = block.Add(new DeletionObject {&firstObjectDeleted});
  for (auto i = 0u; i < Block::slabSize; i++) {
    block.Add(new DeletionObject {&wasDeleted[i]});
    wasDeleted[i] = false;
  }

  block.Delete(deletionPoint.next);
  V8MONKEY_CHECK(std::all_of(std::begin(wasDeleted), std::end(wasDeleted), [](bool& b) { return b; }),
                 "All objects that should have been deleted were");
  V8MONKEY_CHECK(!firstObjectDeleted, "Correct objects left untouched");
}


V8MONKEY_TEST(ObjectBlock015, "Deleting to most-recent end is a no-op") {
  TestingBlock tb {};
  TestingBlock::Limits deletionPoint {nullptr, nullptr};

  static_assert(TestingBlock::slabSize >= 10, "This test makes bad assumptions about the slab size");
  for (auto i = 0u; i < 10u; i++) {
    deletionPoint = tb.Add(new unsigned int {});
  }

  auto originalSlotCount = tb.NumberOfItems();
  tb.Delete(deletionPoint.next);
  V8MONKEY_CHECK(tb.NumberOfItems() == originalSlotCount, "Slot count unchanged");
}


V8MONKEY_TEST(ObjectBlock016, "Templated slabSize works as expected") {
  // VS2013 doesn't support constexpr
  #define SLOTS 10
  using Block = ObjectBlock<unsigned int, std::shared_ptr<unsigned int>*, SLOTS>;
  Block block {};
  Block::Limits oldLimits {nullptr, nullptr};

  for (auto i = 0u; i < SLOTS; i++) {
    oldLimits = block.Add(new unsigned int {});
  }

  Block::Limits limits = block.Add(new unsigned int {});
  V8MONKEY_CHECK(limits.limit != oldLimits.limit, "Moved to new block");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock017, "Iterating over an empty object block works") {
  using Block = ObjectBlock<IterateObject>;
  Block block {};

  block.Iterate(objectVisitor);
  V8MONKEY_CHECK(true, "Didn't crash when iterating over empty");
}


V8MONKEY_TEST(ObjectBlock018, "Intra-block iteration works as expected (1)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  using Block = ObjectBlock<IterateObject>;
  static_assert(Block::slabSize >= SLOTS, "This test makes bad assumptions about the slab size");
  Block block {};

  for (auto i = 0; i < SLOTS; i++) {
    block.Add(new IterateObject {i});
  }

  resetForIteration();
  block.Iterate(objectVisitor);

  auto limit = std::begin(objectVisited);
  std::advance(limit, SLOTS);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock019, "Intra-block iteration works as expected (2)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  using Block = ObjectBlock<IterateObject>;
  static_assert(Block::slabSize >= SLOTS + 1, "This test makes bad assumptions about the slab size");
  Block block {};
  Block::Limits deletionPoint;

  // The game here is to add another object, delete it, and confirm it wasn't iterated, but the survivors were
  for (auto i = 0; i < SLOTS; i++) {
    deletionPoint = block.Add(new IterateObject {i});
  }

  block.Add(new IterateObject {SLOTS});
  block.Delete(deletionPoint.next);

  resetForIteration();
  block.Iterate(objectVisitor);

  auto limit = std::begin(objectVisited);
  std::advance(limit, SLOTS);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  V8MONKEY_CHECK(!objectVisited[SLOTS], "Deleted object was not iterated");
  #undef SLOTS
}


V8MONKEY_TEST(ObjectBlock020, "Inter-block iteration works as expected (1)") {
  using Block = ObjectBlock<IterateObject>;
  Block block {};

  for (auto i = 0; i < iterationObjectMax; i++) {
    block.Add(new IterateObject {i});
  }

  resetForIteration();
  block.Iterate(objectVisitor);

  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), std::end(objectVisited), [](bool& b) { return b; }),
                 "All objects that should have been iterated were");
}


V8MONKEY_TEST(ObjectBlock021, "Inter-block iteration works as expected (2)") {
  using Block = ObjectBlock<IterateObject>;
  Block block {};
  Block::Limits deletionPoint;

  // The game here is to add another object, delete it, and confirm it wasn't iterated, but the survivors were
  for (auto i = 0; i < iterationObjectMax - 1; i++) {
    deletionPoint = block.Add(new IterateObject {i});
  }

  block.Add(new IterateObject {iterationObjectMax - 1});
  block.Delete(deletionPoint.next);

  resetForIteration();
  block.Iterate(objectVisitor);

  auto limit = std::begin(objectVisited);
  std::advance(limit, iterationObjectMax - 1);
  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), limit, [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  V8MONKEY_CHECK(!objectVisited[iterationObjectMax - 1], "Deleted object was not iterated");
}


V8MONKEY_TEST(ObjectBlock022, "Iterating with data over an empty object block works") {
  using Block = ObjectBlock<IterateObject>;
  Block block {};

  void* objData {nullptr};
  block.Iterate(objectDataVisitor, objData);
  V8MONKEY_CHECK(true, "Didn't crash when iterating over empty");
}


V8MONKEY_TEST(ObjectBlock023, "Intra-block iteration with data works as expected (1)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  using Block = ObjectBlock<IterateObject>;
  static_assert(Block::slabSize >= SLOTS, "This test makes bad assumptions about the slab size");
  Block block {};

  for (auto i = 0; i < SLOTS; i++) {
    block.Add(new IterateObject {i});
  }

  resetForIteration();
  void* objData {reinterpret_cast<void*>(0xbeef)};
  block.Iterate(objectDataVisitor, objData);

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


V8MONKEY_TEST(ObjectBlock024, "Intra-block iteration with data works as expected (2)") {
  // VS2013 doesn't implement constexpr
  #define SLOTS 10

  using Block = ObjectBlock<IterateObject>;
  static_assert(Block::slabSize >= SLOTS + 1, "This test makes bad assumptions about the slab size");
  Block block {};
  Block::Limits deletionPoint;

  // The game here is to add another object, delete it, and confirm it wasn't iterated, but the survivors were
  for (auto i = 0; i < SLOTS; i++) {
    deletionPoint = block.Add(new IterateObject {i});
  }

  block.Add(new IterateObject {SLOTS});
  block.Delete(deletionPoint.next);

  resetForIteration();
  void* objData {reinterpret_cast<void*>(0xdead)};
  block.Iterate(objectDataVisitor, objData);

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


V8MONKEY_TEST(ObjectBlock025, "Inter-block iteration with data works as expected (1)") {
  using Block = ObjectBlock<IterateObject>;
  Block block {};

  for (auto i = 0; i < iterationObjectMax; i++) {
    block.Add(new IterateObject {i});
  }

  resetForIteration();
  void* objData {reinterpret_cast<void*>(0xcafe)};
  block.Iterate(objectDataVisitor, objData);

  V8MONKEY_CHECK(std::all_of(std::begin(objectVisited), std::end(objectVisited), [](bool& b) { return b; }),
                 "All objects that should have been iterated were");

  V8MONKEY_CHECK(std::all_of(std::begin(objectVisitData), std::end(objectVisitData),
                 [&objData](void*& data) { return data == objData; }), "All objects visited with correct data");
}


V8MONKEY_TEST(ObjectBlock026, "Inter-block iteration with data works as expected (2)") {
  using Block = ObjectBlock<IterateObject>;
  Block block {};
  Block::Limits deletionPoint;

  // The game here is to add another object, delete it, and confirm it wasn't iterated, but the survivors were
  for (auto i = 0; i < iterationObjectMax - 1; i++) {
    deletionPoint = block.Add(new IterateObject {i});
  }

  block.Add(new IterateObject {iterationObjectMax - 1});
  block.Delete(deletionPoint.next);

  resetForIteration();
  void* objData {reinterpret_cast<void*>(0xf00d)};
  block.Iterate(objectDataVisitor, objData);

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
