#include <string>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <set>


#include "V8MonkeyTest.h"


// Used to create the bitmask denoting the result of command-line parsing
#define USAGE_REQUESTED 1
#define LIST_REQUESTED 2
#define FILE_REQUEST 4
#define TEST_REQUEST 8
#define FILE_ERROR 16
#define NAME_ERROR 32
#define UNEXPECTED_VALUE 64


using namespace std;


/*
 * Takes a C-style string. If the first character is a opening square bracket, then it is removed, and the last
 * character is then inspected to check if it is a closing square bracket, in which case it is likewise removed.
 * Otherwise, the string is returned unchanged.
 *
 */
static char*
stripLeadingTrailingSquareBrackets(char* aOriginal)
{
  if (*aOriginal != '[') {
    return aOriginal;
  }

  aOriginal++;
  char* c = aOriginal;

  while (*c) c++;

  if (c > aOriginal && *(c - 1) == ']') {
    *(c - 1) = '\0';
  }

  return aOriginal;
}


/*
 * Parse the command line arguments, returning a bitmask denoting whether any of the following occurred:
 *   If bit 0 set: the user requested help
 *   If bit 1 set: the user requested a listing of all registered tests
 *   If bit 2 set: the user requested all the tests in one or more files be executed (the given filenames set will
 *                 contain all such files)
 *   If bit 3 set: the user requested one or more specific tests be executed (the given testnames set will contain
 *                 the codenames of the specified tests)
 *   If bit 4 set: the user supplied the switch for a specific file, but failed to supply the file
 *   If bit 5 set: the user supplied the switch for a specific test, but failed to supply the name of the test
 *   If bit 6 set: an unexpected value was supplied (eg a filename without the preceding -f switch)
 *
 * Thus a value of 0 represents the default action: all registered tests should be executed
 *
 * If an error occurs, (ie bit 4 or bit 5 is set) then no more argument parsing occurs, and we return immediately.
 *
 * A help request overrides all other options: if it is found, no more arguments will be parsed, and we return
 * immediately.
 *
 * If help is not requested, but a test listing is requested, then the value returned will ignore any file and test
 * options that were supplied (although the sets may have been modified).
 *
 * Specific file and specific test requests are NOT considered mutually exclusive. Thus a user can request all the
 * tests from some files, and additional tests from other files.
 *
 */

short
parseArgs(int argc, char** argv, set<string> &aFilenames, set<string> &aTestNames)
{
  short bitmask = 0;

  int i;
  for (i = 1; i < argc; i++) {
    char* arg = argv[i];

    if (arg[0] != '-') {
      return UNEXPECTED_VALUE;
    }

    // If we find a help request, all other command line arguments are ignored
    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
      return USAGE_REQUESTED;
    }

    // If we find a listing request, all other command line arguments (except help requests, which take
    // precedence) are ignored
    if (strcmp(arg, "-l") == 0 || strcmp(arg, "--list") == 0) {
      bitmask |= LIST_REQUESTED;
      continue;
    }

    if (strcmp(arg, "-f") == 0 || strcmp(arg, "--file") == 0) {
      // For the file switch, read the name of the file, erroring out if it's not present
      i++;

      if (i == argc || argv[i][0] == '-') {
        return FILE_ERROR;
      }

      bitmask |= FILE_REQUEST;
      aFilenames.insert(argv[i]);
      continue;
    }

    if (strcmp(arg, "-n") == 0 || strcmp(arg, "--name") == 0) {
      // For the name switch, read the name of the test, erroring out if it's not present
      i++;

      if (i == argc || argv[i][0] == '-') {
        return NAME_ERROR;
      }

      bitmask |= TEST_REQUEST;
      // Strip square brackets if the user copied them from a test listing, and record the requested test
      aTestNames.insert(stripLeadingTrailingSquareBrackets(argv[i]));
      continue;
    }

    return UNEXPECTED_VALUE;
  }

  return bitmask;
}


/*
 * Print usage information to standard output, after printing any supplied error message to standard error.
 * Returns a suggested exit code, which is 0 if no error message was printed, and 1 otherwise.
 *
 */

