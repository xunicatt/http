-- TEST
A custom python script for automated testing.
This script tests the functionality of the library as a single unit.

** To use this testing script YOU MUST install 'http' system-wide or in such a way
   that 'pkg-config' can locate it.
** This test script looks up for environment variable 'CXX' to get the compiler.
   YOU MUST export CXX in your environment, example for bash: "export CXX=g++".

-- How to add a Test?
+ Each test directory should be named as: test<number>/
  It must contain the following files:

    - test<number>.cc
      The main C++ source file for the test.

    - curl.txt
      This file contains curl request arguments.
      Each line represents a separate curl command's arguments.

    - ans.txt
      This file contains the expected output for each curl request.
      Each line corresponds to the expected output of the respective request.

+ Add the new "test<number>/" directory to the 'path' list in test.py
  - If the new test's answers are in json format then add the test in 'isjson' dictionary too
+ To clean executables, run: rm *.out

-- How to Run?
> python test.py
        or
> python test.py test<n1> test<n2> ...

-- Notes:
+ Make sure curl.txt and ans.txt have the same number of lines.
+ The script matches each curl output line-by-line with ans.txt
