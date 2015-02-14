// -*-coding:utf-8-unix;-*-
#include <iostream>
#include <thread>

#include "linux/fds.hh"

class StateMachine
    : public base::UserEvent<StateMachine> {
 public:
  void OnEvent(eventfd_t event) {
    auto state = static_cast<State>(event);
    switch (state) {
      case kFollower: {
        std::cout << "become follower" << std::endl;
        break;
      }
      case kCandidate: {
        std::cout << "become candidate" << std::endl;
        break;
      }
      case kLeader: {
        std::cout << "become leader" << std::endl;
        break;
      }
      default: {
        std::cout << "bad state" << std::endl;
        break;
      }
    }
  }
  //
  bool BecomeFollwer() {
    auto state = static_cast<eventfd_t>(kFollower);
    return Notify(state);
  }
  bool BecomeCadidate() {
    auto state = static_cast<eventfd_t>(kCandidate);
    return Notify(state);
  }
  bool BecomeLeader() {
    auto state = static_cast<eventfd_t>(kLeader);
    return Notify(state);
  }
 private:
  enum State {
    kFollower = 1,
    kCandidate = 1<<1,
    kLeader = 1<<2
  };
};

int main() {
  StateMachine sm;
  if(sm.Init()) {
    std::thread waiter([&sm](){
        while(true) {
          if(0 >= sm.Wait(-1)) {
            return;
          }
        }
      });
    std::cout << "create waiter done" << std::endl;
    std::thread sender([&sm](){
        int i = 1000;
        while(--i) {
          std::this_thread::sleep_for(std::chrono::seconds(1));
          sm.BecomeLeader();
          std::this_thread::sleep_for(std::chrono::seconds(1));
          sm.BecomeFollwer();
          std::this_thread::sleep_for(std::chrono::seconds(1));
          sm.BecomeCadidate();
        }
      });
    std::cout << "create sender done" << std::endl;
    sender.join();
    waiter.join();
    return 0;
  }
  std::cout << "StateMachine Init error" << std::endl;
  return -1;
}
