package Logic;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.List;
import java.util.Map;

//Class encapsulating connection to the database
public class DAO {

	// Only thing we need is filling products list with data taken from base
	public void fillInfo(Map<String, Product> products) {
		try {
			Connection con = DriverManager.getConnection("jdbc:mysql://" + Constants.SERVER + "/" + Constants.BASE, Constants.USERNAME, Constants.PASSWORD);
			Statement st = con.createStatement();

			ResultSet set = st.executeQuery("select * from " + Constants.TABLE + ";");
			while (set.next()) {
				products.put(set.getString(Constants.ID), parseProduct(set));
			}

			con.close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}

	// Returns a product generated from result set
	private Product parseProduct(ResultSet set) throws SQLException {
		String id = set.getString(Constants.ID);
		String name = set.getString(Constants.NAME);
		String image = set.getString(Constants.IMAGE);
		double price = Double.parseDouble(set.getString(Constants.PRICE));

		return new Product(id, name, image, price);
	}
}
