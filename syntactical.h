#ifndef SYNTACTICAL_H
#define SYNTACTICAL_H

#include "lexical.h"

using namespace std;

class Syntactical {
private:
	// attributes
	vector<Token> tokens;
	int idNextToken;
	Token currToken;
	int nErrors;
	ofstream out;
	string currInstr;

	// Function for getting the next lexeme
	void nextToken() {
		// Printing the current token
		if (idNextToken <= tokens.size() and idNextToken > 0) {
			out << "Token: " << currToken.type << "\t Lexeme: " << currToken.lexeme << endl;
			out << currInstr << endl;
			currInstr = "";
		}
		// Getting the next token
		if (idNextToken < tokens.size()) currToken = tokens[idNextToken++];
		else currToken = Token(currToken.line, "None", "");
	}

	// Function for printing errors
	void printError(string lexeme, string expected) {
		out << "Lexeme: " << lexeme << " Syntax Error in line " << currToken.line << ": " << expected << " expected" << endl;
		cout << "Lexeme: " << lexeme << " Syntax Error in line " << currToken.line << ": " << expected << " expected" << endl;
		nErrors++;
	}

public:
	// Constructor Function
	Syntactical() : idNextToken(0), nErrors(0) {}
	// Launcher Function
	bool analyze(vector<Token> tks) {
		out.open("result_syntax.txt");
		tokens = tks;
		nextToken();
		RL_RAT20SU();
		if (currToken.type != "None") {
			cout << "Syntax Error in line " << currToken.line << ": Unprocessed Tokens" << endl;
			out << "Syntax Error in line " << currToken.line << ": Unprocessed Tokens" << endl;
			nErrors++;
		}
		out.close();
		return (nErrors == 0);
	}

