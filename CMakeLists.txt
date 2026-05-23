cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED BENTO_KIT_VERSION)
  set(BENTO_KIT_VERSION "0.1.0")
endif()

project(bento-kit-cpp VERSION ${BENTO_KIT_VERSION} LANGUAGES CXX)

option(BENTO_KIT_BUILD_TESTS "Build unit tests" ON)
option(BENTO_KIT_BUILD_EXAMPLES "Build examples" ON)

# Top-level builds: artifacts under output/, CMake cache under output/build/.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(BENTO_KIT_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/output")
    if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BENTO_KIT_OUTPUT_DIR}/bin")
    endif()
    if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BENTO_KIT_OUTPUT_DIR}/lib")
    endif()
    if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BENTO_KIT_OUTPUT_DIR}/lib")
    endif()
endif()

add_library(bento_kit INTERFACE)
add_library(bento::kit ALIAS bento_kit)

target_include_directories(bento_kit INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(bento_kit INTERFACE cxx_std_17)

include(GNUInstallDirs)

install(TARGETS bento_kit EXPORT bento-kit-cppTargets)
install(DIRECTORY src/bento/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bento)

install(EXPORT bento-kit-cppTargets
    FILE bento-kit-cppTargets.cmake
    NAMESPACE bento::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit-cpp
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/bento-kit-cppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bento-kit-cppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/bento-kit-cppConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit-cpp
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/bento-kit-cppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/bento-kit-cppConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit-cpp
)

if(BENTO_KIT_BUILD_EXAMPLES)
    add_executable(session_demo src/examples/session_demo.cpp)
    target_link_libraries(session_demo PRIVATE bento::kit)

    add_executable(id_demo src/examples/id_demo.cpp)
    target_link_libraries(id_demo PRIVATE bento::kit)

    add_executable(mask_demo src/examples/mask_demo.cpp)
    target_link_libraries(mask_demo PRIVATE bento::kit)
endif()

if(BENTO_KIT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(src/tests)
endif()
