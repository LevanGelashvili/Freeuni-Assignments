// StringCodeTest
// Some test code is provided for the early HW1 problems,
// and much is left for you to add.

package assign1;

import static org.junit.Assert.*;
import org.junit.Test;
import assign1.StringCode;

public class StringCodeTest {
	//
	// blowup
	//
	@Test
	public void testBlowup1() {
		// basic cases
		assertEquals("xxaaaabb", StringCode.blowup("xx3abb"));
		assertEquals("xxxZZZZ", StringCode.blowup("2x3Z"));
	}

	@Test
	public void testBlowup2() {
		// things with digits

		// digit at end
		assertEquals("axxx", StringCode.blowup("a2x3"));

		// digits next to each other
		assertEquals("a33111", StringCode.blowup("a231"));

		// try a 0
		assertEquals("aabb", StringCode.blowup("aa0bb"));
	}

	@Test
	public void testBlowup3() {
		// weird chars, empty string
		assertEquals("AB&&,- ab", StringCode.blowup("AB&&,- ab"));
		assertEquals("", StringCode.blowup(""));

		// string with only digits
		assertEquals("", StringCode.blowup("2"));
		assertEquals("33", StringCode.blowup("23"));
	}

	//
	// maxRun
	//
	@Test
	public void testRun1() {
		assertEquals(2, StringCode.maxRun("hoopla"));
		assertEquals(3, StringCode.maxRun("hoopllla"));
	}

	@Test
	public void testRun2() {
		assertEquals(3, StringCode.maxRun("abbcccddbbbxx"));
		assertEquals(0, StringCode.maxRun(""));
		assertEquals(3, StringCode.maxRun("hhhooppoo"));
	}

	@Test
	public void testRun3() {
		// "evolve" technique -- make a series of test cases
		// where each is change from the one above.
		assertEquals(1, StringCode.maxRun("123"));
		assertEquals(2, StringCode.maxRun("1223"));
		assertEquals(2, StringCode.maxRun("112233"));
		assertEquals(3, StringCode.maxRun("1112233"));
	}

	@Test
	// Usual test cases
	public void testIntersect1() {
		assertEquals(false, StringCode.stringIntersect("abcdef", "12345", 2));
		assertEquals(true, StringCode.stringIntersect("abcdef", "ab", 2));
		assertEquals(true, StringCode.stringIntersect("abcdef", "bcde", 3));
		assertEquals(false, StringCode.stringIntersect("abcdef", "12345", 2));
		assertEquals(false, StringCode.stringIntersect("abcdef", "acdbfe", 3));
		assertEquals(true, StringCode.stringIntersect("abcdef", "acdbfe", 2));
		assertEquals(true, StringCode.stringIntersect("abcdef", "c4bw3d6def", 3));
	}

	@Test
	// Edge test cases and invalid inputs
	public void testIntersect2() {
		assertEquals(false, StringCode.stringIntersect("abcdef", "", 2));
		assertEquals(false, StringCode.stringIntersect("", "", 2));
		assertEquals(false, StringCode.stringIntersect("abc", "qwert", 4));
		assertEquals(false, StringCode.stringIntersect("abcdefg", "qwert", 7));
		assertEquals(true, StringCode.stringIntersect("t", "t", 1));
	}
	
	@Test
	/**
	 * I'm writing this method, because i don't get 100 % coverage, if i don't call a class constructor at least once.
	 * Since i don't use StringCode constructor anywhere, i write this test just to use it once and get all the coverage.
	 * Other classes constructors of which i use, don't need this kind of test.
	 */
	public void testConstructor() {
		StringCode a = new StringCode();
	}
}
