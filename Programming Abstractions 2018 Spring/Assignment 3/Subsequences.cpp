/*
 * File: Subsequences.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Subsequences problem
 * on Assignment #3.
 * [TODO: extend the documentation]
 */

#include <iostream>
#include <string>
#include "console.h"
#include "simpio.h"
using namespace std;

/* Given two strings, returns whether the second string is a
 * subsequence of the first string.
   Since empty string is a subsequence of everything, we erase letters
   from text and subsequence until it becomes empty. If text and subsequence
   start with the same letter, rest of both strings can be subsequential too.
   Else, we erase only text's first letter. If this way subsequence becomes empty,
   it means strings are subsequential.
 */
bool isSubsequence(string text, string subsequence) {
	if(subsequence.length() == 0) return true;
	if(text.length() == 0 && subsequence.length() !=0) return false;
	
	if(text[0] == subsequence[0]) subsequence.erase(0,1);
	text.erase(0,1);

	return (isSubsequence(text,subsequence));
	return false;
}

int main() {
	while(true) {
		string text = getLine("Enter text : ");
		string subs = getLine("Enter subsequence : ");
		if(isSubsequence(text,subs)) {
			cout << "true" << endl;
		} else {
			cout << "false" << endl;
		}
	}
    return 0;
}
