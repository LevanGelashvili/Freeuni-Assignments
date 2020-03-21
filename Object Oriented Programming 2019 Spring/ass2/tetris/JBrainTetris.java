package tetris;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Random;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.UIManager;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import tetris.Brain.Move;

public class JBrainTetris extends JTetris {

	private JCheckBox brainBox, animateBox;
	private JSlider adversary;
	private JLabel ok;
	private Brain brain;
	private Random rand;

	//Constructor for a subclass, initializes variables
	JBrainTetris(int pixels) {
		super(pixels);
		brain = new DefaultBrain();
		brainBox = new JCheckBox();
		animateBox = new JCheckBox();
		rand = new Random();
	}

	//Main method to run tetris
	public static void main(String[] args) {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception ignored) {

		}
		JTetris tetris = new JBrainTetris(16);
		JFrame frame = JTetris.createFrame(tetris);
		frame.setVisible(true);
	}

	@Override
	//Creates panels and adds checkboxes to it
	public JComponent createControlPanel() {
		JComponent panel = super.createControlPanel();
		addCheckBox(panel, brainBox, "Brain :", "Brain Passive", "Brain Active", false);
		addCheckBox(panel, animateBox, "Animate Falling :", "Animate Active", "Animate Passive", true);
		addAdversary(panel);
		return panel;
	}

	@Override
	//Makes a tick move, but with a brain
	public void tick(int verb) {
		Brain.Move move = null;
		//If brain needs to make a move, makes adjustments to a piece
		if (brainBox.isSelected() && verb == DOWN) {
			board.undo();
			move = brain.bestMove(board, currentPiece, board.getHeight(), move);
			if (move != null) applyChanges(move,verb);
		}
		super.tick(verb);
	}

	//Makes changes to a piece, changing it's x and y and rotation if needed
	private void applyChanges(Move move, int verb) {
		if (move.x > currentX) tick(RIGHT);
		if (move.x < currentX) tick(LEFT);
		if (!move.piece.equals(currentPiece)) tick(ROTATE);
		else {
			//Drops the piece down immediately
			if (!animateBox.isSelected()) tick(DROP);
		}
	}

	//Method to add a label, checkbox and add a listener to it.
	//Flag determines the initial state of a checkbox.
	private void addCheckBox(JComponent panel, JCheckBox box, String desc, String orig, String change, boolean flag) {
		panel.add(new JLabel(desc));
		box.setText(orig);
		box.setSelected(flag);
		box.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent a) {
				box.setText(box.isSelected() ? change : orig);
			}
		});
		panel.add(box);
	}

	//Adds a slider to window
	private void addAdversary(JComponent panel) {
		panel.add(new JLabel("Adversary : "));
		adversary = new JSlider(0, 100, 0);
		adversary.setPreferredSize(new Dimension(100, 15));
		panel.add(adversary);
		ok = new JLabel("OK");
		panel.add(ok);
	}

	@Override
	//Chooses next piece for tetris, does usually if randomly generater number is 
	//smaller than value of a slider
	public Piece pickNextPiece() {
		int randomInt = rand.nextInt(101);
		if (randomInt >= adversary.getValue()) {
			ok.setText("OK");
			return super.pickNextPiece();
		}
		ok.setText("*OK*");
		return lowestScore();
	}

	//Else, chooses the worst scoring piece and returns it, makes the worst desicion
	private Piece lowestScore() {
		double curScore = Integer.MIN_VALUE;
		Piece worst = null;
		Brain.Move curMove = new Brain.Move();
		for (Piece p : super.pieces) {
			curMove = brain.bestMove(board, p, board.getHeight(), curMove);
			if (curMove == null) return p;
			//If this is the worst piece, save it's info
			if (curMove.score > curScore) {
				worst = p;
				curScore = curMove.score;
			}
		}
		return worst;
	}
}
