#include <bento/mask/mask.hpp>

#include <iostream>

int main() {
  std::cout << "phone=" << bento::mask::maskPhone("13812345678") << '\n';
  std::cout << "email=" << bento::mask::maskEmail("alice@example.com") << '\n';
  std::cout << "token=" << bento::mask::maskToken("sk-1234567890abcdef") << '\n';
  std::cout << "name=" << bento::mask::maskName("Alice") << '\n';
  std::cout << "secret=" << bento::mask::maskSecret("vault:AES256:abcXYZ", 3) << '\n';
  return 0;
}
