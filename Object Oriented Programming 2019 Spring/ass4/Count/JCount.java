package Count;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

import javafx.scene.control.Label;

public class JCount extends JPanel {

	private static final int HEIGHT = 500;
	private static final int WIDTH = 150;
	private static final int FIELD_SIZE = 10;
	private static final int JCOUNT_NUM = 4;
	private static final int DIVISIBLE = 10000;
	private static final int RIGID_PIXELS = 40;
	private static final int MAX_VALUE_DEFAULT = 1000000000;

	private int maxValue = MAX_VALUE_DEFAULT;
	private JTextField field;
	private JLabel value;
	private JButton start, stop;
	private Thread thread;

	public class Worker extends Thread {

		@Override
		//Worker thread for a JCount class.
		//Runs in a loop and shows label every 10000 iterations.
		//Returns if gets interrupted
		public void run() {
			checkIfNumeric();
			int i = 0;

			while (true) {
				if (isInterrupted() || i > maxValue) break;

				if (i % DIVISIBLE == 0) {
					try {
						Thread.sleep(100);
					} catch (InterruptedException e) {
						break;
					}
					//Updates label in an interval
					updateLabel(i);
				}
				i++;
			}
		}

		//Single we need to update a JLabel, it's run in a seperate Swing thread
		private void updateLabel(int i) {
			SwingUtilities.invokeLater(new Runnable() {

				@Override
				public void run() {
					value.setText(i + "");
				}
			});
		}
	}

	//Constructor for Jcount, inits frame
	public JCount() {
		super();
		setVisible(true);
		setSize(WIDTH, HEIGHT);
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

		initVariables();
	}

	//Initializes variables, thread object and other gui features
	private void initVariables() {
		thread = new Thread(new Worker());

		addTextField();

		value = new JLabel("0");
		add(value);

		addStart();

		addStop();

		add(Box.createRigidArea(new Dimension(0, RIGID_PIXELS)));
	}

	//Adds a textfield with listeners, to check for a change every single time it's clicked
	private void addTextField() {
		field = new JTextField(FIELD_SIZE);
		field.getDocument().addDocumentListener(new DocumentListener() {

			@Override
			public void removeUpdate(DocumentEvent e) {
				checkIfNumeric();
			}

			@Override
			public void insertUpdate(DocumentEvent e) {
				checkIfNumeric();
			}

			@Override
			public void changedUpdate(DocumentEvent e) {
				checkIfNumeric();
			}
		});
		add(field);
	}

	//Adds a start button with an actionlistener
	private void addStart() {
		start = new JButton("Start");
		start.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				//Interrupts a thread if it's alive
				if (thread.isAlive()) thread.interrupt();

				//In any case, runs a new thread and runs it
				thread = new Thread(new Worker());
				thread.start();
			}
		});
		add(start);
	}

	//Adds a stop button with an actionlistener
	private void addStop() {
		stop = new JButton("Stop");
		stop.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				//Same logic here, interrups a thread only if it's alive.
				if (thread.isAlive()) thread.interrupt();
			}
		});
		add(stop);
	}

	//Main method, runs the program in a swing thread
	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				createAndShowGUI();
			}
		});
	}

	//Creates GUI and adds seperate JCounts to the window
	private static void createAndShowGUI() {
		JFrame frame = new JFrame();

		frame.setLayout(new BoxLayout(frame.getContentPane(), BoxLayout.Y_AXIS));
		for (int i = 0; i < JCOUNT_NUM; i++) {
			JCount cur = new JCount();
			frame.add(cur);
		}

		frame.setVisible(true);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.pack();
	}

	//Checks if value enetered in a textfield is an int, if so, parses it
	private void checkIfNumeric() {
		int value = 0;
		try {
			value = Integer.parseInt(field.getText());
		} catch (NumberFormatException e) {
			value = MAX_VALUE_DEFAULT;
		}
		maxValue = value;
	}
}
