package assign3;

/**
 * Constants for database assignment, self explainatory names
 */
public class Constants {

	//Constants for connecting to base
	public static final String USERNAME = "root";
	public static final String PASSWORD = "password";
	public static final String SERVER = "localhost";
	public static final String DATABASE = "metropolis";
	public static final String TABLE = "metropolises";

	//Constants for specifying number of column
	public static final int LABEL_COL = 0;
	public static final int CITY_COL = 1;
	public static final int CONTINENT_COL = 2;
	public static final int POPULATION_COL = 3;
	public static final int NUM_COLS = 3;

	//Constants for View class
	public static final int FRAME_WIDTH = 600;
	public static final int FRAME_HEIGHT = 500;
	public static final int FIELD_SIZE = 15;
	public static final String POP_LARGER = "Population Larger Than";
	public static final String POP_SMALLER = "Population Smaller Than Or Equal To";
	public static final String MATCH_EXACT = "Exact Match";
	public static final String MATH_PARTIAL = "Partial Match";

	//Other constants
	public static final String[] COLUMN_NAMES = { "Metropolis", "Continent", "Population" };
	public static final String SELECT_ALL = "select * from " + TABLE + " ";
}
