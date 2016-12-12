#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <list>
#include <stdio.h>

const int t_memmove = 1; //time to move 1 frame of memory in defrag
const int memSize = 256; //size of the memory block
const int frameSize = 3; //number of virtual frames avilable

//struct to simplify the code for contiguous function
//makes it so that the function to determine where to store the
//memory can be passed into a function

//Worst Fit Code
struct FindHomeForWorstFit {
	//string used for printing
	std::string name() {
		return "Worst-Fit";
	}
	//function used to determine where in the memory to put the memory
	//returns -1 if there is no block of memory that will fit the entire process
	int operator() (int startLoc, int numFrames, std::vector<char> memory) {
		int finalLoc = -1;
		int biggestSize = 0;
		//go from the beginning on the memory vector
		for (int i = 0; i < memSize; i++) {
			//see if this is a free block in memory
			if (memory[i] == '.') {
				//this is a free block of memory, so calculate its size
				int curSize = 0;
				//keep counting until a used block of memory is found
				for (int j = i; j < memSize && memory[j] == '.'; j++) {
					curSize++;
				}

				//determine if this is large enough to hold the pocess,
				// and if it is, if it is larger (worse) than the previous largest
				if (curSize >= numFrames && biggestSize < curSize) {
					biggestSize = curSize;
					finalLoc = i;
				}
			}
		}

		//return the location that fits (-1 if none exist)
		return finalLoc;
	}
};

//Find Next Code
struct FindHomeForNextFit {
	//string used for printing
	std::string name() {
		return "Next-Fit";
	}

	//function used to determine where in the memory to put the memory
	//returns -1 if there is no block of memory that will fit the entire process
	int operator() (int startLoc, int numFrames, std::vector<char> memory){
		//iterate through the memory, starting at the end of the most recently placed
		// process, which is passed in
		for(int i = startLoc; i < memSize; i++){
			if(memory[i] == '.'){
				//see if we have a large enough block of memory to save this process
				int blockSize = 0;

				int j = i;
				while(memory[j] == '.' && j < memSize){
					j++;
					blockSize++;
				}

				if(blockSize >= numFrames){
					//this block that we found is large enough to hold the process
					//since this is next-fit, we just want the first one found
					return i;
				} else{
					//this block is not large enough, so start looking for a larger one
					// right after this one (don't recheck everything)
					i = j;
				}
			}
		}
		//we could not find space after the end of the most recently placed process, so
		// look for one from the start of memory
		for(int i = 0; i < startLoc; i++){
			if(memory[i] == '.'){
				//see if we have a large enough block of memory to save this process
				int blockSize = 0;

				int j = i;
				//count the size of the found memory block
				while(memory[j] == '.' && j < memSize){
					j++;
					blockSize++;
				}

				if(blockSize >= numFrames){
					//we found a block of memory large enough for the process
					// so we store it there
					return i;
				} else{
					i = j;
				}
			}
		}
		return -1;
	}
};

//best fit
struct FindHomeForBestFit {
	//have a name for best fit for printing
	std::string name() {
		return "Best-Fit";
	}

	//function used to determine where in the memory to put the memory
	//returns -1 if there is no block of memory that will fit the entire process
	int operator() (int startLoc, int numFrames, std::vector<char> memory){
		int minBlockSize = 0;  //holds the smallest size of free partition
		startLoc = -1;
		for (int i=0; i<memSize; i++) {
			//if find a '.' determine the size of that free parititon block
			if (memory[i] == '.') {
				int blockSize = 0;
				int j = i;
				//determine the size of the memory block
				while ((memory[j] == '.') && (j < memSize)) {
					j++;
					blockSize++;
				}

				//if the process fits in this block and its size is smaller
				// than what is already there then use this block
				if ( ((minBlockSize == 0) || (minBlockSize > blockSize)) && blockSize >= numFrames) {
					minBlockSize = blockSize;
					startLoc = j - blockSize;
				}

				i = j;
			}
		}

		return startLoc;
	}
};

//class that stores information for contiguous and non-contiguous memory
class Process
{
public:
	//constructor
	Process(char name_, int numFrames_, std::list<std::pair<int, int > > arrivalRunTimes_){
		name = name_;
		numFrames = numFrames_;
		arrivalRunTimes = arrivalRunTimes_;
	}


