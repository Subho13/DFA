#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Error codes
#define ERROR_INCOMPLETE_TABLE -22
#define ERROR_INVALID_CHARACTER_IN_INPUT -20

// struct to represent a Deterministic Finite Automaton
// DFA (Q, Σ, q0, F, δ)
typedef struct dfa {
	int characters, // Number of charcters in the alphabet set
		states, // Number of states in the DFA
		initialState, // Initial state of the DFA
		numberOfFinalStates, // Number of final states
		*finalStates, // List of final states
		incompleteCells; // Number of cells in the transition table yet to be filled

	char *characterSet; // Alphabet set of the DFA

	int **transitionTable; // State transition table
} DFA;

/*
	Creates a DFA with the given parameters and empty transition table

	@param chars A string containing all unique characters in the alphabet set (Σ)
	@param nStates Number of states in the set of states (Q)
	@param initState The initial state (q0)
	@param nFinStates Number of final states
	@param finStates Array of integer representing the final states (F)

	returns A pointer to the DFA struct or NULL in case of error in memory allocation
*/

DFA* createDFA(char *chars, int nStates, int initState, int nFinStates, int *finStates) {
	// Allocate memory for the DFA from heap
	DFA *dfa = malloc(sizeof(DFA));
	// Error in memory allocation
	if(dfa == NULL) return NULL;
	// Number of states in the DFA
	dfa->states = nStates;
	// The initial state
	dfa->initialState = initState;
	// Number of final states in the DFA
	dfa->numberOfFinalStates = nFinStates;
	// Allocate the set of final states
	dfa->finalStates = malloc(nFinStates * sizeof(int));
	// Reuse variable initState as loop variable
	for(initState = 0; initState < nFinStates; initState++) dfa->finalStates[initState] = finStates[initState];
	// Reuse variable initState to hold number of alphabets in alphabet set
	initState = strlen(chars);
	// Number of alphabets in alphabet set
	dfa->characters = initState;
	// Allocate the character set
	dfa->characterSet = malloc(initState * sizeof(char));
	// Reuse variable nFinStates as loop variable
	for(nFinStates = 0; nFinStates < initState; nFinStates++) dfa->characterSet[nFinStates] = chars[nFinStates];
	// Allocate the transition table
	dfa->transitionTable = (int**)malloc(nStates * sizeof(int*));
	// Reuse nFinStates as loop variable
	for(nFinStates = 0; nFinStates < nStates; nFinStates++) dfa->transitionTable[nFinStates] = malloc(initState * sizeof(int));
	// Number of empty cells in the transition table
	dfa->incompleteCells = nStates * initState;
	return dfa;
}

/*
	Finds the index of a character in the alphabet set of a given DFA

	@param d Pointer to the DFA
	@param c The character to be searched in the alphabet set of the DFA

	return An integer denoting the location of the character in the alphabet set of the DFA (0 based indexing) or -1 if not found
*/

int indexOf(DFA *dfa, char c) {
	// Loop variable
	int i;
	// Linear search for the character in the string
	for (i = 0; i < dfa->characters; ++i) {
		if(c == dfa->characterSet[i]) return i; // character found
	}
	// End of string passed, indicating character is not present in the string
	return -1;
}

/*
	Fills up an entry in the transition table

	@param dfa A pointer to the DFA
	@param initState The initial state from where the transition will be defined
	@param inp the input character
	@param finalState The state reached after the transition
*/

void addTransition(DFA *dfa, int initState, char inp, int finalState) {
	// Get the index of the character in the string
	int index = indexOf(dfa, inp);
	// character not present in string
	if (index < 0) return;
	// Make an entry in the transition table
	dfa->transitionTable[initState][index] = finalState;
	dfa->incompleteCells--;
}

/*
	Checks whether the given input string is accepted by the DFA or not

	@param dfa A pointer to the DFA
	@param inp Pointer to the input string

	return An integer denoting the result of computation
*/

int checkString(DFA *dfa, char *inp) {
	// Check whether transition table is complete or not
	if(dfa->incompleteCells) return ERROR_INCOMPLETE_TABLE; // Error code for incomplete table
	// Variable to keep track of current state of the DFA
	int state = dfa->initialState;
	// Temporary variable
	int index;
	// Length of input string and loop variable
	int l = strlen(inp), i;
	for (i = 0; i < l; ++i) {
		// Find index of the input character
		index = indexOf(dfa, inp[i]);
		// Character in out of alphabet set of DFA
		if(index < 0) return ERROR_INVALID_CHARACTER_IN_INPUT; // Error code for invalid character
		// Change the current state according to transition table
		state = dfa->transitionTable[state][index];
	}

	// Flag variable; Assume final state is not reached
	index = 0;
	// Iterate over list of final states to check if final state is reached or not
	for (i = 0; i < dfa->numberOfFinalStates; ++i) {
		if(state == dfa->finalStates[i]) { // Final state reached
			index = 1;
			break; // No need to check other final states
		}
	}

	return index;
}

/*
	Deallocates the memory held by the DFA

	@param dfa A pointer to the dfa struct which is to be freed
*/

void freeDFA(DFA *dfa) {
	// Release all memory held by:

	// list of final states,
	free(dfa->finalStates);
	// alphabet set,
	free(dfa->characterSet);
	int i; // Loop variable
	// individual rows of the transition table,
	for(i = 0; i < dfa->states; i++) free(dfa->transitionTable[i]);
	// transition table,
	free(dfa->transitionTable);
	// and finally the dfa struct itself.
	free(dfa);
}

// Sample driver program
int main(void) {
	int states, initialState, numberOfFinalStates, *finalStates;
	char *characterSet;
	int i, j, characters;
	char inStr[50];

	printf("Enter number of characters in alphabet set: "); scanf("%d", &characters);
	characterSet = malloc(characters * sizeof(char));
	printf("Enter all the unique alphabets in a single line:\n"); scanf("%s", characterSet);
	printf("Enter number of states: "); scanf("%d", &states);
	printf("Enter initial state: "); scanf("%d", &initialState);
	printf("Enter number of final states: "); scanf("%d", &numberOfFinalStates);
	finalStates = malloc(numberOfFinalStates * sizeof(int));
	for (i = 0; i < numberOfFinalStates; ++i) {
		printf("Enter final state %d: ", i + 1);
		scanf("%d", &finalStates[i]);
	}

	DFA *dfa;
	dfa = createDFA(characterSet, states, initialState, numberOfFinalStates, finalStates);

	free(finalStates);
	printf("Enter transition table:\n");
	for(i = 0; i < states; i++) {
		printf("State %d\n", i);
		for(j = 0; j < characters; j++) {
			printf("\tInput %c: ", characterSet[j]);
			scanf("%d", &initialState);
			addTransition(dfa, i, characterSet[j], initialState);
		}
	}
	printf("\n");
	free(characterSet);

	do {
		printf("Enter string to check:\n");
		scanf("%s", inStr);
		j = checkString(dfa, inStr);
		if (j == 1) printf("Accepted\n");
		else printf("Not accepted\n");

		printf("Do you want to continue? (y/n)\n"); scanf("%s", inStr);
	} while(inStr[0] == 'y' || inStr[0] == 'Y');
	
	freeDFA(dfa);

}