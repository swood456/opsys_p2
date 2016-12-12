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
const int frameSize = 3;

struct FindHomeForWorstFit {
	std::string name() {
		return "Worst-Fit";
	}

	int operator() (int startLoc, int numFrames, std::vector<char> memory) {
		int finalLoc = -1;
		int biggestSize = 0;
		for (int i = 0; i < memSize; i++) {
			if (memory[i] == '.') {
				int curSize = 0;
				for (int j = i; j < memSize && memory[j] == '.'; j++) {
					curSize++;
				}

				if (curSize >= numFrames && biggestSize < curSize) {
					biggestSize = curSize;
					finalLoc = i;
				}
			}
		}

		return finalLoc;
	}
};

struct FindHomeForNextFit {
	std::string name() {
		return "Next-Fit";
	}

	int operator() (int startLoc, int numFrames, std::vector<char> memory){
		//std::cout << "    starting find home\n";
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
					//we found a match
					return i;
				} else{
					i = j;
				}
			}
		}

		for(int i = 0; i < startLoc; i++){
			if(memory[i] == '.'){
				//see if we have a large enough block of memory to save this process
				int blockSize = 0;

				int j = i;
				while(memory[j] == '.' && j < memSize){
					j++;
					blockSize++;
				}

				if(blockSize >= numFrames){
					//we found a match
					return i;
				} else{
					i = j;
				}
			}
		}
		return -1;
	}
};

struct FindHomeForBestFit {
	std::string name() {
		return "Best-Fit";
	}

