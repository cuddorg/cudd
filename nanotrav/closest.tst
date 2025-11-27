# Nanotrav Version #0.13, Release date 2015/7/15
# nanotrav/nanotrav -p 1 -reordering genetic -drop -closest ./nanotrav/closest.blif
# CUDD Version 3.0.0
Order before final reordering
a b c 
Number of inputs = 3
BDD reordering with genetic: from 7 to ... 7 nodes in 0 sec
New order
c a b 
Testing Cudd_bddClosestCube
TEST-CC:: H(f, f)
T-F    : 4 nodes 1 leaves 3 minterms
T-G    : 4 nodes 1 leaves 3 minterms
T-C (0) : 3 nodes 1 leaves 2 minterms
T-GN   : 4 nodes 1 leaves 5 minterms
T-N (1) : 4 nodes 1 leaves 1 minterms
TEST-CC:: H(f, g)
T-F    : 4 nodes 1 leaves 3 minterms
T-G    : 4 nodes 1 leaves 1 minterms
T-C (2) : 4 nodes 1 leaves 1 minterms
T-GN   : 4 nodes 1 leaves 7 minterms
T-N (0) : 3 nodes 1 leaves 2 minterms
TEST-CC:: H(f, h)
T-F    : 4 nodes 1 leaves 3 minterms
T-G    : 4 nodes 1 leaves 1 minterms
T-C (0) : 4 nodes 1 leaves 1 minterms
T-GN   : 4 nodes 1 leaves 7 minterms
T-N (0) : 4 nodes 1 leaves 1 minterms
TEST-CC:: H(g, f)
T-F    : 4 nodes 1 leaves 1 minterms
T-G    : 4 nodes 1 leaves 3 minterms
T-C (2) : 4 nodes 1 leaves 1 minterms
T-GN   : 4 nodes 1 leaves 5 minterms
T-N (0) : 4 nodes 1 leaves 1 minterms
TEST-CC:: H(g, g)
T-F    : 4 nodes 1 leaves 1 minterms
T-G    : 4 nodes 1 leaves 1 minterms
T-C (0) : 4 nodes 1 leaves 1 minterms
T-GN   : 4 nodes 1 leaves 7 minterms
T-N (1) : 4 nodes 1 leaves 1 minterms
TEST-CC:: H(g, h)
T-F    : 4 nodes 1 leaves 1 minterms
T-G    : 4 nodes 1 leaves 1 minterms
T-C (3) : 4 nodes 1 leaves 1 minterms
T-GN   : 4 nodes 1 leaves 7 minterms
T-N (0) : 4 nodes 1 leaves 1 minterms
TEST-CC:: H(h, f)
T-F    : 4 nodes 1 leaves 1 minterms
T-G    : 4 nodes 1 leaves 3 minterms
T-C (0) : 4 nodes 1 leaves 1 minterms
T-GN   : 4 nodes 1 leaves 5 minterms
T-N (1) : 4 nodes 1 leaves 1 minterms
TEST-CC:: H(h, g)
T-F    : 4 nodes 1 leaves 1 minterms
T-G    : 4 nodes 1 leaves 1 minterms
T-C (3) : 4 nodes 1 leaves 1 minterms
T-GN   : 4 nodes 1 leaves 7 minterms
T-N (0) : 4 nodes 1 leaves 1 minterms
TEST-CC:: H(h, h)
T-F    : 4 nodes 1 leaves 1 minterms
T-G    : 4 nodes 1 leaves 1 minterms
T-C (0) : 4 nodes 1 leaves 1 minterms
T-GN   : 4 nodes 1 leaves 7 minterms
T-N (1) : 4 nodes 1 leaves 1 minterms
End of test.  Performed 0 recursive calls.
**** CUDD modifiable parameters ****
Hard limit for cache size: 2796202
Cache hit threshold for resizing: 30%
Garbage collection enabled: yes
Limit for fast unique table growth: 1677721
Maximum number of variables sifted per reordering: 1000000
Maximum number of variable swaps per reordering: 1000000000
Maximum growth while sifting a variable: 1.2
Dynamic reordering of BDDs enabled: no
Default BDD reordering method: 4
Dynamic reordering of ZDDs enabled: no
Default ZDD reordering method: 4
Realignment of ZDDs to BDDs enabled: no
Realignment of BDDs to ZDDs enabled: no
Dead nodes counted in triggering reordering: no
Group checking criterion: 7
Recombination threshold: 0
Symmetry violation threshold: 10
Arc violation threshold: 10
GA population size: 0
Number of crossovers for GA: 0
Next reordering threshold: 22
**** CUDD non-modifiable parameters ****
Memory in use: 3183048
Peak number of nodes: 1022
Peak number of live nodes: 18
Number of BDD variables: 3
Number of ZDD variables: 0
Number of cache entries: 32768
Number of cache look-ups: 91
Number of cache hits: 32
Number of cache insertions: 60
Number of cache collisions: 0
Number of cache deletions: 9
Cache used slots = 0.17% (expected 0.16%)
Soft limit for cache size: 4096
Number of buckets in unique table: 1024
Used buckets in unique table: 4.98% (expected 5.11%)
Number of BDD and ADD nodes: 54
Number of ZDD nodes: 0
Number of dead BDD and ADD nodes: 42
Number of dead ZDD nodes: 0
Total number of nodes allocated: 57
Total number of nodes reclaimed: 153
Garbage collections so far: 1
Time for garbage collection: 0.00 sec
Reorderings so far: 1
Time for reordering: 0.00 sec
Final size: 7
total time = 0.00 sec
Runtime Statistics
------------------
Machine name: runnervmg1sw1
User time      0.0 seconds
System time    0.0 seconds

Average resident text size       =     0K
Average resident data+stack size =     0K
Maximum resident size            =  5992K

Virtual memory limit             = unlimited (unlimited)
Major page faults = 0
Minor page faults = 380
Swaps = 0
Input blocks = 0
Output blocks = 8
Context switch (voluntary) = 0
Context switch (involuntary) = 1
