// iostream
#include <iostream>

// set
#include <set>

// string
#include <string>

// Unit-testing support
#include "V8MonkeyTest.h"


using namespace std;


using TestName = V8MonkeyTest::TestName;
using ContainingFiles = set<V8MonkeyTest::ContainingFile>;
using TestNames = V8MonkeyTest::TestNames;
using ExecutedTests = V8MonkeyTest::ExecutedTests;
using TestFailures = V8MonkeyTest::TestFailures;


enum ArgRequest {Usage, TestList, RegisteredTestCount, RunByFile, RunByName, RunAll, None, BadArg, FileError, NameError};

struct ArgParseResult {
  ArgRequest request1 {None};
  ArgRequest request2 {None};
  ContainingFiles filenames {};
  TestNames testnames {};
  string badArg {};

  ArgParseResult() {}

  bool UsageRequested() const { return request1 == Usage || request2 == Usage; }
  bool TestListRequested() const { return request1 == TestList || request2 == TestList; }
  bool RegisteredTestCountRequested() const { return request1 == RegisteredTestCount || request2 == RegisteredTestCount; }
  bool RunByFileRequested() const { return request1 == RunByFile || request2 == RunByFile; }
  bool RunByNameRequested() const { return request1 == RunByName || request2 == RunByName; }
  bool RunAllRequested() const { return request1 == RunAll || request2 == RunAll; }
  bool NothingRequested() const { return request1 == None && request2 == None; }
  bool BadRequestMade() const { return request1 == BadArg || request2 == BadArg; }
  bool FileNameErrorMade() const { return request1 == FileError || request2 == FileError; }
  bool TestNameErrorMade() const { return request1 == NameError || request2 == NameError; }

  bool canRequestTestList() const { return !UsageRequested(); };
  bool canRequestTestCount() const { return !UsageRequested() && !TestListRequested(); };
  bool canRequestByFileOrName() const { return !UsageRequested() && !TestListRequested() && !RegisteredTestCountRequested(); }

  void setTo(ArgRequest r) { request1 = request2 = r; filenames.clear(); testnames.clear(); }
  void setTo(ArgRequest r1, ArgRequest r2) { request1 = r1; request2 = r2; }
  void setError(string s) { badArg = s; request1 = request2 = BadArg; filenames.clear(); testnames.clear(); }
  void setError(ArgRequest r) { request1 = request2 = r; filenames.clear(); testnames.clear(); }
  void setNext(ArgRequest r) { if (request1 == None) { request1 = r; } else { request2 = r; } }

  void addFile(char* fileName) { filenames.emplace(fileName); }
  void addTest(string testName) { testnames.insert(testName); }
};


/*
 * Takes a string. If the first character is a opening square bracket, then it is removed, and the last character is
 * then inspected to check if it is a closing square bracket, in which case it is likewise removed.
 * Otherwise, the string is returned unchanged.
 *
 */

TestName stripLeadingTrailingSquareBrackets(string original) {
  if (original.front() != '[' || original.back() != ']') {
    return TestName {original};
  }

  return TestName {original, 1, original.size() - 2};
}


/*
 * Parse the command line arguments, returning an ArgParseResult sruct denoting the requested actions.
 *
 * As soon as an error occurs, parsing terminates.
 *
 * A help request overrides all other options: when encountered, parsing stops immediately.
 * A test listing request overrides test count, file and name options: when encountered, all later requests other than help will be ignored.
 * A test count request overrides file and name options: when encountered, all later requests other than help and test listing will be ignored.
 *
 * Specific file and specific test requests are NOT considered mutually exclusive. Thus a user can request all the tests
 * from some files, and additional tests from other files.
 *
 */

ArgParseResult parseArgs(int argc, char** argv) {
  ArgParseResult result;

  for (auto i = 1; i < argc; i++) {
    const string arg {argv[i]};

    if (arg.front() != '-') {
      result.setError(arg);
      return result;
    }

    // If we find a help request, all other command line arguments are ignored
    if (arg == "-h" || arg == "--help") {
      result.setTo(Usage);
// Note that we continue to check the command line so we can report any errors
      continue;
    }

    if ((arg == "-l" || arg == "--list") && result.canRequestTestList()) {
      result.setTo(TestList);
      continue;
    }

    if ((arg == "-c" || arg == "--count") && result.canRequestTestCount()) {
      result.setTo(RegisteredTestCount);
      continue;
    }

    if ((arg == "-f" || arg == "--file") && result.canRequestByFileOrName()) {
      // For the file switch, read the name of the file, erroring out if it's not present
      i++;

      if (i == argc || argv[i][0] == '-') {
        result.setError(FileError);
        return result;
      }

      result.addFile(argv[i]);
      result.setNext(RunByFile);
      continue;
    }

    if ((arg == "-n" || arg == "--name") && result.canRequestByFileOrName()) {
      // For the name switch, read the name of the test, erroring out if it's not present
      i++;

      if (i == argc || argv[i][0] == '-') {
        result.setError(NameError);
        return result;
      }

      // Strip square brackets if the user copied them from a test listing, and record the requested test
      result.addTest(stripLeadingTrailingSquareBrackets(argv[i]));
      result.setNext(RunByName);
      continue;
    }

    result.setError(argv[i]);
    return result;
  }

  if (result.NothingRequested()) {
    result.setNext(RunAll);
  }

  return result;
}


/*
 * Print usage information to standard output, after printing any supplied error message to standard error.
 * Returns a suggested exit code, which is 0 if no error message was printed, and 1 otherwise.
 *
 */

