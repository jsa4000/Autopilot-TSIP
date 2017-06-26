@echo off
echo "START"

:: Enable Functionality to vc++ 14
set c_version="c99"

:: Check if the bin folder doesn't exist yet
if not exist "bin/" mkdir "bin"

echo "COMPILING MAIN PROGRAM..."

set crc_in_file="src/tsip/crc32.c"
set crc_out_file="bin/crc32.o"


set output_file="bin/tsip.exe"
set input_file="src/tsip/main.c"

gcc -c -std=%c_version% -o%crc_out_file% %crc_in_file%
gcc -g %input_file% -std=%c_version% -o%output_file% %crc_out_file%

echo "DONE"
