# Specify the target system
SET(CMAKE_SYSTEM_NAME Windows)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify the cross-compiling toolchain
SET(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)

# Specify the Windows target sysroot (optional, but recommended)
SET(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# Adjust search paths so that CMake finds the right libraries and headers for cross-compiling
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Specify the Windows output binary type
SET(CMAKE_CXX_STANDARD 20)
