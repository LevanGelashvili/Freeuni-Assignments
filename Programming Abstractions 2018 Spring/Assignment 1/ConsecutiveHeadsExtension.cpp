#include <iostream>
#include "console.h"
#include <string>
#include "strlib.h"
#include "simpio.h"
#include "random.h"
using namespace std;

//Checks if entered string is invalid, we only need H-s and T-s
bool isAnError(string str) {

	if(str.length() == 0) return true; 
	for (int i=0; i<str.length(); i++) {
		if(str[i]!='H' && str[i]!='T') return true;
	}
	return false;
}

//Generates a random flip, flipping a coin n times
string randomFlip(int n) {
	string result;

	for (int i=0; i<n; i++) {
		if(randomChance(0.5)) {
			result += "H";
		} else {
			result += "T";
		}
	}
	return result;
}

//Generates all statistics
void generate(string originalFlip) {
	int attempts = 0;
	
	string newFlip;
	while(originalFlip != newFlip) {
		newFlip = randomFlip(originalFlip.length());
		attempts++;
		cout << newFlip << endl;
	}
	cout << "It took " << attempts << " attempts to reach " << newFlip << endl;
}

//Runs the program
int main() {
	string str = getLine("Enter a string consisting of H-s and T-s : ");
	while(isAnError(toUpperCase(str))) {
		str = getLine("Error. Enter string again : ");
	}

	generate(toUpperCase(str));

	return 0;
}