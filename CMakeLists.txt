cmake_minimum_required(VERSION 3.5)

message("C Compiler: ${CMAKE_C_COMPILER}")
message("CXX Compiler: ${CMAKE_CXX_COMPILER}")

# Build type
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# Project declaration
project(ExgineRoot)

# we need to check if we're actually on windows, not just cross-compiling
if (WIN32 AND NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
	message(STATUS "Not on Windows, not enabling unicode support")
elseif (WIN32)
	message(STATUS "Manually enabling unicode support")
    add_compile_options(/utf-8)
endif()

add_subdirectory(vendor/megasource/libs/)

file (GLOB_RECURSE SOURCES src/*.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/starter.cpp)

add_library (Exgine ${SOURCES})

target_include_directories(Exgine
		PUBLIC
				$<INSTALL_INTERFACE:include>
				$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/glfw/include/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/glad/include/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/glm/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/imgui/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/imgui/backends/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/imgui/misc/cpp/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/stb/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/lua/include/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/sol2/include/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/IconFontCppHeaders/
				${CMAKE_SOURCE_DIR}/vendor/megasource/libs/
		PRIVATE
				${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Exgine PRIVATE glfw glad spdlog glm stb imgui lua54 sol2)

add_executable(starter src/starter.cpp)
target_link_libraries(starter Exgine spdlog)
