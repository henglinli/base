# -*-coding:utf-8-unix;-*-
CXX := clang++
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra -I. -fno-exceptions -fno-rtti
LDFLAGS := -pthread

LINUX := test/timer_test test/event_test
TESTS := test/pi_test test/queue_test

all: $(TESTS)
linux: ${LINUX} ${TEST}

clean:
	@rm -rf *~ test/*~ $(TESTS) ${LINUX}
