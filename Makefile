CXX := clang++
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra -I. -fno-exceptions -fno-rtti
LDFLAGS := -pthread

TESTS := test/pi_test test/timer_test test/event_test

all: $(TESTS)

clean:
	@rm -rf *~ test/*~ $(TESTS)
