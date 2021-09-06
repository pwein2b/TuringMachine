#include <vector>
#include <string>
#include <map>

class Tape;

enum Direction {
	LEFT = 0, RIGHT
};

struct Rule;

struct State {
	std::string name;
	std::vector<Rule> rules;
	bool finalState = false;
};

struct Rule {
	char readSymbol;
	char writeSymbol;
	Direction direction;
	State*	target;
};

class TuringMachine {
	
private:

	// map to find states easier
	std::map<std::string, State> states;
	std::string start;

public:
	
	TuringMachine() = default;
	/**
	 * Add a rule to a state. Constructs the state if it does not exist.
	 * Newly constructed States won't be final states by default.
	 * 
	 * @param origin 			The alias of the origin state of the rule
	 * @param readSymbol	The symbol to read on the tape
	 * @param writeSymbol	The symbol to write onto the tape
	 * @param direction		The direction to move to
	 * @param target			The alias of the target state
	 */
	void addRule(std::string origin, char readSymbol, char writeSymbol,
							Direction direction, std::string target);
	
	/**
	 * Set the state with the name to a finalstate.
	 */
	void setFinalState(std::string name, bool finalState = true);
	
	/**
	 * Set the state to begin with.
	 */
	void setStart(std::string name);
	
	/**
	 * Deletes all rules and states of the machine.
	 */
	void reset();
	
	/**
	 * Run the machine on a given input.
	 * Currently this works just for deterministic machines.
	 * 
	 * @param tape			Pointer to the input tape
	 * @param showDebug	Show the tape after each step
	 * 
	 * @return true if program ended on a final state
	 */
	bool run(Tape* tape, bool showDebug = true);
	
	/**
	 * Execute just a step of the program, waiting for cin.get and
	 * showing the current state of the machine.
	 * 
	 * @param tape		Pointer to the input tape
	 * 
	 * @return true if program ended on a final state
	 */
	bool step(Tape* tape);
	
	/**
	 * Output the TuringMachine as a string to a stream
	 */
	std::ostream& outputMachine(std::ostream& stream);
};

std::ostream& operator<<(std::ostream& stream, TuringMachine& tm);
std::ostream& operator<<(std::ostream& stream,
												const Direction& direction);