	/* data */
	char name;
	int numFrames;
	std::list<std::pair<int, int> > arrivalRunTimes;

	// operator to sort the processes by name, so that ties are broken that way
	bool operator < (const Process& str) const{
		return name < str.name;
	}
};

//class that stores information for virtual frames
class VirtualFrame{
public:
	VirtualFrame(int page_, int count_){
		page = page_;
		counter = count_;
	}
	int page;
	int counter;
};

// defragment memory function for contiguous memory schemes
// 	returns number of processes moved
//	 as well as a list of processes moved (passed by reference)
unsigned int defragMemory(std::vector<char>& memory, std::list<char>& procsMoved) {
	bool keepGoing = true;	// tell function to continue
	unsigned int pindex = 0;	// keeps track of the first period found
	unsigned int numFramesMoved = 0;	// keeps track of number of frames moved
	while (keepGoing) {
		if (pindex >= memory.size()) {
			// we have reached the end of memory, so stop
			keepGoing = false;
		} else if (memory[pindex] == '.') {
			// we found a period
			// search for the next letter and place it here
			unsigned int i;
			for (i = pindex; i < memory.size(); ++i ) {
				if (memory[i] != '.') {
					if (procsMoved.back() != memory[i]) {
						// add this process to the list of moved processes
						//	if it's not already in the list
						procsMoved.push_back(memory[i]);
					}
					memory[pindex] = memory[i];
					memory[i] = '.';
					numFramesMoved++;
					break;
				}
			}

			// if we failed to find a letter,
			//	stop searching
			if (i == memory.size()) {
				keepGoing = false;
			}
		}
		pindex++;
	}

	// return number of frames moved
	return numFramesMoved;
}

//function prototypes for contiguous memory
void Contiguous_Next_Fit(std::list<Process>);
void Contiguous_Best_Fit(std::list<Process>);
void Contiguous_Worst_Fit(std::list<Process>);

//function prototype for non-contiguous memory
void NonContiguous(std::list<Process>);

//function prototype for virtual memory
void VirtualLRU(std::vector<int>);
void VirtualLFU(std::vector<int>);
void VirtualOPT(std::vector<int>);

//function that prints a memory diagram for contiguous and non-contiguous
void printMemoryDiagram(std::vector<char> memory){
	//first, print the top line

	std::cout << "================================";

	//print the values, as well as newlines every 32 chars
	for(int i = 0; i < (int)memory.size(); i++){
		if(i % 32 == 0){
			std::cout << '\n';
		}

		std::cout << memory[i];
	}
	//print the end line
	std::cout << "\n================================\n";
}

int main(int argc, char* argv[]){
	//check command line arguments
	//will be in form of ./a.out memory_management_file virtual_file

	//parse input file
	std::ifstream inFile;
	inFile.open(argv[1]);

	//make sure input file 1 opened correctly
	if(!inFile){
		fprintf(stderr, "ERROR: invalid file for Contiguous and Non-Contiguous memory\n");
		return EXIT_FAILURE;
	}


	std::string line;

	//get the number of processes
	//int numProcesses;
	std::getline(inFile, line);
	//numProcesses = atoi(line.c_str());

	char processName;
	int numMemFrames;


	//create a list of the processes to be used for contiguous and non-contiguous
	std::list<Process> processes;

	while(std::getline(inFile, line)){
		//if the line is not a comment or whitespace
		if(line[0] != '#' && line[0] != ' ' && line[0] != '	' && line[0] != '\n' && line[0] != '\0'){
			//this line matches to a process. Create a new Process object and add it to the list

			//make a list of the arrival and run times for this process
			std::list<std::pair<int, int> > arrivalRunTimes;

			//get the name of the process
			processName = line[0];

			//remove the name from the line
			line.erase(0,2);

			//tokenize the input string to get the frame size
			char * token = strtok(const_cast<char*>(line.c_str()), " ");

			//convert the frame size to an int
			numMemFrames = atoi(token);

			//loop through until all arrival and departure times are found
			int arrivalTime;
			int runTime;
			token = strtok(NULL, " /");

			while(token != NULL){
				arrivalTime = atoi(token);
				token = strtok(NULL, " /");
				runTime = atoi(token);

				//add a pair into the list that is a corresponding arrival and run time
				arrivalRunTimes.push_back(std::make_pair(arrivalTime, runTime));

				token = strtok(NULL, " /");

			}
			//add the completed process into the list of processes
			processes.push_back(Process(processName, numMemFrames, arrivalRunTimes));

		}
	}
	//close the file for contiguous and nno-contiguous
	inFile.close();

	//read input from virtual input file
	inFile.open(argv[2]);
	if(!inFile){
		fprintf(stderr, "ERROR: invalid file for Virtual memory\n");
		return EXIT_FAILURE;
	}

	//make a vector of ints to correspond to the input from the file
	std::vector<int> virtualReferences;

	//read in the file into a string
	std::getline(inFile, line);

	//break the string up into tokens divided by spaces
	char * token = strtok(const_cast<char*>(line.c_str()), " ");

	//add each number into the vector
	while(token != NULL){
		virtualReferences.push_back(atoi(token));
		token = strtok(NULL, " ");
	}

	//function calls to do contiguous simulations
		//next-fit
		Contiguous_Next_Fit(processes);
		std::cout << std::endl;
		//best-fit
		Contiguous_Best_Fit(processes);
		std::cout << std::endl;

		//worst-fit
		Contiguous_Worst_Fit(processes);
		std::cout << std::endl;

	//Non-contiguous Memory Management
		NonContiguous(processes);
		std::cout << std::endl;

	//function calls to do virtual memory
		VirtualOPT(virtualReferences);
		std::cout << std::endl;
		VirtualLRU(virtualReferences);
		std::cout << std::endl;
		VirtualLFU(virtualReferences);

	return EXIT_SUCCESS;
}

