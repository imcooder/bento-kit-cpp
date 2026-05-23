string(REGEX MATCH "^([0-9]+\\.[0-9]+)" _ "${VERSION}")
set(BENTO_KIT_RELEASE_REF "release_${CMAKE_MATCH_1}")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO imcooder/bento-kit-cpp
    REF "${BENTO_KIT_RELEASE_REF}"
    SHA512 d30f285804335f17f53dfa5e29f7547948d70e28afb25d07191e5dd5b3779ffb920ab0e8d825f558d2c721c7054daf3512f1d9262fa4b16df7ff93854c186e3d
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DBENTO_KIT_BUILD_TESTS=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
