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
unsigned int defragMemory(std::vector<char>& memory) {
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
	return numFramesMoved * t_memmove;
}

void Contiguous_Next_Fit(std::list<Process>);

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
	//printf("Hello world\n");
	std::cout << "hello world" <<std::endl;

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
	Contiguous_Next_Fit(processes);
		//best-fit

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

int findHomeForNextFit(int startLoc, int numFrames, std::vector<char> memory){
	std::cout << "    starting find home\n";
	for(int i = startLoc; i < memSize; i++){
		if(memory[i] == '.'){
			//see if we have a large enough block of memory to save this process
			int blockSize = 0;

			int j = i;
			while(memory[j] == '.' && j < memSize){
				j++;
				blockSize++;
			}
			std::cout << "blockSize = " << blockSize << "starting at index i=" << i << " numFrames=" << numFrames << std::endl;
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
			int blockSize = 1;

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

void storeProcess(int startLoc, int numFrames, char processName, std::vector<char>& memory){
	for(int i = 0; i < numFrames; i++){
		memory[startLoc + i] = processName;
	}
}

void Contiguous_Next_Fit(std::list<Process> processes){
	int curTime = 0;
	std::vector<char> memory(memSize, '.');
	int freeMemory = memSize;
	int lastSaved = 0;
	int defragTime = 0;


	//sort the process array
	//std::sort(processes.begin(), processes.end());
	processes.sort();

	std::cout << "time 0ms: Simulator started (Contiguous -- Next-Fit)\n";

	while(processes.size() > 0){

		//go through each process in the array
		std::list<Process>::iterator itr;

		for(itr = processes.begin(); itr != processes.end(); itr++){
			//determine if this processes needs something
			if(itr->arrivalRunTimes.front().first == curTime){

				//announce that a process is arriving
				std::cout << "time " << curTime + defragTime<< "ms: Process " << itr->name << " arrived (requires " << itr->numFrames << " frames)\n";
				//process arriving
				if(itr->numFrames > freeMemory){
					//there is not enough memory for this process no matter what
					std::cout << "time " << curTime + defragTime << "ms: Cannot place process " << itr->name << " -- skipped!\n";
					itr->arrivalRunTimes.pop_front();
					if(itr->arrivalRunTimes.size() == 0){
						//remove this process since it is now done
						processes.erase(itr);
						itr--;
					}

				} else{
					//determine if there is enough memory in a single block for this thing
					std::cout << "   startloc: " << lastSaved << std::endl; 
					int storeLoc = findHomeForNextFit(lastSaved, itr->numFrames, memory);


					if(storeLoc == -1){
						std::cout << "time " << curTime + defragTime << "ms: Cannot place process "<< itr->name << " -- starting defragmentation\n";
						//defrag
						std::list<char> processesMoved;
						unsigned int numMoves = defragMemory(memory);

						//update timeing for everything
						defragTime += numMoves * t_memmove;

						//set the store loc to the new thing
						std::cout << "time "<< curTime + defragTime << "ms: Defragmentation complete (moved " << itr->name << " frames: B, C, D, E, F)\n";

					}

					//store the process
					storeProcess(storeLoc, itr->numFrames, itr->name, memory);
					lastSaved = storeLoc + itr->numFrames;

					//print to the world that things have been saved
					std::cout << "time " << curTime + defragTime << "ms: Placed process " << itr->name << ":\n";
					printMemoryDiagram(memory);

					freeMemory -= itr->numFrames;


				}

				//do the arrival stuff
			}
		}

	}

}
