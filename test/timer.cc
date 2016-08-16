// -*-coding:utf-8-unix;-*-
#include <iostream>
#include <thread>

#include "pi.hh"
#include "linux/fds.hh"

static void print_elapsed_time(void) {
  static struct timespec start;
  struct timespec curr;
  static int first_call = 1;
  int secs, nsecs;

  if (first_call) {
    first_call = 0;
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
      return;
  }

  if (clock_gettime(CLOCK_MONOTONIC, &curr) == -1) {
    return;
  }
  secs = curr.tv_sec - start.tv_sec;
  nsecs = curr.tv_nsec - start.tv_nsec;
  if (nsecs < 0) {
    secs--;
    nsecs += 1000000000;
  }
  std::cout << secs << "." << ((nsecs + 500000) / 1000000) << std::endl;
}
//
class RaftTimer : public base::Timer<RaftTimer> {
 public:
  void OnTimeout() {
    print_elapsed_time();
    auto timeout = base::RaftTimeout(index_);
    NextTimeout(timeout);
  }
  //
  bool Start() {
    index_ = 0;
    auto timeout = base::RaftTimeout(index_);
    return NextTimeout(timeout);
  }
 private:
  int index_;
};

int main() {
  RaftTimer timer;
  if (timer.Init()) {
    if (timer.Start()) {
      std::thread waiter([&timer](){
          while(true) {
            if(0 >= timer.Wait(-1)) {
              return;
            }
          }
        });
      waiter.join();
      return 0;
    }
    std::cout << "timer NextTimeout error" << std::endl;
  }
  std::cout << "timer Init error" << std::endl;
  return -1;
}
