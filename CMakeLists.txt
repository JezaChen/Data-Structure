cmake_minimum_required(VERSION 3.7)
project(Tree)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp BinTree.h BinTree.cpp Common.h BST.h BST.cpp BTree.h BTree.cpp)
add_executable(Tree ${SOURCE_FILES})