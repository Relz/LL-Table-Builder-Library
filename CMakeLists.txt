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

IF (NOT TARGET InputLibrary)
	ADD_SUBDIRECTORY(InputLibrary)
ENDIF ()
IF (NOT TARGET TokenLibrary)
	ADD_SUBDIRECTORY(TokenLibrary)
ENDIF ()

TARGET_LINK_LIBRARIES(LLTableBuilderLibrary InputLibrary)
