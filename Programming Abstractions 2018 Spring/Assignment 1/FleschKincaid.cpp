/*
 * File: FleschKincaid.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Flesch-Kincaid problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include "console.h"
#include "simpio.h"
#include <fstream>
#include <cctype>
#include "tokenscanner.h"
using namespace std;

const double C0 = -15.59;
const double C1 = 0.39;
const double C2 = 11.8;

int getSyllables(string word);
bool isVowel (char ch);
bool isEligible(string word, int index);
double grade(int words, int sentences, int syllbables);


/*Runs the program, asks user to enter filename correctly to generate is difficulty grade.
	Keeps track of words, syllables and sentences.
*/
int main() {

	ifstream inp;
	string filename = getLine("Enter filename : ");
	inp.open(filename.c_str());

	//Makes sure user enters correct filename at some point
	while(inp.fail()) {
		inp.clear();
		filename = getLine("Error, enter filename again : ");
		inp.open(filename.c_str());
	}

	TokenScanner tk(inp);
	tk.ignoreWhitespace();
	tk.addWordCharacters("'");

	int words = 0;
	int sentences = 0;	
	int syllables = 0;

	while(tk.hasMoreTokens()) {
		string word = tk.nextToken();
		
		//Spcial case for '
		if(tk.getTokenType(word) == WORD && word!="'") {
			words++;
			syllables += getSyllables(word);
		}

		if(word[0] == '.' || word[0] == '?' || word[0] == '!') sentences++;
	}

	inp.close();
	
	cout << "Words : " << words << endl;
	cout << "Syllables : " << syllables << endl;
	cout << "Sentences : " << sentences << endl;
	cout << "Grade of this text equals : " << grade(words,sentences,syllables) << endl; 

    return 0;
}

/*Generates syllables count. If length is 1 or 0, returns 1, else counts it.
*/
int getSyllables(string word) {

	int result = 0;

	if(word.length() <=2) {
		return 1;
	}

	if(isVowel(tolower(word[0]))) result++;

	for (int i=1; i<word.length(); i++) {
		if(isEligible(word,i)) {
			result++;
		}
	}

	if(result == 0) return 1;

	return result;
}

//Two vowels cant be one after another in a syllables and last 'e' doesnt count.
bool isEligible(string word, int index) {
	char thisCh = tolower(word[index]);
	char beforeCh = tolower(word[index-1]);
	int lastIndex = word.length()-1;
	
	if(isVowel(thisCh) && !isVowel(beforeCh) && !(index == lastIndex && thisCh == 'e')) {
		return true;
	}
	return false;
}

//Checks if ch is a vowel or 'y'
bool isVowel(char ch) {
	if(ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u' || ch == 'y') {
		return true;
	}
	return false;
}

//Generates difficulty grade
double grade(int words, int sentences, int syllables) {
	return (C0 + C1 * ((double)words/sentences) + C2 * ((double)syllables/words));
}
