CFLAGS := -I. -ggdb

all: turing

turing: turing.cpp
	g++ $(CFLAGS) -O1 $^ -o $@

clean:
	rm -f turing

.PHONY: clean