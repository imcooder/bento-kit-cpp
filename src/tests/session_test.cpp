#include <bento/id/session.hpp>

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

namespace {

int g_failures = 0;

void check(bool ok, const char *expr, const char *file, int line) {
  if (!ok) {
    ++g_failures;
    std::cerr << file << ':' << line << ": CHECK failed: " << expr << '\n';
  }
}

#define CHECK(expr) check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)

constexpr std::size_t kSuffixLen = 5;

std::mutex &prefixTestLock() {
  static std::mutex mutex;
  return mutex;
}

bool suffixIsBase36(const std::string &suffix) {
  if (suffix.size() != kSuffixLen) {
    return false;
  }
  for (char ch : suffix) {
    const bool ok = (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z');
    if (!ok) {
      return false;
    }
  }
  return true;
}

void testMinimumLengthWithKnownPrefix() {
  const std::lock_guard lock(prefixTestLock());
  bento::id::setSessionPrefix("ab");
  const std::string sessionId = bento::id::generateSessionId();
  CHECK(sessionId.rfind("ab", 0) == 0);
  CHECK(sessionId.size() >= 2 + 1 + kSuffixLen);
  CHECK(suffixIsBase36(sessionId.substr(sessionId.size() - kSuffixLen)));
}

void testCustomPrefix() {
  const std::lock_guard lock(prefixTestLock());
  bento::id::setSessionPrefix("test_prefix_alpha");
  const std::string sessionId = bento::id::generateSessionId();
  CHECK(sessionId.rfind("test_prefix_alpha", 0) == 0);
}

void testEmptyPrefixIgnored() {
  const std::lock_guard lock(prefixTestLock());
  bento::id::setSessionPrefix("known_prefix_beta");
  bento::id::setSessionPrefix("");
  const std::string sessionId = bento::id::generateSessionId();
  CHECK(sessionId.rfind("known_prefix_beta", 0) == 0);
}

void testBatchUnique() {
  const std::lock_guard lock(prefixTestLock());
  bento::id::setSessionPrefix("uniq");
  std::set<std::string> seen;
  constexpr int n = 1000;
  for (int i = 0; i < n; ++i) {
    seen.insert(bento::id::generateSessionId());
  }
  CHECK(seen.size() == static_cast<std::size_t>(n));
}

void testConcurrentGenerate() {
  constexpr int threadCount = 8;
  constexpr int perThread = 250;
  std::vector<std::thread> workers;
  std::mutex resultMutex;
  std::set<std::string> seen;

  workers.reserve(threadCount);
  for (int t = 0; t < threadCount; ++t) {
    workers.emplace_back([&]() {
      std::vector<std::string> local;
      local.reserve(perThread);
      for (int i = 0; i < perThread; ++i) {
        local.push_back(bento::id::generateSessionId());
      }
      std::lock_guard lock(resultMutex);
      for (const auto &id : local) {
        seen.insert(id);
      }
    });
  }
  for (auto &worker : workers) {
    worker.join();
  }
  CHECK(seen.size() == static_cast<std::size_t>(threadCount * perThread));
}

void testConcurrentSetAndGenerate() {
  constexpr int threadCount = 4;
  std::vector<std::thread> workers;
  workers.reserve(threadCount);

  for (int t = 0; t < threadCount; ++t) {
    workers.emplace_back([t]() {
      bento::id::setSessionPrefix("prefix_" + std::to_string(t));
      for (int i = 0; i < 50; ++i) {
        const std::string sessionId = bento::id::generateSessionId();
        if (sessionId.empty()) {
          CHECK(false);
        }
      }
    });
  }
  for (auto &worker : workers) {
    worker.join();
  }
}

using TestFn = void (*)();

struct TestCase {
  const char *name;
  TestFn fn;
};

} // namespace

int main() {
  const TestCase tests[] = {
      {"minimum length with known prefix", testMinimumLengthWithKnownPrefix}, {"custom prefix", testCustomPrefix}, {"empty prefix ignored", testEmptyPrefixIgnored}, {"batch unique", testBatchUnique}, {"concurrent generate", testConcurrentGenerate}, {"concurrent set and generate", testConcurrentSetAndGenerate},
  };

  for (const auto &testCase : tests) {
    std::cout << "RUN  " << testCase.name << '\n';
    testCase.fn();
  }

  if (g_failures != 0) {
    std::cerr << g_failures << " check(s) failed\n";
    return EXIT_FAILURE;
  }

  std::cout << "All tests passed.\n";
  return EXIT_SUCCESS;
}
