all: version, procs

version: version.cpp
	g++ -o version version.cpp -fopenmp

procs: procs.cpp
	export OMP_PROC_BIND=true
	export OMP_PLACES=threads
	g++ -o procs procs.cpp -fopenmp

.PHONY: all version procs
