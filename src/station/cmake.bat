@echo off
echo "START"

set c_version="c++14"

echo "COMPILING SERVER LIBRARY..."

set include_path="C:/MinGW/boost/boost/include/boost-1_64"
set lib_path="C:/MinGW/boost/boost/lib"
set output_server_file="../../bin/server.o"
set lib_server_file="../../bin/drivers.a"
set cpp_files="server.cpp"
set libs="-lws2_32"

g++ -g -c -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_server_file% %cpp_files% %libs%
ar rvs %lib_server_file% %output_server_file%


echo "COMPILING MAIN PROGRAM..."

set include_path="C:/MinGW/boost/boost/include/boost-1_64"
set lib_path="C:/MinGW/boost/boost/lib"
set output_file="../../bin/station.exe"
set cpp_files="main.cpp"
set libs=%lib_server_file%
set libs2=%lib_rtos_file%
set libs3="-lws2_32"

g++ -g -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_file% %cpp_files% %libs% %libs2% %libs3%

echo "DONE"
