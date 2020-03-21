/*
 * File: NumericConversions.cpp
 * ---------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Numeric Conversions problem.
 * [TODO: rewrite the documentation]
 */

#include <iostream>
#include <string> 
#include "simpio.h"
#include <cmath>
#include "console.h"
using namespace std;

/* Function prototypes */
string intToString(int n);
string intToStringIfNotZero(int n);
int stringToInt(string str);

/* Main program */

int main() {

	while (true) {
		int n = getInteger("Enter int : ");
		cout << intToString(n) << endl;
	}

    return 0;
}
	
/*Converts int to string. If entered string is "0", returns 0.
	Else, invokes a completely different method.
*/

string intToString(int n) {	
	if(n==0) return "0";
	return intToStringIfNotZero(n);	
}

/*This method converts int to a string (if not "0"). If int is negative, "-" is added 
	to a string and its positive counterpart is calculated. If int is from
	1 to 9 its char version gets added automatically. Else, last number is 
	extracted and rest of the int gets added to string recursively.
*/

string intToStringIfNotZero(int n) {
	
	if(n<0) return "-" + intToString(-n);
	if(n<=9 && n>=1) string(1, (char(n + '0')));
	if(n == 0) return "";

	return intToStringIfNotZero(n/10) + string(1, (char(n%10 + '0')));
}

/*This method converts string to an integer. First line checks
	if entered string is a negative integer. In this case, it 
	takes the number part of it and makes it positive.
	Second line checks the base case and returns one number.

	FirstNum takes strings first number character and multiplies it
	by a power of ten and then adds the rest of the number recursively.
*/

int stringToInt(string str) {
	if(str[0] == '-') return -stringToInt(str.substr(1));
	if(str.length() == 1) return str[0] - '0';

	int firstNum = (str[0] - '0') * pow(10,(double)(str.length()-1));
	return firstNum + stringToInt(str.substr(1));
}
