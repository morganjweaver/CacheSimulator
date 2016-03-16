// hw4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<stdlib.h>
#include<vector>
#include<fstream>
#include<time.h>
#include<bitset>

using namespace std;

class Cache {
public:
	Cache() {
		ID = 0; timeStamp = 0; 
	}
	int ID;
	time_t timeStamp;
};

// Signatures for all four associativity types; Ways takes 2- and 4-way configs

void DirectMapped(vector<int> &addr, int csize, int bsize);
void FullyAssoc(vector<int> &addr, int csize, int bsize, bool LRU);
void Ways(vector<int> &addr, int csize, int bsize, bool LRU, int ways);

int main(int argc, char *argv[])
{
	if (argc != 3) // argc should be 2 for correct execution
				   // We print argv[0] assuming it is the program name
	{
		cout << "Please try again with two arguments.\n";
		return -1;
	}

	string config = argv[1];
	string trace = argv[2];
	string line;

	int dataSize;
	int blockSize;
	int assoc;
	int LRU;
	bool boolLRU = true;

	ifstream file(config.c_str());

	if (file)
	{
		while (file.good())//while there are remaining characters...
		{
			//CHECKED! Reading in correct values.
			file >> dataSize;
			file >> blockSize;
			file >> assoc;
			file >> LRU;
			cout << "\ndata size: " << dataSize << "  block size: " << blockSize << "  associativity: " << assoc << "  LRU? " << LRU << endl;
		}
		// Validate config input
		if (!(dataSize == 16 || dataSize == 32 || dataSize == 64 ))
		{
			cout << "Sorry, the config file was not valid." << endl;
			return -1;
		}
		if(!( blockSize == 2 || blockSize == 4 ))
		{ 
			cout << "Sorry, the config file was not valid." << endl;
			return -1;
		}
		if (!(assoc == 0 || assoc == 1 || assoc == 2 || assoc == 4))
		{
			cout << "Sorry, the config file was not valid." << endl;
			return -1;
		}
		if (!(LRU == 0 || LRU == 1))
		{
			cout << "Sorry, the config file was not valid." << endl;
			return -1;
		}
		if (LRU == 0)
		{
			boolLRU = false;
		}
		
		file.close();
	}

	ifstream tFile(trace.c_str());

	vector<int> addresses;

	if (tFile)
	{
		int i = 0;
		int temp;
		while (tFile.good()) //CHECKED: Hex read-in working. Have vector of all values.
		{
			tFile >> hex >> temp;
			addresses.push_back(temp);
			//cout << hex << addresses[i] << endl;
			i++;
		}
		tFile.close();
		if (addresses.empty())
		{
			cout << "Sorry, the tracefile was not valid." << endl;
			return -1;
		}

		
	}
	//cout << "Assoc: " << assoc << endl;
	if (assoc == 0)
	{
		DirectMapped(addresses, dataSize, blockSize);
	}
	else if (assoc == 1)
	{
		FullyAssoc(addresses, dataSize, blockSize, boolLRU);
	}
	else {
		Ways(addresses, dataSize, blockSize, boolLRU, assoc);
	}

	cout << "\nPress any key to close.";
	cin.get();
	return 0;
}

