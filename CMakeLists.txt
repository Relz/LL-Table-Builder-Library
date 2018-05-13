CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(LLTableBuilderLibrary)

SET(CMAKE_CXX_STANDARD 17)

SET(SOURCE_FILES
	LLTableBuilder.cpp
	LLTableBuilder.h
	Table/Table.cpp
	Table/Table.h
	Table/TableRow/TableRow.cpp
	Table/TableRow/TableRow.h
	Symbol/Symbol.cpp
	Symbol/Symbol.h)

ADD_LIBRARY(LLTableBuilderLibrary SHARED ${SOURCE_FILES})

ADD_SUBDIRECTORY(InputLibrary)

TARGET_LINK_LIBRARIES(LLTableBuilderLibrary InputLibrary)
