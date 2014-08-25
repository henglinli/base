#include <iostream>
#include "pi.hh"

int main() {
  std::cout << "kSeed=" << base::kPi1kSeed << std::endl
	    << "pi1k=" << sizeof(base::kPi1k) << std::endl
	    << base::kPi1k << std::endl;

  for(auto i(0); i < 10000; ++i) {
    auto timeout = base::RaftTimeout(i);
    std::cout << "timeout=" << timeout << std::endl;
  }
}
