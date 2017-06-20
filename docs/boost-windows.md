# Installing boost libraries for GCC (MinGW) on Windows

## Folder setup
1. Extract downloaded boost source, e.g. `C:\MinGW\boost\boost_1_64_0`.
2. Create a folder for Boost.Build installation, e.g. `C:\MinGW\boost\boost-build`.
2. Create a folder within for building, i.e. `C:\MinGW\boost\boost_1_64_0\build`.
3. Create a folder for installation, e.g. `C:\MinGW\boost\boost`.

## GCC setup
1. Open Command Prompt.
2. Run `g++ --version`.
3. If the output contains g++ version number then GCC should be set up properly to run from command line and you can continue.

## Boost.Build setup
1. Open Command Prompt and navigate to `C:\MinGW\boost\boost_1_64_0\tools\build`.
2. Run `bootstrap.bat mingw` or `bootstrap.bat gcc` or `bootstrap.bat` .
3. Run `b2 install --prefix="C:\MinGW\boost\boost-build"`.
4. Add `C:\MinGW\boost\boost-build\bin` to Windows PATH.
> rundll32 sysdm.cpl,EditEnvironmentVariables

## boost building
1. Open Command Prompt and navigate to `C:\MinGW\boost\boost_1_64_0`.
2. Run
```
b2 --build-dir="C:\MinGW\boost\boost_1_64_0\build" --prefix="C:\MinGW\boost\boost" toolset=gcc install
```

## Project setup
1. Add include folder, i.e. `C:\MinGW\boost\include\boost-1_59`.
2. Add linker folder, i.e. `C:\MinGW\boost\lib`.
3. Link required libraries, e.g. `libboost_regex-mgw48-mt-1_59.a`.

4. Example, e.g. g++ -Wall -g -L`C:\MinGW\boost\lib` -I`C:\MinGW\boost\include\boost-1_59` `class.spp` `libboost_regex-mgw48-mt-1_59.a`

## Visual Studio Code

1. configuration needed in to compile using MinGW

```json

{
    "version": "0.1.0",
    "command": "g++",
    "isShellCommand": true,
    // Compiles and links with default compilation file
    //"args": ["-g", "src\\main.cpp"],
    // Compiles and links with debugger information
    "args": ["-g", "-std=c++11",
             "-I", "C:/MinGW/boost/boost/include/boost-1_64", 
             "-L", "C:/MinGW/boost/boost/lib", 
             "-o", "bin\\tsip.exe",  
             "src\\tsip.cpp", "src\\main.cpp"],
    // Without debugger information
    // "args": ["-o", "bin\\tsip.exe", "src\\main.cpp"],
    "showOutput": "always"
}

```

2. Example using lambda from Boost library

```c

#include <boost/lambda/lambda.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>
#include "tsip.h"

using namespace std;

void tsip::print() {
     cout << "Print Function" <<  endl; 
}

void tsip::test_boost() {
    using namespace boost::lambda;
    typedef istream_iterator<int> in;

    cout << "Type in any number: ";

    std::for_each(
        in(cin), in(),cout 
                << (_1 * 10) 
                << "\nType in another number: " );

}




```