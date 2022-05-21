#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip> // for setw()

#include "include/TuringMachine.hpp"
#include "include/Tape.hpp"

void TuringMachine::addRule(std::string origin, char readSymbol,
														char writeSymbol, Direction direction,
														std::string target) {
	
	// does the origin state exist?
	if(this->states.count(origin) == 0) {
		
		// create the state
		this->states.insert({origin, {origin, {}}});
	}
	
	// does the target state exist?
	if(this->states.count(target) == 0) {
		
		// create the state
		this->states.insert({target, {target, {}}});
	}
	
	
	// construct the specific rule
	Rule rule = {
		readSymbol, writeSymbol, direction, &this->states[target]
	};
	
	this->states[origin].rules.push_back(rule);
}

void TuringMachine::addJump(std::string origin, char readSymbol, char writeSymbol, Direction direction, char stop, std::string target) {
	
	// create a unique name. The tm is deterministic, so origin and readsymbol should be enough
	std::string loopName = "Loop_" + origin + '.' + readSymbol;
	
	Direction reverse = Direction::STAND;
	if (direction == Direction::LEFT) reverse = Direction::RIGHT;
	else if (direction == Direction::RIGHT) reverse = Direction::LEFT;
	
	// make the machine go into a loop state
	this->addRule(origin, readSymbol, writeSymbol, direction, loopName);
	for (char symbol : this->tapeAlphabet) {
		if (symbol != stop) {
			this->addRule(loopName, symbol, symbol, direction, loopName);
		} else {
			// the machine should stop one cell before the symbol
			this->addRule(loopName, stop, stop, reverse, target);
		}
	}
}

void TuringMachine::setFinalState(std::string name, bool finalState) {
	
	if(this->states.count(name) == 0) {
		std::cout << "The state does not exist!" << std::endl;
	} else {
		this->states[name].finalState = finalState;
	}
	
}

void TuringMachine::setStart(std::string name) {
	this->start = name;
}

void TuringMachine::setTapeAlphabet(std::vector<char>& tapeAlphabet) {
	this->tapeAlphabet = tapeAlphabet;
}

void TuringMachine::reset() {
	
	// clear the states
	this->states.clear();
}

bool TuringMachine::run(Tape* tape, bool showDebug) {
	
	if(this->states.count(this->start) == 0) {
		std::cout << "No starting state found!" << std::endl;
		return false;
	}
	
	bool running = true;
	State* currentState = &this->states[this->start];
	
	if(showDebug)
		std::cout << *tape << std::endl;
	
	while(running) {
		// default if no suitable rules will be found
		running = false;
		
		// show more information
		if(showDebug) {
			std::cout << "Current state: " << currentState->name;
			std::cout << "\n-----" << std::endl;
		}
		
		// search for a suitable rule to apply
		for(auto rule : currentState->rules) {
			if(rule.readSymbol == tape->getSymbol()) {
				
				// apply the rule
				tape->putSymbol(rule.writeSymbol);
				
				if(rule.direction == Direction::LEFT) {
					tape->stepLeft();
				} else if (rule.direction == Direction::RIGHT) {
					tape->stepRight();
				} else {
					// do nothing, standing still
				}
				
				currentState = rule.target;
				
				// show the current state (tape)
				if(showDebug)
					std::cout << *tape << std::endl;

				running = true;
				break; // TODO: Replace that nasty break
			}
		}
	}
	
	if(currentState->finalState)
		return true;
	
	return false;
}

bool TuringMachine::step(Tape* tape) {
	
	if(this->states.count(this->start) == 0) {
		std::cout << "No starting state found!" << std::endl;
		return false;
	}
	
	bool running = true;
	State* currentState = &this->states[this->start];
	
	std::cout << *tape << std::endl;
	
	while(running) {
		// default if no suitable rules will be found
		running = false;
		
		// wait for user input
		std::cout << "Current state: " << currentState->name;
		std::cout << "\n-----" << std::flush;
		std::cin.get();
		
		// search for a suitable rule to apply
		for(auto rule : currentState->rules) {
			if(rule.readSymbol == tape->getSymbol()) {
				
				// apply the rule
				tape->putSymbol(rule.writeSymbol);
				
				if(rule.direction == Direction::LEFT) {
					tape->stepLeft();
				} else if (rule.direction == Direction::RIGHT) {
					tape->stepRight();
				} else {
					// do nothing, standing still
				}
				
				currentState = rule.target;
				
				// show the current state (tape)
				std::cout << *tape << std::endl;
				running = true;
				break;
			}
		}
	}
	
	if(currentState->finalState)
		return true;
	
	return false;
}

