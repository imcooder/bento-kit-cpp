string(REGEX MATCH "^([0-9]+\\.[0-9]+)" _ "${VERSION}")
set(BENTO_KIT_RELEASE_REF "release_${CMAKE_MATCH_1}")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO imcooder/bento-kit-cpp
    REF "${BENTO_KIT_RELEASE_REF}"
    SHA512 14e883c0b7ea14f3f5e54f131a2ee54b554176fe4c57b2bf77789890e16d5805d57e2d6641333f9076f12105e2522bb22bd33f49b4e19d353314724367efc85b
    HEAD_REF main
)

set(VCPKG_BUILD_TYPE release) # header-only

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBENTO_KIT_BUILD_TESTS=OFF
        -DBENTO_KIT_BUILD_EXAMPLES=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
