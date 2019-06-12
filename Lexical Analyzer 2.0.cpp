
#include "pch.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <locale>


using namespace std;

//Two-Dimensional Array that will manage state switching within the lexical analyzer
int dimArray[12][13] = {
//   1  2  3  4  5  6  7  8  9 10 11 12  13	
	{2, 4, 2, 6, 6, 5, 6, 6, 1, 1, 7, 9, 12}, // 1 STARTING STATE
	{2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 1, 12}, // 2 INSIDE STRING
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 12}, // 3 FINAL STATE - Out of string
	{10, 4, 10, 10, 10, 10, 10, 10, 10, 10, 10, 9, 12}, // 4 INSIDE INT
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 12}, // 5 FINAL STATE - Operator
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 12}, // 6 FINAL STATE - Seperator
	{7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 1, 12}, // 7 INSIDE COMMENT
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 12}, // 8 FINAL STATE - Out of Comment
	{11, 9, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12}, // 9 INSIDE REAL
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 12}, // 10 FINAL STATE - Out of integer
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 12}, // 11 FINAL STATE - Out of real
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // 12 FINAL STATE - Value not readable
};
	
//Array with keywords that should be recognized by the lexer
string keywords[15] = { "int", "float", "bool", "if", "else", "then", "do", "while", "whileend", "do", "doend", "for", "and", "or", "function" };

//Input a character and return the corressponding column number in the 2D array
int getCol(char c) {	//Some columns reach the same state in lexical analyzer however I have kept these columns seperate, this is for future-proofing
	locale loc;			
	if (isalpha(c, loc)) return 1;

	if (isdigit(c, loc)) return 2;

	if (c == '$') return 3;

	if (c == '{' || c == '}') return 4;
	
	if (c == '(' || c == ')') return 5;
	
	if (c == '>' || c == '<' || c == '=' || c == '*' || c == '+' || c == '-' || c == '/' || c == '%') return 6;
	
	if (c == ',') return 7;
	
	if (c == ';' || c == ':' || c == '.' || c == '[' || c == ']') return 8;

	if (c == ' ') return 9;

	if (c == '\t') return 10;

	if (c == '!') return 11;

	if (c == '.') return 12;

	return 0;
}

void DFA(string line, string oFile) {											
	int sp = 0;		//string position
	int bp = 0;		//begin position
	int sl = 0;		//string length
	char cc;		//current character
	int lex_state = 1;	//lexical state

	ofstream myfile;				//output file
	myfile.open(oFile, ios::app);	//open file in append mode
	if (myfile.is_open()) {
		while (sp != line.length()) {													//Loops until all characters have been read
			cc = line[sp];																//Current character becomes the character at string position sp in line
			lex_state = dimArray[lex_state - 1][getCol(cc) - 1];						//Changes lexical state based on current lexical state and the column corressponding to the character
			if (lex_state == 1 || lex_state == 7) {										//If state is 1 (initial state) or 7 (inside of comment) then increase string position and begin position and set string length to 0
				sp++;
				bp++;
				sl = 0;
			}
			if (lex_state == 2 || lex_state == 4 || lex_state == 9) {					//If state is 2 (inside string) or 4 (inside integer) or 9 (inside real) increase string position and string length
				sp++;
				sl++;
			}
			if (lex_state == 3) {														//If state is 3 (string final state) then output to file and set begin position to string position, string length to 0, and lex state to 1
				string temp = "IDENTIFIER";
				for (unsigned int i = 0; i < 15; i++) {									//Checks if the string is a keyword
					if (line.substr(bp, sl) == keywords[i]) {
						temp = "KEYWORD\t";
					}
				}
				myfile << temp << "\t = \t" << line.substr(bp, sl) << '\n';
				bp = sp;
				sl = 0;
				lex_state = 1;
			}
			if (lex_state == 5) {														//If state is 5 (operator final state) then output to file the operator and set begin position to string position and string length to 0
				myfile << "OPERATOR\t = \t" << line.substr(bp, 1) << endl;
				bp = sp;
				sl = 0;
			}
			if (lex_state == 6) {														//If state is 6 (seperator final state) then output to file the seperator and set begin position to string position and string length to 0
				myfile << "SEPARATOR\t = \t" << line.substr(bp, 1) << endl;
				bp = sp;
				sl = 0;
			}
			if (lex_state == 8) {														//If state is 8 (out of comment) then set begin position to string position and set string length to 0
				bp = sp;
				sl = 0;
			}
			if (lex_state == 10) {														//If state is 10 (integer final state) then output to file the integer and set begin position to string position, string length to 0, and lex state to 1
				myfile << "INTEGER \t = \t" << line.substr(bp, sl) << endl;
				bp = sp;
				sl = 0;
				lex_state = 1;
			}
			if (lex_state == 11) {														//If state is 11 (real final state) then output to file the real and set begin position to string position, string length to 0 and lex state to 1
				myfile << "REAL\t \t = \t" << line.substr(bp, sl) << endl;
				bp = sp;
				sl = 0;
				lex_state = 1;
			}
			if (sp == line.length() && lex_state != 1) {								//If at the end of the string and lex state is not 1, check the state and output the corressponding set of characters to the file  
				if (lex_state == 3) {
					string temp = "IDENTIFIER";
					for (unsigned int i = 0; i < 15; i++) {
						if (line.substr(bp, sl) == keywords[i]) {
							temp = "KEYWORD\t";
						}
					}
					myfile << temp << "\t = \t" << line.substr(bp, sl) << endl;
				}
				if (lex_state == 10) {													
					myfile << "INTEGER \t = \t" << line.substr(bp, sl) << endl;
					bp = sp;
					sl = 0;
					lex_state = 1;
				}
				if (lex_state == 11) {
					myfile << "REAL\t \t = \t" << line.substr(bp, sl) << endl;
					bp = sp;
					sl = 0;
					lex_state = 1;
				}
			}
			if (lex_state == 12) {														//If state is 12 (unable to read character) display error message and stop parsing the line
				cout << "Unable to fully parse line. Unreadable character: " << line.substr(bp, 1) << endl;
				break;
			}
		}
		myfile.close();
	}
	else cout << "Unable to open output file";
	
}

void lexer(string fileName) {
	string line;																		//String that will hold lines from the text file
	ifstream myfile;
	ofstream myfileO;
	myfile.open(fileName);
	if (myfile.is_open()) {
		cout << fileName << " has successfully opened and will now be parsed.\n" << endl;
		cout << "Please enter the name of the text file you would like to output results to: ";
		string outFile;
		cin >> outFile;
		myfileO.open(outFile);															//Opens the file in order to clear anything that has been written on it
		myfileO.close();
		while (getline(myfile, line)) {													//Continues to input lines until the end of the text file has been reached
			DFA(line, outFile);													
		}
		myfile.close();
		cout << endl << fileName << " successfully parsed." << endl;
	}
	else cout << "Unable to open file";

}


int main()
{
	cout << "Please enter the name of the text file you would like to parse: ";
	string inpFile;
	cin >> inpFile;
	lexer(inpFile);
	
	cout << endl;
	system("pause");

	return 0;
}
