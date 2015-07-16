#ifndef V8MONKEY_V8MONKEYTEST_H
#define V8MONKEY_V8MONKEYTEST_H

/*
// map
#include <map>

// std::logic_error
#include <stdexcept>

// set
#include <set>

// string
#include <string>
*/


/*
 * Here, we essentially follow the strategy of Cctest in V8's source tree, defining a class that wraps a test function,
 * and build a linked-list of such tests, with the head of the linked-list a static pointer member of the test class.
 *
 */

/*
class V8MonkeyTest {
  public:
    using TestDescription = std::string;
    using TestfileName = std::string;
    using TestName = std::string;
    using TestNames = std::set<TestName>;
    using ExecutedTests = std::set<TestName>;
    using TestFailures = std::set<TestName>;


    struct TestResult {
      bool failed;
      bool processIsChild;
    };


    V8MonkeyTest(const char* file, const char* name, const char* desc, void (*testFunction)());

    // Return the test's filename
    TestfileName GetFileName() { return fileName; }

    // Return the test's "codename"
    TestName GetName() { return testName; }

    // Return the test's description
    TestDescription GetDescription() { return description; }

    // Return a description of the test, suitable for displaying to standard output
    TestDescription GetFullDescription();

    // Run the test function wrapped by this test (does not traverse the linked list). Returns true if the test
    // without error, false otherwise
    TestResult Run();

    // Prints to stdout the names of all registered tests
    static void ListAllTests();

    // Prints a count of the number of registered tests to stdout
    static void CountTests();

    // Run all the tests for a given filename, warning on error to standard error if there are no such registered tests.
    // Fills the given set aFileName with the codenames of the tests ran, and the set aFailures with messages describing
    // any tests that failed.
    static TestResult RunTestsForFile(const TestfileName& fileName, ExecutedTests& testsExecuted, TestFailures& failures);

    // Run the test with a given name, or print an error to standard error if there is no such test. If the test fails,
    // a description of the test is added to the set aFailures.
    static TestResult RunNamedTest(const TestName& testName, ExecutedTests& testsExecuted, TestFailures& failures);

    // Execute all tests known to the test harness, filling the given set with descriptions of the test failures
    static TestResult RunAllTests(TestFailures& failures);

  private:
    // A mapping from filenames to the names of the tests they contain
    static std::map<TestfileName, TestNames> testsByFileName;

    // A mapping from test codenames to the relevant tests
    static std::map<TestName, V8MonkeyTest*> testsByName;

    // Note we convert the char*'s to C++ strings to ensure membership tests compare contents, not pointers
    TestfileName fileName;
    TestName testName;
    TestDescription description;
    void (*test)();
};
*/


/*
 * Many tests define an auxillary function which performs the bulk of the test's work and returns a single value
 * cast to void*, allowing the function to be both called from the main thread and supplied as the start procedure
 * when creating a child thread. This macro can be used to define such procedures, giving them the correct signature,
 * and setting up default arguments, so that one doesn't need to explicitly supply a nullptr value.
 *
 */

#ifndef V8MONKEY_TEST_HELPER
#define V8MONKEY_TEST_HELPER(name) void* name(void* arg = nullptr); \
void* name(void* arg)
#endif


/*
 * The macro for registering tests with the test harness. This macro:
 *   1) Declares the test function name, to allocate a pointer to it
 *   2) Creates a V8MonkeyTest object wrapping the function (construction of which adds the function to the list
 *      of tests)
 *   3) Defines the test function
 *
 * Note this macro definition gives us uniqueness of codenames for "free", as the codename is used to form the
 * definition of the test function; 2 tests with the same codename will generate the usual redefinition of a function
 * error.
 *
 */

#ifndef V8MONKEY_TEST
#define V8MONKEY_TEST(name, description) \
   static void Test##name(); \
   V8MonkeyTest test##name(__FILE__, #name, description, &Test##name); \
   static void Test##name()
#endif


#ifndef V8MONKEY_STRINGIFY_MACRO
#define V8MONKEY_STRINGIFY_MACRO(x) #x
#endif


#ifndef V8MONKEY_STRINGIFY_EXPANSION
#define V8MONKEY_STRINGIFY_EXPANSION(x) V8MONKEY_STRINGIFY_MACRO(x)
#endif


/*
 * A simple assertion macro. Condition is assumed to evaluate to a truthy value, and if it fails, a std::logic_error is
 * thrown. The test harness catches such errors to report test failures. Be mindful of stack unwinding behaviour: this
 * macro should only be used in the main thread, not child threads!
 *
 */

#ifndef V8MONKEY_CHECK
#define V8MONKEY_CHECK(condition, description) \
  do { \
    if (!(condition)) { \
      throw std::logic_error("Check at " __FILE__ " line " V8MONKEY_STRINGIFY_EXPANSION(__LINE__) " (" description ") failed "); \
    } \
  } while (0);
#endif

#endif
