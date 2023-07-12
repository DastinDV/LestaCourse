# LestaCourse

GameDev C++ courses from LestaGames.

## Building project

```
git clone --recurse-submodules https://github.com/DastinDV/LestaCourse.git

WINDOWS

Download compilers and build generator
1. llvm https://github.com/llvm/llvm-project/releases/tag/
2. https://github.com/ninja-build/ninja/releases
3. https://cmake.org/download/

Build process from the root of the project LestaCourse
1. git clone --recurse-submodules https://github.com/DastinDV/LestaCourse.git
2. mkdir build && cd build
3. cmake -G "Ninja" ..
4. cmake --build .

LINUX

1. cd LestaCourse/externals/SDL3
2. mkdir build && cd build
3. cmake ..
4. cmake --build .
5. sudo cmake --install .

Build process from the root of the project LestaCourse
1. mkdir build && cd build
2. cmake ..
3. cmake --build .

```

## Run Executables
```
cd build/Task3-Engine
./Engine
```
