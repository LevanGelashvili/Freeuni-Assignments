/*
 * File: ConsecutiveHeads.cpp
 * --------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Consecutive Heads problem.
 * [TODO: rewrite the documentation]
 */

#include < iostream >
#include "console.h"
#include "random.h"
using namespace std;

/*In headCount we store amount of consecutive heads, which becomes 0,
	if tails shows up. Flips determines total flip amount to reach 3 
	straight heads. Since chance of bo th is 50%, rand is divided by 2 
	to generate either 1 or 0.
*/
int main() {
	
	int headCount = 0;
	int flips = 0;

	while (true) {

		if(randomChance(0.5)) {
			headCount++;
			cout << "heads" << endl;
		} else {
			headCount = 0;
			cout << "tails" << endl;
		}
		
		flips++;

		if (headCount == 3)	break;
	}
	
	cout << "It took " << flips << " flips to get 3 consecutive heads." << endl;

	return 0;
}
