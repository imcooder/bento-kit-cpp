vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO imcooder/bento-kit-cpp
    REF "v${VERSION}"
    SHA512 e8bacd00159b3d9cb993a725e85108460919215a48ebdc4e82b4ddaa553ffa4cb83fc5efafa1b76cff2b4820da7f9cd35a5319894b572e378044204dd11638f9
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DBENTO_KIT_BUILD_TESTS=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
