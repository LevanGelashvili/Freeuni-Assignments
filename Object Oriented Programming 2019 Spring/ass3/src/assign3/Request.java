package assign3;

public class Request {

	private Metropolis metropolis;
	private boolean population, match, isInsert;
	
	/**
	 * Request is a class which gathers info from view textfields and passes it to the controller.
	 * @param population : is population larger or smaller
	 * @param match : is match exact or partial
	 * @param metropolis : metropolis based on input
	 * @param isInsert : should insert this data or serch based on this info.
	 * Other than that, standart getter and setter methods.
	 */
	public Request(boolean population, boolean match, Metropolis metropolis, boolean isInsert) {
		this.population = population;
		this.match = match;
		this.metropolis = metropolis;
		this.isInsert = isInsert;
	}
	
	public boolean isPopulationLarger() {
		return this.population;
	}
	
	public boolean isExactMatch() {
		return this.match;
	}
	
	public Metropolis getMetropolis() {
		return this.metropolis;
	}
	
	public boolean shouldInsertData() {
		return this.isInsert;
	}
}
