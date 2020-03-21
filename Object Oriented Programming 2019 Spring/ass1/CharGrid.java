// HW1 2-d array Problems
// CharGrid encapsulates a 2-d grid of chars and supports
// a few operations on the grid.

package assign1;

public class CharGrid {
	private char[][] grid;

	/**
	 * Constructs a new CharGrid with the given grid. Does not make a copy.
	 * 
	 * @param grid
	 */
	public CharGrid(char[][] grid) {
		this.grid = grid;
	}

	/**
	 * Returns the area for the given char in the grid. (see handout).
	 * 
	 * @param ch char to look for
	 * @return area for given char
	 */
	public int charArea(char ch) {
		int minX = Integer.MAX_VALUE, minY = Integer.MAX_VALUE;
		int maxX = Integer.MIN_VALUE, maxY = Integer.MIN_VALUE;
		for (int y = 0; y < grid.length; y++) {
			for (int x = 0; x < grid[0].length; x++) {
				if (grid[y][x] == ch) {
					minX = Math.min(x, minX);
					maxX = Math.max(x, maxX);
					minY = Math.min(y, minY);
					maxY = Math.max(y, maxY);
				}
			}
		}
		// If minX remains same, then we haven't encountered ch once and need to return
		// 0
		if (minX == Integer.MAX_VALUE)
			return 0;
		return (maxX - minX + 1) * (maxY - minY + 1);
	}

	/**
	 * Returns the count of '+' figures in the grid (see handout).
	 * 
	 * @return number of + in grid
	 */
	public int countPlus() {
		int res = 0;
		for (int i = 1; i < grid.length - 1; i++) {
			for (int j = 1; j < grid[0].length - 1; j++) {
				if (isPlus(i, j, grid[i][j])) {
					res++;
					//System.out.println(i + " " + j);
				}
			}
		}
		return res;
	}

	/**
	 * Returns whether a plus is formed in current row-column pair. It counts number
	 * of same character all 4 directions and returns true if all of them are equal and positive,
	 * because a single character with 0 occurrences followed is not a plus. 
	 */
	private boolean isPlus(int row, int col, char c) {
		int right = countRight(row, col, c);
		int left = countLeft(row, col, c);
		int up = countUp(row, col, c);
		int down = countDown(row, col, c);
		return right == left && left == up && up == down && right != 0;
	}

	//Counts chars down
	private int countDown(int row, int col, char c) {
		int res = 0;
		for (int i = row+1; i < grid.length; i++)
			if (grid[i][col] == c)
				res++;
			else
				break;
		return res;
	}

	//Counts chars up
	private int countUp(int row, int col, char c) {
		int res = 0;
		for (int i = row-1; i >= 0; i--)
			if (grid[i][col] == c)
				res++;
			else
				break;
		return res;
	}

	//Counts chars left
	private int countLeft(int row, int col, char c) {
		int res = 0;
		for (int i = col-1; i >= 0; i--)
			if (grid[row][i] == c)
				res++;
			else
				break;
		return res;
	}

	//Counts chars right
	private int countRight(int row, int col, char c) {
		int res = 0;
		for (int i = col+1; i < grid[0].length; i++)
			if (grid[row][i] == c)
				res++;
			else
				break;
		return res;
	}
}
