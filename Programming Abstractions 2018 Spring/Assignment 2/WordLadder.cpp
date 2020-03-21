/*
 * File: WordLadder.cpp
 * --------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Word Ladder problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include "strlib.h"
#include "console.h"
#include "lexicon.h"
#include "simpio.h"
#include "queue.h"
using namespace std;

//Prints the result ladder.
void printResult (Vector <string> &vec, string dest) {
	string result = "Ladder found : ";
	for (int i=0; i<vec.size()-1; i++) {
		result += vec[i] + " -> ";
	}
	result += dest;
	cout << result << endl;
	cout << endl;
}

/*Creates a queue, adds the start word to it. Later, generates every possible word
	with 1 char difference from the last dequeued ladder and checks if it equals
	the destination word. If so, returns the found answer, otherwise, repeats the 
	process and enqueues another vector full of words with 1 char difference.
*/
int main() {

	Lexicon lex("EnglishWords.dat");

	while(true) {

		//This boolean determines whether ladder is found or not.
		bool ladderFound = false;

		Queue <Vector<string> > queue;
		//This set keeps track of words we have already used, not to repeat them.
		Set <string> usedWords;

		string start = getLine("Enter start word (or press return to quit) : ");
		if(start == " ") break;
		string dest = getLine("Enter destination word : ");
		
		while(dest.length() != start.length()) {
			dest = getLine("Error, words must be of same length : ");
		}

		cout << "Searching..." << endl;
		
		Vector <string> firstVec;
		usedWords += start;
		firstVec.add(start);
		queue.enqueue(firstVec);
		
		//Starts the while loop
		while(!queue.isEmpty()) {

			if(start == dest) break;

			Vector <string> vec = queue.dequeue();
			string lastWord = vec[vec.size()-1];

			if(lastWord == dest) {
				ladderFound = true;
				printResult(vec,dest);
				break;
			}

			string temp = lastWord;
			
			//Checks for all words with 1 letter difference
			for (int i=0; i<temp.length(); i++) {
				temp = lastWord;
				for (char ch = 'a'; ch <= 'z'; ch++) {
					temp[i] = ch;
					if(lex.contains(temp) && !usedWords.contains(temp)) {
						Vector <string> currentVec = vec;
						currentVec.add(temp);
						usedWords += temp;
						queue.enqueue(currentVec);
					}
				}
			}
		}
		if(!ladderFound) cout << "No ladder found. " << endl;
	}

    return 0;
}
