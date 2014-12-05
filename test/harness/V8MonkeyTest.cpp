#include <map>
#include <set>
#include <utility>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <exception>
#include <unistd.h>
#include <sys/wait.h>


#include "V8MonkeyTest.h"


using namespace std;


// XXX Need to make sure filenames are basenames, both here and in run_v8monkey_tests
// XXX Do we want the containers to contain const V8MonkeyTest*

map<string, set<string>> V8MonkeyTest::sTestsByFileName;
map<string, V8MonkeyTest*> V8MonkeyTest::sTestsByName;


V8MonkeyTest::V8MonkeyTest(const char* aFileName, const char* aCodeName, const char* aDescription, void (*aTestFunction)()):
  mFileName(aFileName),
  mCodeName(aCodeName),
  mDescription(aDescription),
  mTestFunction(aTestFunction)
{
  sTestsByName.insert(make_pair(aCodeName, this));

  // There are two scenarios for the following operation: either we will insert a new set (on the first time we
  // encountered the given filename), or the insertion will fail, as we've encountered this filename before.
  //
  // Either way, the result of this expression will be a <string, set<string>> iterator, which we can dereference to
  // get at the set associated with the filename. We can then insert the test codename.
  map<string, set<string>>::iterator filenameEntry = sTestsByFileName.insert(make_pair(aFileName, set<string>())).first;
  filenameEntry->second.insert(aCodeName);
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

bool
V8MonkeyTest::Run()
{
  pid_t processID = fork();

  if (processID == 0) {
    // We are in the child process. Run the test
    cout << "Running " << GetFullDescription() << "... ";

    try {
      mTestFunction();
      cout << "OK" << endl;
      exit(0);
    } catch (exception& e) {
      cout << "ERROR " << e.what() << endl;
      exit(1);
    }
  } else if (processID == -1) {
    // The fork failed. In the absence of any better ideas, report this as a test failure
    return false;
  } else {
    int status;

    // Wait on the child
    waitpid(processID, &status, 0);

    // If the child exited abnormally, lets call that a failure
    if (!WIFEXITED(status)) {
      return false;
    }

    // Otherwise, treat exit code == 0 as success, and anything else as failure
    return WEXITSTATUS(status) == 0;
  }
}


/*
 * Return a string describing this test, suitable for output.
 *
 */
string
V8MonkeyTest::GetFullDescription()
{
  stringstream s;
  s << "[" << mCodeName << "] " << mDescription;
  return s.str();
}


/*
 * Print a list of registered tests, grouped by filename, to standard output.
 *
 * Each entry lists the tests "codename" in square brackets (which can be used to run that specific test) and a
 * description of the test
 *
 */

void
V8MonkeyTest::ListAllTests()
{
  map<string, set<string>>::const_iterator filenameIterator = sTestsByFileName.cbegin();
  map<string, set<string>>::const_iterator fileEnd = sTestsByFileName.cend();

  // Deal with the degenerate case
  if (filenameIterator == fileEnd) {
    cout << "No tests registered" << endl;
    return;
  }

  while (filenameIterator != fileEnd) {
    cout << "Tests defined by file " << filenameIterator->first << ":" << endl;

    // If there is an entry for a filename, we are guaranteed that the set is not degenerate; it must contain at least
    // one entry. Likewise, we are guaranteed that the set members exist in sTestsByName. Calling find on sTestsByName
    // will not yield the end iterator.

    set<string>::const_iterator testIterator = filenameIterator->second.cbegin();
    set<string>::const_iterator testEnd = filenameIterator->second.end();

    while (testIterator != testEnd) {
      V8MonkeyTest* test = sTestsByName.find(*testIterator)->second;
      cout << test->GetFullDescription() << endl;
      testIterator++;
    }

    cout << endl;
    filenameIterator++;
  }
}


/*
 * Run the test with a given name, or print an error to standard error if there is no such test. If the test fails, a
 * description of the test will be added to the given set aFailures.
 *
 */

void
V8MonkeyTest::RunTestByName(const string& aTestName, set<string>& aFailures)
{
  map<string, V8MonkeyTest*>::iterator lookupResult = sTestsByName.find(aTestName);

  if (lookupResult == sTestsByName.end()) {
    cerr << "Cannot run test named " << aTestName << " - no such test" << endl;
    aFailures.insert(string("[") + aTestName + string("] <Test not found>"));
    return;
  }

  bool result = lookupResult->second->Run();
  if (!result) {
    aFailures.insert(lookupResult->second->GetFullDescription());
  }
}


/*
 * Run all the tests for a given filename, warning on error to standard error if the given filename is not known to the
 * test harness, and add the codename of each test ran to the given set.
 *
 */

void
V8MonkeyTest::RunTestsForFile(const string& aFileName, set<string>& aTestsRan, set<string>& aFailures)
{
  map<string, set<string>>::const_iterator testsForFileIterator = sTestsByFileName.find(aFileName);
  map<string, set<string>>::const_iterator fileEnd = sTestsByFileName.cend();

  // Handle an unknown file
  if (testsForFileIterator == fileEnd) {
    cerr << "No tests found for file " << aFileName << endl;
    return;
  }

  // If there is an entry for a filename, we are guaranteed that the set is not degenerate; it must contain at least
  // one entry.
  set<string>::const_iterator testIterator = testsForFileIterator->second.cbegin();
  set<string>::const_iterator testEnd = testsForFileIterator->second.end();

  while (testIterator != testEnd) {
    RunTestByName(*testIterator, aFailures);
    aTestsRan.insert(*testIterator);
    testIterator++;
  }
}


/*
 * Execute all tests known to the test harness.
 *
 */

void
V8MonkeyTest::RunAllTests(set<string>& aFailures)
{
  map<string, set<string>>::const_iterator filename = sTestsByFileName.cbegin();
  map<string, set<string>>::const_iterator fileEnd = sTestsByFileName.cend();

  // Deal with the degenerate case
  if (filename == fileEnd) {
    cout << "No tests registered" << endl;
    return;
  }

  while (filename != fileEnd) {
    // If there is an entry for a filename, we are guaranteed that the set is not degenerate; it must contain at least
    // one entry.

    set<string>::const_iterator test = filename->second.cbegin();
    set<string>::const_iterator testEnd = filename->second.end();

    while (test != testEnd) {
      RunTestByName(*test, aFailures);
      test++;
    }

    filename++;
  }
}