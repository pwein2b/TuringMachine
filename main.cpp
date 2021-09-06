#include <iostream>
#include <cstring>

#include "include/Tape.hpp"
#include "include/TuringMachine.hpp"

const char EMP = Tape::EMPTY_SYMBOL;
const Direction L = Direction::LEFT;
const Direction R = Direction::RIGHT;

int main() {
		
	TuringMachine tm;
	
	/* determine if there is an even number of 1s on the tape */
	
	// first character is the result as a bool: 0 -> not even, 1 -> even 
	tm.addRule("S1", EMP, '1', R, "F2");
	tm.addRule("S1", '1', '0', R, "S2");
	
	// find the next 1 on the tape
	tm.addRule("S2", '1', '#', L, "S3");
	tm.addRule("S2", '#', '#', R, "S2");
	tm.addRule("S2", EMP, EMP, L, "F1");
	
	// go back to switch the first character
	tm.addRule("S3", '#', '#', L, "S3");
	tm.addRule("S3", '1', '0', R, "S2");
	tm.addRule("S3", '0', '1', R, "S2");
	
	// clean up after operation
	tm.addRule("F1", '#', EMP, L, "F1");
	tm.addRule("F1", '1', '1', R, "F2");
	tm.addRule("F1", '0', '0', R, "F2");
	
	// go back to the beginning
	tm.addRule("F2", EMP, EMP, L, "F3");
	
	tm.setStart("S1");
	tm.setFinalState("F3");
	
	Tape tape("1111");
	tm.step(&tape);

}
