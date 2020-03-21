package assign3;

import java.util.*;

/*
 * Encapsulates a Sudoku grid to be solved.
 * CS108 Stanford.
 */
public class Sudoku {
	// Provided grid data for main/testing
	// The instance variable strategy is up to you.

	// Provided easy 1 6 grid
	// (can paste this text into the GUI too)
	public static final int[][] easyGrid = Sudoku.stringsToGrid("1 6 4 0 0 0 0 0 2", "2 0 0 4 0 3 9 1 0",
			"0 0 5 0 8 0 4 0 7", "0 9 0 0 0 6 5 0 0", "5 0 0 1 0 2 0 0 8", "0 0 8 9 0 0 0 3 0", "8 0 9 0 4 0 2 0 0",
			"0 7 3 5 0 9 0 0 1", "4 0 0 0 0 0 6 7 9");

	// Provided medium 5 3 grid
	public static final int[][] mediumGrid = Sudoku.stringsToGrid("530070000", "600195000", "098000060", "800060003",
			"400803001", "700020006", "060000280", "000419005", "000080079");

	// Provided hard 3 7 grid
	// 1 solution this way, 6 solutions if the 7 is changed to 0
	public static final int[][] hardGrid = Sudoku.stringsToGrid("3 7 0 0 0 0 0 8 0", "0 0 1 0 9 3 0 0 0",
			"0 4 0 7 8 0 0 0 3", "0 9 3 8 0 0 0 1 2", "0 0 0 0 4 0 0 0 0", "5 2 0 0 0 6 7 9 0", "6 0 0 0 2 1 0 4 0",
			"0 0 0 5 3 0 9 0 0", "0 3 0 0 0 0 0 5 1");

	public static final int SIZE = 9; // size of the whole 9x9 puzzle
	public static final int PART = 3; // size of each 3x3 part
	public static final int MAX_SOLUTIONS = 100;

	// Provided various static utility methods to
	// convert data formats to int[][] grid.

	/**
	 * Returns a 2-d grid parsed from strings, one string per row. The "..." is a
	 * Java 5 feature that essentially makes "rows" a String[] array. (provided
	 * utility)
	 * 
	 * @param rows array of row strings
	 * @return grid
	 */
	public static int[][] stringsToGrid(String... rows) {
		int[][] result = new int[rows.length][];
		for (int row = 0; row < rows.length; row++) {
			result[row] = stringToInts(rows[row]);
		}
		return result;
	}

	/**
	 * Given a single string containing 81 numbers, returns a 9x9 grid. Skips all
	 * the non-numbers in the text. (provided utility)
	 * 
	 * @param text string of 81 numbers
	 * @return grid
	 */
	public static int[][] textToGrid(String text) {
		int[] nums = stringToInts(text);
		if (nums.length != SIZE * SIZE) {
			throw new RuntimeException("Needed 81 numbers, but got:" + nums.length);
		}

		int[][] result = new int[SIZE][SIZE];
		int count = 0;
		for (int row = 0; row < SIZE; row++) {
			for (int col = 0; col < SIZE; col++) {
				result[row][col] = nums[count];
				count++;
			}
		}
		return result;
	}

	/**
	 * Given a string containing digits, like "1 23 4", returns an int[] of those
	 * digits {1 2 3 4}. (provided utility)
	 * 
	 * @param string string containing ints
	 * @return array of ints
	 */
	public static int[] stringToInts(String string) {
		int[] a = new int[string.length()];
		int found = 0;
		for (int i = 0; i < string.length(); i++) {
			if (Character.isDigit(string.charAt(i))) {
				a[found] = Integer.parseInt(string.substring(i, i + 1));
				found++;
			}
		}
		int[] result = new int[found];
		System.arraycopy(a, 0, result, 0, found);
		return result;
	}

	// Provided -- the deliverable main().
	// You can edit to do easier cases, but turn in
	// solving hardGrid.
	public static void main(String[] args) {
		Sudoku sudoku = new Sudoku(hardGrid);
		System.out.println(sudoku); // print the raw problem
		int count = sudoku.solve();
		System.out.println("solutions:" + count);
		System.out.println("elapsed:" + sudoku.getElapsed() + "ms");
		System.out.println(sudoku.getSolutionText());
	}

	/**
	 * 
	 * 
	 * 
	 * 
	 * 
	 * PROVIDED CODE OVER
	 * 
	 * 
	 * 
	 * 
	 * 
	 */

	private int[][] grid; // Grid that stores sudoku matrix
	private List<Spot> spots; // Arraylist of spots
	private List<Integer> allValues; // Stores all possible values of numbers [1..9] in this example, useful for spot
										// constructor
	/*
	 * Maps which allow to check next move in o(1) rowMap stores rows as keys and
	 * elements in that row as a value (in a hashmap) Same happens with colMap and
	 * squareMap, but they save info for columns and squares as ints(explained
	 * below)
	 */
	private HashMap<Integer, HashSet<Integer>> rowMap, colMap, squareMap;
	private int solutionCount = 0; // Number of solutions
	private String firstSolution; // String storing the first solution
	private long start; // Time of start

	/*
	 * Spot is an inner class, which stores info for a single cell in sudoku matrix.
	 */
	public class Spot implements Comparable<Spot> {

		private int row, col; // Row and column for current spot
		private HashSet<Integer> possibleValues; // Possible values of this spot

		// Constructs a new spot object
		public Spot(int row, int col) {
			this.row = row;
			this.col = col;

			possibleValues = new HashSet<Integer>(allValues);
			reduceValues();
		}

		// Returns row
		public int getRow() {
			return row;
		}

		// Returns column
		public int getCol() {
			return col;
		}

