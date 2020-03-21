import static org.junit.Assert.*;
import org.junit.Test;

public class AccountManagerTest {
	
	//Test class was not mandatory, wrote it just to be sure, covers every case

	//Test logins
	@Test
	public void test1() {
		AccountManager m = AccountManager.getInstance();
		assertEquals(true, m.login("Patrick", "1234"));
		assertEquals(true, m.login("Molly", "FloPup"));
		assertEquals(false, m.login("Patrick", "FloPup"));
		assertEquals(false, m.login("Levan", "1234"));
	}
	
	//Test create accounts
	@Test
	public void test2() {
		AccountManager m = AccountManager.getInstance();
		assertEquals(true, m.createAccount("Levan", "abcd"));
		assertEquals(true, m.login("Levan", "abcd"));
		assertEquals(false, m.login("Levan", "abcdefgh"));
		assertEquals(false, m.createAccount("Levan", "newPassword"));
	}

} 