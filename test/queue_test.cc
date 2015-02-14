// -*-coding:utf-8-unix;-*-
#include <iostream>
#include "queue.hh"

using namespace ::base::atomic;
class Slist
    : public SlistEntry<Slist> {
};

int main() {
  SlistHead<Slist> header;
  auto first = header.Next();
  first->Next();
  Slist one;
  std::cout << one.Next() << " " << header.Next() << std::endl;
  std::cout << header.Insert(&one) << std::endl;;
  std::cout << one.Next() << " " << header.Next() << std::endl;
  return 0;
}
