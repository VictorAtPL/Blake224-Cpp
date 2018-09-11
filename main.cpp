/* 
 * File:   main.cpp
 * Author: Piotr
 *
 * Created on 9 listopada 2015, 16:16
 */

#include <iostream>
#include <regex>
#include <string>
#include "Blake224.h"
/*
 * 
 */
int main(int argc, char** argv) {
	using std::cout;
	
	char *input;
	
	if (argc == 2) {
		input = argv[1];
	}
	else {
		input = (char *)malloc(sizeof(char));
		*input = '\0';
	}
	
	Blake224 blake(input);
	blake.calculate();
	blake.printHash();
	
	free(input);
	
	return 0;
}