void DirectMapped(vector<int> &addr, int csize, int bsize) {
	int addressSize = addr.size();
	int cachelines = csize / bsize; //i.e., cache capacity words (data portion) divided by block size
	int blockOffset = log2(bsize); // bits needed for block offset
	int setIndex = log2(cachelines); //bits needed for set index
	int tag = 32 - blockOffset - setIndex; //tag size

	cout << "\n\nSizes:" << " Address queue size: " << dec << addressSize  << " cache lines: " << cachelines << "  block offset: " << blockOffset << " set index: " << dec << setIndex << "  tag: " << dec << tag << endl;
	vector<Cache> cache(cachelines); //create vector of cache objects containing
	//number of initialized cache objects equal to correct number of cache lines
	
	for (int i = 0; i < addressSize; i++) // For each memory location request....
	{
		//cout << "\n Initial bin rep of memory address: " << bitset<16>(addr[i]) << endl;
		//grab first two bits for block offset
		int k = addr[i] & ((1 << blockOffset) - 1);
		//cout << "Bin rep of block offset: " << bitset<16>(k) << "\n";
			
		//grab middle n bits for set index
		unsigned  mask;
		mask = ((1 << setIndex) - 1) << blockOffset;
		unsigned setIndexBits = (addr[i] & mask) >> blockOffset;
	
		cout << "dec rep of set index:" << dec << (setIndexBits) << "\n";

		//grab tag
		mask = ((1 << tag) - 1) << blockOffset + setIndex;
		unsigned tagBits = (addr[i] & mask) >> blockOffset >> setIndex;
		cout << "dec rep of tag: " << dec << (tagBits) << "\n";

		
			if(cache[setIndexBits].ID == tagBits)
		{
			cout << hex << addr[i] << "          " << "Hit\n";
		}
			else
			{
				cout << hex << addr[i] << "          " << "Miss\n";
				cache[setIndexBits].ID = tagBits;
			}

	}
}

void FullyAssoc(vector<int> &addr, int csize, int bsize, bool LRU) {
	int addressSize = addr.size();
	int cachelines = csize / bsize; //i.e., cache capacity words (data portion) divided by block size
	int blockOffset = log2(bsize); // bits needed for block offset
	int tag = 32 - blockOffset; //tag size
	int clock = 1;

	cout << "\n\nSizes:" << " cache lines: " << cachelines << "  block offset: " << blockOffset << "  tag: " << dec << tag << endl;

	vector<Cache> cache(cachelines);

	for (int i = 0; i < addressSize; i++) // For each memory location request....
	{
		bool placed = false;

		//grab first log2(block size) bits for block offset
		int k = addr[i] & ((1 << blockOffset) - 1);

		//grab tag
		unsigned mask = ((1 << tag) - 1) << blockOffset;
		unsigned tagBits = (addr[i] & mask) >> blockOffset;

		// Now the fun part: find an empty spot for the tag!
		if (placed == false)
		{
			for (int i = 0; i < cachelines; i++) //see if tag already in cache
			{
				if (cache[i].ID == tagBits)
				{
					cout << hex << addr[i] << "          " << "Hit\n";
					cache[i].timeStamp = clock;
					clock++;
					placed = true;
				}
			}
		}
		//if not, see if there's an empty space
		if (placed == false) {
			int index = 0;

			while (index < cachelines && placed == false)
			{
				if (cache[index].ID == 0)
				{
					cache[index].ID = tagBits;
					cache[index].timeStamp = clock;
					clock++;
					cout << hex << addr[index] << "          " << "Miss\n";
					placed = true;
				}
				index++;
			}
		}
		//otherwise, use LRU to evict appropriate cache slot
		if (placed == false && LRU == 1) {

			int leastIndex = 0;
			for (int k = 1; k < cachelines; k++)
			{
				if (cache[k].timeStamp < cache[leastIndex].timeStamp)
				{
					leastIndex = k;
				}
			}
			cache[leastIndex].ID = tagBits;
			cache[leastIndex].timeStamp = clock;
			clock++;
			cout << hex << addr[leastIndex] << "          " << "Miss\n";
			placed = true;
		}
		if (placed == false && LRU == 0) {
		
				int randIndex;
				srand(time(NULL));
				randIndex = rand() % cachelines;
				cache[randIndex].ID = tagBits;
				cache[randIndex].timeStamp =  clock;
				clock++;
				cout << hex << addr[randIndex] << "          " << "Miss\n";
				placed = true;
			
		}
	}
}


