Q: When using AddressSanitizer (ASan) in Visual Studio 2022, the application runs fine within the IDE, but when executing from the command line, it reports an error about not being able to find `clang_rt.asan_dynamic-x86_64.dll`.
A: Use the Developer Command Prompt, such as the **x64 Native Tools Command Prompt for VS 2022**.

Sure! Here's a concise FAQ-style question in English based on your issue:

Q: Why does ThreadSanitizer (TSan) report "FATAL: ThreadSanitizer: unexpected memory mapping" when running my program?
A: This error typically occurs because ThreadSanitizer (TSan) requires a predictable virtual memory layout to function correctly. To resolve it, run your program with ASLR (Address Space Layout Randomization) disabled by prefixing the command with setarch -R. For example: "setarch -R ctest --test-dir out -C Debug --output-on-failure"