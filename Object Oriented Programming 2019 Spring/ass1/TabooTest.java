// TabooTest.java
// Taboo class tests -- nothing provided.
package assign1;

import static org.junit.Assert.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.junit.Test;

public class TabooTest {

	@Test
	public void noFollowTest1() {
		List<Integer> list = Arrays.asList(1, 2, 3, 1, 4, 3, 5);
		Taboo<Integer> t = new Taboo<Integer>(list);
		assertEquals(listToSet(Arrays.asList(2, 4)), t.noFollow(1));
		assertEquals(listToSet(Arrays.asList(1, 5)), t.noFollow(3));
		assertEquals(listToSet(Arrays.asList(3)), t.noFollow(2));
		assertEquals(listToSet(Arrays.asList()), t.noFollow(5)); // empty list
	}

	@Test
	public void noFollowTest2() {
		List<Character> list = Arrays.asList('a', 'b', 'c', 'd', 'b', 'c', 'b', 'e');
		Taboo<Character> t = new Taboo<Character>(list);
		assertEquals(listToSet(Arrays.asList('b')), t.noFollow('a'));
		assertEquals(listToSet(Arrays.asList('c', 'e')), t.noFollow('b'));
		assertEquals(listToSet(Arrays.asList('d', 'b')), t.noFollow('c'));
		assertEquals(listToSet(Arrays.asList()), t.noFollow('e'));
		assertEquals(listToSet(Arrays.asList()), t.noFollow('f')); // not present in a list
	}

	@Test
	public void noFollowTest3() {
		// Tests with nulls in a list
		List<Character> list = Arrays.asList(null, 'a', 'b', null, 'c', 'b', 'a', 'd', null);
		Taboo<Character> t = new Taboo<Character>(list);
		assertEquals(listToSet(Arrays.asList('b', 'd')), t.noFollow('a'));
		assertEquals(listToSet(Arrays.asList('a')), t.noFollow('b'));
		assertEquals(listToSet(Arrays.asList()), t.noFollow('d'));
		assertEquals(listToSet(Arrays.asList()), t.noFollow(null));
	}

	@Test
	public void reduceTest1() {
		List<Character> list = Arrays.asList('a', 'b', 'a', 'c', 'd', 'e', 'a');
		Taboo<Character> t = new Taboo<Character>(list);
		checkReduceTest(t, new ArrayList<Character>(Arrays.asList('a', 'b', 'a', 'c', 'd')),
				Arrays.asList('a', 'a', 'd'));
		checkReduceTest(t, new ArrayList<Character>(Arrays.asList('a', 'b', 'b', 'b', 'b', 'c', 'c')),
				Arrays.asList('a'));
		checkReduceTest(t, new ArrayList<Character>(Arrays.asList('f', 'b', 'b', 'd', 'a', 'e', 'f')),
				Arrays.asList('f', 'b', 'b', 'd', 'a', 'e', 'f'));
		checkReduceTest(t, new ArrayList<Character>(Arrays.asList('a', 'b')), Arrays.asList('a'));
		checkReduceTest(t, new ArrayList<Character>(), new ArrayList<Character>());
	}

	@Test
	public void reduceTest2() {
		List<Integer> list = Arrays.asList(1, 2, null, 1, 3, 4, 1, 5, 6, null);
		Taboo<Integer> t = new Taboo<Integer>(list);
		checkReduceTest(t, new ArrayList<Integer>(Arrays.asList(1, 2, 4, 1, 5, 3)), Arrays.asList(1, 4, 5, 3));
		checkReduceTest(t, new ArrayList<Integer>(Arrays.asList(2, 5, 6, 1, 1, 1, 1, 1)),
				Arrays.asList(2, 5, 1, 1, 1, 1, 1));
		checkReduceTest(t, new ArrayList<Integer>(Arrays.asList(1,2,2,2,2,2,2,2,2,2,2,2,2)),
				Arrays.asList(1));
		checkReduceTest(t, new ArrayList<Integer>(Arrays.asList(2)),
				Arrays.asList(2));
	}

	//Checks if test was correct
	private <T> void checkReduceTest(Taboo<T> t, ArrayList<T> original, List<T> result) {
		t.reduce(original);
		assertEquals(original, result);
	}

	//Returns a generic set filled with elements of a list
	private <T> Set<T> listToSet(List<T> list) {
		Set<T> res = new HashSet<T>();
		for (T elem : list)
			res.add(elem);
		return res;
	}

}
