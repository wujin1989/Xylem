#### Q: When using AddressSanitizer (ASan) in Visual Studio 2022, the application runs fine within the IDE, but when executing from the command line, it reports an error about not being able to find `clang_rt.asan_dynamic-x86_64.dll`.
> **A:** Use the Developer Command Prompt, such as the **x64 Native Tools Command Prompt for VS 2022**.

#### Q: Why does ThreadSanitizer (TSan) report "FATAL: ThreadSanitizer: unexpected memory mapping" when running my program?
> **A:** This error typically occurs because ThreadSanitizer (TSan) requires a predictable virtual memory layout to function correctly. To resolve it, run your program with ASLR (Address Space Layout Randomization) disabled by prefixing the command with `setarch -R`. For example:  
> ```bash
> setarch -R ctest --test-dir out -C Debug --output-on-failure
> ```
> Note: TSan is only supported on Unix; it is not available on Windows.

#### Q: Can AddressSanitizer (ASan), ThreadSanitizer (TSan), and UndefinedBehaviorSanitizer (UBSan) be enabled simultaneously?
> **A:** No, they are not designed to run together. It is best to enable only one sanitizer at a time to avoid conflicts, performance issues, and ambiguous diagnostics.

#### Q: How do I enable a specific sanitizer (ASan/TSan/UBSan) in my CMake project?
> **A:** Configure your build with the corresponding CMake option. For example:
> ```bash
> cmake -B out -DXYLEM_ENABLE_ASAN=ON    # ASan
> cmake -B out -DXYLEM_ENABLE_TSAN=ON    # TSan (Unix)
> cmake -B out -DXYLEM_ENABLE_UBSAN=ON   # UBSan
> ```
> Replace `XYLEM_ENABLE_*` with the actual option names used in your project. Remember: **only one sanitizer can be active at a time**.

#### Q: How do I generate a code coverage report for my tests?

> **A:** Enable coverage during configuration and run the `coverage` target, it automatically runs all tests, and generates the report:
> ```bash
> # Configure with coverage enabled
>   cmake -B out -DXYLEM_ENABLE_COVERAGE=ON
> # Build executable
>   cmake --build out --config Debug -j 8
> # Generate HTML report
>   cmake --build out --target coverage
> ```
> The final report is available at `out/coverage`.  
> **Platform notes**:  
> - **Windows**: Requires [OpenCppCoverage](https://github.com/OpenCppCoverage/OpenCppCoverage) in your `PATH`.  
> - **Linux**: Requires `lcov` and `genhtml` (e.g., `sudo apt install lcov`).