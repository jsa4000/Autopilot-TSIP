@echo off
echo "START"

:: Enable Functionality to vc++ 14
set c_version="c++14"
:: Boost library path
set boost_path="C:/MinGW/boost"
::set boost_path="C:/jsantos/Software/MinGW/boost/boost
:: Boost C++ Library compiled x64 intel
set include_boost_path=%boost_path%/include/boost-1_64"
set lib_boost_path=%boost_path%/lib"
:: Library to load the Sockets. Always at the end of g++ compiling
set lib_sockets="ws2_32"

:: Check if the bin folder doesn't exist yet
if not exist "bin" mkdir "bin"

echo "COMPILING SERVER LIBRARY..."

set output_server_file="bin/server.o"
set lib_drivers_file="bin/drivers.a"
set cpp_files="src/station/server/server.cpp"

g++ -g -c -std=%c_version% -pthread -I%include_boost_path% -L%lib_boost_path% -o%output_server_file% %cpp_files% -l%lib_sockets% 
ar rvs %lib_drivers_file% %output_server_file%

echo "COMPILING MAIN PROGRAM..."

set output_file="bin/station.exe"
set cpp_files="src/station/main.cpp"

g++ -g -std=%c_version% -pthread -I%include_boost_path% -L%lib_boost_path% -o%output_file% %cpp_files% %lib_drivers_file% -l%lib_sockets% 

echo "DONE"
