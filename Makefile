CXX := clang++
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra -I.
LDFLAGS :=

TESTS := test/pi_test

all: $(TESTS)

clean:
	@rm -rf *~ test/*~ $(TESTS)
