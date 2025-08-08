#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <unordered_map>
using namespace std;

// This will create a distinction between everything
enum class TokenType {let, set, display, variable, integer};

// Token structure
class Token {
private:
	TokenType type;
	string keyword; // for the command name
	int value; // for numbers, wont do anything for anything else
	string varName; // for variable names
public:
	Token(TokenType t) : type(t) {
		if (type == TokenType::let) {
			keyword = "let";
		}
		else if (type == TokenType::set) {
			keyword = "set";
		}
		else if (type == TokenType::display) {
			keyword = "display";
		}
	};
	Token(TokenType t, int n) : type(t), value(n) {};
	Token(TokenType t, string n) : type(t), varName(n) {};

	TokenType getType() {
		return type;
	}

	string getKeyword() {
		return keyword;
	}

	string getName() {
		return varName;
	}

	int getValue() {
		return value;
	}
};

class Statement {
private:
	string statement;
	string varName;
	int value;
public:
	Statement(string s, string n) : statement(s), varName(n) {}; // Let and Display Statement
	Statement(string s, string n, int v) : statement(s), varName(n), value(v) {}; // Set Statement

	string getName() {
		return varName;
	}
	int getValue() {
		return value;
	}

	string getStatement() {
		return statement;
	}
};

vector<vector<Token>> tokenize() {
	ifstream file("code.txt");
	vector<vector<Token>> tokenizedLines;
	vector<Token> tokens;
	vector<string> errors;
	string line;
	string token;

	if (!file.is_open()) {
		cout << "Could not open file" << endl;
		exit(1);
	}

	while (getline(file, line)) {
		tokens.clear();
		token.clear();

		for (int i = 0; i < line.size(); i++) {
			if (line[i] == ' ') {
				if (!token.empty()) {
					if (token == "let") {
						tokens.push_back(Token(TokenType::let));
					}
					else if (token == "set") {
						tokens.push_back(Token(TokenType::set));
					}
					else if (token == "display") {
						tokens.push_back(Token(TokenType::display));
					}
					else if (all_of(token.begin(), token.end(), ::isdigit)) {
						tokens.push_back(Token(TokenType::integer, stoi(token)));
					}
					else if (token[0] >= 'a' && token[0] <= 'z') {
						bool valid = true;
						for (int j = 0; j < token.size(); j++) {
							if (!isalpha(token[j]) && !isdigit(token[j])) {
								valid = false;
								break;
							}
						}
						if (valid) {
							tokens.push_back(Token(TokenType::variable, token));
						}
						else {
							errors.push_back(token + " is an invalid token\n");
						}
					}
					else {
						errors.push_back(token + " is an invalid token\n");
					}

					token.clear();
				}
			}
			else {
				token.push_back(line[i]);
			}
		}

		// Proccess last token because i didnt do it before lol + too lazy to refactor
		if (!token.empty()) {
			if (token == "let") {
				tokens.push_back(Token(TokenType::let));
			}
			else if (token == "set") {
				tokens.push_back(Token(TokenType::set));
			}
			else if (token == "display") {
				tokens.push_back(Token(TokenType::display));
			}
			else if (all_of(token.begin(), token.end(), ::isdigit)) {
				tokens.push_back(Token(TokenType::integer, stoi(token)));
			}
			else if (token[0] >= 'a' && token[0] <= 'z') {
				bool valid = true;
				for (int j = 0; j < token.size(); j++) {
					if (!isalpha(token[j]) && !isdigit(token[j])) {
						valid = false;
						break;
					}
				}
				if (valid) {
					tokens.push_back(Token(TokenType::variable, token));
				}
				else {
					errors.push_back(token + " is an invalid token\n");
				}
			}
			else {
				errors.push_back(token + " is an invalid token\n");
			}

			token.clear();
		}

		tokenizedLines.push_back(tokens);
	}

	if (errors.empty()) {
		return tokenizedLines;
	}
	else {
		for (string message : errors) {
			cout << message;
		}
		exit(1);
	}
}