	int operator() (int startLoc, int numFrames, std::vector<char> memory){
		int minBlockSize = 0;  //holds the smallest size of free partition
		startLoc = -1;
		for (int i=0; i<memSize; i++) {
			//if find a '.' determine the size of that free parititon block
			if (memory[i] == '.') {
				int blockSize = 0;
				int j = i;

				while ((memory[j] == '.') && (j < memSize)) {
					j++;
					blockSize++;
				}

				//if the process fits in this block and its size is smaller than what is already there then use this block
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

class Process
{
public:
	Process(char name_, int numFrames_, std::list<std::pair<int, int > > arrivalRunTimes_){
		name = name_;
		numFrames = numFrames_;
		arrivalRunTimes = arrivalRunTimes_;
	}


	/* data */
	char name;
	int numFrames;
	std::list<std::pair<int, int> > arrivalRunTimes;

	bool operator < (const Process& str) const{
		return name < str.name;
	}
};

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

void Contiguous_Next_Fit(std::list<Process>);
void Contiguous_Best_Fit(std::list<Process>);
void Contiguous_Worst_Fit(std::list<Process>);
void NonContiguous(std::list<Process>);

void VirtualLRU(std::vector<int>);
void VirtualLFU(std::vector<int>);
void VirtualOPT(std::vector<int>);

void printMemoryDiagram(std::vector<char> memory){
	//first, print the top line

	//for(int i = 0; i < )
	std::cout << "================================";

	for(int i = 0; i < (int)memory.size(); i++){
		if(i % 32 == 0){
			std::cout << '\n';
		}

		std::cout << memory[i];
	}

	std::cout << "\n================================\n";
}

int main(int argc, char* argv[]){
	//check command line arguments
	//will be in form of ./a.out memory_management_file virtual_file

	//parse input file
	std::ifstream inFile;
	inFile.open(argv[1]);

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


	std::list<Process> processes;

	while(std::getline(inFile, line)){
		if(line[0] != '#' && line[0] != ' ' && line[0] != '	' && line[0] != '\n' && line[0] != '\0'){

			std::list<std::pair<int, int> > arrivalRunTimes;

			processName = line[0];

			line.erase(0,2);

			char * token = strtok(const_cast<char*>(line.c_str()), " ");

			numMemFrames = atoi(token);

			int arrivalTime;
			int runTime;
			//go until we have all the process pair things
			token = strtok(NULL, " /");
			while(token != NULL){
				arrivalTime = atoi(token);
				token = strtok(NULL, " /");
				runTime = atoi(token);

				arrivalRunTimes.push_back(std::make_pair(arrivalTime, runTime));


				token = strtok(NULL, " /");

			}
			//Process p
			processes.push_back(Process(processName, numMemFrames, arrivalRunTimes));

		}
	}

	inFile.close();

	//read input from virtual input file
	inFile.open(argv[2]);
	if(!inFile){
		fprintf(stderr, "ERROR: invalid file for Virtual memory\n");
		return EXIT_FAILURE;
	}

	//get that file shit swaggie
	std::vector<int> virtualReferences;
	std::getline(inFile, line);
	//test print xd
	char * token = strtok(const_cast<char*>(line.c_str()), " ");
	while(token != NULL){
		virtualReferences.push_back(atoi(token));
		token = strtok(NULL, " ");
	}



	//notes start on 11-14

	//do Contiguous Memory Management
		//next-fit
		// Contiguous_Next_Fit(processes);
		// std::cout << std::endl;
		//best-fit
		// Contiguous_Best_Fit(processes);
		// std::cout << std::endl;

		//worst-fit
		// Contiguous_Worst_Fit(processes);
		// std::cout << std::endl;

	//Non-contiguous Memory Management
		// NonContiguous(processes);

	//virtual memory
		VirtualOPT(virtualReferences);
		std::cout << std::endl;
		VirtualLRU(virtualReferences);
		std::cout << std::endl;
		VirtualLFU(virtualReferences);
		std::cout << std::endl;

	return EXIT_SUCCESS;
}

void storeProcess(int startLoc, int numFrames, char processName, std::vector<char>& memory){
	for(int i = 0; i < numFrames; i++){
		memory[startLoc + i] = processName;
	}
}

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


	//sort the process array
	//std::sort(processes.begin(), processes.end());
	processes.sort();

	std::cout << "time 0ms: Simulator started (Contiguous -- " << placementalgorithm.name() << ")\n";

	while(processes.size() > 0){

		//go through each process in the array
		std::list<Process>::iterator itr;

		for(itr = processes.begin(); itr != processes.end(); itr++){
			//determine if this processes needs something

			//////////
			//ARIVAL//
			//////////
			if(itr->arrivalRunTimes.front().first == curTime){

				//announce that a process is arriving
				std::cout << "time " << curTime + defragTime<< "ms: Process " << itr->name << " arrived (requires " << itr->numFrames << " frames)\n";
				//process arriving
				if(itr->numFrames > freeMemory){
					//there is not enough memory for this process no matter what
					std::cout << "time " << curTime + defragTime << "ms: Cannot place process " << itr->name << " -- skipped!\n";
					printMemoryDiagram(memory);
					itr->arrivalRunTimes.pop_front();
					if(itr->arrivalRunTimes.size() == 0){
						//remove this process since it is now done
						processes.erase(itr);
						itr--;
					}

				} else{
					//determine if there is enough memory in a single block for this thing
					int storeLoc = placementalgorithm(lastSaved, itr->numFrames, memory);


					if(storeLoc == -1){
						std::cout << "time " << curTime + defragTime << "ms: Cannot place process "<< itr->name << " -- starting defragmentation\n";
						//defrag
						std::list<char> processesMoved;
						unsigned int numMoves = defragMemory(memory, processesMoved);

						//update timeing for everything
						defragTime += numMoves * t_memmove;

						//set the store loc to the new thing
						std::cout << "time "<< curTime + defragTime << "ms: Defragmentation complete (moved " << numMoves << " frames: ";
						std::list<char>::iterator listItr = processesMoved.begin();
						std::cout << *listItr;
						listItr++;

						for( ; listItr != processesMoved.end(); listItr++){
							std::cout << ", " << *listItr;
						}

						std::cout << ")\n";

						printMemoryDiagram(memory);

						storeLoc = memSize - freeMemory;

					}

					//store the process
					storeProcess(storeLoc, itr->numFrames, itr->name, memory);
					lastSaved = storeLoc + itr->numFrames;

					//print to the world that things have been saved
					std::cout << "time " << curTime + defragTime << "ms: Placed process " << itr->name << ":\n";
					printMemoryDiagram(memory);

					freeMemory -= itr->numFrames;


				}

			}

			////////////
			//FINISHES//
			////////////
			else if(curTime == itr->arrivalRunTimes.front().first + itr->arrivalRunTimes.front().second){
				//the process is done with its rund

				//first remove from memory
				removeFromMemory(itr->name, memory);

				//print that we removed it from memory
				std::cout << "time " << curTime + defragTime << "ms: Process "<<itr->name<<" removed:\n";
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

		curTime++;
	}

	std::cout << "time " << curTime + defragTime - 1 << "ms: Simulator ended (Contiguous -- " << placementalgorithm.name() << ")\n";
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


void storeProcess(int numFrames, char processName, std::vector<char>& memory) {
	for (int i=0; i<memSize; i++) {
		if (memory[i] == '.' && numFrames > 0) {
			memory[i] = processName;
			numFrames--;
		}
	}
}

void NonContiguous(std::list<Process> processes){
	int curTime = 0;
	std::vector<char> memory(memSize, '.');
	int freeMemory = memSize;


	//sort the process array
	//std::sort(processes.begin(), processes.end());
	processes.sort();

	std::cout << "time 0ms: Simulator started (Non-contiguous)\n";

	while(processes.size() > 0){

		//go through each process in the array
		std::list<Process>::iterator itr;

		for(itr = processes.begin(); itr != processes.end(); itr++){
			//determine if this processes needs something

			//////////
			//ARIVAL//
			//////////
			if(itr->arrivalRunTimes.front().first == curTime){

				//announce that a process is arriving
				std::cout << "time " << curTime << "ms: Process " << itr->name << " arrived (requires " << itr->numFrames << " frames)\n";
				//process arriving
				if(itr->numFrames > freeMemory){
					//there is not enough memory for this process no matter what
					std::cout << "time " << curTime << "ms: Cannot place process " << itr->name << " -- skipped!\n";
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

			////////////
			//FINISHES//
			////////////
			else if(curTime == itr->arrivalRunTimes.front().first + itr->arrivalRunTimes.front().second){
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

		curTime++;
	}

	std::cout << "time " << curTime - 1 << "ms: Simulator ended (Non-contiguous)\n";
}
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

void VirtualLRU(std::vector<int> pageRefs){

	std::cout << "Simulating LRU with fixed frame size of " << frameSize << std::endl;

	std::vector<VirtualFrame*> virtualMemory (frameSize, NULL);

	int replaceIndex;
	int furthestDistance;

	int faultCounter = 0;

	for(unsigned int i = 0; i < pageRefs.size(); i++){

		//update time thing
		for(int k = 0; k < frameSize; k++){
			if(virtualMemory[k] != NULL){
				virtualMemory[k]->counter++;
			}
		}

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
				if(virtualMemory[j]->page == pageRefs[i]){
					virtualMemory[j]->counter = 0;
					break;
				} else{
					if(virtualMemory[j]->counter > furthestDistance){
						replaceIndex = j;
						furthestDistance = virtualMemory[j]->counter;
					}
				}
			}
		}

		if(j == frameSize){
			//the current page is not in the virtual memeory
			//so page fault occurs

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
				// if(virtualMemory[j]->page != pageRefs[i]){
				// 	for (unsigned int k = i + 1; k < pageRefs.size(); ++k) {
				// 		if (pageRefs[k] == virtualMemory[j]->page
				// 			&& (k-i > furthestDistance
				// 				|| k-i == furthestDistance
				// 				&& virtualMemory[j]->page < virtualMemory[replaceIndex]->page)
				// 			) {
				// 			furthestDistance = k - i;
				// 			replaceIndex = j;
				// 			k = pageRefs.size();
				// 		}
				// 	}
				// } else {
				// 	break;
				// }
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
				// if (k + i + 1 == pageRefs.size())
				// 	k = 0;
				std::cout << "index " << j << " distance is " << k << std::endl;
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
