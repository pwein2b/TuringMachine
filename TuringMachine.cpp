#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip> // for setw()
#include <fstream>
#include <regex>

#include "include/TuringMachine.hpp"
#include "include/Tape.hpp"

std::vector<std::string>
split_string (std::string to_split, std::string delimiter) {
  std::vector<std::string> result;
  std::string s = to_split;

  size_t pos = 0;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    std::string token = s.substr(0, pos);
    result.push_back(token);
    s.erase(0, pos + delimiter.length());
  }

  result.push_back(s);

  return result;
}

std::vector<char>
parse_character_class (std::string character_class) {
  std::vector<char> result;

  if(character_class.size() == 0)
    return result;

  if(character_class.size() == 1) {
    result.push_back(character_class[0]);
    return result;
  }

  std::vector<std::string> preresult = split_string(character_class.substr(1, character_class.size() - 2), ",");
  for(std::string character : preresult)
    result.push_back(character[0]);

  return result;
}

TuringMachine
TuringMachine::create_from_file (std::string filename) {
	std::ifstream in(filename);

	TuringMachine tm;

	std::regex ruleRegEx("([a-zA-Z0-9_,]+): (\\{[^}]+\\}|[^{]),(.,)?(.) -> ([a-zA-Z0-9_]+)( #.*)?");
	std::regex finalRegEx("([a-zA-Z0-9_]+) final;( #.*)?");
	int linecount = 0;
	for(std::string line; std::getline(in, line);) {
		linecount++;
		std::smatch match;

		if (std::regex_match(line, match, ruleRegEx)) {
			/* Match a rule or a collection of rules, because multiple rules can be on one line*/
			Direction d = Direction::LEFT;
			if(match[4].str() == "R")
				d = Direction::RIGHT;

			std::vector<std::string> origins = split_string(match[1], ",");
			std::vector<char> reads = parse_character_class (match[2]);
			for (std::string origin : origins) {
			  for(char read : reads) {
			    char write;
			    if(match[3].length() == 0)
			      write = read;
			    else
			      write = match[3].str()[0];

			    tm.addRule(origin, read, write, d, match[5].str());
			  }

			  if(tm.start == "")
				  tm.setStart(match[1]);
			}

		} else if(std::regex_match(line, match, finalRegEx)) {
			tm.addState(match[1].str());
			tm.setFinalState(match[1].str(), true);

			if(tm.start == "")
				tm.setStart(match[1]);
		} else if(line[0] != '#' && line[0] != '\0') {
			std::cout << "Line " << linecount << ": syntax error" << std::endl;
		}
	}

	return tm;
}

void
TuringMachine::addState(std::string name) {
	if(states.count(name) == 0)
		states.insert({name, {name, {}}});
}

void TuringMachine::addRule(std::string origin, char readSymbol,
														char writeSymbol, Direction direction,
														std::string target) {
	
	// create the origin and target states, if they don't exist yet
	addState(origin);
	addState(target);
	
	// construct the specific rule
	Rule rule = {
		readSymbol, writeSymbol, direction, &this->states[target]
	};
	
	this->states[origin].rules.push_back(rule);
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
				} else {
					tape->stepRight();
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
				} else {
					tape->stepRight();
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

bool
TuringMachine::graph_to_file (std::string filename) {
	std::ofstream out(filename, std::ios::trunc);
	out << "digraph G {" << std::endl;

	/* iterate over states */
	for(const auto& [state_name, state] : states) {
		if (state.finalState)
			out << "  " << state_name << " [shape=doublecircle];" << std::endl;
		else
			out << "  " << state_name << ";" << std::endl;
	}

	out << "  ___start [label=start;shape=none];" << std::endl;
	out << "  ___start -> " << start << ";" << std::endl;

	/* iterate over states again for rules */
	for(const auto& [state_name, state] : states) {
	  /* iterate over destination states to merge rules that lead the same way to one label */
	  std::map<std::string, std::vector<Rule>> rules_by_target;
	  for (Rule rule : state.rules) {
	    if (rules_by_target.count(rule.target->name) == 0){
	      std::vector<Rule> rules {rule};
	      rules_by_target.insert({rule.target->name, rules});
	    } else {
	      std::vector<Rule> *rules = &rules_by_target[rule.target->name];
	      rules->push_back(rule);
	    }
	  }

		for (const auto& [target_state_name, rules] : rules_by_target) {
			out << "  " << state_name << " -> " << target_state_name << " [label=\"";
			for (Rule rule : rules) {
			  out << rule.readSymbol << "/" << rule.writeSymbol << "/";
			  if (rule.direction == Direction::LEFT)
  				out << "L";
	  		else
	  			out << "R";
	  		out << " \\n";
	  	}
	  	out << "\"];" << std::endl;
		}
	}
	out << "}";

	return true;
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
