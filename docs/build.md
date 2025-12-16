# Compile
    cmake -B out
    cmake --build out --config Debug -j 8

# Test
    ctest --test-dir out -C Debug --output-on-failure

# Coverage
    cmake --build out --target coverage

# Install
    cmake --install out