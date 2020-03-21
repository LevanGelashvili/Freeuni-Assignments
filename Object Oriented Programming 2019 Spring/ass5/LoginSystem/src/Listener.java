import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;
import javax.servlet.annotation.WebListener;

/**
 * Application Lifecycle Listener implementation class Listener
 *
 */
@WebListener
public class Listener implements ServletContextListener {

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
		AccountManager manager = AccountManager.getInstance();
		e.getServletContext().setAttribute(Constants.ACCOUNT_MANAGER, manager);
	}

}
