//
// TetrisGrid encapsulates a tetris board and has
// a clearRows() capability.
package assign1;

import java.util.LinkedList;
import java.util.Queue;

public class TetrisGrid {

	private boolean[][] grid;

	/**
	 * Constructs a new instance with the given grid. Does not make a copy.
	 * 
	 * @param grid
	 */

	public TetrisGrid(boolean[][] grid) {
		this.grid = grid;
	}

	/**
	 * Does row-clearing on the grid (see handout).
	 * Method is implemented with a queue, which stores index of a column,
	 * which needs to be replaced. As soon as we encounter a valid column(not full)
	 * we copy its content to the one, inserted earliest (queue.peek) and add it to queue
	 * because it needs to be replaced as well. Any column left in the queue after for cycle
	 * must be filled with falses, empty array can't be cleared so we have a base case in the start.
	 */
	public void clearRows() {
		if (grid.length == 0 || grid[0].length == 0)
			return;
		Queue<Integer> queue = new LinkedList<Integer>();
		for (int col = 0; col < grid[0].length; col++) {
			if (isFullCol(col))
				queue.add(col);
			else {
				if (!queue.isEmpty()) {
					copyCol(col, queue.peek());
					queue.remove();
					queue.add(col);
				}
			}
		}
		while (!queue.isEmpty()) {
			fillWithFalses(queue.peek());
			queue.remove();
		}
	}

	/**
	 * Takes a column from a grid and fills it with falses Useful at the end of the
	 * method
	 */
	private void fillWithFalses(int col) {
		for (int i = 0; i < grid.length; i++)
			grid[i][col] = false;
	}

	/**
	 * Takes indexes of two columns and copies one to the other
	 */
	private void copyCol(int from, int to) {
		for (int i = 0; i < grid.length; i++)
			grid[i][to] = grid[i][from];
	}

	/**
	 * Checks whether given column is full, meaning every element in it is true
	 */
	private boolean isFullCol(int col) {
		for (int i = 0; i < grid.length; i++)
			if (!grid[i][col])
				return false;
		return true;
	}

	/**
	 * Returns the internal 2d grid array.
	 * 
	 * @return 2d grid array
	 */
	boolean[][] getGrid() {
		return grid;
	}
}
