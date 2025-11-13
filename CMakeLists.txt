cmake_minimum_required(VERSION 3.12)
project(commentRemover.x)

set(CMAKE_CXX_STANDARD 17)

add_executable(main.x main.cpp linkedlist.cpp LCRS.cpp STLL.cpp AST.cpp)
