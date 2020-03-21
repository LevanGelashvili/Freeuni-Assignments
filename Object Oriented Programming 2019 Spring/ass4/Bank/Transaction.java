package Bank;

public class Transaction {

	private final int from, to, amount;

	//Class for a single transaction. Stores account ids, which participated in a transaction.
	//Also stores amount of money transacted.
	public Transaction(int from, int to, int amount) {
		this.from = from;
		this.to = to;
		this.amount = amount;
	}
	
	//Returns who transacted
	public int getFrom() {
		return from;
	}
	
	//Returns who was transacted
	public int getTo() {
		return to;
	}
	
	//Returns amount of money transacted
	public int getAmount() {
		return amount;
	}

	@Override
	public String toString() {
		return from + " - " + to + " - " + amount;
	}
}
