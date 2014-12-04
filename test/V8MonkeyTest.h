#ifndef V8MONKEY_V8MONKEYTEST_H
#define V8MONKEY_V8MONKEYTEST_H 

#include <stdlib.h>
#include <stdexcept>
#include <map>
#include <set>
#include <string>


/*
 * Here, we essentially follow the strategy of Cctest in V8's source tree, defining a class that wraps a test function,
 * and build a linked-list of such tests, with the head of the linked-list a static pointer member of the test class.
 *
 */

class V8MonkeyTest {
  public:
    V8MonkeyTest(const char* aFileName, const char* aCodeName, const char* aDescription, void (*aTestFunction)());

    // Return the test's filename
    std::string GetFileName() { return mFileName; }

    // Return the test's "codename"
    std::string GetCodeName() { return mCodeName; }

    // Return the test's description
    std::string GetDescription() { return mDescription; }

    // Return a description of the test, suitable for displaying to standard output
    std::string GetFullDescription();

    // Run the test function wrapped by this test (does not traverse the linked list). Returns true if the test
    // without error, false otherwise
    bool Run();

    // Prints to stdout the names of all registered tests
    static void ListAllTests();

    // Run all the tests for a given filename, warning on error to standard error if there are no such registered tests.
    // Fills the given set aFileName with the codenames of the tests ran, and the set aFailures with messages describing
    // any tests that failed.
    static void RunTestsForFile(const std::string& aFileName, std::set<std::string>& aTestsRan,
                                                              std::set<std::string>& aFailures);

    // Run the test with a given name, or print an error to standard error if there is no such test. If the test fails,
    // a description of the test is added to the set aFailures.
    static void RunTestByName(const std::string& aTestName, std::set<std::string>& aFailures);

    // Execute all tests known to the test harness, filling the given set with descriptions of the test failures
    static void RunAllTests(std::set<std::string>& aFailures);

  private:
    // A mapping from filenames to the names of the tests they contain
    static std::map<std::string, std::set<std::string>> sTestsByFileName;

    // A mapping from test codenames to the relevant tests
    static std::map<std::string, V8MonkeyTest*> sTestsByName;

    // Note we convert the char*'s to C++ strings to ensure membership tests compare contents, not pointers
    std::string mFileName;
    std::string mCodeName;
    std::string mDescription;
    void (*mTestFunction)();
};


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


#ifndef V8MONKEY_CHECK
#define V8MONKEY_CHECK(condition, description) \
  do { \
    if (!(condition)) { \
      throw std::logic_error("Check at " __FILE__ " line " V8MONKEY_STRINGIFY_EXPANSION(__LINE__) " (" description ") failed "); \
    } \
  } while (0);
#endif

#endif
