# IMS projekt 2021
# xpokor78, xandra05

CC=g++
CPPFLAGS=-Wextra -pedantic -Wall -O2 -std=c++11 -g
LDFLAGS= -lsimlib -lm

.PHONY: all sim

all: sim

sim:
	$(CC) $(CPPFLAGS) -o sim sim.cpp $(LDFLAGS)

run:
	./sim

clean:
	rm -f sim ./*.out