//helper function that stores values into an array of memory at a given index and a given number
// of values to write
void storeProcess(int startLoc, int numFrames, char processName, std::vector<char>& memory){
	for(int i = 0; i < numFrames; i++){
		memory[startLoc + i] = processName;
	}
}

//removes from the memory every instance of the given character and replaces it with .
void removeFromMemory(char processName, std::vector<char>& memory){
	for(int i = 0; i < memSize; i++){
		if(memory[i] == processName){
			memory[i] = '.';
		}
	}
}


template <typename Algo>
void SimulateContiguous(std::list<Process> processes, Algo placementalgorithm ){
	int curTime = 0;
	std::vector<char> memory(memSize, '.');
	int freeMemory = memSize;
	int lastSaved = 0;
	int defragTime = 0;


	//sort the processes list, so that ties are broken in alphabetical order
	processes.sort();

	//announce start of simulation
	std::cout << "time 0ms: Simulator started (Contiguous -- "
		<< placementalgorithm.name() << ")\n";

	//keep going until all processes are finished
	while(processes.size() > 0){

		//go through each process in the list
		std::list<Process>::iterator itr;


			////////////
			//FINISHES//
			////////////
		for(itr = processes.begin(); itr != processes.end(); itr++){
			if(curTime == itr->arrivalRunTimes.front().first + itr->arrivalRunTimes.front().second){
				//the process is finished with its run, so remove it from memory

				//first remove from memory
				removeFromMemory(itr->name, memory);

				//print that we removed it from memory
				std::cout << "time " << curTime + defragTime << "ms: Process "<<itr->name<<" removed:\n";
				printMemoryDiagram(memory);
				freeMemory += itr->numFrames;

				//remove this arrival from the process's list of arrivals
				itr->arrivalRunTimes.pop_front();

				//determine if there are any more processes to run for this process
				if(itr->arrivalRunTimes.size() == 0){
					//if there are no more processes to run, remove this process
					processes.erase(itr);
					itr--;
				}

			}
		}

		for(itr = processes.begin(); itr != processes.end(); itr++){
			//////////
			//ARIVAL//
			//////////

			//determine if the current time matches the time of an arival
			if(itr->arrivalRunTimes.front().first == curTime){

				//announce that a process is arriving
				std::cout << "time " << curTime + defragTime<< "ms: Process " <<
					itr->name << " arrived (requires " << itr->numFrames << " frames)\n";

				if(itr->numFrames > freeMemory){
					//there is not enough memory for this process no matter what
					std::cout << "time " << curTime + defragTime << "ms: Cannot place process "
						<< itr->name << " -- skipped!\n";
					printMemoryDiagram(memory);
					itr->arrivalRunTimes.pop_front();

					//see if there are any more arrivals after this point
					if(itr->arrivalRunTimes.size() == 0){
						//remove this process, there are no more arrivals
						processes.erase(itr);
						itr--;
					}

				} else{
					//determine if there is enough memory in a single block for this process
					int storeLoc = placementalgorithm(lastSaved, itr->numFrames, memory);

					if(storeLoc == -1){
						//the placement algortihm could not find a
						// contiguous block of memory for the process
						std::cout << "time " << curTime + defragTime << "ms: Cannot place process "
							<< itr->name << " -- starting defragmentation\n";

						//run defrag
						std::list<char> processesMoved;
						unsigned int numMoves = defragMemory(memory, processesMoved);

						//update timeing for everything
						defragTime += numMoves * t_memmove;

						//announce that defrag has occured
						std::cout << "time "<< curTime + defragTime <<
							"ms: Defragmentation complete (moved " << numMoves << " frames: ";

						//iterate through the list of blocks that were moved by defrag and print them
						std::list<char>::iterator listItr = processesMoved.begin();
						std::cout << *listItr;
						listItr++;

						for( ; listItr != processesMoved.end(); listItr++){
							std::cout << ", " << *listItr;
						}

						std::cout << ")\n";

						printMemoryDiagram(memory);

						//calculate the new location for this write to occur
						storeLoc = memSize - freeMemory;

					}

					//store the process
					storeProcess(storeLoc, itr->numFrames, itr->name, memory);
					lastSaved = storeLoc + itr->numFrames;

					//print to the world that things have been saved
					std::cout << "time " << curTime + defragTime <<
						"ms: Placed process " << itr->name << ":\n";

					printMemoryDiagram(memory);

					freeMemory -= itr->numFrames;


				}

			}
		}

		

		curTime++;
	}

	std::cout << "time " << curTime + defragTime - 1 <<
		"ms: Simulator ended (Contiguous -- " << placementalgorithm.name() << ")\n";
}