int
usage(const char* progName, const char* errorMessage = nullptr)
{
  bool errorSupplied = errorMessage && *errorMessage != '\0';

  if (errorSupplied) {
    cerr << errorMessage << endl;
    cout << "Usage:" << endl << endl;
  }

  cout << progName << ": run V8Monkey testsuite" << endl;
  cout << "OPTIONS" << endl;
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

void
runTestsByFile(const set<string>& aFileNames, set<string>& aTestsRan, set<string>& aFailures)
{
  // This is just a simple matter of iterating over the list of filenames
  set<string>::const_iterator filename = aFileNames.cbegin();
  set<string>::const_iterator end = aFileNames.cend();

  while (filename != end) {
    cout << "Running tests for file: " << *filename << endl;
    V8MonkeyTest::RunTestsForFile(*filename, aTestsRan, aFailures);
    filename++;
    if (filename != end) {
      cout << endl;
    }
  }
}


/*
 * Run all the tests named in the given set, unless the test name is present in the given set of tests already ran.
 *
 */

void
runTestsByName(const set<string>& aTestNames, const set<string>& aTestsRan, set<string>& aFailures)
{
  set<string>::const_iterator test = aTestNames.cbegin();
  set<string>::const_iterator lastTest = aTestNames.cend();
  set<string>::const_iterator notRan = aTestsRan.end();

  // For aesthetic purposes
  bool hadOutputAlready = aTestsRan.size() > 0;
  bool selfTriggeredOutput = false;

  while (test != lastTest) {
    // Ensure that if a named test has already been executed when we ran all the tests from a particular file, then
    // don't run it again
    if (aTestsRan.find(*test) == notRan) {
      if (hadOutputAlready && !selfTriggeredOutput) {
        cout << endl;
        selfTriggeredOutput = true;
      }

      V8MonkeyTest::RunTestByName(*test, aFailures);
    }

    test++;
  }
}


/*
 * Describe the tests that failed on standard output
 *
 */
void
reportFailures(const set<string>& aFailures)
{
  size_t failureCount = aFailures.size();

  cout << failureCount << " test failure";
  if (failureCount > 1) {
    cout << "s";
  }
  cout << ":" << endl;

  set<string>::const_iterator message = aFailures.cbegin();
  set<string>::const_iterator lastMessage = aFailures.cend();

  while (message != lastMessage) {
    cout << *message << endl;
    message++;
  }
}


/*
 * Extract the basename for a file from a C string, by returning the substring starting from the character after the
 * last slash (/) character. If no slashes are present, the original string will be returned.
 *
 */

char*
getBaseName(char* aFilePath)
{
  char* c = aFilePath;
  char* lastSlash = aFilePath - 1;
  while (*c) {
    if (*c == '/') lastSlash = c;
    c++;
  }

  return lastSlash + 1;
}


int
main(int argc, char** argv)
{
  char* progName = getBaseName(argv[0]);

  set<string> filenames;
  set<string> testnames;

  short result = parseArgs(argc, argv, filenames, testnames);

  if (result & USAGE_REQUESTED) {
    exit(usage(progName));
  } else if (result & LIST_REQUESTED) {
    V8MonkeyTest::ListAllTests();
    exit(0);
  } else if (result & FILE_ERROR) {
    exit(usage(progName, "Missing argument for -f/--file option"));
  } else if (result & NAME_ERROR) {
    exit(usage(progName, "Missing argument for -n/--name option"));
  } else if (result & UNEXPECTED_VALUE) {
    exit(usage(progName, "Unrecognised option in command line"));
  }
  
  // Compile the sets of tests ran to avoid duplication
  set<string> testsRan;

  // Also track test failures
  set<string> testFailures;

  if (result & FILE_REQUEST) {
    runTestsByFile(filenames, testsRan, testFailures);
  }

  if (result & TEST_REQUEST) {
    runTestsByName(testnames, testsRan, testFailures);
  }

  if (!result) {
    V8MonkeyTest::RunAllTests(testFailures);
  }

  if (testFailures.size() > 0) {
    cout << endl;
    reportFailures(testFailures);
  }

  return 0;
}
