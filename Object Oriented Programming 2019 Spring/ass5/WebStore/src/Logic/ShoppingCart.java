package Logic;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Map.Entry;

public class ShoppingCart {

	private double totalPrice; // Total price of products
	private Map<Product, Integer> products; // Map containing products and their respective quantities

	public ShoppingCart() {
		totalPrice = 0;
		products = new LinkedHashMap<Product, Integer>();
	}

	// Returns total price
	public double getTotalPrice() {
		return totalPrice;
	}

	// Returns an iterator to products map
	public Iterator<Entry<Product, Integer>> getProducts() {
		return products.entrySet().iterator();
	}

	// Adds a new product to a cart, or increases its count by 1
	public void buyProduct(Product product) {
		if (products.containsKey(product)) {
			products.put(product, products.get(product) + 1);
		} else {
			products.put(product, 1);
		}
		totalPrice += product.getPrice();
	}

	// Updates number of quantities in the cart
	public void updateQuantities(Product product, int value) {
		totalPrice += product.getPrice() * (value - products.get(product));
		if (value == 0) {
			products.remove(product);
			if(products.isEmpty()) totalPrice = 0;
		}
		else products.put(product, value);
	}
}