void Contiguous_Next_Fit(std::list<Process> processes) {
	struct FindHomeForNextFit fhfnf;
	SimulateContiguous(processes, fhfnf);
}

void Contiguous_Best_Fit(std::list<Process> processes){
	struct FindHomeForBestFit fhfbf;
	SimulateContiguous(processes, fhfbf);
}

void Contiguous_Worst_Fit(std::list<Process> processes) {
	struct FindHomeForWorstFit fhfwf;
	SimulateContiguous(processes, fhfwf);
}

//helper function to store a noncontiguous process
void storeProcess(int numFrames, char processName, std::vector<char>& memory) {
	//iterate through the block of memory
	for (int i=0; i<memSize; i++) {
		//see if there is a free block of memory and there is more to store
		if (memory[i] == '.' && numFrames > 0) {
			//store part of the process here
			memory[i] = processName;
			numFrames--;
		}
	}
}

//function that handles non-contiguous memory thing
void NonContiguous(std::list<Process> processes){
	int curTime = 0;
	std::vector<char> memory(memSize, '.');
	int freeMemory = memSize;


	//sort the process list
	processes.sort();

	std::cout << "time 0ms: Simulator started (Non-contiguous)\n";

	while(processes.size() > 0){

		//go through each process in the list
		std::list<Process>::iterator itr;

		for(itr = processes.begin(); itr != processes.end(); itr++){
			////////////
			//FINISHES//
			////////////
			if(curTime == itr->arrivalRunTimes.front().first + itr->arrivalRunTimes.front().second){
				//the process is done with its rund

				//first remove from memory
				removeFromMemory(itr->name, memory);

				//print that we removed it from memory
				std::cout << "time " << curTime << "ms: Process "<<itr->name<<" removed:\n";
				printMemoryDiagram(memory);
				freeMemory += itr->numFrames;

				//remove from the list of arrivaltimethings, and remove the process if that list is now empty
				itr->arrivalRunTimes.pop_front();
				if(itr->arrivalRunTimes.size() == 0){
					processes.erase(itr);
					itr--;
				}

			}
		}

		for(itr = processes.begin(); itr != processes.end(); itr++){
			//////////
			//ARIVAL//
			//////////
			if(itr->arrivalRunTimes.front().first == curTime){

				//announce that a process is arriving
				std::cout << "time " << curTime << "ms: Process " <<
					itr->name << " arrived (requires " << itr->numFrames << " frames)\n";
				//process arriving
				if(itr->numFrames > freeMemory){
					//there is not enough memory for this process no matter what
					std::cout << "time " << curTime <<
						"ms: Cannot place process " << itr->name << " -- skipped!\n";
					printMemoryDiagram(memory);
					itr->arrivalRunTimes.pop_front();
					if(itr->arrivalRunTimes.size() == 0){
						//remove this process since it is now done
						processes.erase(itr);
						itr--;
					}

				} else{
					//break up storing the process in wherever it fits
					storeProcess(itr->numFrames, itr->name, memory);

					//print to the world that things have been saved
					std::cout << "time " << curTime << "ms: Placed process " << itr->name << ":\n";
					printMemoryDiagram(memory);

					freeMemory -= itr->numFrames;


				}

			}
		}



		curTime++;
	}

	std::cout << "time " << curTime - 1 << "ms: Simulator ended (Non-contiguous)\n";
}

