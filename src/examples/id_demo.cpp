#include <bento/bento.hpp>

#include <iostream>

int main() {
  bento::id::setSessionPrefix("myapp");
  std::cout << "sessionId=" << bento::id::generateSessionId() << '\n';
  std::cout << "uuidV4=" << bento::id::uuidV4() << '\n';
  std::cout << "uuidV7=" << bento::id::uuidV7() << '\n';
  std::cout << "nanoid=" << bento::id::nanoid() << '\n';
  std::cout << "randomInt=" << bento::id::randomInt(0, 100) << '\n';
  std::cout << "randomString=" << bento::id::randomString(12) << '\n';
  return 0;
}
