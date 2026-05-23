vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO imcooder/bento-kit-cpp
    REF "v${VERSION}"
    SHA512 19e03685b8ef41a1aa60733aab3a5604bf4d519ff1719a1726816c28f810a7cae7573e6363dcba9db7cf4e5db89a87c90dcac16abb28bf84eb331509dd534aa2
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DBENTO_KIT_BUILD_TESTS=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
