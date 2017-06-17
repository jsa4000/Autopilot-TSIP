# Autopilot-TSIP
Autopilot-TSIP Packet Parser Simulator


1. Requirements

- MinGW C++ compiler libraries (x64)
> "-std=c++11" support -pthread (x64-4.8.1-release-posix-sjlj-rev5.7z)
- Visual studio Code (with Microsoft C/C++, CMake and Native Debug extensions)
- Boost libraries. (Compiled using gcc)



1.1 Configuration

  - tasks.json

```json

{
    "version": "0.1.0",
    "command": "g++",
    "isShellCommand": true,
    // Compiles and links with default compilation file
    //"args": ["-g", "src\\main.cpp"],
    // Compiles and links with debugger information
    "args": ["-g", "-std=c++11", "-pthread",
             "-I", "C:/MinGW/boost/boost/include/boost-1_64", 
             "-L", "C:/MinGW/boost/boost/lib", 
             "-o", "bin\\tsip.exe",  
             "src\\tsip.cpp", "src\\main.cpp"],
    // Without debugger information
    // "args": ["-o", "bin\\tsip.exe", "src\\main.cpp"],
    "showOutput": "always"
}
```

  - launch.json
 
```json
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

```

2. Autopilot TSIP



3. References

>> https://code.visualstudio.com/docs/languages/cpp
>> http://stephencoakley.com/2015/01/21/guide-setting-up-a-simple-c-development-environment-on-windows
>> https://medium.com/audelabs/c-development-using-visual-studio-code-cmake-and-lldb-d0f13d38c563
