#include <bento/mask/mask.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

#include "unicode_fixtures.hpp"

namespace {

int g_failures = 0;

void check(bool ok, const char *expr, const char *file, int line) {
  if (!ok) {
    ++g_failures;
    std::cerr << file << ':' << line << ": CHECK failed: " << expr << '\n';
  }
}

#define CHECK(expr) check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)

#define CHECK_EQ(actual, expected) CHECK((actual) == (expected))

void testPhoneWellFormed() {
  CHECK_EQ(bento::mask::maskPhone("13812345678"), "138****5678");
}

void testPhoneMalformed() {
  CHECK_EQ(bento::mask::maskPhone("12345"), "1***5");
  CHECK_EQ(bento::mask::maskPhone("138-1234-5678"), "1***********8");
}

void testPhoneEmpty() {
  CHECK_EQ(bento::mask::maskPhone(""), "");
}

void testEmailTypical() {
  CHECK_EQ(bento::mask::maskEmail("alice@example.com"), "a***@example.com");
}

void testEmailSingleCharLocal() {
  CHECK_EQ(bento::mask::maskEmail("a@b.com"), "a***@b.com");
}

void testEmailNoAt() {
  CHECK_EQ(bento::mask::maskEmail("not-an-email"), "n***********");
}

void testEmailStartsWithAt() {
  CHECK_EQ(bento::mask::maskEmail("@example.com"), "@example.com");
}

void testIdCard18() {
  CHECK_EQ(bento::mask::maskIdCard("110101199001011234"), "110101********1234");
}

void testIdCard15() {
  CHECK_EQ(bento::mask::maskIdCard("123456789012345"), "123456*****2345");
}

void testIdCardFallback() {
  CHECK_EQ(bento::mask::maskIdCard("123"), "1*3");
  CHECK_EQ(bento::mask::maskIdCard("12345"), "1***5");
}

void testBankCardTypical() {
  CHECK_EQ(bento::mask::maskBankCard("6225881234567890"), "6225********7890");
}

void testBankCardShort() {
  CHECK_EQ(bento::mask::maskBankCard("12345678"), "1******8");
}

void testTokenTypical() {
  CHECK_EQ(bento::mask::maskToken("sk-1234567890abcdef"), "sk-1***********cdef");
}

void testTokenShort() {
  CHECK_EQ(bento::mask::maskToken("abc12345"), "********");
  CHECK_EQ(bento::mask::maskToken(""), "");
}

void testNameOneChar() {
  CHECK_EQ(bento::mask::maskName(bento::test::utf8CjkNameOne()), bento::test::utf8CjkNameOne());
}

void testNameTwoChars() {
  CHECK_EQ(bento::mask::maskName(bento::test::utf8CjkNameTwo()), bento::test::utf8CjkNameTwoMasked());
}

void testNameThreeChars() {
  CHECK_EQ(bento::mask::maskName(bento::test::utf8CjkNameThree()), bento::test::utf8CjkNameThreeMasked());
}

void testNameFourChars() {
  CHECK_EQ(bento::mask::maskName(bento::test::utf8CjkNameFour()), bento::test::utf8CjkNameFourMasked());
}

void testMaskMiddleTooShort() {
  CHECK_EQ(bento::mask::maskMiddle("abc", 2, 2), "***");
  CHECK_EQ(bento::mask::maskMiddle("ab", 1, 1), "**");
}

void testMaskMiddleUnicode() {
  CHECK_EQ(bento::mask::maskMiddle(bento::test::utf8CjkDigitsFive(), 1, 1), bento::test::utf8CjkMaskMiddleExpected());
}

void testMaskSecretTypical() {
  CHECK_EQ(bento::mask::maskSecret("vault:AES256:abcXYZ", 3), "vau***XYZ");
}

void testMaskSecretThreeStars() {
  const std::string longSecret(50, 'a');
  CHECK_EQ(bento::mask::maskSecret(longSecret + "END12345", 4), "aaaa***2345");
}

void testMaskSecretShort() {
  CHECK_EQ(bento::mask::maskSecret("abcdef", 3), "abcdef");
  CHECK_EQ(bento::mask::maskSecret("ab", 3), "ab");
}

void testMaskSecretSentinels() {
  CHECK_EQ(bento::mask::maskSecret(std::nullopt, 3), "<none>");
  CHECK_EQ(bento::mask::maskSecret("", 3), "<empty>");
}

void testMaskSecretKeepZero() {
  CHECK_EQ(bento::mask::maskSecret("hello", 0), "***");
}

void testMaskSecretUnicode() {
  CHECK_EQ(bento::mask::maskSecret(bento::test::utf8CjkDigitsEight(), 2), bento::test::utf8CjkMaskSecretExpected());
}

using TestFn = void (*)();

struct TestCase {
  const char *name;
  TestFn fn;
};

} // namespace

int main() {
  const TestCase tests[] = {
      {"phone well formed", testPhoneWellFormed},
      {"phone malformed", testPhoneMalformed},
      {"phone empty", testPhoneEmpty},
      {"email typical", testEmailTypical},
      {"email single char local", testEmailSingleCharLocal},
      {"email no at", testEmailNoAt},
      {"email starts with at", testEmailStartsWithAt},
      {"id card 18", testIdCard18},
      {"id card 15", testIdCard15},
      {"id card fallback", testIdCardFallback},
      {"bank card typical", testBankCardTypical},
      {"bank card short", testBankCardShort},
      {"token typical", testTokenTypical},
      {"token short", testTokenShort},
      {"name one char", testNameOneChar},
      {"name two chars", testNameTwoChars},
      {"name three chars", testNameThreeChars},
      {"name four chars", testNameFourChars},
      {"maskMiddle too short", testMaskMiddleTooShort},
      {"maskMiddle unicode", testMaskMiddleUnicode},
      {"maskSecret typical", testMaskSecretTypical},
      {"maskSecret three stars", testMaskSecretThreeStars},
      {"maskSecret short", testMaskSecretShort},
      {"maskSecret sentinels", testMaskSecretSentinels},
      {"maskSecret keep zero", testMaskSecretKeepZero},
      {"maskSecret unicode", testMaskSecretUnicode},
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
