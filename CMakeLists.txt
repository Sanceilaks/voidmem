cmake_minimum_required(VERSION 3.0)
project(voidmem LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)    
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories( "memory_signature/include" )

add_library(voidmem voidmem.cpp)