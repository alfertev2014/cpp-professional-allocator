cmake_minimum_required(VERSION 3.5)

project(cpp-professional-allocator)

add_executable(allocator_cli main.cpp)
set_target_properties(allocator_cli PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)
