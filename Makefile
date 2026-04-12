.PHONY: all demo

all: demo server

server: my_server.cpp database_impl/server.cpp
	g++ -o $@ $^ -lpthread


DEMOS := $(wildcard demo/*.cpp)
DEMO_EXE := $(patsubst %.cpp, %, $(DEMOS))

demo: $(DEMO_EXE)

%: %.cpp
	g++ -o $@ $< database_impl/client.cpp


clean:
	rm $(DEMO_EXE) server