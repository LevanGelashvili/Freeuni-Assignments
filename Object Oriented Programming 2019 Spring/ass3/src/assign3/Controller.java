package assign3;

import java.util.Observable;
import java.util.Observer;

public class Controller implements Observer {

	private Model model;

	// Creates a new controller, with a passed model from main.
	public Controller(Model model) {
		this.model = model;
	}

	@Override
	/**
	 * Standart observer pattern. When buttons are clicked in view, view notifies
	 * its observer, controller in this case. Depending on a state of request (explained in Request itself)
	 * controller decides whether to insert data into base, or search for queries.
	 */
	public void update(Observable o, Object returnedData) {
		Request request = (Request) returnedData;
		if (request.shouldInsertData()) {
			model.insertMetropolis(request);
		} else {
			model.getInfo(request);
		}
	}
}
