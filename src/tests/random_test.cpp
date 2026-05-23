#include <bento/id/nanoid.hpp>
#include <bento/id/random.hpp>
#include <bento/id/uuid.hpp>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <set>
#include <string>
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

bool isHexChar(char ch) {
  return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

void testRandomIntInRange() {
  for (int i = 0; i < 1000; ++i) {
    const auto n = bento::id::randomInt(5, 10);
    CHECK(n >= 5 && n < 10);
  }
}

void testRandomIntNegativeRange() {
  for (int i = 0; i < 200; ++i) {
    const auto n = bento::id::randomInt(-10, -1);
    CHECK(n >= -10 && n < -1);
  }
}

void testRandomStringLength() {
  for (const std::size_t len : {std::size_t{0}, std::size_t{1}, std::size_t{8}, std::size_t{32}, std::size_t{256}}) {
    CHECK(bento::id::randomString(len).size() == len);
  }
}

void testRandomStringAlphabet() {
  static constexpr char kAlphabet[] = "0123456789qwertyuioplkjhgfdsazxcvbnm";
  const std::string s = bento::id::randomString(500);
  for (char ch : s) {
    bool found = false;
    for (char a : kAlphabet) {
      if (ch == a) {
        found = true;
        break;
      }
    }
    CHECK(found);
  }
}

void testUuidV4Shape() {
  const std::string id = bento::id::uuidV4();
  CHECK(id.size() == 36);
  CHECK(id[14] == '4');
  int hyphens = 0;
  for (char ch : id) {
    if (ch == '-') {
      ++hyphens;
    }
  }
  CHECK(hyphens == 4);
}

void testUuidV4Simple() {
  const std::string id = bento::id::uuidV4Simple();
  CHECK(id.size() == 32);
  for (char ch : id) {
    CHECK(isHexChar(ch));
  }
}

void testUuidV4Unique() {
  CHECK(bento::id::uuidV4() != bento::id::uuidV4());
}

void testUuidV7Shape() {
  const std::string id = bento::id::uuidV7();
  CHECK(id.size() == 36);
  CHECK(id[14] == '7');
}

void testUuidV7Ordered() {
  std::vector<std::string> ids;
  ids.reserve(32);
  for (int i = 0; i < 32; ++i) {
    ids.push_back(bento::id::uuidV7());
  }
  std::vector<std::string> sorted = ids;
  std::sort(sorted.begin(), sorted.end());
  CHECK(ids == sorted);
}

void testBatchUuidV4NoCollision() {
  std::set<std::string> seen;
  for (int i = 0; i < 1000; ++i) {
    seen.insert(bento::id::uuidV4());
  }
  CHECK(seen.size() == 1000);
}

void testBatchUuidV7NoCollision() {
  std::set<std::string> seen;
  for (int i = 0; i < 1000; ++i) {
    seen.insert(bento::id::uuidV7());
  }
  CHECK(seen.size() == 1000);
}

void testNanoidDefaultLength() {
  CHECK(bento::id::nanoid().size() == bento::id::kDefaultNanoidLen);
}

void testNanoidCustomLength() {
  for (const std::size_t len : {std::size_t{4}, std::size_t{10}, std::size_t{32}, std::size_t{64}}) {
    CHECK(bento::id::nanoidWithLen(len).size() == len);
  }
}

void testNanoidUrlSafeAlphabet() {
  const std::string id = bento::id::nanoidWithLen(64);
  for (char ch : id) {
    const bool ok = (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '-';
    CHECK(ok);
  }
}

void testNanoidCustomAlphabet() {
  const std::string id = bento::id::nanoidWithAlphabet(50, "xyz");
  CHECK(id.size() == 50);
  for (char ch : id) {
    CHECK(ch == 'x' || ch == 'y' || ch == 'z');
  }
}

void testBatchNanoidNoCollision() {
  std::set<std::string> seen;
  for (int i = 0; i < 1000; ++i) {
    seen.insert(bento::id::nanoid());
  }
  CHECK(seen.size() == 1000);
}

using TestFn = void (*)();

struct TestCase {
  const char *name;
  TestFn fn;
};

} // namespace

int main() {
  const TestCase tests[] = {
      {"randomInt in range", testRandomIntInRange},
      {"randomInt negative range", testRandomIntNegativeRange},
      {"randomString length", testRandomStringLength},
      {"randomString alphabet", testRandomStringAlphabet},
      {"uuidV4 shape", testUuidV4Shape},
      {"uuidV4Simple hex", testUuidV4Simple},
      {"uuidV4 unique", testUuidV4Unique},
      {"uuidV7 shape", testUuidV7Shape},
      {"uuidV7 ordered", testUuidV7Ordered},
      {"batch uuidV4 no collision", testBatchUuidV4NoCollision},
      {"batch uuidV7 no collision", testBatchUuidV7NoCollision},
      {"nanoid default length", testNanoidDefaultLength},
      {"nanoid custom length", testNanoidCustomLength},
      {"nanoid url-safe alphabet", testNanoidUrlSafeAlphabet},
      {"nanoid custom alphabet", testNanoidCustomAlphabet},
      {"batch nanoid no collision", testBatchNanoidNoCollision},
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
