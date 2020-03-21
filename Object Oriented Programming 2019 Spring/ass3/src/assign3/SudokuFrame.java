package assign3;

import javax.swing.*;
import javax.swing.border.TitledBorder;
import javax.swing.event.*;

import java.awt.*;
import java.awt.event.*;

public class SudokuFrame extends JFrame {

	private static final int ROW_SIZE = 15;
	private static final int COL_SIZE = 20;
	private static final int WIDTH = 800;
	private static final int HEIGHT = 700;

	private JTextArea puzzle, solution;
	private JButton check;
	private JCheckBox box;

	// Creates new frame, sets sizes and layout
	public SudokuFrame() {
		super("Sudoku Solver");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		setVisible(true);
		setLayout(new BorderLayout(4, 4));
		setSize(WIDTH, HEIGHT);

		addTextAreas();
		addButtons();

		pack();
	}

	// Adds buttons and other Swing items
	private void addButtons() {
		check = new JButton("Check");
		check.addActionListener(new ActionListener() {

			@Override
			public void actionPerformed(ActionEvent e) {
				// Creates new sudoku when this button is clicked
				createSudoku();
			}
		});

		box = new JCheckBox("Auto Check", true);

		JPanel panel = new JPanel();
		panel.add(check);
		panel.add(box);
		add(panel, BorderLayout.SOUTH);
	}

	// Creates new sudoku and tries to solve it, if text is passed correctly, else
	// throws exception
	private void createSudoku() {
		Sudoku sudoku = null;

		try {
			sudoku = new Sudoku(Sudoku.textToGrid(puzzle.getText()));
		} catch (Exception e) {
			solution.setText("Parsing Problem");
			return;
		}

		int count = sudoku.solve();
		if (count > 0) {
			System.out.println(sudoku);
			solution.setText(sudoku.getSolutionText());
			solution.append("Solutions : " + count + "\n");
			solution.append("Elapsed : " + sudoku.getElapsed());
		}
	}

	// Adds text areas to the frame. If puzzle area is changed while auto update is
	// selected, craetes a sudoku immediately
	private void addTextAreas() {
		puzzle = new JTextArea(ROW_SIZE, COL_SIZE);
		puzzle.setBorder(new TitledBorder("Puzzle"));
		puzzle.getDocument().addDocumentListener(new DocumentListener() {

			@Override
			public void removeUpdate(DocumentEvent e) {
				if (box.isSelected()) createSudoku();
			}

			@Override
			public void insertUpdate(DocumentEvent e) {
				if (box.isSelected()) createSudoku();
			}

			@Override
			public void changedUpdate(DocumentEvent e) {
				if (box.isSelected()) createSudoku();
			}

		});
		add(puzzle, BorderLayout.CENTER);

		solution = new JTextArea(ROW_SIZE, COL_SIZE);
		solution.setBorder(new TitledBorder("Solution"));
		add(solution, BorderLayout.EAST);
	}

	//Start the app
	public static void main(String[] args) {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception ignored) {

		}
		SudokuFrame frame = new SudokuFrame();
	}
}
