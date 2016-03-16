========================================================================
    CONSOLE APPLICATION : hw4 Cache Simulator
========================================================================

Purpose: This program is a cache simulator which exemplified four common
cache algorithms: FUlly Associative, Direct-Mapped, 2-Way and 4-Way Associative.

Input: .txt file consisting of four parameters: cache size in words (16/32/64),
block size of 2 or 4 words, associativity code (0,1,2,4 for Direct, Fully, 2-
and 4-way respectively), and LRU choice (0 or 1 doe off or on), each on a separate
line.  The other file is a trace file consisting of hexadecimal memory addresses
of 2-byte size (e.g. 0x5c41).  The program is called with the two text files
as arguments, e.g. (in Linux) hw4 config.txt trace.txt

Output: A list of each memory location from the trace file and "Hit" or "Miss"
depending on whether the tag was found in the virtual cache.  Additional 
information such as set index in decimal, tag values, etc. may be obtained by 
uncommenting cout lines in each function. 

/////////////////////////////////////////////////////////////////////////////

Compilation: Use g++ to compile the main file, e.g. g++ hw4.cpp and then
use ./a.out config.txt trace.txt to run the program

/////////////////////////////////////////////////////////////////////////////
Other notes:


/////////////////////////////////////////////////////////////////////////////