string  displayParserError(vector<Token> tokens) {
	string errorMessage;
	for (int i = 0; i < tokens.size(); i++) {
		if (tokens[i].getType() == TokenType::let || tokens[i].getType() == TokenType::set || tokens[i].getType() == TokenType::display) {
			errorMessage += (tokens[i].getKeyword() + " ");
		}
		else if (tokens[i].getType() == TokenType::integer) {
			errorMessage += (to_string(tokens[i].getValue()) + " ");
		}
		else if (tokens[i].getType() == TokenType::variable) {
			errorMessage += (tokens[i].getName() + " ");
		}
	}

	errorMessage += ("is an invalid statement\n");

	return errorMessage;
}

vector<Statement> parser(vector<vector<Token>> tokenizedLines) {
	vector<Statement> statements;
	vector<string> errors;

	for (vector<Token> tokens : tokenizedLines) {
		if (tokens.size() > 3) {
			errors.push_back(displayParserError(tokens));
		}
		else if (tokens[0].getType() == TokenType::let) {
			if (tokens.size() != 2) {
				errors.push_back(displayParserError(tokens));
			}
			else if (tokens[1].getType() == TokenType::variable) {
				statements.push_back(Statement(tokens[0].getKeyword(), tokens[1].getName()));
			}
			else {
				errors.push_back(displayParserError(tokens));
			}
		}
		else if (tokens[0].getType() == TokenType::set) {
			if (tokens.size() != 3) {
				errors.push_back(displayParserError(tokens));
			}
			else if (tokens[1].getType() == TokenType::variable) {
				if (tokens[2].getType() == TokenType::integer) {
					statements.push_back(Statement(tokens[0].getKeyword(), tokens[1].getName(), tokens[2].getValue()));
				}
				else {
					errors.push_back(displayParserError(tokens));
				}
			}
			else {
				errors.push_back(displayParserError(tokens));
			}
		}
		else if (tokens[0].getType() == TokenType::display) {
			if (tokens.size() != 2) {
				errors.push_back(displayParserError(tokens));
			}
			else if (tokens[1].getType() == TokenType::variable) {
				statements.push_back(Statement(tokens[0].getKeyword(), tokens[1].getName()));
			}
			else {
				errors.push_back(displayParserError(tokens));
			}
		}
		else {
			errors.push_back(displayParserError(tokens));
		}
	}

	if (errors.empty()) {
		return statements;
	}
	else {
		for (string message : errors) {
			cout << message;
		}
		exit(1);
	}
}

void semanticChecker(vector<Statement> statements) {
	unordered_map<string, int> variableTable;
	vector<string> errors;
	vector<string> output;

	for (Statement statement : statements) {
		if (statement.getStatement() == "let") {
			if (variableTable.count(statement.getName())) {
				errors.push_back(statement.getName() + " has already been declared\n");
			}
			else {
				variableTable[statement.getName()] = -1;
			}
		}
		else if (statement.getStatement() == "set") {
			if (!variableTable.count(statement.getName())) {
				errors.push_back(statement.getName() + " has not been declared\n");
			}
			else {
				variableTable[statement.getName()] = statement.getValue();
			}
		}
		else if (statement.getStatement() == "display") {
			if (!variableTable.count(statement.getName())) {
				errors.push_back(statement.getName() + " has not been declared\n");
			}
			else if (variableTable[statement.getName()] < 0) {
				errors.push_back(statement.getName() + " has not been declared\n");
			}
			else {
				output.push_back(to_string(variableTable[statement.getName()]));
			}
		}
	}

	if (errors.empty()) {
		for (string out : output) {
			cout << out << endl;
		}
	}
	else {
		for (string message : errors) {
			cout << message;
		}
	}
}

int main() {
	vector<vector<Token>> tokenizedLines;
	vector<Statement> statements;

	tokenizedLines = tokenize();

	statements = parser(tokenizedLines);

	semanticChecker(statements);

	return 0;
}