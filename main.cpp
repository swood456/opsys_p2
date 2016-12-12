#include <iostream>
#include <stdlib.h>
#include <vector>

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

	//parse input file
	//file format is NAME NUM_FRAMES ARRIVALTIME_0/RUNTIME_0 ARRIVALTIME_1/RUNTIME_1 ...

	//notes start on 11-14

	//do Contiguous Memory Management
		//next-fit

		//best-fit

		//worst-fit



	//Non-contiguous Memory Management



	//virtual memory

	return EXIT_SUCCESS;
}