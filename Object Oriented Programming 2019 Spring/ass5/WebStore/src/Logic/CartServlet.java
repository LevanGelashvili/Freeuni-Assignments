package Logic;

import java.io.IOException;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.Map;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet implementation class CartServlet
 */
@WebServlet("/CartServlet")
public class CartServlet extends HttpServlet {
	private static final long serialVersionUID = 1L;

	/**
	 * @see HttpServlet#HttpServlet()
	 */
	public CartServlet() {
		super();
	}

	/**
	 * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	protected void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		response.getWriter().append("Served at: ").append(request.getContextPath());
	}

	/**
	 * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse
	 *      response)
	 */
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		ShoppingCart cart = (ShoppingCart) request.getSession().getAttribute(Constants.CART_ATTR);
		ProductManager manager = (ProductManager) request.getServletContext().getAttribute(Constants.MANAGER_ATTR);

		//If cart is null, then we create a new one which will remain allocated
		if (cart == null) {
			cart = new ShoppingCart();
			request.getSession().setAttribute(Constants.CART_ATTR, cart);
		}

		//If main key is not null, then user has decided to buy, so we buy one product 
		if (request.getParameter(Constants.ID_ATTR) != null) {
			cart.buyProduct(manager.getProduct(request.getParameter(Constants.ID_ATTR)));
		} else { 
			//Else, user has clicked update button and we have to update every single key and value
			//We find those keys and values in request map, passed from jsp files' inputs
			for (String id : request.getParameterMap().keySet()) {
				int value = Integer.parseInt(request.getParameter(id));
				cart.updateQuantities(manager.getProduct(id), value);
			}
		}

		request.getRequestDispatcher(Constants.CART).forward(request, response);
	}

}
