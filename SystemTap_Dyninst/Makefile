DTS      = /opt/rh/devtoolset-11/root
CXXFLAGS = -g -I /usr/include/dyninst
DYNINSTLIBS = $(DTS)/usr/lib64/dyninst


.PHONY: all
all: count exercise

count: count.cpp
	g++ $(CXXFLAGS) count.cpp -I /usr/include/dyninst -c
	g++ $(CXXFLAGS) count.o -L $(DYNINSTLIBS) -ldyninstAPI -o count.elf

exercise: exercise.cpp
	g++ $(CXXFLAGS) exercise.cpp -o exercise.elf

.PHONY: clean
clean:
	rm -rf *~ *.o count.elf exercise.elf