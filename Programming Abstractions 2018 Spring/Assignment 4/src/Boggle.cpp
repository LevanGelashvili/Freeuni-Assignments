/*
 * File: Boggle.cpp
 * ----------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the main starter file for Assignment #4, Boggle.
 * [TODO: extend the documentation]
 */

/*	!!!!!!!!!!!!!
	EXTENSION is included in this cpp file. However, programs asks you to enter yes, whenever you
	want to play a game with an implemented extension. First extension is choosing 5x5 grid(big boggle),
	second is allowing user to enter all the chars himself and third is choosing to enter words by mouse.
*/

#include <iostream>
#include "gobjects.h"
#include "gboggle.h"
#include "gevents.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "strlib.h"
#include "simpio.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;
const int PAUSE_DELAY = 450;

const string STANDARD_CUBES[16]  = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
 
const string BIG_BOGGLE_CUBES[25]  = {
    "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
    "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
    "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
    "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
    "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

/* Function prototypes */

void start(GWindow &gw);
void welcome();
void giveInstructions();
void setupGame(GWindow &gw, Grid<char> &gr);
int chooseSize();
void chooseLetters(int size,Vector<string> &letters, Vector<char> &chars);
void swap(Vector<string> &letters, int n, int m);
void chooseChars(Vector<string> &letters, Vector<char> &chars);
void drawLetters(int size,Vector<char> &chars, Grid<char> &gr, bool enteredManually);
void playGamePlayer(Grid<char> &gr, Lexicon &lex,Set<string> &usedWords,GWindow &gw);
void playGameComputer(Grid<char> &gr, Lexicon &lex, Set<string> &usedWords);
bool isAValidWord(string guess,Lexicon &lex, Set<string> &usedWords);
bool canBeFormedOnGrid(string guess, Grid<char> &gr);
bool wordIsFormed(string guess, Grid<char> &gr, Grid<bool> &visited, int row, int col);
void startGuessingFrom(Grid<char> &gr, int row, int col, Lexicon &lex, Set<string> &usedWords, Grid<bool> &visited, string str);
bool answerIsYes(string question);
void enterManually(int size, Vector<char> &chars);
bool isValidManualInput(string ch);
void colorSquares(Grid<bool> &visited);
void paintSquares(Grid<bool> &visited, bool flag);
string chooseAWordByMouse(Grid<char> &gr,GWindow &gw, int n);
string startEntering(Grid<char> &gr,GWindow &gw, GPoint p1, GPoint p2, int n);

/* Main program */

//Runs the program
int main() {
    GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
    initGBoggle(gw);
    welcome();
    giveInstructions();
	start(gw);
    return 0;
}

//Runs one gameplay
void start(GWindow &gw) {
	Grid<char> gr;
	Lexicon lex("EnglishWords.dat");
	Set<string> usedWords;
	setupGame(gw,gr);
	playGamePlayer(gr,lex,usedWords,gw);
	playGameComputer(gr,lex,usedWords);
	if(answerIsYes("Would you like to play again ? : ")) start(gw); 
}

//Returns true if user entered "yes", otherwise returns false
bool answerIsYes(string question) {
	bool result;
	string line = getLine(question);
	while(true) {
		if(toLowerCase(line) == "yes") {
			result = true;
			break;
		}
		if(toLowerCase(line) == "no") {
			result = false;
			break;
		}
		line = getLine("Please enter yes or no : ");
	}
	return result;
}

//Initiates computers turn
void playGameComputer(Grid<char> &gr, Lexicon &lex, Set<string> &usedWords) {
	cout << "Computer's turn " << endl; 
	for (int i=0; i<gr.numRows(); i++) {
		for (int j=0; j<gr.numCols(); j++) {
			Grid <bool> visited(gr.numRows(),gr.numCols());
			string empty = "";
			startGuessingFrom(gr,i,j,lex,usedWords,visited,empty);
		}
	}
}

/*Starts guessing words from eneter row, col. If lexicon doesnt contain a word with given prefix, row,col is out of bounds
	or we have already visited this char, returns false. Else, adds this char to a string and searches for next char in 
	neighboring locations.
*/
void startGuessingFrom(Grid<char> &gr, int row, int col, Lexicon &lex, Set<string> &usedWords, Grid <bool> &visited, string str) {

	if(!lex.containsPrefix(str) || !gr.inBounds(row,col) || visited[row][col]) return;
	if(str.length() >= 4 && lex.contains(str) && !usedWords.contains(toUpperCase(str))) {
		usedWords += toUpperCase(str);
		recordWordForPlayer(str,COMPUTER);
	}
	visited[row][col] = true;
	str += gr[row][col];

	for (int i = row-1; i<row+2; i++) {
		for (int j = col-1; j<col+2; j++) {
			startGuessingFrom(gr,i,j,lex,usedWords,visited,str);
		}
	}
	//We need to set this to false, for backtracking
	visited[row][col] = false;
}

//Initiates players turn
void playGamePlayer(Grid<char> &gr, Lexicon &lex,Set<string> &usedWords,GWindow &gw) {

	cout << "Player's turn " << endl;
	bool mouseChoice;

	//Extension : Lets a player choose to enter with mouse
	if(answerIsYes("Would you like to choose a word with a mouse? : ")) {
		mouseChoice = true;
	} else {
		mouseChoice = false;
	}

	while(true) {
		string guess;
		if(mouseChoice) {
			guess = chooseAWordByMouse(gr,gw,gr.numRows());
		} else {
			guess = getLine("Enter a word (hit return to end your turn) : ");
		}
		if(guess == "") break;
		if(isAValidWord(guess,lex,usedWords)) {
			if(canBeFormedOnGrid(toUpperCase(guess),gr)) {
				usedWords += toUpperCase(guess);
				recordWordForPlayer(guess,HUMAN);
				cout << guess << " is a valid word, congrats! " << endl; 
			} else {
				cout << toUpperCase(guess) << " can not be formed on this grid." << endl;
			}
		}
	}
}

// Returns true if string is a valid word.
bool isAValidWord(string guess,Lexicon &lex, Set<string> &usedWords) {
	if(guess.length() < 4) {
		cout << "Minimum word length is 4." << endl;
		return false;
	} else if(!lex.contains(guess)) {
		cout << toUpperCase(guess) << " is not a valid word." << endl;
		return false;
	} else if(usedWords.contains(toUpperCase(guess))) {
		cout << "You have already used this word." << endl;
		return false;
	}
	return true;
}

//Checks if a word can be formed on grid, checking every possible location
bool canBeFormedOnGrid(string guess, Grid<char> &gr) {
	Grid <bool> visited(gr.numRows(),gr.numCols());
	for (int i=0; i<gr.numRows(); i++) {
		for (int j=0; j<gr.numCols(); j++) {
			if(guess[0] == gr[i][j]) {
				if(wordIsFormed(guess,gr,visited,i,j)) {
					return true;
				}
			}
		}
	}
	return false;
}

/* If we stumbled upon a char, that is first char of entered word, we start searching. Just like in
	other algorithm, we return false is we have already visited that location or we are out of bounds.
	If string is empty, we have exhausted the word and return true. Else we continue searching in neighbroing locations.
*/
bool wordIsFormed(string guess, Grid<char> &gr, Grid<bool> &visited, int row, int col) {
	//Base cases
	if(guess.length() == 0) {
		colorSquares(visited);
		return true;
	}
	if(!gr.inBounds(row,col) || visited[row][col]) return false;

	if(guess[0] == gr[row][col]) {
		visited[row][col] = true;
		string rest = guess.substr(1);
		for (int i = row-1; i<row+2; i++) {
			for (int j = col-1; j<col+2; j++) {
				if(wordIsFormed(rest,gr,visited,i,j)) return true;
			}
		}
		//Backtracking
		visited[row][col] = false;
	}
	return false;
}

//Highlights squares on graphicsProgram of a guessed word
void colorSquares(Grid<bool> &visited) {
	paintSquares(visited,true);
	pause(PAUSE_DELAY);
	paintSquares(visited,false);
}

//If flag is true, paints all squares on grid, else, reverts them to normal form
void paintSquares(Grid<bool> &visited, bool flag) {
	for (int row = 0; row<visited.nRows; row++) {
		for (int col = 0; col<visited.nCols; col++) {
			if(visited[row][col]) {
				highlightCube(row,col,flag);
			}
		}	
	}
}	

//Sets the game up
void setupGame(GWindow &gw, Grid<char> &gr) {
	int size = chooseSize();
	gr.resize(size,size);
	drawBoard(size,size);
	Vector<string> letters;
	Vector<char> chars;
	bool enteredManually = false;
	if(answerIsYes("Would you like to enter chars manually ? : ")) {
		enterManually(size,chars);
		enteredManually = true;
	} else {
		chooseLetters(size,letters,chars);
	}
	drawLetters(size,chars,gr,enteredManually);
}

//Asks user to enter chars manually
void enterManually(int size,Vector<char> &chars) {
	for (int i=0; i<size; i++) {
		for (int j=0; j<size; j++) {
			string pos = "[" + integerToString(i+1) + "," + integerToString(j+1) + "]";
			string ch = toUpperCase(getLine("Enter char at " + pos + " : "));
			while(!isValidManualInput(ch)) {
				ch = toUpperCase(getLine("Please enter a valid char : "));
			}
			chars += ch[0];
			labelCube(i,j,ch[0]);
		}
	}
}

//Returns true if input is a letter
bool isValidManualInput(string ch) {
	return (ch.length() == 1 && isalpha(ch[0]));
}

//Lets user choose a size of a grid
int chooseSize() {
	cout << "You can choose standart boggle (4x4 grid)" << endl;
	cout << "Or Big Boggle (5x5). " << endl;
	if(answerIsYes("Would you like Big Boggle ? : ")) {
		return 5;
	} else {
		return 4;
	}
}

//Chooses random letters from string vector and puts them into a char vector
void chooseLetters(int size, Vector<string> &letters, Vector<char> &chars) {
	if(size == 4) {
		for (int i=0; i<16; i++) {
			letters += STANDARD_CUBES[i];
		}
	} else {
		for (int i=0; i<25; i++) {
			letters += BIG_BOGGLE_CUBES[i];
		}
	}
	for(int i=0; i<letters.size(); i++) {
		int rand = randomInteger(i,letters.size()-1);
		swap(letters,rand,i);
	}
	chooseChars(letters,chars);
}

// swaps two strings in a vector
void swap(Vector<string> &letters, int n, int m) {
	string temp = letters[n];
	letters[n] = letters[m];
	letters[m] = temp;
}

//Chooses a random char from a string
void chooseChars(Vector<string> &letters, Vector<char> &chars) {
	foreach(string word in letters) {
		int rand = randomInteger(0,word.length()-1);
		char ch = word[rand];
		chars.add(ch);
	}
}

//Draws chosen letters on a grid
void drawLetters(int size, Vector<char> &chars, Grid<char> &gr, bool enteredManually) {
	int index = 0;
	for (int i=0; i<size; i++) {
		for (int j=0; j<size; j++) {
			char ch = chars[index];
			if(!enteredManually) {
				labelCube(i,j,ch);
			}
			gr[i][j] = ch;
			index++;
		}
	}
}

//Extension : Lets a user click chars on a window, by getting row and col for clicked x and y coordinates
string startEntering(Grid<char> &gr,GWindow &gw, GPoint p1, GPoint p2, int n) {
	cout << endl;
	cout << "Click chars inside a grid. To finish entering a word, click outside a grid." << endl;
	cout << endl;

	string result;
	GMouseEvent e;
	//calculates cube size
	double cube = (p2.getY() - p1.getY())/n;
	while(true) {
		e = waitForEvent(MOUSE_EVENT);
		//Only does this, if user clicked a mouse, doesn't work for dragging, moving etc.
		if (e.getEventType() == MOUSE_PRESSED) {
			double x = e.getX();
			double y = e.getY();
			//checks if user clicked outside a grid, meaning input is over
			if((x < p1.getX() || x > p2.getX()) || (y < p1.getY() || y > p2.getY())) {
				cout << "You clicked outside a grid. Entering is over." << endl;
				break;
			}
			int row = 0;
			int col = 0;
			//Calculates column
			for (int i=0; i<n; i++) {
				if(x >= p1.getX() + i*cube && x < p1.getX() + (i+1)*cube) {
					col = i;
					break;
				}
			}
			//Calculates row
			for (int i=0; i<n; i++) {
				if(y >= p1.getY() + i*cube && y < p1.getY() + (i+1)*cube) {
					row = i;
					break;
				}
			}
			result += gr[row][col];
			cout << "Your input looks like this so far : " << result << endl;
		}
	}
	return result;
}

/*Extension : lets a player choose a word by clicking chars on a window, needed to initialize GPoints
	Since this gwindow did not support returning objects, i calculated approximate locations myself
*/
string chooseAWordByMouse(Grid<char> &gr,GWindow &gw, int n) {
	//Approximate points
	GPoint p1(168,41);
	GPoint p2(383,257);
	return startEntering(gr,gw,p1,p2,n);
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
    cout << endl;
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl << endl;
    cout << "Hit return when you're ready...";
    getLine();
}