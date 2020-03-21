package tetris;

import static org.junit.Assert.*;

import org.junit.*;

public class BoardTest {
	Board b;
	Piece pyr1, stick, square, snake, snake2, l1;

	// This shows how to build things in setUp() to re-use
	// across tests.

	// In this case, setUp() makes shapes,
	// and also a 3X6 board, with pyr placed at the bottom,
	// ready to be used by tests.

	@Before
	public void setUp() throws Exception {
		pyr1 = new Piece(Piece.PYRAMID_STR);
		stick = new Piece(Piece.STICK_STR);
		square = new Piece(Piece.SQUARE_STR);
		snake = new Piece(Piece.S1_STR);
		snake2 = new Piece(Piece.S2_STR);
		l1 = new Piece(Piece.L1_STR);
	}

	@Test
	public void testWidthHeight() {
		b = new Board(3, 6);
		assertEquals(b.place(pyr1, 0, 0), Board.PLACE_ROW_FILLED);
		b.commit();
		assertEquals(b.getRowWidth(0), 3);
		assertEquals(b.getRowWidth(1), 1);
		assertEquals(b.getColumnHeight(1), 2);
		assertEquals(b.place(stick, 0, 1), Board.PLACE_OK);
		b.commit();
		assertEquals(b.getMaxHeight(), 5);
		assertEquals(b.getRowWidth(5), 0);
		assertEquals(b.getRowWidth(4), 1);
		assertEquals(b.getWidth(), 3);
		assertEquals(b.getHeight(), 6);
	}

	@Test
	public void testWidthHeightAndErrors() {
		b = new Board(4, 10);
		assertEquals(b.place(square, 0, 0), Board.PLACE_OK);
		b.commit();
		assertEquals(b.getMaxHeight(), 2);
		assertEquals(b.getRowWidth(1), 2);
		assertEquals(b.place(l1, 2, 0), Board.PLACE_ROW_FILLED);
		b.commit();
		assertEquals(b.getMaxHeight(), 3);
		assertEquals(b.getRowWidth(1), 3);
		assertEquals(b.place(square, 1, 2), Board.PLACE_BAD);
		b.commit();
		assertEquals(b.place(snake, 0, 0), Board.PLACE_BAD);
		b.commit();
		assertEquals(b.place(square, -1, -1), Board.PLACE_OUT_BOUNDS);
		b.commit();
		assertEquals(b.place(square, 18, 2), Board.PLACE_OUT_BOUNDS);
		b.commit();
		assertEquals(b.place(square, 0, 15), Board.PLACE_OUT_BOUNDS);
		b.commit();
		assertEquals(b.place(l1, 4, 4), Board.PLACE_OUT_BOUNDS);
		b.commit();
		assertEquals(b.getColumnHeight(-1), -1);
		assertEquals(b.getColumnHeight(100), -1);
		assertEquals(b.getRowWidth(15), -1);
		assertEquals(b.getRowWidth(-15), -1);
		assertEquals(b.getGrid(0, 0), true);
		assertEquals(b.getGrid(3, 3), false);
		assertEquals(b.getGrid(-1, 100), true);
		assertEquals(b.getGrid(100, -1), true);
		assertEquals(b.place(square, 3, -3), Board.PLACE_OUT_BOUNDS);
		b.commit();
	}

	@Test
	public void testClear() {
		b = new Board(3, 6);
		assertEquals(b.place(pyr1, 0, 0), Board.PLACE_ROW_FILLED);
		b.commit();
		assertEquals(b.place(stick, 0, 1), Board.PLACE_OK);
		b.commit();
		assertEquals(b.clearRows(), 1);
		b.commit();
		assertEquals(b.getMaxHeight(), 4);
		assertEquals(b.getRowWidth(0), 2);
		assertEquals(b.getColumnHeight(2), 0);
		assertEquals(b.clearRows(), 0);
		b.commit();
		assertEquals(b.getMaxHeight(), 4);
		assertEquals(b.getRowWidth(0), 2);
		assertEquals(b.getColumnHeight(2), 0);

		assertEquals(b.place(snake.computeNextRotation(), 1, 1), Board.PLACE_ROW_FILLED);
		b.commit();
		assertEquals(b.clearRows(), 1);
		b.commit();
		assertEquals(b.getMaxHeight(), 3);
		assertEquals(b.getRowWidth(1), 2);
	}

	@Test
	public void testUndo() {
		b = new Board(3, 6);
		assertEquals(b.place(pyr1, 0, 0), Board.PLACE_ROW_FILLED);
		assertEquals(b.getMaxHeight(), 2);
		b.undo();
		assertEquals(b.getMaxHeight(), 0);
		b.commit();
		assertEquals(b.place(l1.computeNextRotation(), 0, 0), Board.PLACE_ROW_FILLED);
		b.commit();
		assertEquals(b.getRowWidth(0), 3);
		b.clearRows();
		assertEquals(b.getRowWidth(0), 1);
		b.undo();
		assertEquals(b.getRowWidth(0), 3);
		b.undo();
		b.undo();
	}

	@Test
	public void testDrop() {
		b = new Board(3, 10);
		assertEquals(b.place(pyr1, 0, 0), Board.PLACE_ROW_FILLED);
		b.commit();
		assertEquals(b.dropHeight(square, 0), 2);
		assertEquals(b.dropHeight(stick, 0), 1);
		assertEquals(b.dropHeight(stick, 1), 2);
		assertEquals(b.dropHeight(pyr1, 0), 2);
		assertEquals(b.dropHeight(pyr1.computeNextRotation(), 0), 2);
		assertEquals(b.dropHeight(pyr1.computeNextRotation(), 1), 1);
		assertEquals(b.dropHeight(pyr1.computeNextRotation(), 10), -1);
	}

	@Test
	public void testDrop2() {
		b = new Board(3, 10);
		assertEquals(b.place(square, 0, 0), Board.PLACE_OK);
		b.commit();
		assertEquals(b.dropHeight(stick, 0), 2);
		assertEquals(b.dropHeight(stick, 2), 0);
		assertEquals(b.dropHeight(pyr1, 0), 2);
		assertEquals(b.dropHeight(pyr1.computeNextRotation(), 1), 1);
		assertEquals(b.dropHeight(pyr1.computeNextRotation(), 0), 2);
		assertEquals(b.place(stick, 2, 0), Board.PLACE_ROW_FILLED);
		b.commit();
		assertEquals(b.dropHeight(pyr1, 0), 4);
		assertEquals(b.dropHeight(pyr1.computeNextRotation(), 0), 2);
		assertEquals(b.dropHeight(pyr1.computeNextRotation(), 1), 4);
		assertEquals(b.dropHeight(snake2.computeNextRotation(), 0), 2);
		assertEquals(b.dropHeight(snake.computeNextRotation(), 1), 4);
		print(b);
	}

	private void print(Board b) {
		System.out.println(b.toString());
	}
}
