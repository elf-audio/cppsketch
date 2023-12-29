

cmake_minimum_required(VERSION 3.18)

set(APP_NAME simple)
project(${APP_NAME} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add main.cpp (or other source files as needed)
add_executable(${APP_NAME} 
    main.cpp
)

# Add the src subdirectory
add_subdirectory(../../src cppsketch)

# Link the executable with the src_lib
target_link_libraries(${APP_NAME} PRIVATE cppsketch_lib)

# OS-specific options
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Add -ldl for Linux
    target_link_libraries(${APP_NAME} ${CMAKE_DL_LIBS})
endif()

set_target_properties(${APP_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
