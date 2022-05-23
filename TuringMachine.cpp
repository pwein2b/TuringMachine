#include <algorithm>
#include <string>
#include <map>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip> // for setw()
#include <fstream>
#include <regex>
#include <filesystem>

#include "include/TuringMachine.hpp"
#include "include/Tape.hpp"

namespace fs = std::filesystem;

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
TuringMachine::create_from_file (std::string filename, std::string state_prefix) {
  fs::path filepath(filename);
  if(!fs::exists(filepath)) {
    std::cerr << "TuringMachine::create_from_file: File '" << fs::absolute(filepath) << "' does not exist" << std::endl;
  }

	std::ifstream in;
	in.open(filename);

	TuringMachine tm;

	std::regex ruleRegEx("([a-zA-Z0-9_,]+): (\\{[^}]+\\}|[^{]),(.,)?(.) -> ([a-zA-Z0-9_]+)( #.*)?");
	std::regex finalRegEx("([a-zA-Z0-9_]+) final;( #.*)?");
	std::regex jumpRegEx("([a-zA-Z0-9_,]+): (\\{[^}]+\\}|[^{]),(.,)?jump (R|L) until (.) -> ([a-zA-Z0-9_]+)( #.*)?");
	std::regex importRegEx("([a-zA-Z0-9_]+): import \"([^\"]+)\"( at ([a-zA-Z0-9_]+))? -> ([a-zA-Z0-9_]+)");
  std::regex alphabetRegEx("alphabet (\\{.(,.)*\\});");
	int linecount = 0;

	for(std::string line; std::getline(in, line);) {
		linecount++;
		std::smatch match;

		if (std::regex_match(line, match, ruleRegEx)) {
			/* Match a rule or a collection of rules, because multiple rules can be on one line*/
			Direction d = Direction::STAND;
			switch(match[4].str()[0]) {
			  case 'R': case 'r':
			  d = Direction::RIGHT; break;
			  case 'L': case 'l':
			  d = Direction::LEFT; break;
			  case 'S': case 's': break;
			  default:
			  std::cout << "Line " << linecount << ": unrecognized direction '" << match[4].str() << "'. Expected L, R or S" << std::endl;
			}

			std::vector<std::string> origins = split_string(match[1], ",");
			std::vector<char> reads = parse_character_class (match[2]);
			for (std::string origin : origins) {
			  for(char read : reads) {
			    char write;
			    if(match[3].length() == 0)
			      write = read;
			    else
			      write = match[3].str()[0];

			    tm.addRule(state_prefix + origin, read, write, d, state_prefix + match[5].str());
			  }

			  if(tm.start == "")
				  tm.setStart(state_prefix + match[1].str());
			}

    } else if(std::regex_match(line, match, jumpRegEx)) {
			Direction d = Direction::STAND;
			switch(match[4].str()[0]) {
			  case 'R': case 'r':
			  d = Direction::RIGHT; break;
			  case 'L': case 'l':
			  d = Direction::LEFT; break;
			  default:
			  std::cout << "Line " << linecount << ": unrecognized direction '" << match[4].str() << "'. Expected L or R" << std::endl;
			}
      char jumpLimit = match[5].str()[0];

			std::vector<std::string> origins = split_string(match[1], ",");
			std::vector<char> reads = parse_character_class (match[2]);

      if(tm.tapeAlphabet.size() == 0)
        std::cout << "Line " << linecount << ": declaring a jump before declaring the tape alphabet will result in unexpected behaviour" << std::endl;

			for (std::string origin : origins) {
			  for(char read : reads) {
			    char write;
			    if(match[3].length() == 0)
			      write = read;
			    else
			      write = match[3].str()[0];

			    tm.addJump(state_prefix + origin, read, write, d, jumpLimit, state_prefix + match[6].str());
			  }

			  if(tm.start == "")
				  tm.setStart(state_prefix + match[1].str());
			}

		} else if(std::regex_match(line, match, finalRegEx)) {
			tm.addState(state_prefix + match[1].str());
			tm.setFinalState(state_prefix + match[1].str(), true);

			if(tm.start == "")
				tm.setStart(state_prefix + match[1].str());

		} else if(std::regex_match(line, match, importRegEx)) {
		  std::string subMachineName = match[1].str();
		  std::string startState = match[4].str(), nextState = match[5].str();
		  if(startState != "")
		    startState = subMachineName + "__" + startState;
		  tm.addState(nextState);
		  tm.addState(subMachineName);

		  /* find the file to import */
		  std::string importfn = match[2].str();
		  fs::path importpath(importfn);
		  if(!fs::exists(importpath)) {
		    importpath = fs::path(filepath).parent_path() / importpath;
		    if(!fs::exists(importpath)) {
		      std::cout << "Line " << linecount << ": Unable to find '" << importfn << "'" << std::endl;
		      break;
		    }
		  }

      /* Import the machine */
		  TuringMachine subMachine = TuringMachine::create_from_file(importpath.string(), subMachineName + "__");
		  if (startState == "")
		    startState = subMachine.start;

		  /* Merge the other machine's states into the current Turing Machine */
		  for(const auto& [state_name, state] : subMachine.states) {
		    std::string newname = state_name;
		    if (state_name == startState)
		      newname = subMachineName;

		    if (state.finalState) {
		      // the final state from the submachine will be merged with the nextState
		      if (state.rules.size() != 0)
		        std::cout << "Line " << linecount << ", importing '" << subMachineName << "': Error - final state '" << state_name << "' of sub machine must not have any rules" << std::endl;

		      continue;
		    }

		    /* Rewrite rules, merging final states with nextState */
		    std::vector<Rule> newrules;
		    for (Rule rule : state.rules) {
		      if (rule.target->finalState)
		        tm.addRule(newname, rule.readSymbol, rule.writeSymbol, rule.direction, nextState);
		      else if(rule.target->name == startState)
		        tm.addRule(newname, rule.readSymbol, rule.writeSymbol, rule.direction, subMachineName);
		      else {
		        tm.addRule(newname, rule.readSymbol, rule.writeSymbol, rule.direction, rule.target->name);
		      }
		    }

		    /* Insert modified state */
		    //tm.states.insert({newname, {newname, newrules, false}});
		  }
    } else if(std::regex_match(line, match, alphabetRegEx)) {
      std::vector<char> alphabet = parse_character_class(match[1].str());
      tm.setTapeAlphabet(alphabet);

		} else if(line[0] != '#' && line[0] != '\0') {
			std::cout << "Line " << linecount << ": syntax error" << std::endl;
		}
	}

  in.close();
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

bool
TuringMachine::graph_to_file (std::string filename) {
	std::ofstream out(filename, std::ios::trunc);
	out << "digraph G {" << std::endl;
	out << "  ___start [label=start;shape=none;fontcolor=red];" << std::endl;
	out << "  ___start -> " << start << ";" << std::endl;

	/* iterate over states */
	for(const auto& [state_name, state] : states) {
		if (state.finalState)
			out << "  " << state_name << " [shape=doublecircle];" << std::endl;
		else
			out << "  " << state_name << ";" << std::endl;
	}

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
			  switch(rule.direction) {
			    case Direction::LEFT:
  				out << "L"; break;
  				case Direction::RIGHT:
  				out << "R"; break;
  				case Direction::STAND:
  				out << "S"; break;
  			}
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
		stream << "S";
	
	return stream;
}
