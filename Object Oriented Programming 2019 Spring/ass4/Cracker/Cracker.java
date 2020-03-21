package Cracker;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.concurrent.CountDownLatch;

public class Cracker {
	// Array of chars used to produce strings
	public static final char[] CHARS = "abcdefghijklmnopqrstuvwxyz0123456789.,-!".toCharArray();
	private final CountDownLatch latch;
	private final String hash;
	private int maxLength;

	//Creates a new cracked object and stores passed info : 
	//Number of threads, maximum length of a password and the hashcode
	public Cracker(int numThreads, int maxLength, String hash) {
		latch = new CountDownLatch(numThreads);
		this.maxLength = maxLength;
		this.hash = hash;
	}

	//Blocks the latch
	public void block() {
		try {
			latch.await();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	public class Worker implements Runnable {

		private int start, end;

		//Worker object, stores start and end indexes of its interval
		public Worker(int start, int end) {
			this.start = start;
			this.end = end;
		}

		@Override
		public void run() {
			for (int i = start; i <= end; i++) {
				//For each char, generates a new String recursively
				String s = new String(CHARS[i] + "");
				checkIfWorks(s);
				generate(s);
			}
			//Notifies latch that it's done
			latch.countDown();
		}

		//Continues generation process while checking, whether it has exceeded the length
		private void generate(String s) {
			if (s.length() >= maxLength) return;
			for (char c : CHARS) {
				s += c;
				checkIfWorks(s);
				generate(s);
				//Backtracking, removes the last letter from deeper recursion
				s = s.substring(0, s.length() - 1);
			}
		}

		//If password is correct, prints it
		private void checkIfWorks(String s) {
			if (generateHash(s).equals(hash)) {
				System.out.println(s);
			}
		}
	}

	//Method that takes a password and generates its hash code using SHA algorithm, found it on javadoc
	public static String generateHash(String password) {
		try {
			MessageDigest md = MessageDigest.getInstance("SHA");
			md.update(password.getBytes());
			return hexToString(md.digest());
		} catch (NoSuchAlgorithmException e) {
			e.printStackTrace();
		}
		return null;
	}

	/*
	 * Given a byte[] array, produces a hex String, such as "234a6f". with 2 chars
	 * for each byte in the array. (provided code)
	 */
	public static String hexToString(byte[] bytes) {
		StringBuffer buff = new StringBuffer();
		for (int i = 0; i < bytes.length; i++) {
			int val = bytes[i];
			val = val & 0xff; // remove higher bits, sign
			if (val < 16) buff.append('0'); // leading 0
			buff.append(Integer.toString(val, 16));
		}
		return buff.toString();
	}

	/*
	 * Given a string of hex byte values such as "24a26f", creates a byte[] array of
	 * those values, one byte value -128..127 for each 2 chars. (provided code)
	 */
	public static byte[] hexToArray(String hex) {
		byte[] result = new byte[hex.length() / 2];
		for (int i = 0; i < hex.length(); i += 2) {
			result[i / 2] = (byte) Integer.parseInt(hex.substring(i, i + 2), 16);
		}
		return result;
	}

	// possible test values:
	// a 86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
	// fm adeb6f2a18fe33af368d91b09587b68e3abcb9a7
	// a! 34800e15707fae815d7c90d49de44aca97e2d759
	// xyz 66b27417d37e024c46526c2f6d358a754fc552f3
}