int usage(const string& progName, const char* errorMessage = nullptr) {
  const bool errorSupplied {errorMessage && errorMessage[0] != '\0'};

  if (errorSupplied) {
    cerr << errorMessage << endl;
    cout << "Usage:" << endl << endl;
  }

  cout << progName << ": run V8Monkey testsuite" << endl;
  cout << "OPTIONS" << endl;
  cout << "-c or --count: Print the number of registered tests" << endl;
  cout << "-f [FILE] or --file [FILE]: Run all the tests from the given file" << endl;
  cout << "-h or --help: Print this help and exit" << endl;
  cout << "-l or --list: List all registered tests" << endl;
  cout << "-n [NAME] or --name [NAME]: Run the test with the given name ";
  cout << "(in listings, the test name is printed between square brackets)" << endl;
  cout << endl;

  return errorSupplied ? 1 : 0;
}


/*
 * Run all the tests for each file in aFileNames, adding output to delineate the different files. While doing so,
 * add the codename of each test ran to aTestsRan, tracking any failures in aFailures.
 *
 */
// XXX Can we indent test output?

V8MonkeyTest::TestResult runTestsByFile(const ContainingFiles& fileNames, ExecutedTests executedTests, TestFailures& testFailures) {
  V8MonkeyTest::TestResult result;
  for (const auto& filename : fileNames) {
    cout << "Running tests from file " << filename << ": " << endl;
    result = V8MonkeyTest::RunTestsForFile(filename, executedTests, testFailures);

    // If we forked, there is no need for the child (which was spawned to run one specific test) to continue iteration
    if (result.processIsChild) {
      return result;
    }

    cout << endl;
  }

  return result;
}


/*
 * Run all the tests named in the given set, unless the test name is present in the given set of tests already executed.
 *
 */

V8MonkeyTest::TestResult runTestsByName(const TestNames& testNames, ExecutedTests& executedTests,
                                        TestFailures& testFailures) {
  auto notExecuted = executedTests.end();

  // For aesthetic purposes
  bool hasPreviousOutput {executedTests.size() > 0};
  bool hasTriggeredOutput {false};
  V8MonkeyTest::TestResult result;

  for (auto& test : testNames) {
    // Ensure that if a named test has already been executed when we ran all the tests from a particular file, then
    // don't run it again
    if (executedTests.find(test) == notExecuted) {
      if (hasPreviousOutput && !hasTriggeredOutput) {
        cout << endl;
        hasTriggeredOutput = true;
      }

      result = V8MonkeyTest::RunNamedTest(test, executedTests, testFailures);

      // If we forked, there is no need for the child (which was spawned to run one specific test) to continue iteration
      if (result.processIsChild) {
        return result;
      }
    }
  }

  return result;
}


/*
 * Describe the tests that failed on standard output
 *
 */

void reportFailures(const TestFailures& testFailures) {
  const auto failureCount = testFailures.size();

  cout << failureCount << " test failure";
  if (failureCount > 1) {
    cout << "s";
  }
  cout << ":" << endl;

  for (const auto& message : testFailures) {
    cout << message << endl;
  }
}


/*
 * Extract the basename for a file from a C string, by returning the substring starting from the character after the
 * last slash (/) character. If no slashes are present, the original string will be returned.
 *
 */

string getBaseName(char* argv0) {
  string name {argv0};
  auto slash = name.rfind('/');
  auto diff = slash == string::npos ? 0 : slash + 1;

  if (diff != 0) {
    auto newSize = name.size() - diff;
    name.replace(0, newSize, name, diff, newSize);
    name.resize(newSize);
  }

  return name;
}


int main(int argc, char** argv) {
  using TestResult =  V8MonkeyTest::TestResult;

  const string progName {getBaseName(argv[0])};
  const auto result = parseArgs(argc, argv);

  if (result.UsageRequested()) {
    return usage(progName);
  } else if (result.TestListRequested()) {
    V8MonkeyTest::ListAllTests();
    return 0;
  } else if (result.RegisteredTestCountRequested()) {
    V8MonkeyTest::CountTests();
    return 0;
  } else if (result.FileNameErrorMade()) {
    return usage(progName, "Missing argument for -f/--file option");
  } else if (result.TestNameErrorMade()) {
    return usage(progName, "Missing argument for -n/--name option");
  } else if (result.BadRequestMade()) {
    return usage(progName, (string("Unrecognised option: ") + result.badArg).c_str());
  }

  // Track test execution, to avoid running a test more than once
  ExecutedTests executedTests {};

  // Also track test failures
  TestFailures testFailures {};

  if (result.RunByFileRequested()) {
    TestResult testResult = runTestsByFile(result.filenames, executedTests, testFailures);

    // Child processes created to run a specific test should exit now
    if (testResult.processIsChild) {
      return testResult.failed ? 1 : 0;
    }
  }

  if (result.RunByNameRequested()) {
    TestResult testResult = runTestsByName(result.testnames, executedTests, testFailures);
    if (testResult.processIsChild) {
      return testResult.failed ? 1 : 0;
    }
  }

  if (result.RunAllRequested()) {
    TestResult testResult = V8MonkeyTest::RunAllTests(testFailures);
    if (testResult.processIsChild) {
      return testResult.failed ? 1 : 0;
    }
  }

  if (!testFailures.empty()) {
    cout << endl;
    reportFailures(testFailures);
  }

  return static_cast<int>(testFailures.size());
}
