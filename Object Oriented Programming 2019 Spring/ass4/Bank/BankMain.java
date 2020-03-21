package Bank;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.StringTokenizer;

public class BankMain {

	//Main method, runs the program
	public static void main(String[] args) {
		
		//Number of threads to run is passed as the first argument
		int numThreads = Integer.parseInt(args[1]);
		
		//Creates a new bank object
		Bank bank = new Bank(numThreads);
		addAccounts(bank);

		try {
			readFromFile(bank, args[0]);
		} catch (IOException e) {
			e.printStackTrace();
		}

		try {
			runWorkers(bank, numThreads);
		} catch (NumberFormatException | InterruptedException e) {
			e.printStackTrace();
		}
	}

	//Runs worker threads.
	private static void runWorkers(Bank bank, int numThreads) throws InterruptedException {
		for (int i = 0; i < numThreads; i++) {
			//After adding actual accounts, adds poison pills numThread times
			bank.getQueue().add(Bank.POISON_PILL);
		}

		//Creates numThread workers
		for (int i = 0; i < numThreads; i++) {
			Thread t = new Thread(bank.new Worker());
			t.start();
		}

		//Passes this line only after all threads have been executes, equivalent to join
		bank.getLatch().await();

		printResults(bank);
	}

	//Prints results
	private static void printResults(Bank bank) {
		for (Account a : bank.getAccounts()) {
			System.out.println(a);
		}
	}

	//Reads from file, until hits the finish mark.
	private static void readFromFile(Bank bank, String filename) throws IOException {
		BufferedReader reader = new BufferedReader(new FileReader(filename));
		StringTokenizer tk;
		String line = "";
		while (true) {
			line = reader.readLine();
			if (line == null) break;
			tk = new StringTokenizer(line);
			//Creates a new transaction and adds it to blocking queue
			bank.getQueue().add(newTransaction(tk));
		}
		reader.close();
	}

	//Creates and returns new transaction object, tk parses the string into ints
	private static Transaction newTransaction(StringTokenizer tk) {
		int from = Integer.parseInt(tk.nextToken());
		int to = Integer.parseInt(tk.nextToken());
		int amount = Integer.parseInt(tk.nextToken());
		return new Transaction(from, to, amount);
	}

	//Adds read accounts to the bank
	private static void addAccounts(Bank bank) {
		for (int i = 0; i < Bank.MAX_WORKERS; i++) {
			bank.getAccounts().add(new Account(i));
		}
	}
}
