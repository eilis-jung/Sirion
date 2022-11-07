Sirion is an MIT-licensed experimental 3D fluid solver framework. With Sirion, you can prototype any mesh/particle-based fluid solver, and debug your solver in real-time.

Sirion conceptually follows [Chimera](https://github.com/eilis-jung/Chimera-old), but with more updated technology (Vulkan/GLFW/C++17) and redesigned UX. Apart from ensuring a clear architecture for quick prototyping, Sirion focuses on cross-platform compatibility and easy debugging experience, providing a smooth start for researchers who are not professional C++ developers but still have the need for benchmarking.

## Compile & Run

In Windows, run 
```
./build_win.bat
```
To build the project for Visual Studio.

## Copyright Info

The reflection part of the code (`source/meta_parser`) is partially based on [CPP-Reflection](https://github.com/AustinBrunkHorst/CPP-Reflection) Copyright (c) 2016 Austin BrunkHorst.
