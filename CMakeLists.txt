cmake_minimum_required(VERSION 3.27)

project(SchemeInterpreter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)


add_executable(SchemeInterpreter object.cpp parser.cpp scheme.cpp tokenizer.cpp)