		// Sets value in a grid, we don't need to store actual value for this spot.
		public void setValue(int value) {
			grid[row][col] = value;
		}

		// Number of possible values
		public int getValueCount() {
			return possibleValues.size();
		}

		// Returns hashset of all possible values
		public HashSet<Integer> getPossibleValues() {
			return possibleValues;
		}

		// Removes illegal values from hashset based on info stored in maps
		private void reduceValues() {
			possibleValues.removeAll(rowMap.get(row));
			possibleValues.removeAll(colMap.get(col));
			possibleValues.removeAll(squareMap.get(getSquare(row, col)));
		}

		@Override
		// Compare method. Useful for sorting
		public int compareTo(Spot other) {
			return (getValueCount() >= other.getValueCount()) ? 1 : -1;
		}

		@Override
		// String representation of a spot
		public String toString() {
			StringBuilder s = new StringBuilder();
			s.append("Row : " + getRow() + ", Col : " + getCol());
			s.append("\nValues : ");
			for (int val : possibleValues) s.append(val + " ");
			return s.toString();
		}
	}

	/**
	 * Sets up based on the given ints.
	 */
	public Sudoku(int[][] ints) {
		initVariables(ints);
		addSpots();
	}

	// Initilizes variabels and copies info to our grid and maps
	private void initVariables(int[][] ints) {
		grid = new int[ints.length][ints[0].length];
		spots = new ArrayList<Spot>();
		allValues = new ArrayList<Integer>();

		rowMap = new HashMap<Integer, HashSet<Integer>>();
		colMap = new HashMap<Integer, HashSet<Integer>>();
		squareMap = new HashMap<Integer, HashSet<Integer>>();

		for (int i = 1; i <= SIZE; i++) allValues.add(i);

		for (int i = 0; i < grid.length; i++) {
			for (int j = 0; j < grid[0].length; j++) {
				grid[i][j] = ints[i][j];
				updateMaps(i, j, grid[i][j]);
			}
		}
	}

	// Creates a spot for every sinlge blank cell and sorts them in the end
	private void addSpots() {
		for (int i = 0; i < grid.length; i++) {
			for (int j = 0; j < grid[0].length; j++) {
				if (grid[i][j] == 0) spots.add(new Spot(i, j));
			}
		}
		Collections.sort(spots);
	}

	/**
	 * Solves the puzzle, invoking the underlying recursive search.
	 */
	public int solve() {
		start = System.currentTimeMillis();
		solveFrom(0);
		return solutionCount;
	}

	/*
	 * A backtracking recursive method to solve sudoku. Returns if we reached the
	 * end. Tries every single possible value for a spot and descends deeper down
	 * for legal ones. Legality is checked in hashmaps, in o(1) time. After
	 * returning back from recursion, restores hashmaps back to earlier state.
	 */
	private void solveFrom(int index) {
		if (index >= spots.size() || solutionCount >= MAX_SOLUTIONS) return;

		Spot cur = spots.get(index);

		for (int val : cur.getPossibleValues()) {

			if (isLegal(cur.getRow(), cur.getCol(), val)) {

				cur.setValue(val);
				updateMaps(cur.getRow(), cur.getCol(), val);

				handleLastSolution(index);
				solveFrom(index + 1);

				revertMaps(cur.getRow(), cur.getCol(), val);
			}
		}

		cur.setValue(0);
	}

	// If we reached the end of the list, we've solved sudoku, therefore, we need to
	// increase count and save first solution.
	private void handleLastSolution(int index) {
		if (index == spots.size() - 1) {
			solutionCount++;
			if (firstSolution == null) firstSolution = toString();
		}
	}

	// Returns solved sudoku
	public String getSolutionText() {
		return (firstSolution == null) ? "No solutions found." : firstSolution;
	}

	// Returns time spent solving this sudoku
	public long getElapsed() {
		return System.currentTimeMillis() - start;
	}

	// Checks if assigning this value to a spot is legal
	private boolean isLegal(int row, int col, int value) {
		return isMissing(rowMap, row, value) && isMissing(colMap, col, value)
				&& isMissing(squareMap, getSquare(row, col), value);
	}

	// Returns true if map does not contain passed value
	private static boolean isMissing(HashMap<Integer, HashSet<Integer>> map, int key, int value) {
		return !map.containsKey(key) || !map.get(key).contains(value);
	}

	// Updates maps with given value
	private void updateMaps(int row, int col, int val) {
		checkAndInsert(rowMap, row, val);
		checkAndInsert(colMap, col, val);
		checkAndInsert(squareMap, getSquare(row, col), val);
	}

	// Checks if key is present, if not, creates new hashmap for it and then adds
	// value
	private static void checkAndInsert(HashMap<Integer, HashSet<Integer>> map, int key, int value) {
		if (!map.containsKey(key)) {
			map.put(key, new HashSet<Integer>());
		}
		map.get(key).add(value);
	}

	/*
	 * Reverts maps back to earlier state, we dont need to check if map contains
	 * keys, since we added them in updateMaps method
	 */
	private void revertMaps(int row, int col, int val) {
		rowMap.get(row).remove(val);
		colMap.get(col).remove(val);
		squareMap.get(getSquare(row, col)).remove(val);
	}

	/*
	 * Returns index of a square in a grid. Since point is not hashable, i decided
	 * to biject squares to ints
	 */
	private static int getSquare(int row, int col) {
		return 3 * (row / 3) + col / 3;
	}

	@Override
	// String representation of a sudoku.=
	public String toString() {
		StringBuilder s = new StringBuilder();
		for (int i = 0; i < grid.length; i++) {
			for (int j = 0; j < grid[0].length; j++) {
				s.append(grid[i][j] + " ");
			}
			s.append('\n');
		}
		return s.toString();
	}

}
