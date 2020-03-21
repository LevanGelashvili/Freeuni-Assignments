package WebLoader;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.table.DefaultTableModel;

import com.sun.xml.internal.bind.v2.schemagen.xmlschema.List;

public class WebFrame extends JFrame {

	private static final int WIDTH = 600;
	private static final int HEIGHT = 300;
	private static final int DEFAULT_THREADS = 4;
	private static final int STATUS_ROW = 1;

	private JTable table;
	private JButton single, concurrent, stop;
	private JTextField field;
	private JLabel runningLabel, completedLabel, elapsedLabel;
	private JProgressBar bar;
	private DefaultTableModel model;
	private WebLauncher launcher;

	private long start;
	private int runningThreads, completedThreads;

	public class WebLauncher extends Thread {

		private int numThreads; // Maximum number of threads running
		private ArrayList<WebWorker> workers; // List of workers

		public WebLauncher(int numThreads) {
			this.numThreads = numThreads;
		}

		// Run method of thread
		public void run() {
			// Lock ensures that no more than numThreads threads run at the same time
			Semaphore lock = new Semaphore(numThreads);

			// Creates a new latch to wait for every downloaded url
			CountDownLatch latch = new CountDownLatch(model.getRowCount());

			workers = new ArrayList<WebWorker>();
			for (int i = 0; i < model.getRowCount(); i++) {
				// Adds a new worker object to the list
				workers.add(new WebWorker((String) model.getValueAt(i, 0), i, WebFrame.this, lock, latch));
			}

			start = System.currentTimeMillis();
			incrementRunning();

			for (WebWorker w : workers) {
				// For every single worker, locks the semaphore and runs it
				try {
					lock.acquire();
					w.start();
					// If worker gets interrupted, interrupts all threads
					if (isInterrupted()) throw new InterruptedException();
				} catch (InterruptedException e) {
					interruptThreads();
					finishLauncher(latch);
					return;
				}
			}
			finishLauncher(latch);
		}

		// Interrupts all threads
		private void interruptThreads() {
			for (WebWorker worker : workers) {
				worker.interrupt();
			}
			interrupt();
		}

		// Method for decomposition, changes buttons after latch is notified to run
		private void finishLauncher(CountDownLatch latch) {
			decrementRunning();
			try {
				latch.await();
				changeButtons();
			} catch (InterruptedException e) {
				
			}
		}
	}

	// Creates a new webframe object (which extends JFrame)
	public WebFrame(File file) {
		JPanel panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));

		runningThreads = completedThreads = 0;

		initTable(panel);
		readFromFile(file);
		initInteractibles(panel);

		setVisible(true);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		add(panel);
		pack();
	}

	// Reads info from files, and stores those urls in a table model
	private void readFromFile(File file) {
		try {
			BufferedReader bf = new BufferedReader(new FileReader(file));

			while (true) {
				String line = bf.readLine();
				if (line == null) break;
				model.addRow(new String[] { line, "" });
			}

			bf.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	// Initializes variables
	private void initInteractibles(JPanel panel) {
		initButtons(panel);

		field = new JTextField();
		field.setText(Integer.toString(DEFAULT_THREADS)); // Chose 4 as default number of threads
		panel.add(field);

		initJLabels(panel);

		// Creates and initializes new progress bar, counting number of threads finished
		bar = new JProgressBar(0, model.getRowCount());
		bar.setValue(0);
		bar.setStringPainted(true);
		panel.add(bar);

		// Initializes stop button with its actionlistener
		stop = new JButton("Stop");
		stop.setEnabled(false);
		stop.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				changeButtons();
				// Interrups threads if launcher is active
				if (launcher != null) {
					launcher.interruptThreads();
				}
			}
		});
		panel.add(stop);
	}

	// Inits other buttons
	private void initButtons(JPanel panel) {
		single = new JButton("Single Thread Fetch");
		single.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				// Single button fires only one thread of webworkers
				changeButtons();
				reset();
				launcher = new WebLauncher(1);
				launcher.start();
			}
		});
		panel.add(single);

		concurrent = new JButton("Concurrent Fetch");
		concurrent.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent arg0) {
				changeButtons();
				reset();
				// Same logic here, but reads number from field
				launcher = new WebLauncher(parseIntFromField());
				launcher.start();
			}
		});
		panel.add(concurrent);
	}

	// Adds Jlabels to GUI
	private void initJLabels(JPanel panel) {
		runningLabel = new JLabel("Running : " + runningThreads);
		completedLabel = new JLabel("Completed : " + completedThreads);
		elapsedLabel = new JLabel("Elapsed : " + start);
		panel.add(runningLabel);
		panel.add(completedLabel);
		panel.add(elapsedLabel);
	}

	// Initializes table and its model
	private void initTable(JPanel panel) {
		model = new DefaultTableModel(new String[] { "URL", "Status" }, 0);
		table = new JTable(model);
		table.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
		JScrollPane scrollpane = new JScrollPane(table);
		scrollpane.setPreferredSize(new Dimension(WIDTH, HEIGHT));
		panel.add(scrollpane);
	}

	// Changes buttons to its reverse state
	private void changeButtons() {
		stop.setEnabled(!stop.isEnabled());
		single.setEnabled(!single.isEnabled());
		concurrent.setEnabled(!concurrent.isEnabled());
	}

	// Reads an integer from field if possible, else returns default value
	private int parseIntFromField() {
		int value = DEFAULT_THREADS;
		try {
			value = Integer.parseInt(field.getText());
		} catch (Exception e) {
			return DEFAULT_THREADS;
		}
		return value;
	}

	// Resets the gui for another download session
	private void reset() {
		bar.setValue(0);
		completedThreads = runningThreads = 0;
		elapsedLabel.setText("Elapsed : " + 0);
		for (int i = 0; i < model.getRowCount(); i++) {
			model.setValueAt("", i, STATUS_ROW);
		}
	}

	// Returns model
	public DefaultTableModel getModel() {
		return model;
	}

	// Increments number of running threads, only one thread can access this as
	// integer should be incremented atomically, runs jlabel painting in swing
	// thread
	public synchronized void incrementRunning() {
		SwingUtilities.invokeLater(new Runnable() {

			@Override
			public void run() {
				runningLabel.setText("Running : " + ++runningThreads);
			}
		});
	}

	// Same as above, but decrements number of threads and increments number of
	// completed ones, also, sets the time for elapsed label
	public synchronized void decrementRunning() {
		SwingUtilities.invokeLater(new Runnable() {

			@Override
			public void run() {
				runningLabel.setText("Running : " + --runningThreads);
				elapsedLabel.setText("Elapsed : " + TimeUnit.MILLISECONDS.toSeconds(System.currentTimeMillis() - start)
						+ "." + TimeUnit.MILLISECONDS.toMillis(System.currentTimeMillis() - start));
				completedLabel.setText("Completed : " + ++completedThreads);
				bar.setValue(completedThreads);
			}
		});
	}
}
