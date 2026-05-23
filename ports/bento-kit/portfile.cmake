string(REGEX MATCH "^([0-9]+\\.[0-9]+)" _ "${VERSION}")
set(BENTO_KIT_RELEASE_REF "release_${CMAKE_MATCH_1}")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO imcooder/bento-kit-cpp
    REF "${BENTO_KIT_RELEASE_REF}"
    SHA512 66f29bc320849609e49ea9c62b0d3203aefd3615c32a8f65a2405f033010bc457601094e680eaf0766cfae7d6e39b87c02b02ae145fd2ceae1636c643d0f1828
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS -DBENTO_KIT_BUILD_TESTS=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/bento-kit")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
