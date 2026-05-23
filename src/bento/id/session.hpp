#pragma once

#include <bento/id/detail/rng.hpp>
#include <bento/id/random.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace bento::id {

/// Set the session prefix used by `generateSessionId`.
///
/// Empty strings are ignored. Thread-safe.
void setSessionPrefix(std::string_view prefix);

/// Generate a session ID: `<prefix><unix_ms><5-char base36 suffix>`.
///
/// Default prefix is a random 12-character base36 string, generated once per
/// process on first use. Thread-safe.
std::string generateSessionId();

namespace detail {

inline constexpr std::size_t kSessionPrefixDefaultLen = 12;
inline constexpr std::size_t kSessionIdRandomSuffixLen = 5;

class SessionPrefixStorage {
public:
  static SessionPrefixStorage &instance() {
    static SessionPrefixStorage storage;
    return storage;
  }

  void setPrefix(std::string_view prefix) {
    if (prefix.empty()) {
      return;
    }
    std::atomic_store(&m_prefix, std::make_shared<const std::string>(prefix));
  }

  std::string copyPrefix() { return *std::atomic_load(&m_prefix); }

private:
  SessionPrefixStorage() : m_prefix(std::make_shared<const std::string>(generateBase36String(kSessionPrefixDefaultLen))) {}

  std::shared_ptr<const std::string> m_prefix;
};

} // namespace detail

inline void setSessionPrefix(std::string_view prefix) {
  detail::SessionPrefixStorage::instance().setPrefix(prefix);
}

inline std::string generateSessionId() {
  const std::string prefix = detail::SessionPrefixStorage::instance().copyPrefix();
  const auto ms = detail::unixMillis();
  const std::string suffix = detail::generateBase36String(detail::kSessionIdRandomSuffixLen);
  std::string sessionId;
  sessionId.reserve(prefix.size() + 20 + suffix.size());
  sessionId.append(prefix);
  sessionId.append(std::to_string(ms));
  sessionId.append(suffix);
  return sessionId;
}

} // namespace bento::id
