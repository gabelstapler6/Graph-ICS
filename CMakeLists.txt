# [1] set Poject settings
# ******************************************************************************
cmake_minimum_required(VERSION 3.0)

## Add CMake custom functions
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake;")
include(Functions)

set(TEMPLATE_APPLICATION_NAME "Graph-ICS" CACHE STRING "Application name")
set(PROJECT_NAME ${TEMPLATE_APPLICATION_NAME})

project(${PROJECT_NAME})

set(TEMPLATE_USE_CPP11 ON CACHE BOOL "Use C++11")

# [2] set project directories
# ******************************************************************************
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Sources)
set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Sources/bin)


set(PROJECT_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
message("This is CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

# [3] set output directories
# ******************************************************************************
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_DIR})


# [4] set QT Settings
# ******************************************************************************
set(TEMPLATE_QT_VERSION "5.11.0" CACHE STRING "Qt Version")
set(TEMPLATE_QT_MODULES Core Gui Widgets Quick Qml Concurrent Network DBus PrintSupport)

find_package(Qt5 ${TEMPLATE_QT_VERSION} CONFIG REQUIRED ${TEMPLATE_QT_MODULES})
set(CMAKE_AUTOMOC true)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")


# [5] binding of libraries:
# ******************************************************************************
set(ITK_DIR "D:\\lib\\ITK\\bin")
find_package(ITK REQUIRED )
message("ITK Pakage: " ${ITK_DIR})
include(${ITK_USE_FILE})

set(OpenCv_DIR "D:\\lib\\opencv\\build")
find_package(OpenCv REQUIRED )
message("OpenCv Pakage: " ${OpenCv_DIR})
set(${OpenCV_INCLUDE_DIRS} ${OpenCv_DIR}/include)
include_directories( ${OpenCV_INCLUDE_DIRS} )
set(OPENCV_LIBRARIES ${OpenCv_DIR}/x64/vc14/bin/)
message("OpenCv includes: " ${OpenCV_INCLUDE_DIRS})

# [6] setting definitions
# ******************************************************************************
add_definitions(${QT_DEFINITIONS})
add_definitions(-DUSE_INSTALL_TARGET)
add_definitions(-DQT_QML_RELEASE)

# [7] getting sources: Add to project configuration
# ******************************************************************************
AddSources("${SOURCE_DIR}" "Sources" list_app_src)
AddSources("${ROOT_DIR}/Sources/filter" "/Sources/filter" list_app_src)

AddResources("${ROOT_DIR}/QML" "QML" list_app_qml)
AddResources("${ROOT_DIR}/QML/filter" "QML/filter" list_app_qml)

AddResources("${ROOT_DIR}/doc" "doc" list_app_qml)

message("QML files on: " "${ROOT_DIR}/QML")
message("Source files on: " "${SOURCE_DIR}")

set(QML_IMPORT_PATH "${ROOT_DIR}/QML" CACHE STRING "Path to QML Files")

set(QT_RESOURCES ${list_app_qml})

# [8] setting the target
# ******************************************************************************
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(${PROJECT_NAME} WIN32 ${list_app_src} ${QT_RESOURCES} ${ROOT_DIR}/doc/myapp.rc)


target_include_directories(${PROJECT_NAME} PUBLIC ${BINARY_DIR})

# [9] check if c++11 enabled
# ******************************************************************************
if(${TEMPLATE_USE_CPP11})
    if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.0)
        set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
        set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
    else()
        if(APPLE)
            list(APPEND CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -stdlib=libc++")
        endif()
        if(UNIX AND NOT APPLE)
            list(APPEND CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif()
    endif()
endif()

# [10] copy the ressources after build on the build folder
# ******************************************************************************
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/QML" ${BUILD_DIR}/QML)
add_custom_target(CopyResources ALL COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/QML" ${BUILD_DIR}/QML)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${ROOT_DIR}/doc" ${BUILD_DIR}/doc)

# copy the .dll from OpenCv
file(GLOB list_globbed "${OPENCV_LIBRARIES}/*.dll")
foreach(file ${list_globbed})
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                    ${file}
                    ${BUILD_DIR})
endforeach()


# [11] linking libraries
# ******************************************************************************
target_link_libraries(${PROJECT_NAME}
    # dlls, die man benötigt
    PUBLIC Qt5::Core
    PUBLIC Qt5::Widgets
    PUBLIC Qt5::Gui
    PUBLIC Qt5::Quick
    PUBLIC Qt5::Qml
    PUBLIC Qt5::Concurrent
    ${ITK_LIBRARIES}
    ${OpenCV_LIBS})

message("OpenCv libs: " ${OPENCV_LIBRARIES})
message("ITK libs: " ${ITK_LIBRARIES})
