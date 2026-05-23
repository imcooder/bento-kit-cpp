vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO imcooder/bento-kit-cpp
    REF "v${VERSION}"
    SHA512 0
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DBENTO_KIT_BUILD_TESTS=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit-cpp")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
