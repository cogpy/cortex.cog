cmake_minimum_required(VERSION 3.5)

project(opencog-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenCog Engine sources
set(OPENCOG_ENGINE_SOURCES
    opencog_engine.cc
)

set(OPENCOG_ENGINE_HEADERS
    atom.h
    atomspace.h
    cognitive_fusion.h
    opencog_engine.h
)

# Create library
add_library(${PROJECT_NAME} STATIC
    ${OPENCOG_ENGINE_SOURCES}
    ${OPENCOG_ENGINE_HEADERS}
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# Link dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    jsoncpp_static
    trantor
)

# Installation
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${OPENCOG_ENGINE_HEADERS}
    DESTINATION include/opencog-engine
)
