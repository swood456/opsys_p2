#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>

const int t_memmove = 1; //time to move 1 frame of memory in defrag

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

	std::cout << "\n================================";
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
	int numProcesses;
	std::getline(inFile, line);
	numProcesses = atoi(line.c_str());

	char processName;
	int numMemFrames;
	//std::vector<std::pair<int, int> > arrivalRunTimes (numProcesses);

	while(std::getline(inFile, line)){
		if(line[0] != '#' && line[0] != ' ' && line[0] != '	' && line[0] != '\n' && line[0] != '\0'){
			
			//std::cout << "got line " << line << std::endl;
			
			processName = line[0];

			line.erase(0,2);
			//get numMemFrames;

			char * token = strtok(const_cast<char*>(line.c_str()), " ");

			numMemFrames = atoi(token);

			std:: cout << "process name " << processName << " numMemFrames = " << numMemFrames << std::endl;

			int arrivalTime;
			int runTime;
			//go until we have all the process pair things
			token = strtok(NULL, " /");
			while(token != NULL){
				arrivalTime = atoi(token);
				//std::cout << " token: |" << token <<"|\n";
				token = strtok(NULL, " /");
				runTime = atoi(token);

				std::cout << " arrival time: " << arrivalTime << " run time: " << runTime << std::endl;


				token = strtok(NULL, " /");

			}

		}
	}


	//notes start on 11-14

	//do Contiguous Memory Management
		//next-fit

		//best-fit

		//worst-fit



	//Non-contiguous Memory Management



	//virtual memory

	return EXIT_SUCCESS;
}