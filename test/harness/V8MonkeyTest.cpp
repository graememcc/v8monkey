// exception
#include <exception>

// cout
#include <iostream>

// string
#include <string>

// waitpid
#include <sys/wait.h>

// make_pair
#include <utility>

// fork
#include <unistd.h>

// Class definition
#include "V8MonkeyTest.h"

using namespace std;


// XXX Need to make sure filenames are basenames, both here and in run_v8monkey_tests
// XXX Do we want the containers to contain const V8MonkeyTest*

std::map<V8MonkeyTest::TestfileName, V8MonkeyTest::TestNames> V8MonkeyTest::testsByFileName;
std::map<V8MonkeyTest::TestName, V8MonkeyTest*> V8MonkeyTest::testsByName;


V8MonkeyTest::V8MonkeyTest(const char* file, const char* name, const char* desc, void (*testFunction)()) :
  fileName(file), testName(name), description(desc), test(testFunction) {
  testsByName.insert(make_pair(name, this));

  // There are two scenarios for the following operation: either we will insert a new set (on the first time we
  // encountered the given filename), or the insertion will fail, as we've encountered this filename before.
  //
  // Either way, the result of this expression will be a <string, set<string>> iterator, which we can dereference to
  // get at the set associated with the filename. We can then insert the test codename.
  auto filenameEntry = testsByFileName.insert(make_pair(file, set<TestName>())).first;
  filenameEntry->second.insert(name);
}


/*
 * Call the underlying test function. Returns true if the function completes without error, false otherwise
 *
 * Note that we fork here, and actually run the test in a separate process. Ideally, we want no shared state between
 * test invocations, however we are constrained by the fact that SpiderMonkey cannot be reinited after shutdown, and
 * the V8 API requirements regarding implicit initialization and the default isolate, which requires some work to be
 * carried out by static initializers. Thus, we aim to minimise the possibility of mutual dependencies in tests by
 * running each test in a new process.
 *
 */

// XXX Need to use Platform primitives for forking
V8MonkeyTest::TestResult V8MonkeyTest::Run() {
  pid_t processID {fork()};

  bool isChild {processID == 0};

  if (isChild) {
    // We are in the child process. Run the test
    cout << "Running " << GetFullDescription() << "... ";

    try {
      test();
      cout << "OK" << endl;
      return TestResult {true, true};
    } catch (exception& e) {
      cout << "ERROR " << e.what() << endl;
      return TestResult {false, true};
    }
  } else if (processID == -1) {
    // The fork failed. In the absence of any better ideas, report this as a test failure
    return TestResult {false, false};
  } else {
    int status;

    // Wait on the child
    waitpid(processID, &status, 0);

    // If the child exited abnormally, lets call that a failure
    if (!WIFEXITED(status)) {
      if (WIFSIGNALED(status)) {
        cout << "ERROR Crash" << endl;
      }
      return TestResult  {false, false};
    }

    // Otherwise, treat exit code == 0 as success, and anything else as failure
    return TestResult {WEXITSTATUS(status) == 0, false};
  }
}


/*
 * Return a string describing this test, suitable for output.
 *
 */

V8MonkeyTest::TestDescription V8MonkeyTest::GetFullDescription() {
  string s {"["};
  s += testName;
  s += "] ";
  s += description;
  return s;
}


/*
 * Print a list of registered tests, grouped by filename, to standard output.
 *
 * Each entry lists the tests "codename" in square brackets (which can be used to run that specific test) and a
 * description of the test
 *
 */

void V8MonkeyTest::ListAllTests() {
  if (testsByFileName.empty()) {
    cout << "No tests registered" << endl;
    return;
  }

  for (const auto& fileTests : testsByFileName) {
    cout << "Tests defined by file " << fileTests.first << ":" << endl;

    for (const auto& testName : fileTests.second) {
      // If there is an entry for a filename, we are guaranteed that the set of test names is not degenerate. Further,
      // we are guaranteed that every member of that set is a member of testsByName. Calling find on testsByName
      // cannot yield the end iterator.
      V8MonkeyTest* test {testsByName.find(testName)->second};
      cout << test->GetFullDescription() << endl;
    }

    cout << endl;
  }
}


/*
 * This simply prints the number of registered tests to stdout. It is designed to be used by Makefiles rather than humans
 *
 */

void V8MonkeyTest::CountTests() {
  cout << "Registered tests: " << testsByName.size() << endl;
}

/*
 * Run the test with a given name, or print an error to standard error if there is no such test. If the test fails, a
 * description of the test will be added to the given set aFailures.
 *
 */

V8MonkeyTest::TestResult V8MonkeyTest::RunNamedTest(const TestName& testName, ExecutedTests& testsExecuted, TestFailures& failures) {
  auto lookupResult = testsByName.find(testName);
  auto notFound = testsByName.end();

  if (lookupResult == notFound) {
    cerr << "Cannot run test named " << testName << " - no such test" << endl;
    failures.insert(string("[") + testName + string("] <Test not found>"));
    return TestResult {false, false};
  }

  TestResult result = lookupResult->second->Run();
  testsExecuted.insert(testName);

  if (!result.processIsChild && result.failed) {
    failures.insert(lookupResult->second->GetFullDescription());
  }

  return result;
}


/*
 * Run all the tests for a given filename, warning on error to standard error if the given filename is not known to the
 * test harness, and add the codename of each test ran to the given set.
 *
 */

V8MonkeyTest::TestResult V8MonkeyTest::RunTestsForFile(const string& fileName, ExecutedTests& testsExecuted, TestFailures& failures) {
  auto testsForFile = testsByFileName.find(fileName);
  auto notFound = testsByFileName.end();

  // Handle an unknown file
  if (testsForFile == notFound) {
    cerr << "No tests found for file " << fileName << endl;
    return TestResult {false, false};
  }

  TestResult result;

  for (auto& testName : testsForFile->second) {
    result = RunNamedTest(testName, testsExecuted, failures);
    if (!result.processIsChild) {
      testsExecuted.insert(testName);
    }

    if (result.processIsChild) {
      return result;
    }
  }

  return result;
}


/*
 * Execute all tests known to the test harness.
 *
 */

V8MonkeyTest::TestResult V8MonkeyTest::RunAllTests(TestFailures& failures) {
  if (testsByFileName.empty()) {
    cout << "No tests registered" << endl;
    return TestResult {false, false};
  }

  ExecutedTests testsExecuted {};
  TestResult result;

  for (auto& file : testsByFileName) {
    for (auto& test : file.second) {
      result = RunNamedTest(test, testsExecuted, failures);
      if (result.processIsChild) {
        return result;
      }
    }
  }

  // Signal to the caller that we're the parent process
  return result;
}
