package WebLoader;

import java.io.File; 

import javax.swing.SwingUtilities;

public class WebMain {

	//Main method, runs the program
	public static void main(String[] args) {
		File file = new File("links.txt");
		SwingUtilities.invokeLater(new Runnable() {

			@Override
			public void run() {
				new WebFrame(file);
			}
		});
	}
}
