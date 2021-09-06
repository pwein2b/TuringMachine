#include <iostream>

class Tape {
	
public:
	
	static const char EMPTY_SYMBOL ='_';

public:
	// main data on the tape
	char* data;
	// current length of the data
	uint32_t length;
	
	// current index on the tape
	uint32_t currentPos;

public:
	/**
	 * Construct the tape with the given input data as a string.
	 * 
	 * @param input		Input to the Turing Machine
	 */
	Tape(const char* input, uint32_t startingPos = 0);
	
	/**
	 * Construct the tape with the given input data.
	 * This will create a copy of the input inside the tape.
	 * 
	 * @param input 	Input to the Turing Machine
	 */
	Tape(char* input, uint32_t length, uint32_t startingPos = 0);
	
	/**
	 * Construct the tape as a copy of the given tape.
	 */
	Tape(const Tape& other);
	
	~Tape();
	
	/**
	 * Output the data to a stream.
	 * 
	 * @param stream		Stream to write to
	 */
	std::ostream& outputTape(std::ostream& stream) const;
	
	/*
	 *  --- functions to interact with the data --- 
	 */
	
	/**
	 * Go one symbol to the sides and extend the tape if necessary.
	 * This moves the head and not the tape.
	 */
	void stepLeft();
	void stepRight();
	
	/**
	 * Get the current symbol.
	 * 
	 * @return Character at the current position.
	 */
	char getSymbol();
	
	/**
	 * Set the current symbol.
	 * Warning: The character '_' is used as the EMPTY_SYMBOL!
	 * 
	 * @param symbol Character to put at the current position.
	 */
	void putSymbol(char symbol);
	
	/**
	 * Put the empty symbol onto the tape.
	 * The same as putSymbol(Tape::EMPTY_SYMBOL);
	 */
	inline void putSymbol() {
		putSymbol(Tape::EMPTY_SYMBOL);
	}
};

std::ostream& operator<<(std::ostream& stream, const Tape& tape);
