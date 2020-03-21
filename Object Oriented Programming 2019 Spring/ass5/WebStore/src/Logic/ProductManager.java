package Logic;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Map.Entry;

//Singleton class for a product manager, encapsulates all info about products
public class ProductManager {

	private static ProductManager manager;
	private DAO dao; // Database access object
	private static Map<String, Product> productMap; // Map which allows to gather product info in O(1)

	// Private constructor
	private ProductManager() {
		dao = new DAO(); //Database access object
		productMap = new LinkedHashMap<String, Product>(); //We use linkedHashMap to maintain insertion order
		dao.fillInfo(productMap);
	}

	// Singleton creation pattern, again, might look like seminar codes
	public static ProductManager getInstance() {
		if (manager == null) {
			synchronized (ProductManager.class) {
				manager = new ProductManager();
			}
		}
		return manager;
	}

	// Returns an iterator to products map
	public Iterator<Entry<String, Product>> getProducts() {
		return productMap.entrySet().iterator();
	}

	// Gets a single product
	public Product getProduct(String id) {
		return productMap.get(id);
	}

}
