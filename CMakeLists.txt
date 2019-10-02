cmake_minimum_required(VERSION 3.1.0)

project( menu )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

set (CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb" )
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

# Populate a CMake variable with the sources
set(packagedb_SRCS
    layout.cpp
    main.cpp
)
# Tell CMake to create the helloworld executable
add_executable(qt_menu ${packagedb_SRCS})
qt5_use_modules(qt_menu Widgets Core Sql)