	// ============ FUNCTIONS FOR THE SYNTAX RULES =================
	void RL_RAT20SU() {
		// Firsts set for this rule
		string firsts[] = { "$$" };
		// Processing rule
		if (isInArray(firsts, 1, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<RAT20SU> -> $$ <OPT_DEC_LIST> <STATEMENT_LIST> $$\n";
			// Consume last token
			nextToken();
			// Now let's process the rest of the rule
			RL_OPT_DEC_LIST();
			RL_STATEMENT_LIST();
			if (currToken.lexeme == "$$") nextToken();
			else printError(currToken.lexeme, "$$");
		}
		else
			printError(currToken.lexeme, "$$");
	}

	void RL_OPT_DEC_LIST() {
		// Firsts set for this rule
		string firsts[] = { "integer", "boolean" };
		// in firsts is also included epsilon then we will also need to the follows for validation
		string follows[] = { "{", "if", "get", "put", "while" }; // Also include TT:Identifier (TokenType:Identifier)
		// Processing rule
		if (isInArray(firsts, 2, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<OPT_DEC_LIST> -> <DECLARATION_LIST> | <EMPTY>\n";
			// Now let's process the rest of the rule
			RL_DECLARATION_LIST();
		}
		else if (!isInArray(follows, 5, currToken.lexeme) and currToken.type != "identifier")
			// printError("'integer', 'boolean', {', 'if', 'get', 'put' or 'while'");
			printError(currToken.lexeme, "A first of a declaration or a statement list");

	}

	void RL_DECLARATION_LIST() {
		// Firsts set for this rule
		string firsts[] = { "integer", "boolean" };
		// Processing rule
		if (isInArray(firsts, 2, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<DECLARATION_LIST> -> <DECLARATION> ; <DECLARATION_LIST_PRIME>\n";
			// Now let's process the rest of the rule
			RL_DECLARATION();
			if (currToken.lexeme == ";") nextToken();
			else printError(currToken.lexeme, ";");
			RL_DECLARATION_LIST_PRIME();
		}
		else
			printError(currToken.lexeme, "'integer' or 'boolean'");
	}

	void RL_DECLARATION_LIST_PRIME() {
		// Firsts set for this rule
		string firsts[] = { "integer", "boolean" };
		// in firsts is also included epsilon then we will also need to the follows for validation
		string follows[] = { "{", "if", "get", "put", "while" }; // Also include TT:Identifier
		// Processing rule
		if (isInArray(firsts, 2, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<DECLARATION_LIST_PRIME> -> <DECLARATION_LIST> | <EMPTY>\n";
			// Now let's process the rest of the rule
			RL_DECLARATION_LIST();
		}
		else if (!isInArray(follows, 5, currToken.lexeme) and currToken.type != "identifier")
			// printError("'integer', 'boolean', '{', 'if', 'get', 'put', 'while' or Identifier");
			printError(currToken.lexeme, "A first of a declaration or a statement list");
	}

	void RL_DECLARATION() {
		// Firsts set for this rule
		string firsts[] = { "integer", "boolean" };
		// Processing rule
		if (isInArray(firsts, 2, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<DECLARATION> -> <QUALIFIER> <TT_Identifier>\n";
			// Now let's process the rest of the rule
			RL_QUALIFIER();
			if (currToken.type == "identifier") nextToken();
			else printError(currToken.lexeme, "Identifier");
		}
		else
			printError(currToken.lexeme, "'integer' or 'boolean'");
	}

	void RL_QUALIFIER() {
		// Firsts set for this rule
		string firsts[] = { "integer", "boolean" };
		// Processing rule
		if (isInArray(firsts, 2, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<QUALIFIER> -> integer | boolean\n";
			// Just Consume last token
			nextToken();
		}
		else
			printError(currToken.lexeme, "'integer' or 'boolean'");

	}

	void RL_STATEMENT_LIST() {
		// Firsts set for this rule
		string firsts[] = { "{", "if", "get", "put", "while" }; // Also include TT:Identifier
		// Processing rule
		if (isInArray(firsts, 5, currToken.lexeme) or currToken.type == "identifier") {
			// Concat the instruction of this rule
			currInstr += "\t<STATEMENT_LIST> -> <STATEMENT> <STATEMENT_LIST_PRIME>\n";
			// Now let's process the rest of the rule
			RL_STATEMENT();
			RL_STATEMENT_LIST_PRIME();
		}
		else
			printError(currToken.lexeme, "A first symbol of Statement");
		// printError("'{', 'if', 'get', 'put', 'while' or Identifier");

	}

	void RL_STATEMENT_LIST_PRIME() {
		// Firsts set for this rule
		string firsts[] = { "{", "if", "get", "put", "while" }; // Also include TT:Identifier
		// in firsts is also included epsilon then we will also need to the follows for validation
		string follows[] = { "$$", "}" };
		// Processing rule
		if (isInArray(firsts, 5, currToken.lexeme) or currToken.type == "identifier") {
			// Concat the instruction of this rule
			currInstr += "\t<STATEMENT_LIST_PRIME> -> <STATEMENT_LIST> | <EMPTY>\n";
			// Now let's process the rest of the rule
			RL_STATEMENT_LIST();
		}
		else if (!isInArray(follows, 2, currToken.lexeme))
			printError(currToken.lexeme, "A first of Statement or $$");
		// printError("'{', 'if', 'get', 'put', 'while', '$$', '}' or Identifier");

	}

	void RL_STATEMENT() {
		// Firsts set for this rule
		string firsts[] = { "{", "if", "get", "put", "while" }; // Also include TT:Identifier
		// Processing rule
		if (isInArray(firsts, 5, currToken.lexeme) or currToken.type == "identifier") {
			// Concat the instruction of this rule
			currInstr += "\t<STATEMENT> -> <COMPOUND> | <ASSIGN> | <IF> | <GET> | <PUT> | <WHILE>\n";
			// Now let's process the rest of the rule
			if (currToken.lexeme == "{") RL_COMPOUND();
			else if (currToken.lexeme == "if") RL_IF();
			else if (currToken.lexeme == "get") RL_GET();
			else if (currToken.lexeme == "put") RL_PUT();
			else if (currToken.lexeme == "while") RL_WHILE();
			else RL_ASSIGN();
		}
		else
			printError(currToken.lexeme, "First symbol of Compound, Assign, If, Get, Put or While");

	}

	void RL_COMPOUND() {
		// Firsts set for this rule
		// The only first for this rule is "{"
		if (currToken.lexeme == "{") {
			// Concat the instruction of this rule
			currInstr += "\t<COMPOUND> -> { <STATEMENT> <STATEMENT_LIST_PRIME> }\n";
			// Consume last token
			nextToken();
			// Now let's process the rest of the rule
			RL_STATEMENT();
			RL_STATEMENT_LIST();
			if (currToken.lexeme == "}") nextToken();
			else printError(currToken.lexeme, "'}'");
		}
		else
			printError(currToken.lexeme, "'{'");
	}

	void RL_ASSIGN() {
		// The only first for this rule is an Identifier(Token Type)
		if (currToken.type == "identifier") {
			// Concat the instruction of this rule
			currInstr += "\t<ASSIGN> -> <TT_Identifier> = <EXPRESSION> ;\n";
			// Consume last token
			nextToken();

			if (currToken.lexeme == "=") nextToken();
			else printError(currToken.lexeme, "'='");

			RL_EXPRESSION();

			if (currToken.lexeme == ";") nextToken();
			else printError(currToken.lexeme, "';'");
		}
		else
			printError(currToken.lexeme, "Identifier");
	}

	void RL_IF() {
		// Firsts set for this rule
		// The only first for this rule is "if"
		if (currToken.lexeme == "if") {
			// Concat the instruction of this rule
			currInstr += "\t<IF> -> if ( <CONDITION> ) <STATEMENT> <IF_PRIME>\n";
			// Consume last token
			nextToken();

			if (currToken.lexeme == "(") nextToken();
			else printError(currToken.lexeme, "'('");

			RL_CONDITION();

			if (currToken.lexeme == ")") nextToken();
			else printError(currToken.lexeme, "')'");

			RL_STATEMENT();
			RL_IF_PRIME();
		}
		else
			printError(currToken.lexeme, "'if'");


	}

	void RL_IF_PRIME() {
		// Firsts set for this rule
		string firsts[] = { "fi", "otherwise" };
		// Processing rule
		if (isInArray(firsts, 2, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<IF_PRIME> -> fi | otherwise <STATEMENT> fi\n";

			// Now let's process the rest of the rule
			if (currToken.lexeme == "fi") nextToken();
			else {
				nextToken();
				RL_STATEMENT();
				if (currToken.lexeme == "fi") nextToken();
				else printError(currToken.lexeme, "'fi'");
			}
		}
		else
			printError(currToken.lexeme, "'fi' or 'otherwise'");
	}

	void RL_PUT() {
		// Firsts set for this rule
		// The only first for this rule is "put"
		if (currToken.lexeme == "put") {
			// Concat the instruction of this rule
			currInstr += "\t<PUT> -> put ( <TT_Identifier> ) ;\n";
			// Consume last token
			nextToken();

			if (currToken.lexeme == "(") nextToken();
			else printError(currToken.lexeme, "'('");

			if (currToken.type == "identifier") nextToken();
			else printError(currToken.lexeme, "Identifier");

			if (currToken.lexeme == ")") nextToken();
			else printError(currToken.lexeme, "')'");

			if (currToken.lexeme == ";") nextToken();
			else printError(currToken.lexeme, "';'");
		}
		else
			printError(currToken.lexeme, "'put'");

	}

	void RL_GET() {
		// Firsts set for this rule
		// The only first for this rule is "get"
		if (currToken.lexeme == "get") {
			// Concat the instruction of this rule
			currInstr += "\t<GET> -> get ( <TT_Identifier> ) ;\n";
			// Consume last token
			nextToken();

			if (currToken.lexeme == "(") nextToken();
			else printError(currToken.lexeme, "'('");

			if (currToken.type == "identifier") nextToken();
			else printError(currToken.lexeme, "Identifier");

			if (currToken.lexeme == ")") nextToken();
			else printError(currToken.lexeme, "')'");

			if (currToken.lexeme == ";") nextToken();
			else printError(currToken.lexeme, "';'");
		}
		else
			printError(currToken.lexeme, "'get'");

	}

	void RL_WHILE() {
		// Firsts set for this rule
		// The only first for this rule is "while"
		if (currToken.lexeme == "while") {
			// Concat the instruction of this rule
			currInstr += "\t<WHILE> -> while ( <CONDITION> ) <STATEMENT>\n";
			// Consume last token
			nextToken();

			if (currToken.lexeme == "(") nextToken();
			else printError(currToken.lexeme, "'('");

			RL_CONDITION();

			if (currToken.lexeme == ")") nextToken();
			else printError(currToken.lexeme, "')'");

			RL_STATEMENT();
		}
		else
			printError(currToken.lexeme, "'while'");

	}

	void RL_CONDITION() {
		// Firsts set for this rule
		string firsts[] = { "-", "(", "true", "false" }; // Also include TT:Identifier and TT:Integer
		// Processing rule
		if (isInArray(firsts, 4, currToken.lexeme) or currToken.type == "identifier" or currToken.type == "integer") {
			// Concat the instruction of this rule
			currInstr += "\t<CONDITION> -> <EXPRESSION> <RELOP> <EXPRESSION>\n";
			// Now let's process the rest of the rule
			RL_EXPRESSION();
			RL_RELOP();
			RL_EXPRESSION();
		}
		else
			printError(currToken.lexeme, "First symbol of a Expression");
		// printError("'-', '(', 'true', 'false', Indentifier or Integer");
	}

	void RL_RELOP() {
		// Firsts set for this rule
		string firsts[] = { "==", ">", "<" };
		// Processing rule
		if (isInArray(firsts, 3, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<RELOP> -> == | > | <\n";
			// Just consume last token
			nextToken();
		}
		else
			printError(currToken.lexeme, "Relational Operators: '==', '>' or '<'");
	}

	void RL_EXPRESSION() {
		// Firsts set for this rule
		string firsts[] = { "-", "(", "true", "false" }; // Also include TT:Identifier and TT:Integer
		// Processing rule
		if (isInArray(firsts, 4, currToken.lexeme) or currToken.type == "identifier" or currToken.type == "integer") {
			// Concat the instruction of this rule
			currInstr += "\t<EXPRESSION> -> <TERM> <EXPRESSION_PRIME>\n";
			// Now let's process the rest of the rule
			RL_TERM();
			RL_EXPRESSION_PRIME();
		}
		else
			printError(currToken.lexeme, "First symbol of a Term");
		// printError("'-', '(', 'true', 'false', Indentifier or Integer");
	}

	void RL_EXPRESSION_PRIME() {
		// Firsts set for this rule
		string firsts[] = { "+", "-" };
		// in firsts is also included epsilon then we will also need to the follows for validation
		string follows[] = { ";", "==", ">", "<", ")" };
		// Processing rule
		if (isInArray(firsts, 2, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<EXPRESSION_PRIME> -> + <TERM> <EXPRESSION_PRIME> | - <TERM> <EXPRESSION_PRIME> | <EMPTY>\n";
			// Consume last token
			nextToken();
			// Now let's process the rest of the rule
			RL_TERM();
			RL_EXPRESSION_PRIME();
		}
		else if (!isInArray(follows, 5, currToken.lexeme))
			printError(currToken.lexeme, "'+', '-', ';' or Relational Operators");
	}

	void RL_TERM() {
		// Firsts set for this rule
		string firsts[] = { "-", "(", "true", "false" }; // Also include TT:Identifier and TT:Integer
		// Processing rule
		if (isInArray(firsts, 4, currToken.lexeme) or currToken.type == "identifier" or currToken.type == "integer") {
			// Concat the instruction of this rule
			currInstr += "\t<TERM> -> <FACTOR> <TERM_PRIME>\n";
			// Now let's process the rest of the rule
			RL_FACTOR();
			RL_TERM_PRIME();
		}
		else
			printError(currToken.lexeme, "First of Factor");
	}

	void RL_TERM_PRIME() {
		// Firsts set for this rule
		string firsts[] = { "*", "/" };
		// in firsts is also included epsilon then we will also need to the follows for validation
		string follows[] = { "+", "-"/*, ";", "==", ">", "<", ")" */ };
		// Processing rule
		if (isInArray(firsts, 2, currToken.lexeme)) {
			// Concat the instruction of this rule
			currInstr += "\t<TERM_PRIME> -> * <FACTOR> <TERM_PRIME> | / <FACTOR> <TERM_PRIME> | <EMPTY>\n";
			// Consume last token
			nextToken();
			// Now let's process the rest of the rule
			RL_FACTOR();
			RL_TERM_PRIME();
		}
		// else if (!isInArray(follows, 7, currToken.lexeme))
		// 	printError("'*', '/', '+', '-', ';', '==', '>', '<' or ')'");
	}

	void RL_FACTOR() {
		// Firsts set for this rule
		string firsts[] = { "-", "(", "true", "false" }; // Also include TT:Identifier and TT:Integer
		// Processing rule
		if (isInArray(firsts, 4, currToken.lexeme) or currToken.type == "identifier" or currToken.type == "integer") {
			// Concat the instruction of this rule
			currInstr += "\t<FACTOR> -> - <PRIMARY>  | <PRIMARY>\n";
			// Now let's process the rest of the rule
			if (currToken.lexeme == "-") nextToken();
			RL_PRIMARY();
		}
		else
			printError(currToken.lexeme, "'-', or Primary");
	}

	void RL_PRIMARY() {
		// Firsts set for this rule
		string firsts[] = { "-", "(", "true", "false" }; // Also include TT:Identifier and TT:Integer
		// Processing rule
		if (isInArray(firsts, 4, currToken.lexeme) or currToken.type == "identifier" or currToken.type == "integer") {
			// Concat the instruction of this rule
			currInstr += "\t<PRIMARY> -> <TT_Identifier> | <TT_Integer> | ( <FACTOR> <TERM_PRIME> <EXPRESSION_PRIME> ) | true | false\n";

			// Now let's process the rest of the rule
			if (currToken.lexeme == "(") {
				nextToken();
				RL_FACTOR();
				RL_TERM_PRIME();
				RL_EXPRESSION_PRIME();
				if (currToken.lexeme == ")") nextToken();
				else printError(currToken.lexeme, ")");
			}
			else nextToken();

		}
		else
			printError(currToken.lexeme, "'-', '(', 'true', 'false', Indentifier or Integer");

	}

	void RL_EMPTY() {
		// in firsts is included just epsilon then we will need to the follows for validation
		string follows[] = { "+", "-", ";", "==", ">", "<", ")", "$$", "{", "}", "if", "get", "put", "while" }; // Also is included TT:Identifier
		// Processing rule
		if (!isInArray(follows, 14, currToken.lexeme) and currToken.type != "identifier" and currToken.type != "integer") {
			printError(currToken.lexeme, "'+', '-', ';', '==', '>', '<', ')', '$$', '{', '}', 'if', 'get', 'put' or 'while'");
		}
	}

};

#endif
