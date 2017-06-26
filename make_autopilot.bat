@echo off
echo "START"

:: Enable Functionality to vc++ 14
set c_version="c++14"

:: Check if the bin folder doesn't exist yet
if not exist "bin/" mkdir "bin"

echo "COMPILING DEVICES LIBRARY..."

set output_com_file="bin/com.o"
set cpp_com_file="src/autopilot/devices/com.cpp"
set output_tsip_file="bin/tsip.o"
set cpp_tsip_file="src/autopilot/devices/tsip.cpp"
set lib_devices_file="bin/devices.a"

g++ -g -c -std=%c_version% -pthread -o%output_com_file% %cpp_com_file%
g++ -g -c -std=%c_version% -pthread -o%output_tsip_file% %cpp_tsip_file%
ar rvs %lib_devices_file% %output_com_file% %output_tsip_file%

echo "COMPILING RTOS LIBRARY..."

set include_path="src/autopilot"
set output_rtos_file="bin/rtos.o"
set output_scheduler_file="bin/scheduler.o"
set output_task_file="bin/task.o"
set cpp_rtos_file="src/autopilot/rtos/rtos.cpp"
set cpp_scheduler_file="src/autopilot/rtos/scheduler.cpp"
set cpp_task_file="src/autopilot/rtos/task.cpp"
set lib_rtos_file="bin/rtos.a"

g++ -g -c -std=%c_version% -pthread -I%include_path% -o%output_rtos_file% %cpp_rtos_file% %lib_devices_file%
g++ -g -c -std=%c_version% -pthread -I%include_path% -o%output_scheduler_file% %cpp_scheduler_file% %lib_devices_file%
g++ -g -c -std=%c_version% -pthread -I%include_path% -o%output_task_file% %cpp_task_file% %lib_devices_file%
ar rvs %lib_rtos_file% %output_queue_file% %output_rtos_file% %output_scheduler_file% %output_task_file%

echo "COMPILING MAIN PROGRAM..."

set output_file="bin/autopilot.exe"
set cpp_files="src/autopilot/main.cpp"

g++ -g -std=%c_version% -pthread -I%include_path% -o%output_file% %cpp_files% %lib_rtos_file% %lib_devices_file%

echo "DONE"
