# Create a build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build specifically in Release mode
cmake --build . --config Release