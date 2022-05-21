#include <iostream>
#include <cstring>

#include "include/Tape.hpp"
#include "include/TuringMachine.hpp"

const char EMP = Tape::EMPTY_SYMBOL;
const Direction L = Direction::LEFT;
const Direction R = Direction::RIGHT;
const Direction S = Direction::STAND;

std::vector<char> tapeAlphabet = std::vector<char>{EMP, '1', '0'};

int main() {
		
	TuringMachine tm;
	tm.setTapeAlphabet(tapeAlphabet);
	
	/* increment a binary number on the tape */
	
	// 'jump' to the right
	tm.addJump("S1", '1', '1', R, EMP, "I1");
	
	// basic addition
	tm.addRule("I1", '0', '1', L, "F1");
	tm.addRule("I1", EMP, '1', L, "F2");
	tm.addRule("I1", '1', '0', L, "I1");
	
	// go to the right and stop
	tm.addJump("F1", '0', '0', L, EMP, "F2");
	tm.addJump("F1", '1', '1', L, EMP, "F2");
	
	tm.setStart("S1");
	tm.setFinalState("F2");
	
	Tape tape("1011");
	tm.step(&tape);
}