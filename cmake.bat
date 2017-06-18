@echo off
echo "START"

set c_version="c++14"

echo "COMPILE DMA LIBRARY"

set include_path="C:/MinGW/boost/boost/include/boost-1_64"
set lib_path="C:/MinGW/boost/boost/lib"
set output_file="bin/dma.o"
set lib_dma_file="bin/dma.a"
set cpp_files="src/dma/socket.cpp"
set libs="-lws2_32"

g++ -g -c -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_file% %cpp_files% %libs%
ar rvs %lib_dma_file% %output_file%

echo "COMPILE RTOS LIBRARY"

set include_path="C:/MinGW/boost/boost/include/boost-1_64"
set lib_path="C:/MinGW/boost/boost/lib"
set output_file="bin/rtos.o"
set lib_rtos_file="bin/rtos.a"
set cpp_files="src/rtos/tsip.cpp"
set libs=%lib_file%

g++ -g -c -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_file% %cpp_files% %libs%
ar rvs %lib_rtos_file% %output_file%

echo "COMPILE MAIN PROGRAM"

set include_path="src"
set lib_path="bin"
set include_path2="C:/MinGW/boost/boost/include/boost-1_64"
set lib_path2="C:/MinGW/boost/boost/lib"

set output_file="bin/tsip.exe"
set cpp_files="src/main.cpp"
set libs=%lib_dma_file%
set libs2=%lib_rtos_file%
set libs3="-lws2_32"

g++ -g -std=%c_version% -pthread -I%include_path%  -I%include_path2% -L%lib_path% -L%lib_path2% -o%output_file% %cpp_files% %libs% %libs2% %libs3%

echo "DONE"

pause