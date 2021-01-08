all: c++11.out c++14.out
	@echo All tests have passed

%.out: test.cpp ../unique_resource.hpp Makefile
	c++ -std=$(patsubst %.out,%, $@) -I.. -Werror -Wall -Wextra -Wshadow -Wno-variadic-macros test.cpp -o test.$@
	./test.$@
