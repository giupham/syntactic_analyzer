#include <iostream>
#include <vector>
#include "lexical.h"
#include "syntactical.h"
using namespace std;


int main() {
	// variables definition
	string inputName;
	Lexer lex;
	Syntactical synt;

	cout << "Enter the name of the file you would like to Analyze: ";
	cin >> inputName;
	cout << endl;
	// Lexical part of the compiler
	if (!lex.analyze(inputName)) cout << endl << "Compilation aborted because of lexical errors!!" << endl << endl;
	// Syntactical part of the compiler
	else {
		cout << "The Lexical Analysis was Successfully Completed. result_lexemes.txt was created in the same directory." << endl << endl;
		if (synt.analyze(lex.getTokens()))
			cout << "The Syntactical Anaysis was Successfully Completed. result_syntax.txt was created in the same directory" << endl << endl;
	}

	return 0;
}