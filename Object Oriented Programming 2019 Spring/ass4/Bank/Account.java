package Bank;

public class Account {

	private static final int DEFAULT_BALANCE = 1000;
	private int balance, transactions, id;

	// Class for a sincle account, stores id, which is passed.
	//Also stores transactions and balance, which are 0 an 1000 by default
	public Account(int id) {
		this.id = id;
		this.balance = DEFAULT_BALANCE;
		this.transactions = 0;
	}

	//Changes balance for this account. Is synchronized, so 2 threads wont increase it at the same time
	public synchronized void changeBalance(int amount) {
		balance += amount;
		transactions++; //Increases transaction count
	}

	@Override
	public String toString() {
		return "acct : " + id + " bal : " + balance + " trans : "  + transactions;
	}
}
