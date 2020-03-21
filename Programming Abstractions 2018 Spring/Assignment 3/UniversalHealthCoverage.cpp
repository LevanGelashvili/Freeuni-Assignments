/*
 * File: UniversalHealthCoverage.cpp
 * ----------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the UniversalHealthCoverage problem
 * on Assignment #3.
 * [TODO: extend the documentation]
 */
#include <iostream>
#include <string>
#include <cmath>
#include "set.h"
#include "simpio.h"
#include "vector.h"
#include "console.h"
using namespace std;

/* Function: canOfferUniversalCoverage(Set<string>& cities,
 *                                     Vector< Set<string> >& locations,
 *                                     int numHospitals,
 *                                     Vector< Set<string> >& result);
 * Usage: if (canOfferUniversalCoverage(cities, locations, 4, result)
 * ==================================================================
 * Given a set of cities, a list of what cities various hospitals can
 * cover, and a number of hospitals, returns whether or not it's
 * possible to provide coverage to all cities with the given number of
 * hospitals.  If so, one specific way to do this is handed back in the
 * result parameter.
 */

/*We solve the problem by iterating over locations, choosing one, removing it from the list and solving the same problem
	without that hospital recursively. If we reach the base case, if our result has covered every city possible, then problem
	is solved, else, we search for another solution.
*/

bool canOfferUniversalCoverage(Set<string>& cities, Vector< Set<string> >& locations, int numHospitals, Vector< Set<string> >& result) {

	//Base case
	if(numHospitals == 0) {
		Set<string> finalCities = cities;
		for (int i=0; i<result.size(); i++) {
			finalCities -= result[i];
		}
		if(finalCities.isEmpty()) return true;
		return false;
	}
	
	for (int i=0; i<locations.size(); i++) {
		Set <string> loc = locations[i];
		Vector <Set<string> > otherLocations = locations;
		otherLocations.remove(i);
		result += loc;
		if(canOfferUniversalCoverage(cities,otherLocations,numHospitals-1,result)) {
			return true;
		} else {
			result.remove(result.size()-1);
		}
	}
	return false;
}	

//Runs the program
int main() {

	while(true) {

		Set<string> cities;
		cities += "A","B","C","D","E","F";

		Set<string> s1,s2,s3,s4;
		s1 += "A","B","C";
		s2 += "A","C","D";
		s3 += "B","F";
		s4 += "C","E","F";

		Vector<Set<string> > locations;
		locations += s1,s2,s3,s4;

		Vector<Set<string> >result;

		int numHopsitals = getInteger("Enter a positive integer : ");
		while(numHopsitals <=0 ) {
			numHopsitals = getInteger("Error. Enter a positive integer : ");
		}

		if(canOfferUniversalCoverage(cities,locations,numHopsitals,result)) {
			cout << "true" << endl;
			cout << result << endl;
		} else {
			cout << "false" << endl;
			if(numHopsitals > locations.size()) cout << "You entered more hospitals than locations" << endl;
		}
		cout << endl;
	}
	
    return 0;
}
