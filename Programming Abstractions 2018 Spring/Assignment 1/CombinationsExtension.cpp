#include <iostream>
#include "strlib.h"
#include "console.h"
#include "simpio.h"
using namespace std;

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


//Generates a string of a pascal numbers
string pascalLine(int n) {
	string result;
	int m = 0;

	while(m<=n) {
		result += integerToString(comb(n,m)) + " ";
		m++;
	}
	return result;
}

//draws pascal triangle, uses spaces to draw a pyramid, not a right triangle
void drawTriangle(int n){

	for (int i=0; i<n; i++) {
		string space;

		for (int k=0; k<n-i; k++) {
			space += " ";
		}
		cout << space << pascalLine(i) << endl;
	}
}

//Runs the program
int main() {
	int x = enterPosInteger("Enter row numbers for pascal triangle : ");
	cout << endl;
	drawTriangle(x);
	return 0;
}