cmake_minimum_required(VERSION 3.15)
project(oclhelpers VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} oclhelpers.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(${PROJECT_NAME} PUBLIC OpenCL::OpenCL)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER oclhelpers.hpp)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} 
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME} FILE ${PROJECT_NAME}.cmake NAMESPACE oclhelpers:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

add_library(oclhelpers::oclhelpers STATIC IMPORTED)
set_target_properties(oclhelpers::oclhelpers PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
)

# Export targets from a build tree for convenience
export(EXPORT ${PROJECT_NAME} 
       FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}.cmake"
       NAMESPACE oclhelpers::
)


#include(CMakePackageConfigHelpers)
#configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
#  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
#)