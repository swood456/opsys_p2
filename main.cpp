#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <list>

const int t_memmove = 1; //time to move 1 frame of memory in defrag
const int memSize = 256; //size of the memory block

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
				//std::cout << "blockSize = " << blockSize << "starting at index i=" << i << " numFrames=" << numFrames << std::endl;
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
				//std::cout << "     blockSize = " << blockSize << "starting at index i=" << i << " numFrames=" << numFrames << std::endl;

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

// defragment memory function for contiguous memory schemes
// returns time required to defragment memory.
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

	// return number of frames moved multiplied
	//	by the amount of time required  to move each frame.
	return numFramesMoved;
}

void Contiguous_Next_Fit(std::list<Process>);
void Contiguous_Best_Fit(std::list<Process>);
void Contiguous_Worst_Fit(std::list<Process>);

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

			//std::cout << "got line " << line << std::endl;
			std::list<std::pair<int, int> > arrivalRunTimes;

			processName = line[0];

			line.erase(0,2);
			//get numMemFrames;

			char * token = strtok(const_cast<char*>(line.c_str()), " ");

			numMemFrames = atoi(token);

			//std:: cout << "process name " << processName << " numMemFrames = " << numMemFrames << std::endl;

			int arrivalTime;
			int runTime;
			//go until we have all the process pair things
			token = strtok(NULL, " /");
			while(token != NULL){
				arrivalTime = atoi(token);
				//std::cout << " token: |" << token <<"|\n";
				token = strtok(NULL, " /");
				runTime = atoi(token);

				//std::cout << " arrival time: " << arrivalTime << " run time: " << runTime << std::endl;
				arrivalRunTimes.push_back(std::make_pair(arrivalTime, runTime));


				token = strtok(NULL, " /");

			}
			//Process p
			processes.push_back(Process(processName, numMemFrames, arrivalRunTimes));

		}
	}
	//checking
	/*for(int i = 0; i < processes.size(); i++){
		std::cout << processes[i].name << " " << processes[i].numFrames << " ";
		for(int j = 0; j < processes[i].arrivalRunTimes.size(); j++){
			std::cout << processes[i].arrivalRunTimes[j].first << "/" << processes[i].arrivalRunTimes[j].second << " ";
		}
		std::cout << '\n';
	}*/




	//notes start on 11-14

	//do Contiguous Memory Management
		//next-fit
	// Contiguous_Next_Fit(processes);
	Contiguous_Next_Fit(processes);
	//best-fit
	Contiguous_Best_Fit(processes);
	Contiguous_Worst_Fit(processes);

		//worst-fit

/*for(int i = 0; i < processes.size(); i++){
		std::cout << processes[i].name << " " << processes[i].numFrames << " ";
		for(int j = 0; j < processes[i].arrivalRunTimes.size(); j++){
			std::cout << processes[i].arrivalRunTimes[j].first << "/" << processes[i].arrivalRunTimes[j].second << " ";
		}
		std::cout << '\n';
	}*/

	//Non-contiguous Memory Management



	//virtual memory

	return EXIT_SUCCESS;
}

/*  Function that finds the location to place the process using the Best Fit model
	Places the process in the smallest free partition available  */
int findHomeForBestFit(int numFrames, std::vector<char> memory) {
	int minBlockSize = 0;  //holds the smallest size of free partition
	int startLoc = -1;
	for (int i=0; i<memSize; i++) {
		//if find a '.' determine the size of that free parititon block
		if (memory[i] == '.') {
			int blockSize = 0;
			int j = i;

			while ((memory[j] == '.') && (j < memSize)) {
				j++;
				blockSize++;
			}

			if ( ((minBlockSize == 0) || (minBlockSize > blockSize)) && blockSize >= numFrames) {
				minBlockSize = blockSize;
				startLoc = j - blockSize;
			}

			i = j;
		}
	}

	return startLoc;
}

int findHomeForNextFit(int startLoc, int numFrames, std::vector<char> memory){
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
			//std::cout << "blockSize = " << blockSize << "starting at index i=" << i << " numFrames=" << numFrames << std::endl;
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
			//std::cout << "     blockSize = " << blockSize << "starting at index i=" << i << " numFrames=" << numFrames << std::endl;

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
					//std::cout << "   startloc: " << lastSaved << std::endl;
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
	int curTime = 0;
	std::vector<char> memory(memSize, '.');
	int freeMemory = memSize;
	// int lastSaved = 0;
	int defragTime = 0;


	//sort the process array
	//std::sort(processes.begin(), processes.end());
	processes.sort();

	std::cout << "time 0ms: Simulator started (Contiguous -- Best-Fit)\n";

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
					//std::cout << "   startloc: " << lastSaved << std::endl;
					int storeLoc = findHomeForBestFit(itr->numFrames, memory);


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

	std::cout << "time " << curTime + defragTime - 1 << "ms: Simulator ended (Contiguous -- Best-Fit)\n";

void Contiguous_Worst_Fit(std::list<Process> processes) {
	struct FindHomeForWorstFit fhfwf;
	SimulateContiguous(processes, fhfwf);
}
