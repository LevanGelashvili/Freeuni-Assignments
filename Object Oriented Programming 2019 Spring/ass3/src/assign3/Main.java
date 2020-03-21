package assign3;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.Statement;

public class Main {

	/**
	 * Static void method which runs the whole application. Creates model, view and
	 * controller. Even though model is passed to the view as a parameter, there is
	 * no coupling going on, since model is needed only to update table data in
	 * view. If model is changed, view won't need any updates. Same with controller,
	 * there is minimal interaction betweeen those classes. App is implemented in
	 * MVC pattern an in addition there is an observer pattern between controller
	 * and view, because controller shouldn't see the insides of a view
	 * 
	 * MVC pattern taken from :
	 * https://examples.javacodegeeks.com/core-java/java-swing-mvc-example/
	 * 
	 * Observer pattern taken from :
	 * https://examples.javacodegeeks.com/core-java/util/observer/java-util-observer-example/
	 */
	public static void main(String[] args) {
		resetTable();
		Model model = new Model();
		View view = new View(model);
		Controller controller = new Controller(model);
		// Controller starts observing view.
		view.addObserver(controller);
	}

	/*
	 * Resets the table, deleting every row.
	 */
	private static void resetTable() {
		try {
			Connection con = DriverManager.getConnection(
					"jdbc:mysql://" + MyDBInfo.MYSQL_DATABASE_SERVER + "/" + MyDBInfo.MYSQL_DATABASE_NAME,
					MyDBInfo.MYSQL_USERNAME, MyDBInfo.MYSQL_PASSWORD);
			String query = "delete from " + Constants.TABLE + ";";
			Statement st = con.createStatement();
			st.executeUpdate(query);
			con.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
