package assign1;
import java.util.*;

public class Appearances {

	/**
	 * Returns the number of elements that appear the same number of times in both
	 * collections. Static method. (see handout).
	 * 
	 * @return number of same-appearance elements
	 */
	public static <T> int sameCount(Collection<T> a, Collection<T> b) {
		Map<T, Integer> aMap = new HashMap<T, Integer>();
		Map<T, Integer> bMap = new HashMap<T, Integer>();
		countElems(a, aMap);
		countElems(b, bMap);
		int res = 0;
		for (T elem : aMap.keySet()) {
			if (bMap.containsKey(elem) && bMap.get(elem).equals(aMap.get(elem)))
				res++;
		}
		return res;
	}

	/**
	 * Takes a list and an empty map, fills map with number of occurrences from the list.
	 */
	private static <T> void countElems(Collection<T> list, Map<T, Integer> map) {
		for (T elem : list) {
			if (map.containsKey(elem))
				map.put(elem, map.get(elem) + 1);
			else
				map.put(elem, 1);
		}
	}

}
