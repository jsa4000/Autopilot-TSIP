# Autopilot-TSIP
Autopilot-TSIP Packet Parser Simulator


1. Requirements

- MinGW and CMake C++ compiler libraries
- Visual studio Code (with Microsoft C/C++, CMake and Native Debug extensions)


1.1 Configuration

  - CMakeLists.txt

      cmake_minimum_required(VERSION 3.0)

      project(hello-vsc)

      set(SOURCE hello-vsc.cpp)

      add_executable(${PROJECT_NAME} ${SOURCE})


  - tasks.json 

      {
          "version": "0.1.0",
          "command": "g++",
          "isShellCommand": true,
          // compiles and links with debugger information
          "args": ["-g", "-o", "hello.exe", "hello.cpp"],
          // without debugger information
          // "args": ["-o", "hello.exe", "hello.cpp"],
          "showOutput": "always"
      }

  - launch.json 

      {
          "version": "0.2.0",
          "configurations": [
              {
                  "name": "C++ Launch (Windows)",
                  "type": "cppdbg",
                  "request": "launch",
                  "program": "${workspaceRoot}/bin/tsip.exe",
                  "MIMode": "gdb",
                  "miDebuggerPath": "C:\\MinGw\\bin\\gdb.exe",
                  "stopAtEntry": false,
                  "cwd": "${workspaceRoot}",
                  "externalConsole": false,
                  "visualizerFile": "${workspaceRoot}/my.natvis"
              }
          ]
      }


2. Autopilot TSIP



3. References

>> https://code.visualstudio.com/docs/languages/cpp
>> http://stephencoakley.com/2015/01/21/guide-setting-up-a-simple-c-development-environment-on-windows
>> https://medium.com/audelabs/c-development-using-visual-studio-code-cmake-and-lldb-d0f13d38c563
