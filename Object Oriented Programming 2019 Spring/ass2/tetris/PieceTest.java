package tetris;

import static org.junit.Assert.*;
import java.util.*;

import org.junit.*;

/*
  Unit test for Piece class -- starter shell.
 */
public class PieceTest {
	// You can create data to be used in the your
	// test cases like this. For each run of a test method,
	// a new PieceTest object is created and setUp() is called
	// automatically by JUnit.
	// For example, the code below sets up some
	// pyramid and s pieces in instance variables
	// that can be used in tests.
	private Piece pyr1, pyr2, pyr3, pyr4, stick1, stick2, stick3, snake11, snake12, snake13, square, l11, l12, l13, l14,
			snake21, snake22, snake23, l21, l22, l23, l24, dummy;

	@Before
	public void setUp() throws Exception {
		pyr1 = new Piece(Piece.PYRAMID_STR);
		pyr2 = pyr1.computeNextRotation();
		pyr3 = pyr2.computeNextRotation();
		pyr4 = pyr3.computeNextRotation();

		stick1 = new Piece(Piece.STICK_STR);
		stick2 = stick1.computeNextRotation();
		stick3 = stick2.computeNextRotation();

		snake11 = new Piece(Piece.S1_STR);
		snake12 = snake11.computeNextRotation();
		snake13 = snake12.computeNextRotation();

		square = new Piece(Piece.SQUARE_STR);

		l11 = new Piece(Piece.L1_STR);
		l12 = l11.computeNextRotation();
		l13 = l12.computeNextRotation();
		l14 = l13.computeNextRotation();

		snake21 = new Piece(Piece.S2_STR);
		snake22 = snake21.computeNextRotation();
		snake23 = snake22.computeNextRotation();

		l21 = new Piece(Piece.L2_STR);
		l22 = l21.computeNextRotation();
		l23 = l22.computeNextRotation();
		l24 = l23.computeNextRotation();
	}

	@Test
	// Tests widths and heights
	public void testSampleSize() {
		equalWH(stick1, 1, 4);
		equalWH(stick2, 4, 1);
		equalWH(stick3, stick1);

		equalWH(pyr1, 3, 2);
		equalWH(pyr2, 2, 3);
		equalWH(pyr3, pyr1);
		equalWH(pyr4, pyr2);

		equalWH(square, 2, 2);
		equalWH(square, square.computeNextRotation());

		equalWH(l11, l21);
		equalWH(l14, l24);
		equalWH(l13, l23);

		equalWH(snake11, 3, 2);
		equalWH(snake12, 2, 3);
		equalWH(snake11, snake21);

		equalWH(l21, l11);
		equalWH(l21, 2, 3);
		equalWH(l22, 3, 2);
	}

	@Test
	// Tests rotations and equal (self equal too)
	public void testRotationsAndEquals() {
		assertTrue(snake11.equals(snake13));
		assertTrue(square.equals(square.computeNextRotation().computeNextRotation()));
		assertTrue(pyr1.equals(pyr1));
		assertTrue(pyr1
				.equals(pyr1.computeNextRotation().computeNextRotation().computeNextRotation().computeNextRotation()));
		assertTrue(snake21.computeNextRotation().equals(snake23.computeNextRotation()));
		assertTrue(stick1.computeNextRotation().equals(stick2));
		assertTrue(!l12.computeNextRotation().equals(l11));
		assertTrue(l11.computeNextRotation().equals(l12));
	}

	@Test
	// Tests fast rotations
	public void testFastRotations() {
		assertTrue(snake11.equals(Piece.getPieces()[Piece.S1].fastRotation().fastRotation()));
		assertTrue(stick1.equals(Piece.getPieces()[Piece.STICK].fastRotation().fastRotation()));
		assertTrue(pyr1
				.equals(Piece.getPieces()[Piece.PYRAMID].fastRotation().fastRotation().fastRotation().fastRotation()));
		assertTrue(snake11.equals(Piece.getPieces()[Piece.S1].fastRotation().fastRotation()));
		// Points to itself, so we can call as many fastRotations as we want
		assertTrue(square.equals(Piece.getPieces()[Piece.SQUARE].fastRotation().fastRotation().fastRotation()
				.fastRotation().fastRotation().fastRotation().fastRotation().fastRotation().fastRotation()));
	}

	// Checks if pieces have equals widths and heights
	private void equalWH(Piece a, Piece b) {
		assertEquals(a.getWidth(), b.getWidth());
		assertEquals(a.getHeight(), b.getHeight());
	}

	// Checks if w and h equal to piece's width and height
	private void equalWH(Piece p, int w, int h) {
		assertEquals(p.getWidth(), w);
		assertEquals(p.getHeight(), h);
	}

	// Test the skirt returned by a few pieces
	@Test
	public void testSkirt() {
		assertTrue(Arrays.equals(new int[] { 0, 0, 0 }, pyr1.getSkirt()));
		assertTrue(Arrays.equals(new int[] { 1, 0, 1 }, pyr3.getSkirt()));
		assertTrue(Arrays.equals(new int[] { 0 }, stick1.getSkirt()));
		assertTrue(Arrays.equals(new int[] { 0, 0, 0, 0 }, stick2.getSkirt()));
		assertTrue(Arrays.equals(new int[] { 0, 0 }, square.getSkirt()));
		assertTrue(Arrays.equals(new int[] { 0, 1, 1 }, l14.getSkirt()));
		assertTrue(Arrays.equals(new int[] { 0, 0 }, l21.getSkirt()));
		assertTrue(Arrays.equals(new int[] { 1, 1, 0 }, l22.getSkirt()));
	}
	
	@Test
	// Code that i found here to test exceptions :
	// https://stackoverflow.com/questions/156503/how-do-you-assert-that-a-certain-exception-is-thrown-in-junit-4-tests
	public void testFooThrowsIndexOutOfBoundsException() {
		boolean thrown = false;
		try {
			dummy = new Piece("abcd");
		} catch (RuntimeException e) {
			thrown = true;
		}
		assertTrue(thrown);
	}
}
