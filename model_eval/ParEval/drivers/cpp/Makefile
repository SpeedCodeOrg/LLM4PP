CXX = g++
MPICXX = mpicxx
CXX_FLAGS = -std=c++17 -O3

SERIAL_DRIVERS = $(patsubst %.cc,%.o, $(wildcard */*serial-driver.cc))
OMP_DRIVERS = $(patsubst %.cc,%.o, $(wildcard */omp-driver.cc))

ALL_DRIVERS = $(SERIAL_DRIVERS) $(OMP_DRIVERS) 

all: $(ALL_DRIVERS)

%.o: %.cc
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

%serial-driver.o: %serial-driver.cc
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

%omp-driver.o: %omp-driver.cc
	$(CXX) $(CXX_FLAGS) -fopenmp -o $@ -c $<

clean:
	rm -f $(ALL_DRIVERS)