//hepler function that prints virtual memory
void printVirtualMemory(std::vector<VirtualFrame*> virtualMemory){
	std::cout << "[mem:";
	for(int i = 0; i < frameSize; i++){
		if(virtualMemory[i] == NULL){
			std::cout << " .";
		} else{

			std::cout << " " <<virtualMemory[i]->page;
		}
	}

	std::cout << "]";

}

//function that simulates LRU virtual
void VirtualLRU(std::vector<int> pageRefs){

	std::cout << "Simulating LRU with fixed frame size of " << frameSize << std::endl;

	//make an array of size frameSize VirtualFrame pointers, initialized to null
	std::vector<VirtualFrame*> virtualMemory (frameSize, NULL);

	int replaceIndex;
	int furthestDistance;

	int faultCounter = 0;

	//iterate through the list of pageRefs
	for(unsigned int i = 0; i < pageRefs.size(); i++){

		//update the counter for every object in the virtual memory
		for(int k = 0; k < frameSize; k++){
			if(virtualMemory[k] != NULL){
				virtualMemory[k]->counter++;
			}
		}

		//initialize the variables to keep track of which was used least recently
		furthestDistance = -1;
		unsigned int j;

		//look through the virtual memory and see if the curen page is there, or if there are null
		// pages that should be filled in
		for(j = 0; j < frameSize; j++){
			if(virtualMemory[j] == NULL){
				//there is a NULL piece of memory, and the current page is not stored
				// a page fault occurs with no victim
				//add the page into the virtual memory
				virtualMemory[j] = new VirtualFrame(pageRefs[i], 0);

				std::cout << "referencing page " << pageRefs[i] << " ";
				printVirtualMemory(virtualMemory);
				std::cout << " PAGE FAULT (no victim page)\n";

				faultCounter++;

				break;
			} else{
				//see if this is the same page as the reference
				if(virtualMemory[j]->page == pageRefs[i]){
					//the piece of memory we are looking at corresponds to the page
					// being referenced now, so reset the last used counter
					virtualMemory[j]->counter = 0;
					break;
				} else{
					//this is not the same page being referene, preemptively determine
					// if this should be the one that is replaces
					if(virtualMemory[j]->counter > furthestDistance){
						replaceIndex = j;
						furthestDistance = virtualMemory[j]->counter;
					}
				}
			}
		}

		//see if we were able to find this page ref in the memory
		if(j == frameSize){
			//the current page is not in the virtual memeory
			// so page fault occurs

			//replace the thing with the index we found
			int victimPage = virtualMemory[replaceIndex]->page;

			virtualMemory[replaceIndex]->page = pageRefs[i];
			virtualMemory[replaceIndex]->counter = 0;

			std::cout << "referencing page " << pageRefs[i] << " ";
			printVirtualMemory(virtualMemory);
			std::cout << " PAGE FAULT (victim page " << victimPage << ")\n";
			faultCounter++;

		}
	}

	std::cout << "End of LRU simulation ("<< faultCounter << " page faults)\n";
}

