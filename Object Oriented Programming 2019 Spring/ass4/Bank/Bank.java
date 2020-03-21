package Bank;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.LinkedBlockingQueue;

import sun.misc.GC.LatencyRequest;

public class Bank {

	//Poison pill is a special transaction object, to let a thread know, when to interrupt
	public static final Transaction POISON_PILL = new Transaction(-1, 0, 0);
	public static final int MAX_WORKERS = 20;
	private static final int MAX_QUEUE_SIZE = 1000000;

	private final List<Account> accounts; //List of accounts
	private final BlockingQueue<Transaction> queue; //Thread queue, main mechanizm of synchronization
	private final CountDownLatch latch; // Latch, equivalent of join

	//Creates a new bank for numThreads threads and initializes variables
	public Bank(int numThreads) {
		accounts = new ArrayList<Account>();
		queue = new LinkedBlockingQueue<Transaction>(MAX_QUEUE_SIZE);
		latch = new CountDownLatch(numThreads);
	}

	//Returns accounts
	public List<Account> getAccounts() {
		return accounts;
	}

	//Returns blocking queue
	public BlockingQueue<Transaction> getQueue() {
		return queue;
	}

	//Returns latch
	public CountDownLatch getLatch() {
		return latch;
	}

	public class Worker implements Runnable {

		@Override
		public void run() {
			//Runs in a loop, processes a transaction, or else if it's a posion pill, returns.
			while (true) {
				Transaction cur = queue.poll();
				if (cur == null) continue;
				if (cur.equals(POISON_PILL)) break;
				accounts.get(cur.getFrom()).changeBalance(-cur.getAmount());
				accounts.get(cur.getTo()).changeBalance(cur.getAmount());
			}
			latch.countDown(); //Notifies latch that it's over.
		}
	}
}
