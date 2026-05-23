/** Triplets exercised in CI and declared in ports/bento-kit/vcpkg.json supports. */

export const VCPKG_SUPPORTS =
  "(windows & !uwp) | linux | osx | ios | android | freebsd | emscripten";

/** Host runners we test in .github/workflows/vcpkg-port.yml */
export const CI_TRIPLETS = [
  { os: "ubuntu-latest", triplet: "x64-linux" },
  { os: "ubuntu-24.04-arm", triplet: "arm64-linux" },
  { os: "macos-latest", triplet: "arm64-osx" },
  { os: "windows-latest", triplet: "x64-windows" },
  { os: "windows-latest", triplet: "arm64-windows" },
];
