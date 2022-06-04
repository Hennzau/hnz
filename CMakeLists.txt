cmake_minimum_required(VERSION 3.16)

project(hnz	VERSION 0.0.1
            LANGUAGES CXX)

file(GLOB_RECURSE sources
                  sources/*.cpp)

add_executable(hnz	${sources})

target_include_directories(hnz	PUBLIC
                           		includes/)

target_compile_features(hnz	PUBLIC
                        	cxx_std_20)

set_target_properties(hnz	PROPERTIES
                              LINKER_LANGUAGE CXX)
