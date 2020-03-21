package WebLoader;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Semaphore;

public class WebWorker extends Thread {

	private String urlString;
	private int row;
	private WebFrame frame;
	private Semaphore lock;
	private CountDownLatch latch;
	private static final int STATUS_ROW = 1;

	// Creates a webworker object, saved passed variables
	public WebWorker(String urlString, int row, WebFrame frame, Semaphore lock, CountDownLatch latch) {
		this.urlString = urlString;
		this.row = row;
		this.frame = frame;
		this.lock = lock;
		this.latch = latch;
	}

	// Runs the thread
	public void run() {
		frame.incrementRunning(); // Increments number of running threads
		download(); // Downloads the data
		frame.decrementRunning(); // Decreases number of running threads
		lock.release(); // Releases the lock of max number of current threads
		latch.countDown(); // Decreases latch state by one
	}

	// Sets text in model's status row
	private void setText(String str) {
		frame.getModel().setValueAt(str, row, STATUS_ROW);
	}

	// Downloads data from urls, provided method, only added few variables myself
	private void download() {
		InputStream input = null;
		StringBuilder contents = null;

		long start = System.currentTimeMillis(); // Start time of run
		SimpleDateFormat formatter = new SimpleDateFormat("HH:mm:ss");
		int bytes = 0; // Number of chars downloaded (Explained in word file)

		try {
			URL url = new URL(urlString);
			URLConnection connection = url.openConnection();

			// Set connect() to throw an IOException
			// if connection does not succeed in this many msecs.

			connection.setConnectTimeout(5000);
			connection.connect();
			input = connection.getInputStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(input));

			char[] array = new char[1000];
			int len;
			contents = new StringBuilder(1000);

			while ((len = reader.read(array, 0, array.length)) > 0) {
				contents.append(array, 0, len);
				bytes += len;
				Thread.sleep(100);
			}

			// If Successful :
			String res = formatter.format(new Date());
			res += "     " + Long.toString((System.currentTimeMillis() - start)) + " ms     ";
			res += bytes + " bytes";
			setText(res);
		}
		// Otherwise control jumps to a catch...
		catch (MalformedURLException ignored) {
			setText("Error");
		} catch (InterruptedException exception) {
			setText("Interrupted");
		} catch (IOException ignored) {
			setText("Error");
		}
		// "finally" clause, to close the input stream
		// in any case
		finally {
			try {
				if (input != null) input.close();
			} catch (IOException ignored) {

			}
		}
	}
}