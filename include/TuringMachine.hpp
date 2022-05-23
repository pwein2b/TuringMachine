#include <vector>
#include <string>
#include <map>

class Tape;

enum Direction {
	LEFT = 0, RIGHT, STAND
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
	std::vector<char> tapeAlphabet;

public:
	
	TuringMachine() = default;

	/**
	 * Construct a new Turing Machine by reading in a specification from a file.
	 * The format of the .tm file is documented in README.
	 *
	 * @param filename	The file from which to read the machine
	 * @param state_prefix	A prefix that should be prepended to all state names read
	 *
	 * @return true if a deterministic turing machine with at least one final state was read.
	 */
	static TuringMachine create_from_file(std::string filename, std::string state_prefix = "");

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
	 * Adds a jump as a construct of multiple rules. Goes as follows:
	 * Read the symbol and overwrite it with the new one.
	 * Then go to the direction until the stop symbol appears.
	 * Halt one cell next to the stop symbol.
	 * 
	 * This only works if the tape alphabet is set.
	 * 
	 * @param origin		The alias of the origin state
	 * @param readSymbol	The symbol to read on the tape
	 * @param writeSymbol	The symhol to write onto the tape
	 * @param direction		The direction to move to
	 * @param stop			The symbol to where the machine should jump
	 * @param target		The alias of the target state
	 */
	void addJump(std::string origin, char readSymbol, char writeSymbol,
				Direction direction, char stop, std::string target);

	/**
	 * Make sure a state with a certain name exists in the machine
	 */
	void addState(std::string name);

	/**
	 * Set the state with the name to a finalstate.
	 */
	void setFinalState(std::string name, bool finalState = true);
	
	/**
	 * Set the state to begin with.
	 */
	void setStart(std::string name);
	
	/**
	 * Set the tape alphabet. It contains all the symbols the machine
	 * may find on the tape and is used internally whenever rules
	 * need to be constructed automatically.
	 */
	void setTapeAlphabet(std::vector<char>& tapeAlphabet);
	
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
	 * Run the machine on a given input; execute just one step at a time,
	 * waiting for cin.get and showing the current state of the machine.
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

	/**
	 * Create a GraphViz file that represents the machine as a graph.
	 * The resulting file can be compiled with dot or its siblings.
	 * GraphViz needs to be installed for compilation.
	 *
	 * @param filename	Name of the file to output the graph into
	 *
	 * @return true on success, false otherwise
	 */
	bool graph_to_file(std::string filename);
};

std::ostream& operator<<(std::ostream& stream, TuringMachine& tm);
std::ostream& operator<<(std::ostream& stream,
												const Direction& direction);
