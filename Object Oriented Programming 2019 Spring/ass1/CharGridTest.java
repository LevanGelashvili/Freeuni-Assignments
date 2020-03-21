// Test cases for CharGrid -- a few basic tests are provided.
package assign1;

import static org.junit.Assert.*;
import org.junit.Test;

public class CharGridTest {
	
	@Test
	public void testCharArea1() {
		char[][] grid = new char[][] {
				{'a', 'y', ' '},
				{'x', 'a', 'z'},
			};		
		CharGrid cg = new CharGrid(grid);
		assertEquals(4, cg.charArea('a'));
		assertEquals(1, cg.charArea('z'));
	}
	
	@Test
	public void testCharArea2() {
		char[][] grid = new char[][] {
				{'c', 'a', ' '},
				{'b', ' ', 'b'},
				{' ', ' ', 'a'}
			};
		CharGrid cg = new CharGrid(grid);
		assertEquals(6, cg.charArea('a'));
		assertEquals(3, cg.charArea('b'));
		assertEquals(1, cg.charArea('c'));
	}
	
	@Test
	public void testCharArea3() {
		char[][] grid = new char[][] {
			{'a','b','c','d'},
			{'a','b','c','d'},
			{'b','a','c','e'},
			{'f','b','d','a'}
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(1, cg.charArea('e'));
		assertEquals(1, cg.charArea('f'));
		assertEquals(16, cg.charArea('a'));
	}
	
	@Test
	public void testCharArea4() {
		char[][] grid = new char[][] {
			{}
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(0, cg.charArea('?'));
	}
	
	@Test
	public void testCharArea5() {
		char[][] grid = new char[][] {
			{'1'},
			{'2'},
			{'3'},
			{'4'},
			{'1'},
			{'3'}
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(5, cg.charArea('1'));
		assertEquals(4, cg.charArea('3'));
		assertEquals(1, cg.charArea('2'));
	}
	
	@Test 
	public void testCountPlus1() {
		char[][] grid = new char[][] {
			" a ".toCharArray(),
			"aaa".toCharArray(),
			" a ".toCharArray(),
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(1, cg.countPlus());
	}
	
	@Test 
	public void testCountPlus2() {
		char[][] grid = new char[][] {
			" t t ".toCharArray(),
			"ttt t".toCharArray(),
			" t tt".toCharArray(),
			"t   t".toCharArray(),
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(1, cg.countPlus());
	}
	
	@Test 
	public void testCountPlus3() {
		char[][] grid = new char[][] {
			"abbaca".toCharArray(),
			"abbaca".toCharArray(),
			"bbbbbc".toCharArray(),
			"aabcab".toCharArray(),
			"babbbb".toCharArray(),
			"bbcbbb".toCharArray()
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(1, cg.countPlus());
	}
	
	@Test 
	public void testCountPlus4() {
		char[][] grid = new char[][] {
			"abbaca".toCharArray(),
			"abbaca".toCharArray(),
			"abbbbc".toCharArray(),
			"aabcab".toCharArray(),
			"babbbb".toCharArray(),
			"bbbbbb".toCharArray()
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(0, cg.countPlus());
	}
	
	@Test 
	public void testCountPlus5() {
		char[][] grid = new char[][] {
			"".toCharArray()
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(0, cg.countPlus());
	}
	
	@Test 
	public void testCountPlus6() {
		char[][] grid = new char[][] {
			"  x    x  ".toCharArray(),
			"xxxxx  xx ".toCharArray(),
			"  x t x t ".toCharArray(),
			" x  x x t ".toCharArray(),
			"xxxaxxxxx ".toCharArray(),
			" xaaa x   ".toCharArray(),
			"   a  x t ".toCharArray()
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(3, cg.countPlus());
	}
	
	@Test 
	public void testCountPlus7() {
		char[][] grid = new char[][] {
			"aaaaa".toCharArray(),
			"aaaaa".toCharArray(),
			"aaaaa".toCharArray(),
			"aaaaa".toCharArray(),
			"aaaaa".toCharArray()
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(1, cg.countPlus());
	}
	
	@Test 
	public void testCountPlus8() {
		char[][] grid = new char[][] {
			"aaaa".toCharArray(),
			"aaaa".toCharArray(),
			"aaaa".toCharArray(),
			"aaaa".toCharArray(),
			"aaaa".toCharArray(),
		};
		CharGrid cg = new CharGrid(grid);
		assertEquals(0, cg.countPlus());
	}
	
}
