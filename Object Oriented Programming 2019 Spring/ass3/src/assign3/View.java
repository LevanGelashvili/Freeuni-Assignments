package assign3;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Observable;
import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JTable;
import javax.swing.JTextField;

public class View extends Observable {

	JTable table;
	JLabel metroLabel, contLabel, popLabel;
	JTextField metroField, contField, popField;
	JButton search, add;
	JComboBox<String> popBox, nameBox;
	JFrame frame;

	/**
	 * Creates a new view.
	 * @param model : Model passed from main
	 * View does not care about insides of the model, they are not couped.
	 * Model just fires changes so table redraws itself.
	 */
	public View(Model model) {
		frame = new JFrame();
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setLayout(new BorderLayout(4, 4));
		frame.setVisible(true);
		frame.setSize(Constants.FRAME_WIDTH, Constants.FRAME_HEIGHT);

		initNorth();
		initEast();

		table = new JTable(model);
		frame.add(table, BorderLayout.CENTER);

		frame.pack();
	}

	//Initialises East border, Creates buttons and comboBoxes.
	private void initEast() {
		Box box = Box.createVerticalBox();
		addButtons(box);
		addComboBoxes(box);
		frame.add(box, BorderLayout.EAST);
	}

	//Creates comboBoxes, popBox has population criterias, nameBox - match criterias.
	private void addComboBoxes(Box box) {
		box.add(new JLabel("Search Options"));

		popBox = new JComboBox<String>();
		popBox.addItem(Constants.POP_LARGER);
		popBox.addItem(Constants.POP_SMALLER);
		box.add(popBox);

		nameBox = new JComboBox<String>();
		nameBox.addItem(Constants.MATCH_EXACT);
		nameBox.addItem(Constants.MATH_PARTIAL);
		box.add(nameBox);
	}

	//Adds buttons
	private void addButtons(Box box) {
		search = new JButton("Search");
		search.addActionListener(new ActionListener() {

			@Override
			//When clicked, notifies controller to search for queries
			public void actionPerformed(ActionEvent arg0) {
				setChanged();
				notifyObservers(generateRequest(false));
			}
		});
		add = new JButton("Add");
		add.addActionListener(new ActionListener() {

			@Override
			//When clicked, notifies controller to add data to base
			public void actionPerformed(ActionEvent e) {
				setChanged();
				notifyObservers(generateRequest(true));
			}
		});
		box.add(add);
		box.add(search);
	}

	/**
	 * @param shouldInsert : Should user insert data or not
	 * @return new Request with user input.
	 */
	private Request generateRequest(boolean shouldInsert) {
		boolean isPopLarger = (popBox.getSelectedItem().equals(Constants.POP_LARGER));
		boolean isExact = (nameBox.getSelectedItem().equals(Constants.MATCH_EXACT));
		return new Request(isPopLarger, isExact, generateMetropolis(), shouldInsert);
	}

	//Returns a new metropolis from used enter texts
	private Metropolis generateMetropolis() {
		return new Metropolis(metroField.getText(), contField.getText(), generatePopulationNumber());
	}

	//If population field is empty, returns -1, else parses it and returns in integer form.
	private int generatePopulationNumber() {
		if (popField.getText().equals("")) return -1;
		return Integer.parseInt(popField.getText());
	}

	//Initializes northern border, adds labels and textfields.
	private void initNorth() {
		Box box = Box.createHorizontalBox();

		metroLabel = new JLabel("Metropolis : ");
		metroField = new JTextField(Constants.FIELD_SIZE);
		box.add(metroLabel);
		box.add(metroField);

		contLabel = new JLabel("Continent : ");
		contField = new JTextField(Constants.FIELD_SIZE);
		box.add(contLabel);
		box.add(contField);

		popLabel = new JLabel("Population : ");
		popField = new JTextField(Constants.FIELD_SIZE);
		box.add(popLabel);
		box.add(popField);

		frame.add(box, BorderLayout.NORTH);
	}
}
