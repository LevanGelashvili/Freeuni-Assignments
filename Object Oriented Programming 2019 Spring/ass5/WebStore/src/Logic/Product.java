package Logic;

//Class encapsulating a single product item
public class Product {

	private String id, name, image;
	private double price;

	// Constructor, taking 4 parameters
	public Product(String id, String name, String image, double price) {
		this.id = id;
		this.name = name;
		this.image = image;
		this.price = price;
	}

	// Returns id
	public String getId() {
		return id;
	}

	// Returns name
	public String getName() {
		return name;
	}

	// Returns image path
	public String getImage() {
		return image;
	}

	// Returns price of item
	public double getPrice() {
		return price;
	}
	
	public String toString() {
		return id + " " + name + " " + image + " " + price;
	}
}
