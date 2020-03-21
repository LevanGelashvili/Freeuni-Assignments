package assign1;

import java.util.HashSet;
import java.util.Set;

// CS108 HW1 -- String static methods

public class StringCode {

	/**
	 * Given a string, returns the length of the largest run. A a run is a series of
	 * adajcent chars that are the same.
	 * 
	 * @param str
	 * @return max run length
	 */
	public static int maxRun(String str) {
		if (str.equals(""))
			return 0;

		int res = 1;
		int cur = 1;
		char last = str.charAt(0);
		for (int i = 1; i < str.length(); i++) {
			if (str.charAt(i) == last)
				cur++;
			else {
				last = str.charAt(i);
				res = Math.max(res, cur);
				cur = 1;
			}
		}
		return Math.max(res, cur);
	}

	/**
	 * Given a string, for each digit in the original string, replaces the digit
	 * with that many occurrences of the character following. So the string "a3tx2z"
	 * yields "attttxzzz".
	 * 
	 * @param str
	 * @return blown up string
	 */
	public static String blowup(String str) {
		String res = "";
		for (int i = 0; i < str.length(); i++) {
			char ch = str.charAt(i);
			if (Character.isDigit(ch))
				res += multiplyChars(str, i);
			else
				res += ch;
		}
		return res;
	}

	/**
	 * Given a string and an index of a digit char, takes that many occurences of a
	 * next character and appends it to a string. Edge case is that index may be the
	 * last index of a string, in that way we append nothing
	 * 
	 * @param str, index
	 * @return multiplied string
	 */
	private static String multiplyChars(String str, int ind) {
		String res = "";
		if (ind == str.length() - 1)
			return res;
		for (int i = 0; i < str.charAt(ind) - '0'; i++) {
			res += str.charAt(ind + 1);
		}
		return res;
	}

	/**
	 * Given 2 strings, consider all the substrings within them of length len.
	 * Returns true if there are any such substrings which appear in both strings.
	 * Compute this in linear time using a HashSet. Len will be 1 or more.
	 */
	public static boolean stringIntersect(String a, String b, int len) {
		if (a.length() == 0 || b.length() == 0 || len > a.length() || len > b.length())
			return false;
		Set<String> set = new HashSet<String>();
		for (int i = 0; i <= a.length() - len; i++)
			set.add(a.substring(i, i + len));
		for (int i = 0; i <= b.length() - len; i++)
			if (set.contains(b.substring(i, i + len)))
				return true;
		return false;
	}
}