std::ostream& TuringMachine::outputMachine(std::ostream& stream) {
	
	// state | read | write | direction | nextState
	
	// constants for the head fo the table
	const std::string CURRENT_STATE = "state";
	const std::string NEXT_STATE = "next state";
	
	// first: get the greatest length of a state name
	size_t longest = CURRENT_STATE.length();
	if(NEXT_STATE.length() > longest)
		longest = NEXT_STATE.length();
	for(auto pair : this->states) {
		if(pair.first.length() > longest)
			longest = pair.first.length();
	}
	longest += 3; // before current state name may be '->' and/or '*'
	
	// output the header of the table
	stream << std::setw(longest) << CURRENT_STATE << " | ";
	stream << "r | w | d | ";
	stream << std::setw(longest) << NEXT_STATE << "\n";
	
	for(size_t i = 0; i < 2*longest + 4 * 3 + 3; i++) {
		stream << "-";
	}
	stream << "\n";
	
	// calculate the right order (TODO: Memory?)
	std::vector<std::string> orderedNames;
	std::queue<State> toSearch;
	// TODO: test for (invalid) tm without start
	toSearch.push(this->states[this->start]);
	
	while(!toSearch.empty()) {
		
		State current = toSearch.front();
		toSearch.pop();
		// append the name if it does not already exist
		if (std::find(orderedNames.begin(), 
			orderedNames.end(), current.name) == orderedNames.end()) {
			
			orderedNames.push_back(current.name);
		}
		
		
		for(auto& rule : current.rules) {
			if (std::find(orderedNames.begin(), 
				orderedNames.end(), rule.target->name) == orderedNames.end()) {
					
				toSearch.push(*rule.target);
			}
		}
	}
	
	// simple version: unordered
	for(auto& name : orderedNames) {
		
		State current = this->states[name];
		
		// iterate through the rules
		for(auto it = current.rules.begin();
				it != current.rules.end(); it++) {
			
				// show the name of the state only in the first line
				std::string displayName = "";
				if(it == current.rules.begin()) {
					if(current.name == this->start)
						displayName += "->";
					if(current.finalState)
						displayName += "*";
					
					displayName += current.name;
				}
				// show the rest of the current rule
				stream << std::setw(longest) << displayName << " | ";
				stream << it->readSymbol << " | " << it->writeSymbol << " | ";
				stream << it->direction << " | "; 
				stream << std::setw(longest) << it->target->name << "\n";
			}
		
		/* 
		 * edge case: the state has no rules
		 * show just the name of the state
		 */
		if(current.rules.begin() == current.rules.end()) {
			
			std::string displayName = "";
			if(current.name == this->start)
				displayName += "->";
			if(current.finalState)
				displayName += "*";
			displayName += current.name;
			
			// show empty rule
			stream << std::setw(longest) << displayName << " | ";
			stream << " " << " | " << " " << " | ";
			stream << " " << " | ";
			stream << std::setw(longest) << " " << "\n";
		}
		
		// seperate the states in the table
		for(size_t i = 0; i < 2*longest + 4 * 3 + 3; i++) {
			stream << "-";
		}
		stream << "\n";
	}
	
	return stream;
}

std::ostream& operator<<(std::ostream& stream,
												TuringMachine& tm) {

	return tm.outputMachine(stream);
}
std::ostream& operator<<(std::ostream& stream,
											const Direction& direction) {
												
	if(direction == Direction::LEFT)
		stream << "L";
	else if(direction == Direction::RIGHT)
		stream << "R";
	else
		stream << "-";
	
	return stream;
}
