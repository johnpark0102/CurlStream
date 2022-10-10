#include <curlstream/CurlStream.hpp>
#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif  // WIN32

namespace cs = curlstream;

int main() {
  try {
#ifdef WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif  // WIN32

    

    return 0;
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -2;
  } catch (...) {
    return -1;
  }
}
