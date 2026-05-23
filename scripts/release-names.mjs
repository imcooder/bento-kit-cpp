/** Shared release / vcpkg naming (product vs CMake package). */

export const PRODUCT = "bento-kit";
export const VCPKG_PORT = "bento-kit";
export const CMAKE_PACKAGE = "bento-kit";
export const VCPKG_PORT_DIR = `ports/${VCPKG_PORT}`;

export function releaseName(version) {
  return `${PRODUCT} v${version}`;
}

export function pkgArchiveName(version) {
  return `${PRODUCT}-${version}`;
}
