package Logic;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;
import javax.servlet.annotation.WebListener;
import javax.servlet.http.HttpSessionEvent;
import javax.servlet.http.HttpSessionListener;

import org.apache.catalina.SessionEvent;
import org.apache.catalina.SessionListener;

/**
 * Application Lifecycle Listener implementation class Listener
 *
 */
@WebListener
public class Listener implements ServletContextListener, HttpSessionListener {

	/**
	 * Default constructor.
	 */
	public Listener() {
		
	}

	/**
	 * @see ServletContextListener#contextDestroyed(ServletContextEvent)
	 */
	public void contextDestroyed(ServletContextEvent e) {

	}

	/**
	 * @see ServletContextListener#contextInitialized(ServletContextEvent)
	 */
	public void contextInitialized(ServletContextEvent e) {
		ProductManager manager = ProductManager.getInstance();
		e.getServletContext().setAttribute(Constants.MANAGER_ATTR, manager);
	}

	@Override
	public void sessionCreated(HttpSessionEvent e) {
		ShoppingCart cart = new ShoppingCart();
		e.getSession().setAttribute(Constants.CART_ATTR, cart);
	}

	@Override
	public void sessionDestroyed(HttpSessionEvent e) {
		
	}

}
