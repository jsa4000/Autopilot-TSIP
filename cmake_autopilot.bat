@echo off
echo "START"

:: Enable Functionality to vc++ 14
set c_version="c++14"
:: Boost C++ Library compiled x64 intel
set include_boost_path="C:/MinGW/boost/boost/include/boost-1_64"
set lib_boost_path="C:/MinGW/boost/boost/lib"
:: Library to load the Sockets. Always at the end of g++ compiling
set lib_sockets="ws2_32"

echo "COMPILING DEVICES LIBRARY..."

set output_client_file="bin/client.o"
set lib_devices_file="bin/devices.a"
set cpp_files="src/autopilot/devices/client.cpp"

g++ -g -c -std=%c_version% -pthread -I%include_boost_path% -L%lib_boost_path% -o%output_client_file% %cpp_files% -l%lib_sockets%
ar rvs %lib_devices_file% %output_client_file%

echo "COMPILING RTOS LIBRARY..."

set include_path="src/autopilot"
set output_rtos_file="bin/rtos.o"
set output_tsip_file="bin/tsip.o"
set output_task_file="bin/task.o"
set cpp_files="src/autopilot/rtos/tsip.cpp"
set cpp_files2="src/autopilot/rtos/rtos.cpp"
set cpp_files3="src/autopilot/rtos/task.cpp"
set lib_rtos_file="bin/rtos.a"

g++ -g -c -std=%c_version% -pthread -I%include_path% -I%include_boost_path% -L%lib_boost_path% -o%output_tsip_file% %cpp_files% %lib_devices_file% -l%lib_sockets%
g++ -g -c -std=%c_version% -pthread -I%include_path% -I%include_boost_path% -L%lib_boost_path% -o%output_rtos_file% %cpp_files2% %lib_devices_file% -l%lib_sockets%
g++ -g -c -std=%c_version% -pthread -I%include_path% -I%include_boost_path% -L%lib_boost_path% -o%output_task_file% %cpp_files3% %lib_devices_file% -l%lib_sockets%
ar rvs %lib_rtos_file% %output_tsip_file% %output_rtos_file% %output_task_file%

echo "COMPILING MAIN PROGRAM..."

set output_file="bin/tsip.exe"
set cpp_files="src/autopilot/main.cpp"

g++ -g -std=%c_version% -pthread -I%include_boost_path% -I%include_path% -L%lib_boost_path% -o%output_file% %cpp_files% %lib_rtos_file% %lib_devices_file% -l%lib_sockets%

echo "DONE"