/*  Function that uses the LFU algorithm for page replacements in virtual memory  */
void VirtualLFU(std::vector<int> pageRefs){

	std::cout << "Simulating LFU with fixed frame size of " << frameSize << std::endl;

	std::vector<VirtualFrame*> virtualMemory (frameSize, NULL);

	int faultCounter = 0;

	for(unsigned int i = 0; i < pageRefs.size(); i++){

		int replaceIndex;  //index of frame in virtual memory to replace
		int leastTimesUsed = -1;  //var to hold the the least number of accesses to frame in memory
		int lowestPage;  //var to hold the lowest page that is LFU (used for breaking ties)
		unsigned int j;  //counter to loop through virtual memory

		//looking through what is in virtual memory
		for(j = 0; j < frameSize; j++){
			if(virtualMemory[j] == NULL){
				//if there is nothing in this space, then this page doesn't exist in virtual memory
				virtualMemory[j] = new VirtualFrame(pageRefs[i], 1);

				std::cout << "referencing page " << pageRefs[i] << " ";
				printVirtualMemory(virtualMemory);
				std::cout << " PAGE FAULT (no victim page)\n";

				faultCounter++;

				break;

			} else{
				//if current page is in virtual memory, increase its counter
				if(virtualMemory[j]->page == pageRefs[i]){
					virtualMemory[j]->counter++;
					break;
				} else{
					//if current page is not in virtual memory then find the page with
					//the lowest number of accesses; if there is a tie, use the page
					//with the lowest numbered page
					if( (leastTimesUsed == -1) || (virtualMemory[j]->counter < leastTimesUsed) ||
						(virtualMemory[j]->counter == leastTimesUsed && virtualMemory[j]->page < lowestPage) ){
						replaceIndex = j;
						leastTimesUsed = virtualMemory[j]->counter;
						lowestPage = virtualMemory[j]->page;
					}
				}
			}
		}

		//the current page is not in the virtual memory
		//so page fault occurs
		if(j == frameSize){

			//replace the frame in virtual memory with the current page
			//using the index we found
			virtualMemory[replaceIndex]->page = pageRefs[i];
			virtualMemory[replaceIndex]->counter = 1;

			std::cout << "referencing page " << pageRefs[i] << " ";
			printVirtualMemory(virtualMemory);
			std::cout << " PAGE FAULT (victim page " << lowestPage << ")\n";
			faultCounter++;

		}
	}

	std::cout << "End of LFU simulation ("<< faultCounter << " page faults)\n";
}

void VirtualOPT(std::vector<int> pageRefs){

	std::cout << "Simulating OPT with fixed frame size of " << frameSize << std::endl;

	std::vector<VirtualFrame*> virtualMemory (frameSize, NULL);

	int replaceIndex;
	int furthestDistance;

	int faultCounter = 0;

	for(unsigned int i = 0; i < pageRefs.size(); i++){
		replaceIndex = 0;
		furthestDistance = -1;
		unsigned int j;

		//see if the current thing is already in there
		for(j = 0; j < frameSize; j++){
			if(virtualMemory[j] == NULL){



				virtualMemory[j] = new VirtualFrame(pageRefs[i], 0);

				std::cout << "referencing page " << pageRefs[i] << " ";
				printVirtualMemory(virtualMemory);
				std::cout << " PAGE FAULT (no victim page)\n";

				faultCounter++;

				break;
			} else{
				if (virtualMemory[j]->page == pageRefs[i]) {
					break;
				}
			}
		}

		if(j == frameSize){
			//the current page is not in the virtual memeory
			//so page fault occurs

			for (j = 0; j < frameSize; ++j) {
				unsigned int k;
				for (k = 0; k + i + 1 < pageRefs.size(); ++k) {
					if (virtualMemory[j]->page == pageRefs[k + i + 1]) {
						break;
					}
				}

				if (furthestDistance < (int)k || (furthestDistance == (int)k && virtualMemory[j]->page < virtualMemory[replaceIndex]->page)) {
					furthestDistance = k;
					replaceIndex = j;
				}
			}

			//replace the thing with the index we found
			int victimPage = virtualMemory[replaceIndex]->page;

			virtualMemory[replaceIndex]->page = pageRefs[i];
			virtualMemory[replaceIndex]->counter = 0;

			std::cout << "referencing page " << pageRefs[i] << " ";
			printVirtualMemory(virtualMemory);
			std::cout << " PAGE FAULT (victim page " << victimPage << ")\n";
			faultCounter++;

		}
	}

	std::cout << "End of OPT simulation ("<< faultCounter << " page faults)\n";
}
