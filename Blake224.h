/* 
 * File:   Blake224.h
 * Author: Piotr
 *
 * Created on 12 listopada 2015, 22:23
 */

#ifndef BLAKE224_H
#define	BLAKE224_H
#include <string>
#include <vector>

class Blake224 {
private:
	std::string input;
	size_t msg_length;
	std::string output;
	int blocks;
	bool additional_block;
	
	std::vector<uint32_t> words;
	std::vector<uint32_t> m;
	std::vector<uint32_t> constants;
	std::vector<uint32_t> chain;
	std::vector< std::vector< int > > permutation;
	std::vector<uint32_t> salt;
	uint64_t counter;
	
	std::vector<uint32_t> v;
	
	void pad();
	void updateV(int block_iterator);
	void G(int r, int i, uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d);
	uint32_t rightRotation(uint32_t value, unsigned int count);
	void updateCounter(int block_iterator);
	
public:
	Blake224(char *input);
	Blake224(const Blake224& orig);
	virtual ~Blake224();
	void calculate();
	bool isError();
	std::string getError();
	void printHash();
	void printState(std::string what);
	int getBlockCount();
};

#endif	/* BLAKE224_H */