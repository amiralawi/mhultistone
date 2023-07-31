# mhultistone
The simplest method to get a multicore Dhrystone score is to use fork.  Unfortunatley, this also adds a large amount of overhead.  This strategy is also at the mercy of the scheduler, and one thread completing early will lead to under-reporting performance.  The trick is to add more forks with shorter run-times to reduce benchmark aliasing - this doesn't scale well with fork which adds O(n) overhead.

The original Dhrystone utility performs a bunch of dumb calculations on a pre-allocated set of memory.  The benchmark is stateful - that is, manipulating that allocated memory in another benchmarking thread will cause an error in the first thread's benchmarking process.  This will likely lead to pointer references to unallocated memory causing an immedaite program crash.  Because of this stateful behavior, and additional working set of memory has to be preallocated for each thread.  Dynamically allocating memory for each thread will add additional pointer dereferences to each benchmarking calculation step, leading to much lower scores even for a single-threaded benchmark.  In order to overcome this limitation, this project uses a python script to generate a separate code-path for each thread.  This allows memory accesses to be hardcoded into the final assembly and eliminates nearly all of the multithreaded overhead.

# requirements
* python 3 to create virtual environment
* gcc
* make
