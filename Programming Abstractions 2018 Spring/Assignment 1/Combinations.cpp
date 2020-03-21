/*
 * File: Combinations.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Combinations problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include "console.h"
#include "simpio.h"
using namespace std;

int comb(int n, int k);
int enterPosInteger(string str);

//Asks user to enter n and k to compute combinations
int main() {

	int n = enterPosInteger("Enter n : ");
	int k = enterPosInteger("Enter k, k must not be bigger than n: ");
	while(k>n) {
		k = enterPosInteger("Error. Enter k again : ");
	}
	cout << "Combinations equals : " << comb(n,k) << endl;

    return 0;
}

//Combinations for (n,k) recursively
int comb(int n, int k) {
	
	if((n==0 || k == 0) || n==k) return 1; 
	
	return comb(n-1, k-1) + comb(n-1,k);
}

/*Since we can not enter a negative integer, we must make sure
	that user enter a positive number
*/

int enterPosInteger(string s){
	int x;
	while(true) {
		x = getInteger(s);
		if (x>=0) return x;
		cout << "Number must not be negative" << endl;
	}
}
