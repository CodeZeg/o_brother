mkdir build
cd build
cmake ..
cmake --build . --config Release
copy Release\libiwasm.dll ..\
pause