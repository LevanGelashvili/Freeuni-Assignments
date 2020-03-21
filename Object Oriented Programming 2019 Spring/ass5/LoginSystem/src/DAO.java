import java.util.HashMap;
import java.util.Map;

/*
 * Database access object class, encapsulates a base of user accounts and passwords
 * For simplicity, we store info in a map
 */
public class DAO {

	// Map containing users and passwords
	private static Map<String, String> base = new HashMap<String, String>();

	// Default info, as mentioned in the word file
	static {
		base.put("Patrick", "1234");
		base.put("Molly", "FloPup");
	}

	// Checks in map if user key is there
	public boolean accountExists(String user) {
		return base.containsKey(user);
	}

	// Checks if given password belongs to the user
	public boolean passwordIsCorrect(String user, String password) {
		return base.get(user).equals(password);
	}

	// Creates a new account
	public void createAccount(String user, String password) {
		base.put(user, password);
	}

}