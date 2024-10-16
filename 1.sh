echo "Compiling..."
cmake --build ./build
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi