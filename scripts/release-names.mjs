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

/** Git branch that release / vcpkg source tarballs use (e.g. release_0.1). */
export function releaseBranchFromVersion(version) {
  const parts = version.split(".");
  if (parts.length < 2) {
    throw new Error(`version must be major.minor.patch, got "${version}"`);
  }
  return `release_${parts[0]}.${parts[1]}`;
}

/** Fork used to open PRs against microsoft/vcpkg (override via env). */
export const VCPKG_FORK_REPO = process.env.VCPKG_FORK_REPO ?? "imcooder/vcpkg";
export const VCPKG_UPSTREAM_REPO = "microsoft/vcpkg";
export const VCPKG_PR_BRANCH = "bento-kit-port";
