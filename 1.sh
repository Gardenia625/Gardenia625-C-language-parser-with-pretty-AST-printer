echo "Compiling..."
cmake --build ./build
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
else
    ./a "./test/1.c"
    # gdb -ex "set args ./test/1.c" -ex "run" -ex "bt" --args ./a
fi