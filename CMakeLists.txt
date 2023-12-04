cmake_minimum_required(VERSION 3.16)

option(BUILD_SHARED_LIBS "Shared libs?" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

if (CMAKE_GENERATOR MATCHES "Ninja")
	file(
		WRITE "${CMAKE_BINARY_DIR}/GNUMakeRulesOverwrite.cmake" 
			"STRING(REPLACE \"-MD\" \"-MMD\" CMAKE_DEPFILE_FLAGS_C \"\${CMAKE_DEPFILE_FLAGS_C}\")\n"
			"STRING(REPLACE \"-MD\" \"-MMD\" CMAKE_DEPFILE_FLAGS_CXX \"\${CMAKE_DEPFILE_FLAGS_CXX}\")\n"
	)
	set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_BINARY_DIR}/GNUMakeRulesOverwrite.cmake" CACHE INTERNAL "")
endif()

project(Game LANGUAGES CXX)
enable_testing()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)

add_compile_options(
	-fcolor-diagnostics
	-fdiagnostics-color=always
	-Wall
	-Wextra
	-Wpedantic
	-Weverything

	-Wno-c++17-extensions
	-Wno-c++20-attribute-extensions
	-Wno-c++98-compat-pedantic
	-Wno-covered-switch-default
	-Wno-ctad-maybe-unsupported
	-Wno-deprecated-copy-dtor
	-Wno-exit-time-destructors
	-Wno-float-conversion
	-Wno-global-constructors
	-Wno-missing-field-initializers
	-Wno-padded
	-Wno-sign-conversion
	-Wno-switch-enum
	-Wno-unused-template
	-Wno-weak-vtables
	-Wunused
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(asan -g -fno-omit-frame-pointer -fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls)
	add_compile_options(${asan} -O0 -fstandalone-debug)
	add_link_options(${asan})
	add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
	add_compile_options(-pg -O3)
	add_link_options(-pg)
	add_definitions(-DNDEBUG)
else()
	add_compile_options(-O3)
	add_definitions(-DNDEBUG)
endif()

add_executable(00 00.cc)
add_executable(01 01.cc)
add_executable(02 02.cc)
add_executable(03 03.cc)
add_executable(04 04.cc)
