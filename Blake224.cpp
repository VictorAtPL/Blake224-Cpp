/* 
 * File:   Blake224.cpp
 * Author: Piotr
 * 
 * Created on 12 listopada 2015, 22:23
 */

#include "Blake224.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <iomanip>
#include <cmath>
#include <stdint.h>
#include <limits.h>

Blake224::Blake224(char *input): permutation(10, std::vector<int>(16)), v(16), m(16) {
	// ustaw stale
	this->constants = {0x243F6A88, 0x85A308D3, 0x13198A2E, 0x03707344, 0xA4093822, 0x299F31D0, 0x082EFA98, 0xEC4E6C89,
	0x452821E6, 0x38D01377, 0xBE5466CF, 0x34E90C6C, 0xC0AC29B7, 0xC97C50DD, 0x3F84D5B5, 0xB5470917};
	
	// ustaw h
	this->chain = { 0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939, 0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4 };
	
	// ustaw permutacje
	this->permutation = { 
		{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
		{14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
		{11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
		{7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
		{9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
		{2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
		{12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
		{13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
		{6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
		{10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0}
	};
	
	// ustaw salt
	this->salt = {0, 0, 0, 0};
	
	// ustaw counter
	this->counter = 0;
	
	// zaladuj wejscie
	this->input = std::string(input);
	
	// jeden hex to 4 bity
	this->msg_length = this->input.length() * 4;
}

Blake224::Blake224(const Blake224& orig) {
}

Blake224::~Blake224() {
}

void Blake224::calculate() {
	this->pad();
	
	for (int block_iterator = 1; block_iterator <= this->blocks; block_iterator++) {
		this->updateV(block_iterator);
		
		std::vector<std::uint32_t>::const_iterator first = (this->words).begin() + (block_iterator - 1) * 16;
		std::vector<std::uint32_t>::const_iterator last = first + 16;
		
		copy(first, last, (this->m).begin());
		
		// 14 rund
		for (int r = 0; r < 14; r++) {			
			this->G(r, 0, this->v[0], this->v[4], this->v[8], this->v[12]);
			this->G(r, 1, this->v[1], this->v[5], this->v[9], this->v[13]);
			this->G(r, 2, this->v[2], this->v[6], this->v[10], this->v[14]);
			this->G(r, 3, this->v[3], this->v[7], this->v[11], this->v[15]);	
			this->G(r, 4, this->v[0], this->v[5], this->v[10], this->v[15]);
			this->G(r, 5, this->v[1], this->v[6], this->v[11], this->v[12]);
			this->G(r, 6, this->v[2], this->v[7], this->v[8], this->v[13]);
			this->G(r, 7, this->v[3], this->v[4], this->v[9], this->v[14]);
		}
		
		this->chain[0] = this->chain[0] xor this->salt[0] xor this->v[0] xor this->v[8];
		this->chain[1] = this->chain[1] xor this->salt[1] xor this->v[1] xor this->v[9];
		this->chain[2] = this->chain[2] xor this->salt[2] xor this->v[2] xor this->v[10];
		this->chain[3] = this->chain[3] xor this->salt[3] xor this->v[3] xor this->v[11];
		this->chain[4] = this->chain[4] xor this->salt[0] xor this->v[4] xor this->v[12];
		this->chain[5] = this->chain[5] xor this->salt[1] xor this->v[5] xor this->v[13];
		this->chain[6] = this->chain[6] xor this->salt[2] xor this->v[6] xor this->v[14];
		this->chain[7] = this->chain[7] xor this->salt[3] xor this->v[7] xor this->v[15];
	}
}

void Blake224::G(int r, int i, uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
	r = (r > 9) ? (r % 10) : r;	
	
	a = a + b + (this->m[ this->permutation[ r ][ 2 * i ] ] xor this->constants[ this->permutation[ r ][ 2 * i + 1 ] ]);
	d = this->rightRotation(d xor a, 16);
	c = c + d;	
	b = this->rightRotation(b xor c, 12);
	a = a + b + (this->m[ this->permutation[ r ][ 2 * i + 1 ] ] xor this->constants[ this->permutation[ r ][ 2 * i ] ]);
	d = this->rightRotation(d xor a, 8);
	c = c + d;
	b = this->rightRotation(b xor c, 7);
}

uint32_t Blake224::rightRotation(uint32_t value, unsigned int count) {
	uint32_t y = value >> count;
	uint32_t z = value << (32 - count);

	return (y | z);
}

void Blake224::printHash() {
	char prev;
	
	prev = std::cout.fill ('0');
	std::ios_base::fmtflags ff;
	ff = std::cout.flags();
	
	std::cout.setf ( std::ios::hex, std::ios::basefield );
	
	std::vector<uint32_t>::iterator begin, end;
	begin = (this->chain).begin();
	end = begin + 7;
	
	for(std::vector<uint32_t>::iterator it = begin; it != end; ++it) {	
		std::cout << std::setw(8) << *it;
	}
	
	std::cout.fill(prev);
	std::cout.flags(ff);
}

void Blake224::pad() {	
	// jezeli brak wielokrotnosci hexa jako 8 tzn, ze bedzie niepelne slowo wiec dopisz 0 do stringa	
	int append = 0;
	if (this->msg_length % 32 > 0) {
		append = (32 - this->msg_length % 32) / 4;
		this->input.append(append, '0');
	}
	
	std::string word_str;
			
	// dopoki wejsciowy string jest dluzszy od 0
	while ((this->input).length() > 1) {
		// pierwsze 8 znakow
		word_str = (this->input).substr(0, 8);
		
		// usun te znaki
		(this->input).erase(0, 8);
		
		// dodaj do wektora slow jako uint32
		(this->words).push_back(strtoul(word_str.c_str(), nullptr, 16));
	}
	
	// ktory bit ustawic na 1
	int which_word = this->msg_length / 32;
	int which_bit =	this->msg_length % 32;

	// jezeli which_word wiekszy od ilosci slow to stworz dodaj nowe slowo o wartosci 0
	if (which_word >= (this->words).size()) {
		(this->words).push_back(0);
	}

	// ustaw bit			
	(this->words).at(which_word) |= 1 << (31 - which_bit);

	// ktory bit obrabiamy
	int which_bit_now = this->msg_length + 1;
	
	// dopoki wiadomosc nie 448 mod 512 to wstawiaj zera
	while (which_bit_now % 512 != 448) {
		if (which_bit_now % 32 == 0) {
			// dodaj slowo skladajace sie z 0
			(this->words).push_back(0);
		}
		
		which_bit_now++;		
	}

	// wiadomosc na 64 bitach
	uint64_t length = this->msg_length;
	// bardziej znaczace bity
	(this->words).push_back(length >> 32);
	// mniej znaczace bity
	(this->words).push_back(length);
	
	which_bit_now += 64;
	
	this->blocks = which_bit_now / 512;
}

void Blake224::printState(std::string what) {
	char prev;
	
	prev = std::cout.fill ('0');
	std::ios_base::fmtflags ff;
	ff = std::cout.flags();
	
	std::cout.setf ( std::ios::hex, std::ios::basefield );
	
	std::vector<uint32_t>::iterator begin, end;
	if (what == "words") {
		begin = (this->words).begin();
		end = (this->words).end();
	}
	else if (what == "v") {
		begin = (this->v).begin();
		end = (this->v).end();
	}
	else if (what == "salt") {
		begin = (this->salt).begin();
		end = (this->salt).end();
	}
	else if (what == "counter") {
		std::vector<uint32_t> counter(2);
		counter[0] = this->counter;
		counter[1] = this->counter >> 32;
		begin = counter.begin();
		end = counter.end();
	}
	else if (what == "chain") {
		begin = (this->chain).begin();
		end = (this->chain).end();
	}
	else if (what == "m") {
		begin = (this->m).begin();
		end = (this->m).end();
	}
	
	std::cout << std::endl << what << ":" << std::endl;
	for(std::vector<uint32_t>::iterator it = begin; it != end; ++it) {	
		std::cout << std::setw(8) << *it<< " ";
	}
	
	std::cout << std::endl;

	std::cout.fill(prev);
	std::cout.flags(ff);
}

int Blake224::getBlockCount() {
	return (this->blocks);
}

void Blake224::updateCounter(int block_iterator) {
	int on_how_many_blocks = (int)ceil((float)this->msg_length / 512);
	if (block_iterator > on_how_many_blocks) {
		// blok tylko na padding, brak wiadomosci
		this->counter = 0;
	}
	else if (block_iterator == on_how_many_blocks) {
		// liczymy dla bloku na ktorym sa jeszcze wiadomosci
		this->counter = this->msg_length;
	}
	else {
		this->counter = block_iterator * 512;
	}
}

void Blake224::updateV(int round_iterator) {
	this->updateCounter(round_iterator);
	
	this->v = {
		this->chain[0],
		this->chain[1],
		this->chain[2],
		this->chain[3],
		this->chain[4],
		this->chain[5],
		this->chain[6],
		this->chain[7],
		this->salt[0] xor this->constants[0],
		this->salt[1] xor this->constants[1],
		this->salt[2] xor this->constants[2],
		this->salt[3] xor this->constants[3],
		(uint32_t)this->counter xor this->constants[4],
		(uint32_t)this->counter xor this->constants[5],
		(uint32_t)(this->counter >> 32) xor this->constants[6],
		(uint32_t)(this->counter >> 32) xor this->constants[7]
	};
}