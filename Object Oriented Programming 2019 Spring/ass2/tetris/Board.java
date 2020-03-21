// Board.java
package tetris;

import static org.junit.Assert.assertArrayEquals;

import java.util.LinkedList;
import java.util.Queue;

/**
 * CS108 Tetris Board. Represents a Tetris board -- essentially a 2-d grid of
 * booleans. Supports tetris pieces and row clearing. Has an "undo" feature that
 * allows clients to add and remove pieces efficiently. Does not do any drawing
 * or have any idea of pixels. Instead, just represents the abstract 2-d board.
 */
public class Board {
	// Some ivars are stubbed out for you:
	private int width, height, maxHeight, maxHeightBackup;
	private boolean[][] grid, gridBackup;
	private boolean DEBUG = true;
	boolean committed;

	private int[] widthArray, heightArray, widthBackup, heightBackup;

	// Here a few trivial methods are provided:

	/**
	 * Creates an empty board of the given width and height measured in blocks.
	 */
	public Board(int width, int height) {
		this.width = width;
		this.height = height;
		grid = new boolean[width][height];
		gridBackup = new boolean[width][height];
		committed = true;

		widthArray = new int[height];
		heightArray = new int[width];
		widthBackup = new int[height];
		heightBackup = new int[width];
		maxHeight = 0;
	}

	/**
	 * Returns the width of the board in blocks.
	 */
	public int getWidth() {
		return width;
	}

	/**
	 * Returns the height of the board in blocks.
	 */
	public int getHeight() {
		return height;
	}

	/**
	 * Returns the max column height present in the board. For an empty board this
	 * is 0.
	 */
	public int getMaxHeight() {
		return maxHeight;
	}

