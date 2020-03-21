package assign1;
import static org.junit.Assert.*;
import org.junit.Test;

import java.util.*;

public class AppearancesTest {
	// utility -- converts a string to a list with one
	// elem for each char.
	private List<String> stringToList(String s) {
		List<String> list = new ArrayList<String>();
		for (int i = 0; i < s.length(); i++) {
			list.add(String.valueOf(s.charAt(i)));
			// note: String.valueOf() converts lots of things to string form
		}
		return list;
	}

	@Test
	public void testSameCount1() {
		List<String> a = stringToList("abbccc");
		List<String> b = stringToList("cccbba");
		assertEquals(3, Appearances.sameCount(a, b));
	}

	@Test
	public void testSameCount2() {
		// basic List<Integer> cases
		List<Integer> a = Arrays.asList(1, 2, 3, 1, 2, 3, 5);
		assertEquals(1, Appearances.sameCount(a, Arrays.asList(1, 9, 9, 1)));
		assertEquals(2, Appearances.sameCount(a, Arrays.asList(1, 3, 3, 1)));
		assertEquals(1, Appearances.sameCount(a, Arrays.asList(1, 3, 3, 1, 1)));
	}

	@Test
	public void testSameCount3() {
		List<Double> a = Arrays.asList(1.0, 1.0, 1.0, 2.0, 3.0, 4.0, 5.0, 5.0, 5.0, 6.0, 7.0, 8.0);
		assertEquals(1, Appearances.sameCount(a, Arrays.asList(1.0, 1.0, 1.0)));
		assertEquals(2, Appearances.sameCount(a, Arrays.asList(1.0, 1.0, 1.0, 2.0)));
		assertEquals(3, Appearances.sameCount(a, Arrays.asList(1.0, 1.0, 1.0, 3.0, 5.0, 5.0, 5.0)));
	}

	@Test
	public void testSameCount4() {
		List<String> a = Arrays.asList("a", "bc", "b", "b", "b", "a", "d", "bc");
		assertEquals(2, Appearances.sameCount(a, Arrays.asList("bc", "b", "b", "e", "f", "bc", "d")));
		assertEquals(4, Appearances.sameCount(a, Arrays.asList("a", "bc", "b", "b", "b", "a", "d", "bc"))); // same list
	}

	@Test
	public void testSameCount5() {
		List<Integer> a = Arrays.asList(1, 1, 2, 3, 4, 5);
		List<Integer> empty = new ArrayList<Integer>();
		assertEquals(0, Appearances.sameCount(a, empty));
		assertEquals(0, Appearances.sameCount(a, Arrays.asList(0)));
		assertEquals(0, Appearances.sameCount(empty, a));
	}
	
	@Test
	/**
	 * I'm writing this method, because i don't get 100 % coverage, if i don't call a class constructor at least once.
	 * Since i don't use Appearances constructor anywhere, i write this test just to use it once and get all the coverage.
	 * Other classes constructors of which i use, don't need this kind of test.
	 */
	public void testConstructor() {
		Appearances a = new Appearances();
	}
}
