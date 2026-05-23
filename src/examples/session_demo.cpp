#include <bento/id/session.hpp>

#include <iostream>

int main() {
  bento::id::setSessionPrefix("myapp");
  const std::string sessionId = bento::id::generateSessionId();
  std::cout << "session_id=" << sessionId << '\n';
  return 0;
}
