cmake_minimum_required (VERSION 3.13)

project(PSTxAnalyzer)

add_definitions( -DLOGIC2 )

# enable generation of compile_commands.json, helpful for IDEs to locate include files.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations")

include(ExternalAnalyzerSDK)

set(SOURCES 
src/PSTxAnalyzer.cpp
src/PSTxAnalyzer.h
src/PSTxAnalyzerResults.cpp
src/PSTxAnalyzerResults.h
src/PSTxAnalyzerSettings.cpp
src/PSTxAnalyzerSettings.h
src/PSTxSimulationDataGenerator.cpp
src/PSTxSimulationDataGenerator.h
)

add_analyzer_plugin(${PROJECT_NAME} SOURCES ${SOURCES})