void Ways(vector<int> &addr, int csize, int bsize, bool LRU, int ways) {
	int addressSize = addr.size();
	int cachelines = csize / (bsize*ways); //i.e., cache capacity words (data portion) divided by block size*cache lines per line
	int blockOffset = log2(bsize); // bits needed for block offset
	int setIndex = log2(cachelines); //bits needed for set index
	int tag = 32 - blockOffset - setIndex; //tag size
	int clock = 1;

    //cout << "\n\nSizes:" << " Address queue size: " << dec << addressSize  << " cache lines: " << cachelines << "  block offset: " << blockOffset << " set index: " << dec << setIndex << "  tag: " << dec << tag << endl;
	vector<Cache> cache(cachelines*ways); //create vector of cache objects containing correct number of lines*ways
	
	/*For the n-way associative cache, I use an array containing cache lines * ways and simply use arithmetic to give each set of sets its own range of memory 
	in the array.  Set index = set index*ways + subset index Example: subset 3 of line 5 of a 4-way associative cache is found at 5*4 + 2 = cache[22]*/


	for (int i = 0; i < addressSize; i++) // For each memory location request....
	{
		//cout << "\n Initial bin rep of memory address: " << bitset<16>(addr[i]) << endl;
		//grab first n bits for block offset
		int k = addr[i] & ((1 << blockOffset) - 1);
		//cout << "Bin rep of block offset: " << bitset<16>(k) << "\n";
		bool placed = false; 
		
		//grab middle n bits for set index
		int subset = 0; //offset for subset index
		unsigned  mask;
		mask = ((1 << setIndex) - 1) << blockOffset;
		unsigned setIndexBits = (addr[i] & mask) >> blockOffset;
		

		//cout << "index rep of set index:" << dec << setIndexBits << "\n";
		//bitset<16>(setIndexBits)
		
		//grab tag
		mask = ((1 << tag) - 1) << blockOffset + setIndex;
		unsigned tagBits = (addr[i] & mask) >> blockOffset >> setIndex;
		//cout << " dec rep of tag: " << dec << tagBits  << "\n";
		//bitset<16>(tagBits)
		
		while (!placed)
		{
			while (subset < ways && !placed)
			{
				if (cache[setIndexBits*ways + subset].ID == tagBits) //actual set index = set index*ways
				{
					cache[setIndexBits*ways + subset].timeStamp = clock;
					clock++;
					cout << hex << addr[i] << "          " << "Hit\n";
					placed = true;
				}
				subset++;
			}
			//if tag not found on cache line or its sub-lines:
			if (!placed)
			{
				subset = 0; //reset

				while (subset < ways && !placed)
			{
				//check range of subsets of each cache line for vacancy

				if (cache[setIndexBits*ways + subset].ID == 0) //functional set index = set index*ways
				{
					cache[setIndexBits*ways + subset].ID = tagBits;
					cache[setIndexBits*ways + subset].timeStamp = clock;
					clock++;
					cout << hex << addr[i] << "          " << "Miss\n";
					placed = true;
				}
				subset++;
			}
				//if no vacancy, evict LRU subset line
				if (!placed && LRU == 1) {
					subset = 0; //reset
					int leastIndex = setIndexBits*ways; //least recently used SUBSET
					for (int k = setIndexBits*ways+subset; k < setIndexBits*ways + ways; k++)
					{
						if (cache[k].timeStamp < cache[leastIndex].timeStamp)
						{
							leastIndex = k;
						}
					}
					cache[leastIndex].ID = tagBits;
					cache[leastIndex].timeStamp = clock;
					clock++;
					cout << hex << addr[i] << "          " << "Miss\n";
					placed = true;
				}
				//If no LRU, evict random line

				if (!placed && LRU == 0) {
					int ind = setIndexBits*ways;
					int randIndex;
					srand(time(NULL));
					randIndex = rand() % ways;
					cache[ind+randIndex].ID = tagBits;
					cache[ind+randIndex].timeStamp = clock;
					clock++;
					cout << hex << addr[i] << "          " << "Miss\n";
					placed = true;
					}

				}
				}
		}
	}




