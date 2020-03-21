/*
 * File: InverseGenetics.cpp
 * --------------------------
 * Name: [TODO: enter name here]
 * Section: [TODO: enter section leader here]
 * This file is the starter project for the Inverse Genetics problem
 * on Assignment #3.
 * [TODO: extend the documentation]
 */

#include <iostream>
#include <string>
#include <fstream>
#include "strlib.h"
#include "simpio.h"
#include "set.h"
#include "vector.h"
#include "map.h"
#include "console.h"
using namespace std;

Map<char, Set<string> > loadCodonMap();
Set<string> getRNAs(string protein, Map<char, Set<string> >& codons);
void listAllRNAStrandsFor(string protein, Map<char, Set<string> >& codons);

int main() {
    /* Load the codon map. */
    Map<char, Set<string> > codons = loadCodonMap();
	while(true) {
		string protein = getLine("Enter protein : ");
		listAllRNAStrandsFor(toUpperCase(protein),codons);
	}
	cout << endl;
    return 0;
}

/* You do not need to change this function. */
/* Function: loadCodonMap();
 * Usage: Map<char, Lexicon> codonMap = loadCodonMap();
 * ==================================================================
 * Loads the codon mapping table from a file.
 */
Map<char, Set<string> > loadCodonMap() {
    ifstream input("codons.txt");
    Map<char, Set<string> > result;

    /* The current codon / protein combination. */
    string codon;
    char protein;

    /* Continuously pull data from the file until all data has been
     * read.
     */
    while (input >> codon >> protein) {
        result[protein] += codon;
    }

    return result;
}

/* Function: listAllRNAStrandsFor(string protein,
 *                                Map<char, Set<string> >& codons);
 * Usage: listAllRNAStrandsFor("PARTY", codons);
 * ==================================================================
 * Given a protein and a map from amino acid codes to the codons for
 * that code, lists all possible RNA strands that could generate
 * that protein
 */
void listAllRNAStrandsFor(string protein, Map<char, Set<string> >& codons) {
	Set<string> rnas = getRNAs(protein,codons);
	foreach (string rna in rnas) cout << rna << endl;
}

/*Returns a set of all possible RNA strands given a protein.
	If protein is an empty set, then we return an empty set.
	Else, we remove first character from a protein, generate a set of 
	all rnas that dont contain that character recursively, create a copy
	of that and add the removed element to every rna possible. Just like
	in an exercise, where all subsets are generates rhis way.
*/
Set<string> getRNAs(string protein, Map<char, Set<string> >& codons) {
	Set<string> result;

	if(protein.length() == 0) {
		Set<string> emptyString;
		emptyString += "";
		result += emptyString;
		return result;
	}
	
	//Element is removed and saved in ch variable
	char ch = protein[0];
	protein.erase(0,1);

	//Copy of all RNAs without ch
	Set<string> secondResult = getRNAs(protein,codons);
	Set<string> set = codons.get(ch);

	//removed element is added to a copy of every set.
	foreach (string str in secondResult) {
		foreach(string cod in set) {
			string rna = cod + str;
			result += rna;
		}
	}

	return result;
}
