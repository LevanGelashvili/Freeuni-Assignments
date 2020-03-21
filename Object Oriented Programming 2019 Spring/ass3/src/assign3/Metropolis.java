package assign3;

public class Metropolis {

	private String name, continent;
	private int population;

	/**
	 * 
	 * @param name : Name of the metropolis
	 * @param continent : Continent of the metropolis
	 * @param population : Population of the metropolis
	 * Standart class, with standart getter methods.
	 */
	public Metropolis(String name, String continent, int population) {
		this.name = name;
		this.continent = continent;
		this.population = population;
	}

	public String getName() {
		return this.name;
	}

	public String getContinent() {
		return this.continent;
	}

	public int getPopulation() {
		return this.population;
	}
	
	@Override
	public String toString() {
		return name + " - " + continent + " - " + population;
	}
}
