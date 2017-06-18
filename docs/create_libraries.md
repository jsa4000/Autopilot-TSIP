# How to create a compile a librariy using GGG

## 1 Create a .o file

    g++ -c -o header.o header.cpp

## 2 Add this file to a library, creating library if necessary

    ar rvs header.a header.o

## 3 Use library

    g++ main.cpp header.a

## 4 Some mode options

    -g - turn on debugging (so GDB gives more friendly output)
    -Wall - turns on most warnings
    -O or -O2 - turn on optimizations
    -o <name> - name of the output file
    -c - output an object file (.o)
    -I<include path> - specify an include directory
    -L<library path> - specify a lib directory
    -l<library> - link with library lib<library>.a

## 5 CMD Example

```sh
echo "START"

echo "Setting Enviroment variables"

set c_version="c++11"
set include_path="C:/MinGW/boost/boost/include/boost-1_64"
set lib_path="C:/MinGW/boost/boost/lib"
set output_file="bin\\tsip.exe"
set cpp_files="src\\tsip.cpp" "src\\socket.cpp" "src\\main.cpp"
set libs="-lws2_32"

echo "Compiling Libarries"
g++ -g -std=%c_version% -pthread -I%include_path% -L%lib_path% -o%output_file% %cpp_files% %libs%

echo "Compiling Libarries"


echo "END"

pause
```