@echo off
echo "START"

set c_version="c++14"

echo "COMPILING DMA LIBRARY..."

set include_path="C:/MinGW/boost/boost/include/boost-1_64"
set lib_path="C:/MinGW/boost/boost/lib"
set output_file="bin/dma.o"
set lib_dma_file="bin/dma.a"
set cpp_files="src/dma/socket.cpp"
set libs="-lws2_32"

g++ -g -c -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_file% %cpp_files% %libs%
ar rvs %lib_dma_file% %output_file%

echo "COMPILING RTOS LIBRARY..."

set include_path="C:/MinGW/boost/boost/include/boost-1_64"
set lib_path="C:/MinGW/boost/boost/lib"
set output_rtos_file="bin/rtos.o"
set output_tsip_file="bin/tsip.o"
set output_task_file="bin/task.o"
set lib_rtos_file="bin/rtos.a"
set cpp_files="src/rtos/tsip.cpp"
set cpp_files2="src/rtos/rtos.cpp"
set cpp_files3="src/rtos/task.cpp"
set libs=%lib_file%

g++ -g -c -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_tsip_file% %cpp_files% %libs%
g++ -g -c -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_rtos_file% %cpp_files2% %libs%
g++ -g -c -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_task_file% %cpp_files3% %libs%
ar rvs %lib_rtos_file% %output_tsip_file% %output_rtos_file% %output_task_file%


echo "COMPILING MAIN PROGRAM..."

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
