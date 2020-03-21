package Cracker;

public class CrackerMain {

	public static final int MAX_THREADS = 40;

	//Main method, runs the program
	public static void main(String[] args) {

		//If args length is 1, it is hash generating mode and we pass only password
		if (args.length == 1) {
			System.out.println(Cracker.generateHash(args[0]));
		} else {
			//Else, its craking mode and we hass hash and number of threads
			crackPassword(args[0], Integer.parseInt(args[1]), Integer.parseInt(args[2]));
		}
	}

	//Creates threads and runs them seperately, each getting a unique interval
	private static void crackPassword(String hash, int maxLength, int numThreads) {
		Cracker cracker = new Cracker(numThreads, maxLength, hash);
		int offset = MAX_THREADS / numThreads; //Size of an interval
		for (int i = 0; i < numThreads; i++) {
			int start = i * offset; //Starting index
			//End index, if it's the last thread, has to compute till the end of the array
			int end = (i == numThreads - 1) ? MAX_THREADS - 1 : start + offset - 1;
			Thread t = new Thread(cracker.new Worker(start, end));
			t.start();
		}
		cracker.block();
		System.out.println("All done");
	}
}
