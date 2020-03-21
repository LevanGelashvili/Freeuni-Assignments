#!/usr/bin/env python

import random # for seed, random
import sys    # for stdout



################################### TEST PART ##################################
################################################################################

# Tests align strands and scores
# Parameters types:
#    score          =  int   example: -6
#    plusScores     = string example: "  1   1  1"
#    minusScores    = string example: "22 111 11 "
#    strandAligned1 = string example: "  CAAGTCGC"
#    strandAligned2 = string example: "ATCCCATTAC"
#
#   Note: all strings must have same length
def test(score, plusScores, minusScores, strandAligned1, strandAligned2):
    print("\n>>>>>>START TEST<<<<<<")

    if testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
        sys.stdout.write(">>>>>>>Test SUCCESS:")
        sys.stdout.write("\n\t\t" + "Score: "+str(score))
        sys.stdout.write("\n\t\t+ " + plusScores)
        sys.stdout.write("\n\t\t  " + strandAligned1)
        sys.stdout.write("\n\t\t  " + strandAligned2)
        sys.stdout.write("\n\t\t- " + minusScores)
        sys.stdout.write("\n\n")
    else:
        sys.stdout.write("\t>>>>!!!Test FAILED\n\n")

# converts character score to int
def testScoreToInt(score):
    if score == ' ':
        return 0
    return int(score)


# computes sum of scores
def testSumScore(scores):
    result = 0
    for ch in scores:
        result += testScoreToInt(ch)
    return result


# test each characters and scores
def testValidateEach(ch1, ch2, plusScore, minusScore):
    if ch1 == ' ' or ch2 == ' ':
        return plusScore == 0 and minusScore == 2
    if ch1 == ch2:
        return plusScore == 1 and minusScore == 0
    return plusScore == 0 and minusScore == 1


# test and validates strands
def testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
    if len(plusScores) != len(minusScores) or len(minusScores) != len(strandAligned1) or len(strandAligned1) != len(
            strandAligned2):
        sys.stdout.write("Length mismatch! \n")
        return False

    if len(plusScores) == 0:
        sys.stdout.write("Length is Zero! \n")
        return False

    if testSumScore(plusScores) - testSumScore(minusScores) != score:
        sys.stdout.write("Score mismatch to score strings! TEST FAILED!\n")
        return False
    for i in range(len(plusScores)):
        if not testValidateEach(strandAligned1[i], strandAligned2[i], testScoreToInt(plusScores[i]),
                                testScoreToInt(minusScores[i])):
            sys.stdout.write("Invalid scores for position " + str(i) + ":\n")
            sys.stdout.write("\t char1: " + strandAligned1[i] + " char2: " +
                             strandAligned2[i] + " +" + str(testScoreToInt(plusScores[i])) + " -" +
                             str(testScoreToInt(minusScores[i])) + "\n")
            return False

    return True

######################## END OF TEST PART ######################################
################################################################################


# Computes the score of the optimal alignment of two DNA strands.
# I wrote this algorithm using a 3 element array :
# 0 - first string
# 1 - second string
# 2 - total score, no more info was needed
def findOptimalAlignment(strand1, strand2, memo):
	#if length equals zero, than 2s will be in the allignment, score will be -2 times length of second string
	if len(strand1) == 0: 
		res = [' ' * len(strand2), strand2, -2 * len(strand2)]
		return res
	if len(strand2) == 0: 
		res = [strand1, ' ' * len(strand1), -2 * len(strand1)]
		return res

	#we use a cache, twople of strings as a key, 3 element array as a value
	if (strand1[1:], strand2[1:]) in memo : bestWith = memo[(strand1[1:], strand2[1:])]
	else : memo [(strand1[1:], strand2[1:])] = bestWith = findOptimalAlignment(strand1[1:], strand2[1:], memo)

	if strand1[0] == strand2[0]:
		res = [strand1[0] + bestWith[0], strand2[0] + bestWith[1], bestWith[2] + 1]
		return res 

	best = [bestWith[0] + strand1[0],bestWith[1] + strand2[0],bestWith[2]-1]

	#two of same logic below, we use first character in one case, we don't in another
	if (strand1, strand2[1:]) in memo : bestWithout = memo[(strand1, strand2[1:])]
	else : memo[(strand1, strand2[1:])] = bestWithout = findOptimalAlignment(strand1, strand2[1:], memo)

	cur = [bestWithout[0] + ' ',bestWithout[1] + strand2[0],bestWithout[2]-2]
	if cur[2] > best[2]:
		best = cur

	#opposite variant

	if (strand1[1:], strand2) in memo : bestWithout = memo[(strand1[1:], strand2)]
	else : memo[(strand1[1:], strand2)] = bestWithout = findOptimalAlignment(strand1[1:], strand2, memo)

	cur = [bestWithout[0] + strand1[0],bestWithout[1] + ' ',bestWithout[2]-2]
	if cur[2] > best[2]:
		best = cur

	return best

# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
	assert minlength > 0, \
	       "Minimum length passed to generateRandomDNAStrand" \
	       "must be a positive number" # these \'s allow mult-line statements
	assert maxlength >= minlength, \
	       "Maximum length passed to generateRandomDNAStrand must be at " \
	       "as large as the specified minimum length"
	strand = ""
	length = random.choice(xrange(minlength, maxlength + 1))
	bases = ['A', 'T', 'G', 'C']
	for i in xrange(0, length):
		strand += random.choice(bases)
	return strand

def getAllignment(array):
	plus = ""
	minus = ""
	for i in range(len(array[0])):
		if array[0][i] == ' ' or array[1][i] == ' ' :
			plus += ' '
			minus += '2'
		elif array[0][i] == array[1][i] :
			plus += '1'
			minus += ' '
		else :
			plus += ' '
			minus += '1'
	return (plus, minus)

def printInfo(c, s) :
	print("   " + c + "   " + s)

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.

def printAlignment(array, out = sys.stdout):
	plusStr, minusStr = getAllignment(array)
	if(False) : #if false, doesn't print my results, if true, does so instead, test doesn't depend on prints
		print("Optimal alignment score is " + str(array[2]))
		printInfo('+',plusStr)
		printInfo(' ',array[0])
		printInfo(' ',array[1])
		printInfo('-',minusStr)
	test(array[2],plusStr,minusStr,array[0],array[1])

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.
 
def main():
	memo = {}
	while (True):
		sys.stdout.write("Generate random DNA strands? ")
		answer = sys.stdin.readline()
		if answer == "no\n": break
		strand1 = generateRandomDNAStrand(100, 150)
		strand2 = generateRandomDNAStrand(100, 150)
		sys.stdout.write("Aligning these two strands: " + strand1 + "\n")
		sys.stdout.write("                            " + strand2 + "\n")
		alignment = findOptimalAlignment(strand1, strand2, memo)
		printAlignment(alignment)
	
if __name__ == "__main__":
  main()
