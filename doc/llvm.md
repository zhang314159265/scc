# Build LLVM
```
cd $llvm_root
mkdir -p build
cd build
cmake -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ../llvm
time make -j
```

It maybe handy to have an opt build alongside the debug build. Replace 'Debug' to 'Release' in the command above for a release build.

To use the clang built from source, I need the following extra flags to find the standard headers and libraries:
1. Add extra path to search for headers: ` -I/Applications/Xcode_13.4.0_fb.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/ -I/Applications/Xcode_13.4.0_fb.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1 `
2. Add extra path to search for libraries: ` -Wl,-syslibroot /Applications/Xcode_13.4.0_fb.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -L/usr/local/lib `

These flags are found by running the stock clang/clang++

# Note
Use llvm version llvmorg-20.1.4 since it's the latest tag on May10 2025.

# Reference
- [Getting Started with the LLVM System](https://llvm.org/docs/GettingStarted.html#getting-started-with-llvm). This one is out of date. Refer to 'Getting Started: Building and Running Clang' instead
- [Getting Started: Building and Running Clang](https://clang.llvm.org/get_started.html)
- [My First Language Frontend with LLVM Tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html): build a toy language called Kaleidoscope
  - worth revisting the entire tutorial
  - next ch6: use defined operators
  - fastfwd
    - ch7: mutable variables # DONE
- [Writing an LLVM Pass](https://llvm.org/docs/WritingAnLLVMPass.html): there is a reference to this doc from 7.3 of 'My First Language Frontend with LLVM Tutorial'.
- [LLVM Lang Ref](https://llvm.org/docs/LangRef.html): Describes the format of LLVM IR.
