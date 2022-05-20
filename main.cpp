#include <iostream>
#include <vector>
#include <cstring>
#include "include/TuringMachine.hpp"
#include "include/Tape.hpp"

using namespace std;

void printHelp() {
	cout << "Run a deterministic Turing Machine" << endl;
	cout << "Invocation:" << endl;
	cout << "  TuringMachine [options] machine.tm word ..." << endl;
	cout << "Where machine.tm is a file that contains the Turing Machine and word ... is the words that should be run" << endl;
	cout << "Possible options:" << endl;
	cout << "  --visualize: Create an output file machine.dot which GraphViz code that represents the machine" << endl;
	cout << "  --batch: Don't show steps, only show whether the words got accepted" << endl;
	cout << "  --interactive: Only skip from one state to the next on request" << endl;
}

int main (int argc, char** argv) {
	if (argc < 3) {
		cout << "Excepted at least two argument" << endl;
		printHelp();
		return 1;
	}

	string filename;
	vector<string> words;
	bool visualize = false, batch = false, interactive = false;

	/* Iterate through options */
	int i = 1;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') // we're done with processing options
			break;

		if(strcmp(argv[i], "--visualize") == 0)
			visualize = true;
		else if(strcmp(argv[i], "--batch") == 0)
			batch = true;
		else if(strcmp(argv[i], "--interactive") == 0)
			interactive = true;
	}

	/* Find filename and words */
	if (i > argc - 2) {
		cout << "Expected filename and words after options" << endl;
		printHelp();
		return 1;
	}

	filename = argv[i];

	for (i++; i < argc; i++) {
		words.push_back(argv[i]);
	}

	/* Parse the Turing Machine */
	TuringMachine tm = TuringMachine::create_from_file(filename);

	/* Visualization */
	if(visualize)
		tm.graph_to_file(filename + ".dot");

	/* Execute on each word */
	for(string word : words) {
		Tape tape(word.c_str());
		cout << "'" << word << "' ... ";

		bool result;
		if (interactive)
			result = tm.step(&tape);
		else
			result = tm.run(&tape, !batch);

		if (result)
			cout << "accepted." << endl;
		else
			cout << "not accepted." << endl;
	}
}
