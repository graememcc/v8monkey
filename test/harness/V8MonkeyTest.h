#ifndef V8MONKEY_V8MONKEYTEST_H
#define V8MONKEY_V8MONKEYTEST_H

// map
#include <map>

// std::logic_error
#include <stdexcept>

// set
#include <set>

// string
#include <string>


/*
 * Here, we essentially follow the strategy of Cctest in V8's source tree, defining a class that wraps a test function,
 * together with its meta-information. A macro is provided for test files to announce their tests, which will
 * construct objects belonging to the wrapper class, which has the side-effect of registering them. Unfortunately, this
 * can make things sensitive to compilation order.
 *
 */

class V8MonkeyTest {
  public:
    using TestDescription = std::string;
    using ContainingFile = std::string;
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
    ContainingFile GetFileName() const { return fileName; }

    // Return the test's "codename"
    TestName GetName() const { return testName; }

    // Return the test's description
    TestDescription GetDescription() const { return description; }

    // Return a description of the test, suitable for displaying to standard output
    TestDescription GetFullDescription() const;

    // Run the test function wrapped by this object. Returns true if the test completed without error, false otherwise
    TestResult Run() const;

    // Prints the names of all registered tests to stdout
    static void ListAllTests();

    // Prints the number of registered tests to stdout
    static void CountTests();

    // Run all the tests for a given filename; if there are no tests registered for the given filename, display a
    // warning on standard error. The given set aFileName will be filled with the codenames of the tests ran, and the
    // set aFailures filled with messages describing any tests that failed. The value returned is a TestResult
    // structure, to allow the harness's child threads to unwind their callstack to main correctly.
    static TestResult RunTestsForFile(const ContainingFile& fileName, ExecutedTests& testsExecuted,
                                            TestFailures& failures);

    // Run the test with a given name, or print an error on standard error if there is no such test. If the test fails,
    // a description of the test is added to the set aFailures. The value returned is a TestResult structure, to allow
    // the harness's child threads to unwind their callstack to main correctly.
    static TestResult RunNamedTest(const TestName& testName, ExecutedTests& testsExecuted,
                                         TestFailures& failures);

    // Execute all tests known to the test harness, filling the given set with descriptions of the test failures. A
    // TestResult is returned, to allow child processes to exit correctly.
    static TestResult RunAllTests(TestFailures& failures);

  private:
    // A mapping from filenames to the names of the tests they contain
    static std::map<const ContainingFile, TestNames> testsByFileName;

    // A mapping from test codenames to the relevant tests
    static std::map<const TestName, const V8MonkeyTest *const> testsByName;

    // We wish to compare contents, not pointers, so convert the incoming char*s to C++ strings.
    ContainingFile fileName;
    TestName testName;
    TestDescription description;
    void (*test)();
};


/*
 * Some tests, particularly those relating to isolates define an auxillary function which performs the bulk of the
 * test's work, so that the behaviour can be tested both on the main thread and child threads (with the auxillary
 * function being supplied as the function to be run by the spawned thread). This macro can be used to define such
 * procedures, giving them the correct signature, and setting up default arguments, so that one doesn't need to
 * explicitly supply a nullptr value.
 *
 */

#ifndef V8MONKEY_TEST_HELPER
#define V8MONKEY_TEST_HELPER(name) extern "C" void* name(void* arg = nullptr); \
void* name(void* arg)
#endif


/*
 * The macro for registering tests with the test harness. This macro:
 *   1) Declares the test function name, so that we may declare and allocate a pointer to it
 *   2) Creates a V8MonkeyTest object wrapping the function (construction of which adds the function to the list of
 *      registered tests)
 *   3) Defines the test function
 *
 * Note this macro definition gives us uniqueness of codenames for "free", as the codename is used to form the
 * definition of the test function; two tests with the same codename will generate the usual redefinition of a function
 * error.
 *
 */

#ifndef V8MONKEY_TEST
#define V8MONKEY_TEST(name, description) \
   static void Test##name(); \
   const V8MonkeyTest test##name(__FILE__, #name, description, &Test##name); \
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