	/**
	 * Checks the board for internal consistency -- used for debugging.
	 */
	public void sanityCheck() {
		if (!DEBUG) return;
		int[] heights = new int[width];
		int[] widths = new int[height];
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				if (grid[i][j]) {
					widths[j]++;
					heights[i] = Math.max(j + 1, heights[i]);
				}
			}
		}
		assertArrayEquals(widths, this.widthArray);
		assertArrayEquals(heights, this.heightArray);
	}

	/**
	 * Given a piece and an x, returns the y value where the piece would come to
	 * rest if it were dropped straight down at that x.
	 * 
	 * <p>
	 * Implementation: use the skirt and the col heights to compute this fast --
	 * O(skirt length).
	 */

	public int dropHeight(Piece piece, int x) {
		int res = 0;
		for (int i = 0; i < piece.getSkirt().length; i++) {
			// Chooses largest difference between height of a column and skirt point
			if (x + i < heightArray.length) res = Math.max(res, heightArray[x + i] - piece.getSkirt()[i]);
			else return -1; // Returns 1 if out of bounds
		}
		return res;
	}

	/**
	 * Returns the height of the given column -- i.e. the y value of the highest
	 * block + 1. The height is 0 if the column contains no blocks.
	 */
	public int getColumnHeight(int x) {
		return (x >= 0 && x < heightArray.length) ? heightArray[x] : -1;
	}

	/**
	 * Returns the number of filled blocks in the given row.
	 */
	public int getRowWidth(int y) {
		return (y >= 0 && y < widthArray.length) ? widthArray[y] : -1;
	}

	/**
	 * Returns true if the given block is filled in the board. Blocks outside of the
	 * valid width/height area always return true.
	 */
	public boolean getGrid(int x, int y) {
		return !inBounds(x, y) || grid[x][y]; // should return true if out of bounds
	}

	/**
	 * Returns true if the given block is filled in the board. Blocks outside of the
	 * valid width/height area always return true.
	 */

	public static final int PLACE_OK = 0;
	public static final int PLACE_ROW_FILLED = 1;
	public static final int PLACE_OUT_BOUNDS = 2;
	public static final int PLACE_BAD = 3;

	/**
	 * Attempts to add the body of a piece to the board. Copies the piece blocks
	 * into the board grid. Returns PLACE_OK for a regular placement, or
	 * PLACE_ROW_FILLED for a regular placement that causes at least one row to be
	 * filled.
	 * 
	 * <p>
	 * Error cases: A placement may fail in two ways. First, if part of the piece
	 * may falls out of bounds of the board, PLACE_OUT_BOUNDS is returned. Or the
	 * placement may collide with existing blocks in the grid in which case
	 * PLACE_BAD is returned. In both error cases, the board may be left in an
	 * invalid state. The client can use undo(), to recover the valid, pre-place
	 * state.
	 */
	public int place(Piece piece, int x, int y) {
		if (!committed) throw new RuntimeException("place commit problem");
		backupData();
		int result = PLACE_OK;
		boolean lock = true;

		for (TPoint point : piece.getBody()) {
			int x0 = x + point.x;
			int y0 = y + point.y;
			if (!inBounds(x0, y0)) return PLACE_OUT_BOUNDS;
			if (grid[x0][y0]) return PLACE_BAD;
			if (updateInfo(x0, y0) && lock) {
				result = PLACE_ROW_FILLED;
				lock = false;
			}
		}
		sanityCheck();
		return result;
	}

	// Updates info in board and returns whether row was filled this time
	private boolean updateInfo(int x0, int y0) {
		grid[x0][y0] = true;
		widthArray[y0]++; // Increases it for each point
		heightArray[x0] = Math.max(y0 + 1, heightArray[x0]); // Updates only if higher point is reached
		maxHeight = Math.max(heightArray[x0], maxHeight);
		return widthArray[y0] == width;
	}

	/**
	 * Deletes rows that are filled all the way across, moving things above down.
	 * Returns the number of rows cleared.
	 */
	public int clearRows() {
		backupData();
		int res = 0;

		// Queue is used to store indexes of columns which need to be removed
		Queue<Integer> queue = new LinkedList<Integer>();
		for (int i = 0; i < height; i++) {
			if (widthArray[i] == width) {
				queue.add(i);
				res++;
			} else checkAndRemove(queue, i);
		}
		clearQueue(queue);
		updateHeights(res);
		sanityCheck();
		return res;
	}

	/**
	 * Updates heights array of board, we have to iterate over grid from behind, if
	 * we see filled spot, we mark it as the height of the row and break the cycle
	 */
	private void updateHeights(int rowsCleared) {
		maxHeight = 0;
		for (int i = 0; i < width; i++) {
			for (int j = height - 1; j >= 0; j--) {
				if (grid[i][j]) {
					heightArray[i] = j + 1;
					break;
				} else if (j == 0) heightArray[i] = 0;
			}
		}
		// Updates maxHeight
		for (int i : heightArray) maxHeight = Math.max(maxHeight, i);
	}

	// Checks if queue is not empty and if so, copies one row from another and adds
	// new one to queue
	private void checkAndRemove(Queue<Integer> queue, int i) {
		if (!queue.isEmpty()) {
			copyRow(i, queue.peek());
			widthArray[queue.peek()] = widthArray[i];
			queue.remove();
			queue.add(i);
		}
	}

	// Clears queue and puts empty rows on the top of grid
	private void clearQueue(Queue<Integer> queue) {
		while (!queue.isEmpty()) {
			int index = queue.peek();
			widthArray[index] = 0; // Deletes a col
			for (int row = 0; row < width; row++) grid[row][index] = false;
			queue.remove();
		}
	}

	// Copies one row to another
	private void copyRow(int from, int to) {
		for (int row = 0; row < width; row++) grid[row][to] = grid[row][from];
	}

	// Backs up data in backup arrays and sets commited to false
	private void backupData() {
		committed = false;
		maxHeightBackup = maxHeight;
		System.arraycopy(widthArray, 0, widthBackup, 0, widthArray.length);
		System.arraycopy(heightArray, 0, heightBackup, 0, heightArray.length);
		for (int row = 0; row < gridBackup.length; row++) {
			System.arraycopy(grid[row], 0, gridBackup[row], 0, grid[row].length);
		}
	}

	/**
	 * Reverts the board to its state before up to one place and one clearRows(); If
	 * the conditions for undo() are not met, such as calling undo() twice in a row,
	 * then the second undo() does nothing. See the overview docs.
	 */
	public void undo() {
		if (!committed) {
			maxHeight = maxHeightBackup;
			System.arraycopy(widthBackup, 0, widthArray, 0, widthArray.length);
			System.arraycopy(heightBackup, 0, heightArray, 0, heightArray.length);
			for (int row = 0; row < grid.length; row++) {
				System.arraycopy(gridBackup[row], 0, grid[row], 0, grid[row].length);
			}
		}
		commit();
	}

	/**
	 * Puts the board in the committed state.
	 */
	public void commit() {
		committed = true;
	}

	// Checks whether current row, column pair is in bounds of grid
	private boolean inBounds(int x, int y) {
		return x >= 0 && x < width && y >= 0 && y < height;
	}

	/*
	 * Renders the board state as a big String, suitable for printing. This is the
	 * sort of print-obj-state utility that can help see complex state change over
	 * time. (provided debugging utility)
	 */
	public String toString() {
		StringBuilder buff = new StringBuilder();
		for (int y = height - 1; y >= 0; y--) {
			buff.append('|');
			for (int x = 0; x < width; x++) {
				if (getGrid(x, y)) buff.append('+');
				else buff.append(' ');
			}
			buff.append("|\n");
		}
		for (int x = 0; x < width + 2; x++) buff.append('-');
		return (buff.toString());
	}
}