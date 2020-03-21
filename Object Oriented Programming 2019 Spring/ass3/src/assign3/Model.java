package assign3;

import java.util.ArrayList;
import java.util.List;

import javax.swing.table.AbstractTableModel;

public class Model extends AbstractTableModel {

	private DataAccessObject baseConnector;
	private List<Metropolis> metropolises;

	/**
	 * Model is a class, which encapsulates all data and logic into itself. In this
	 * case, it has a list of metropolises, which containts info to display on GUI.
	 * It does not see the view, but has a fireTableDataChanged method, which
	 * notifies some unknown to him table with listeners that data was changed and
	 * it needs to redraw itself.
	 */
	public Model() {
		baseConnector = new DataAccessObject();
		metropolises = new ArrayList<Metropolis>();
	}

	/**
	 * Gets queries from base
	 * @param req : Request passed by user
	 */
	public void getInfo(Request req) {
		try {
			metropolises = baseConnector.selectFromBase(req);
		} catch (Exception e) {
			e.printStackTrace();
		}
		fireTableDataChanged();
	}

	/**
	 * Inserts a metropolis into database
	 * @param req : Request passer by user
	 */
	public void insertMetropolis(Request req) {
		try {
			baseConnector.addToBase(req.getMetropolis());
		} catch (Exception e) {
			e.printStackTrace();
		}
		metropolises.clear();
		metropolises.add(req.getMetropolis());
		fireTableDataChanged();
	}

	@Override
	/**
	 * Returns number of columns in a table
	 */
	public int getColumnCount() {
		return Constants.NUM_COLS;
	}

	@Override
	/**
	 * Returns number of rows in a table
	 */
	public int getRowCount() {
		return metropolises.size();
	}

	@Override
	/**
	 * Returns value from grid table.
	 * If list is null, then returns null, else, gathers info.
	 */
	public Object getValueAt(int row, int col) {
		if (metropolises.isEmpty()) return null;
		try {
			switch (col + 1) {
			case Constants.CITY_COL:
				return metropolises.get(row).getName();
			case Constants.CONTINENT_COL:
				return metropolises.get(row).getContinent();
			case Constants.POPULATION_COL:
				return metropolises.get(row).getPopulation();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

}
