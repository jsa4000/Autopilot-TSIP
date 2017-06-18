# Autopilot-TSIP

Autopilot-TSIP Packet Parser Simulator

## 1 Requirements

- GCC or MinGW C++ compiler libraries (x64)
    > "-std=c++0x" support -pthread (x64-4.8.1-release-posix-sjlj-rev5.7z or x86_64-7.1.0-release-posix-sjlj-rt_v5-rev0)
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

- keybindings.json

```json
 { "key": "f8",          "command": "workbench.action.tasks.build" }
```

## 2 Autopilot-TSIP

### 2.1 Terminology

Some concepts need to be claryfied prior start talking about the current Architecture used in the Exercise and Tests.

- RTOS

    A real-time operating system (RTOS) is an operating system (OS) intended to serve real-time applications that process data as it comes in, typically without buffer delays. Processing time requirement (including any OS delay) are measured in tenths of seconds or shorter increments of time. They either are event driven or time sharing.
    Event driven systems switch between tasks based on their priorities while time sharing systems switch the task based on clock interrupts.

-DMA

    Direct memory access (DMA) is a means of having a peripheral device control a processor's memory bus directly. DMA permits 
    the peripheral, such as a UART, to transfer data directly to or from memory without having each byte (or word) handled by 
    the processor. Thus DMA enables more efficient use of interrupts, increases data throughput, and potentially reduces hardware 
    costs by eliminating the need for peripheral-specific FIFO buffers.

- Preemption

    In computing, preemption is the act of temporarily interrupting a task being carried out by a computer system, without requiring its 
    cooperation, and with the intention of resuming the task at a later time. Such changes of the executed task are known as context switches.
    It is normally carried out by a privileged task or part of the system known as a preemptive scheduler, which has the power to preempt, or 
    interrupt, and later resume, other tasks in the system.

### 2.2 Test-Driven Development (TDD)

Test-driven development (TDD) is a software development process that relies on the repetition of a very short development cycle: requirements are turned into very specific test cases, then the software is improved to pass the new tests, only. This is opposed to software development that allows software to be added that is not proven to meet requirements.

- The TDD Cycle: Red-Green-Refactor

When doing TDD, you repeat a short cycle of the following:

 1 - Write a test (“red”).
 2 - Get the test to pass (“green”).
 3 - Optimize the design (“refactor”).

This cycle is often summarized as red-green-refactor, a phrase that stems from unit testing tools used for TDD. Red (fail) and pass (green) derive from SUnit (the first unit testing tool designed to support TDD) and similar GUI tools that use the colors to provide immediate feedback on test results. The text-based output of Google Mock uses red and green when executed in a terminal with color support.

### 2.3 Implementation



## 3 References

>> https://code.visualstudio.com/docs/languages/cpp
>> http://stephencoakley.com/2015/01/21/guide-setting-up-a-simple-c-development-environment-on-windows
>> https://medium.com/audelabs/c-development-using-visual-studio-code-cmake-and-lldb-d0f13d38c563
