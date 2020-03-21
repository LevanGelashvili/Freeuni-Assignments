
//Class for managing accounts and their info
public class AccountManager {

	// Static AccountManager object, we only need one instance of it
	private static AccountManager manager;
	private DAO dao; // Database access object, we don't care what stands behind it

	private AccountManager() {
		dao = new DAO();
	}

	/*
	 * I M P O R T A N T
	 * 
	 * Code might look like Giorgi Kiknklade's seminars.
	 * Especially AccountManager pattern
	 * 
	 */
	
	// Since this is a singleton class, we use usual singleton creation pattern
	public static AccountManager getInstance() {
		if (manager == null) {
			synchronized (AccountManager.class) {
				manager = new AccountManager();
			}
		}
		return manager;
	}

	// Logins into system. Returns name if successful, else null
	public boolean login(String user, String password) {
		return dao.accountExists(user) && dao.passwordIsCorrect(user, password);
	}

	// Creates an account, returns true of false based on the outcome
	public boolean createAccount(String user, String password) {
		if (dao.accountExists(user)) {
			return false;
		}
		dao.createAccount(user, password);
		return true;
	}

}
