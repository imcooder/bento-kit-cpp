#pragma once

#include <bento/id/detail/rng.hpp>

#include <array>
#include <cstdint>
#include <mutex>
#include <random>
#include <string>

namespace bento::id {

inline std::string uuidV4();
inline std::string uuidV4Simple();
inline std::string uuidV7();
inline std::string uuidV7Simple();

namespace detail {

inline char hexDigit(std::uint8_t nibble) {
  static constexpr char kHex[] = "0123456789abcdef";
  return kHex[nibble & 0x0f];
}

inline std::string formatUuid(const std::uint8_t bytes[16], bool hyphenated) {
  std::string out;
  out.reserve(hyphenated ? 36 : 32);
  auto appendByte = [&](std::uint8_t byte) {
    out.push_back(hexDigit(byte >> 4));
    out.push_back(hexDigit(byte & 0x0f));
  };
  if (hyphenated) {
    for (int i = 0; i < 4; ++i) {
      appendByte(bytes[i]);
    }
    out.push_back('-');
    appendByte(bytes[4]);
    appendByte(bytes[5]);
    out.push_back('-');
    appendByte(bytes[6]);
    appendByte(bytes[7]);
    out.push_back('-');
    appendByte(bytes[8]);
    appendByte(bytes[9]);
    out.push_back('-');
    for (int i = 10; i < 16; ++i) {
      appendByte(bytes[i]);
    }
    return out;
  }
  for (int i = 0; i < 16; ++i) {
    appendByte(bytes[i]);
  }
  return out;
}

inline std::array<std::uint8_t, 16> makeUuidV4Bytes() {
  std::array<std::uint8_t, 16> bytes{};
  fillRandomBytes(bytes.data(), bytes.size());
  bytes[6] = static_cast<std::uint8_t>((bytes[6] & 0x0f) | 0x40);
  bytes[8] = static_cast<std::uint8_t>((bytes[8] & 0x3f) | 0x80);
  return bytes;
}

inline std::array<std::uint8_t, 16> assembleUuidV7Bytes(std::uint64_t ms, std::uint16_t randA) {
  auto &rng = threadRng();
  std::uniform_int_distribution<int> dist(0, 255);

  std::array<std::uint8_t, 16> bytes{};
  bytes[0] = static_cast<std::uint8_t>(ms >> 40);
  bytes[1] = static_cast<std::uint8_t>(ms >> 32);
  bytes[2] = static_cast<std::uint8_t>(ms >> 24);
  bytes[3] = static_cast<std::uint8_t>(ms >> 16);
  bytes[4] = static_cast<std::uint8_t>(ms >> 8);
  bytes[5] = static_cast<std::uint8_t>(ms);
  bytes[6] = static_cast<std::uint8_t>(0x70 | ((randA >> 8) & 0x0f));
  bytes[7] = static_cast<std::uint8_t>(randA & 0xff);
  bytes[8] = static_cast<std::uint8_t>(0x80 | (dist(rng) & 0x3f));
  for (int i = 9; i < 16; ++i) {
    bytes[i] = static_cast<std::uint8_t>(dist(rng));
  }
  return bytes;
}

/// Global monotonic UUID v7 sequence. Every call mutates state, so a plain
/// mutex (not a read-write lock) is required. Only timestamp + counter are
/// guarded; random tail bytes are filled outside the lock.
class UuidV7Generator {
public:
  static UuidV7Generator &instance() {
    static UuidV7Generator gen;
    return gen;
  }

  std::array<std::uint8_t, 16> nextBytes() {
    std::uint64_t ms = 0;
    std::uint16_t randA = 0;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      ms = unixMillis() & 0xFFFFFFFFFFFFULL;
      if (ms != m_lastMs) {
        m_lastMs = ms;
        m_randA = static_cast<std::uint16_t>(std::uniform_int_distribution<int>(0, 0x0fff)(threadRng()));
      } else {
        m_randA = static_cast<std::uint16_t>((m_randA + 1) & 0x0fff);
      }
      randA = m_randA;
    }
    return assembleUuidV7Bytes(ms, randA);
  }

private:
  std::mutex m_mutex;
  std::uint64_t m_lastMs = 0;
  std::uint16_t m_randA = 0;
};

} // namespace detail

inline std::string uuidV4() {
  const auto bytes = detail::makeUuidV4Bytes();
  return detail::formatUuid(bytes.data(), true);
}

inline std::string uuidV4Simple() {
  const auto bytes = detail::makeUuidV4Bytes();
  return detail::formatUuid(bytes.data(), false);
}

inline std::string uuidV7() {
  const auto bytes = detail::UuidV7Generator::instance().nextBytes();
  return detail::formatUuid(bytes.data(), true);
}

inline std::string uuidV7Simple() {
  const auto bytes = detail::UuidV7Generator::instance().nextBytes();
  return detail::formatUuid(bytes.data(), false);
}

} // namespace bento::id
