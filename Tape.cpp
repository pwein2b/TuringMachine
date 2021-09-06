#include <cstring>

#include "include/Tape.hpp"

// extend the tape by this many symbols, if space is tight
#define TAPE_INCREMENT	5

Tape::Tape(const char* input, uint32_t startingPos)
	: currentPos(startingPos) {
		
	// get the length of the string
	uint32_t len = std::strlen(input);
	
	// round up to have space before and after the start
	this->length = ((len / TAPE_INCREMENT) + 2 ) * TAPE_INCREMENT;

	this->data = new char[this->length];
	
	// initialize to EMPTY_SYMBOL
	memset(this->data, this->EMPTY_SYMBOL, this->length);
	
	// convert the const char* to a char[] by copying it
	memcpy(this->data + TAPE_INCREMENT, input, len);
	this->currentPos += TAPE_INCREMENT;
}
	
Tape::Tape(char* input, uint32_t length, uint32_t startingPos)
	: length(length), currentPos(startingPos) {
	
	// round up to have space before and after the start
	this->length = ((length / TAPE_INCREMENT) + 2 ) * TAPE_INCREMENT;
	
	// create a copy of input and store it
	this->data = new char[this->length];
	
	// initialize to EMPTY_SYMBOL
	memset(this->data, this->EMPTY_SYMBOL, this->length);
	
	// copy the data
	memcpy(this->data + TAPE_INCREMENT, input, length);
	this->currentPos += TAPE_INCREMENT;
}

Tape::Tape(const Tape& other) : length(other.length),
	currentPos(other.currentPos) {
	
	// create a new data
	this->data = new char[this->length];
	
	// copy the data from the other tape
	memcpy(this->data, other.data, this->length);
	
}

Tape::~Tape() {
	delete this->data;
}
	
std::ostream& Tape::outputTape(std::ostream& stream) const {
	// output the tape data
	for(uint32_t i = 0; i < this->length; i++) {
		stream << this->data[i];
	}
	stream << '\n';
	
	// show the current position
	for(uint32_t i = 0; i < this->length; i++) {
		if(i == this->currentPos)
			stream << '^';
		else 
			stream << ' ';
	}
	return stream;
}

void Tape::stepLeft() {
	
	// move to left if possible
	if(this->currentPos >= 1) {
		this->currentPos--;
	} else {
		
		// extend the data to the left
		char* newData = new char[this->length + TAPE_INCREMENT];
		memset(newData, this->EMPTY_SYMBOL, this->length + TAPE_INCREMENT);
		memcpy(newData + TAPE_INCREMENT, this->data, this->length);
		
		// swao the data
		delete this->data;
		this->data = newData;
		this->length = this->length + TAPE_INCREMENT;
		
		// move the new cursor to one less then the previous first
		this->currentPos += TAPE_INCREMENT - 1;
	}
}

void Tape::stepRight() {
	
	// move to right if possible
	if(this->currentPos < this->length - 1) {
		this->currentPos++;
	} else {
		
		// extend the data to the right
		char* newData = new char[this->length + TAPE_INCREMENT];
		memset(newData, this->EMPTY_SYMBOL, this->length + TAPE_INCREMENT);
		memcpy(newData, this->data, this->length);
		
		// swap the data
		delete this->data;
		this->data = newData;
		this->length += TAPE_INCREMENT;
		
		// move the cursor to the next position
		this->currentPos++;
	}
}

char Tape::getSymbol() {
	return this->data[this->currentPos];
}

void Tape::putSymbol(char symbol) {
	this->data[this->currentPos] = symbol;
}

std::ostream& operator<<(std::ostream& stream, const Tape& tape) {
	return tape.outputTape(stream);
}
