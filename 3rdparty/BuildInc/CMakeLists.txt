cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

set(PROJECT BuildInc)
enable_language(C)
project(${PROJECT} C)

add_executable(${PROJECT} BuildInc.c)
set_target_properties(${PROJECT} PROPERTIES FOLDER 3rdparty)
