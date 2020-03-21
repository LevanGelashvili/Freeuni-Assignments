/*
 * File: RandomWriter.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Random Writer problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include "console.h"
#include "random.h"
#include "simpio.h"
#include <fstream>
#include <cctype>
#include "map.h"
#include "vector.h"
using namespace std;

//Generates a random character from a given vector
char randomChar(Vector<char> vec) {
	int num = randomInteger(0,vec.size()-1);
	return vec[num];
}

//Chooses a seed from map, by iterating over strings and checking their value vector sizes
string chooseSeed(Map<string, Vector<char> > &map) {
	string result = "";
	int currentSize = 0;

	foreach(string str in map) {
		if(map[str].size() > currentSize) {
			result = str;
			currentSize = map[str].size();
		}
	}
	return result;
}

/*Writes information into map. Generates string consisting of num characters. Adds that string to 
	a map with values of all characters, procceeding that string in a text file.
*/
void gatherInfo(Map <string, Vector <char> > &map, ifstream &inp, int num) {
	
	char c;

	while(inp.get(c)) {

		string currentChars = "";

		for (int i=0; i<num; i++) {
			currentChars += c;
			inp.get(c);
		}
	
		//Checks if map already contains that string.
		if(map.containsKey(currentChars)) {
			map[currentChars].add(c);
		} else {
			Vector<char> newVec;
			newVec.add(c);
			map.put(currentChars,newVec);
		}

		//To generate every possible string, we need to go back num times.
		for (int i=0; i<num; i++) {
			inp.unget();
		}
	}
}


//Runs the program
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

	int k = getInteger("Enter order level [1-10]: ");
	while(k<1 || k>10) {
		k = getInteger("Value is out of range : ");
	}

	Map<string, Vector <char> > map; 	
	gatherInfo(map,inp,k);

	string seed = chooseSeed(map);
	//randomText keeps track of all random info we generated. It is set as seed at start.
	string randomText = seed;

	for (int i=0; i<2000 - seed.length(); i++) {
		if(!map.containsKey(seed)) break;
		char nextCh = randomChar(map[seed]);
		seed.erase(0,1);
		seed += nextCh;
		randomText += nextCh;
	}

	cout << randomText << endl;

    return 0;
}
