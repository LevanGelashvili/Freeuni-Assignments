package assign3;

import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;

/**
 * @author levan This is a class that seperates model from base. If base is
 *         changed, model doesn't need to change since this class encapsulates
 *         relationship with a database.
 */
public class DataAccessObject {

	private Connection con;
	private ResultSet set;
	private Statement st;

	/**
	 * @param m : Adds a metropolis to database
	 * @throws SQLException
	 */
	public void addToBase(Metropolis m) throws SQLException {
		connectToBase();

		String query = "Insert into " + Constants.TABLE + " values ";
		query += "(\'" + m.getName() + "\' , \'" + m.getContinent() + "\' , " + m.getPopulation() + ");";
		Statement st = con.createStatement();
		st.executeUpdate(query);

		disconnectFromBase();

	}

	/**
	 * Selects info from base based on request
	 * 
	 * @param request : Request passed by user
	 * @return : List of metropolises matching that criteria
	 * @throws SQLException
	 */
	public List<Metropolis> selectFromBase(Request request) throws SQLException {
		List<Metropolis> cities = new ArrayList<Metropolis>();

		connectToBase();
		search(cities, request);
		disconnectFromBase();

		return cities;
	}

	/**
	 * Actual searching method, used for decomposition
	 * 
	 * @param cities  : List to update
	 * @param request : Request passed by user
	 * @throws SQLException
	 */
	private void search(List<Metropolis> cities, Request request) throws SQLException {
		st = con.createStatement();

		String query = Constants.SELECT_ALL + "where (population";
		query += (request.isPopulationLarger()) ? " > " : " <= ";
		query += request.getMetropolis().getPopulation();
		query += " or " + request.getMetropolis().getPopulation() + " = -1 )";
		query += getStringBoolean(request);

		set = st.executeQuery(query);
		while (set.next()) cities.add(createMetropolis());
	}

	// Generates a string for boolean checking to pass to database, used for
	// decomposition
	private String getStringBoolean(Request request) {
		Metropolis cur = request.getMetropolis();
		String query = " and (( \'" + cur.getName() + "\'";
		if (request.isExactMatch()) {
			query += " = \'\' or metropolis = \'" + cur.getName() + "\' )";
			query += " and (\'" + cur.getContinent() + "\' = \'\' or continent = \'" + cur.getContinent() + "\' ));";
		} else {
			query += " = \'\' or metropolis like \'%" + cur.getName() + "%\' )";
			query += " and (\'" + cur.getContinent() + "\' = \'\' or continent like \'%" + cur.getContinent()
					+ "%\' ));";
		}
		return query;
	}

	// Creates a new metropolis
	private Metropolis createMetropolis() throws SQLException {
		String name = set.getString(Constants.CITY_COL);
		String continent = set.getString(Constants.CONTINENT_COL);
		String population = set.getString(Constants.POPULATION_COL);
		return new Metropolis(name, continent, Integer.parseInt(population));
	}

	// Connects to base
	private void connectToBase() throws SQLException {
		con = DriverManager.getConnection(
				"jdbc:mysql://" + MyDBInfo.MYSQL_DATABASE_SERVER + "/" + MyDBInfo.MYSQL_DATABASE_NAME,
				MyDBInfo.MYSQL_USERNAME, MyDBInfo.MYSQL_PASSWORD);
	}

	// Disconects from database
	private void disconnectFromBase() throws SQLException {
		con.close();
	}
}
