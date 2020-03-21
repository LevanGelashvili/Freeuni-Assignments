/*
 HW1 Taboo problem class.
 Taboo encapsulates some rules about what objects
 may not follow other objects.
 (See handout).
*/
package assign1;

import java.util.*;

public class Taboo<T> {

	private List<T> rules;

	/**
	 * Constructs a new Taboo using the given rules (see handout.)
	 * 
	 * @param rules rules for new Taboo
	 */
	public Taboo(List<T> rules) {
		this.rules = rules;
	}

	/**
	 * Returns the set of elements which should not follow the given element.
	 * 
	 * @param elem
	 * @return elements which should not follow the given element
	 */
	public Set<T> noFollow(T elem) {
		Set<T> res = new HashSet<T>();
		if (elem == null)
			return res;
		Iterator<T> iter = rules.iterator();
		while (iter.hasNext()) {
			T cur = iter.next();
			if (cur != null)
				checkNext(cur, iter, res, elem);
		}
		return res;
	}
	
	/**
	 * Method that adds an item to the set, if next is not null.
	 * Also checks that next element is present.
	 */
	private void checkNext(T cur, Iterator<T> iter, Set<T> res, T elem) {
		if (cur.equals(elem) && iter.hasNext()) {
			T next = iter.next();
			if (next != null) {
				res.add(next);
			}
		}
	}

	/**
	 * Removes elements from the given list that violate the rules (see handout).
	 * 
	 * @param list collection to reduce
	 */
	public void reduce(List<T> list) {
		Map<T, HashSet<T>> map = new HashMap<T, HashSet<T>>();
		fillMap(map);
		deleteFromList(map, list);
	}

	/**
	 * For each element stores elements (in a hashset) which should not proceed it.
	 * If we count this information once, there will be no need to check it over again
	 * at every iteration.
	 */
	private void fillMap(Map<T, HashSet<T>> map) {
		for (T elem : rules) {
			if (elem != null && !map.containsKey(elem))
				map.put(elem, (HashSet<T>) noFollow(elem));
		}
	}

	/**
	 * Deletes elements from the list, depending on info gathered in fillMap method.
	 * Iterates over list, checks for current and next elements and removes next element
	 * if current element's hashset contains it. We use simple for cycle instead of collection
	 * iteration, because we might not need to move to the next element and stay on current one.
	 */
	private void deleteFromList(Map<T, HashSet<T>> map, List<T> list) {
		for (int i = 0; i < list.size() - 1;) {
			T cur = list.get(i);
			T next = list.get(i + 1);
			if (map.containsKey(cur) && map.get(cur).contains(next))
				list.remove(i + 1);
			else
				i++;
		}
	}
}
