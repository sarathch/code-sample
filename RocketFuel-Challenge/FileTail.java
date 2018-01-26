/*  Implementation of Unix/Linux tail command.
 *  Output the part of files as per the input command. 
 *  With more than one FILE, precede each with a header giving the file  name.
 *  @author	sarath
 */

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Scanner;

class TailCommand {

	public HashMap < String, RandomAccessFile > map = null;
	Thread[] monitors = null; // threads that monitor files for updates

	public void tailCommand(String option, ArrayList < String > files, long K,
	boolean getByLines, boolean getByBytes, boolean plus) {
		map = new HashMap < String, RandomAccessFile > ();
		RandomAccessFile file = null;
		monitors = new Thread[files.size()];
		PrintThread pt[] = new PrintThread[files.size()];

		try {
			for (int i = 0; i < files.size(); i++) {
				file = new RandomAccessFile(files.get(i), "r");
				if (files.size() > 1) { // Display head ==> Name if multiple
					// files
					System.out.println("File Name:" + files.get(i));
				}
				if (option.equals("-n")) {
					// "TAIL -n [No. of lines] [FILENAME]"
					// fetch file pointer at the start of 10th line from the eof
					long fd = 0;
					// to get lines
					if (getByLines) {
						if (plus) { // from begin
							long l_count = K;
							while (l_count > 1) {
								file.readLine();
								l_count--;
							}
						} else
						// from end
						fd = fetchFdLines(file, K);
						// Print lines from this file pointer to eof
						fd = printLines(file);
					} // to get bytes
					else if (getByBytes) {
						if (plus) { // from begin
							long b_count = K;
							while (b_count > 1) {
								int curr_byte = file.readByte();
								if (curr_byte == 0xA || curr_byte == 0xD) continue;
								b_count--;
							}
							// Print lines from this file pointer to eof
							fd = printLines(file);
						} else { // from end
							String res = fetchKBytesEnd(file, K);
							System.out.println(res);
						}
					}
					// print last 10 lines and monitor file
				} else if (option.equals("-f")) {
					// "TAIL -f [FILENAME]"
					long fd = fetchFdLines(file, 10);
					// prints last 10 lines
					fd = printLines(file);
					// monitor thread
					pt[i] = new PrintThread(files.get(i), fd);
					monitors[i] = new Thread(pt[i]);
					monitors[i].start();
				}
				map.put(files.get(i), file);
			}
		} catch (FileNotFoundException e) {
			System.out.println("Invalid FileName");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * This is the method to fetch required file pointer of the Kth line from
	 * eof
	 * 
	 * @param file
	 * @param lines
	 *            - K lines from eof
	 * @return file pointer of Kth line
	 */
	public long fetchFdLines(RandomAccessFile file, long lines) {
		int line = 0;
		long fd = 0;
		try {
			long file_sz = file.length() - 1;
			for (fd = file_sz; fd != -1; fd--) {
				file.seek(fd);
				int curr_byte = file.readByte();
				// line end feed track
				if (curr_byte == 0xA) {
					// System.out.println("*****line feed bitch****"+" "+ fd);
					if (line == lines) {
						if (fd == file_sz) {
							continue;
						}
						break;
					}
					line = line + 1;
				} else if (curr_byte == 0xD) {
					// System.out.println("*****carriage return****"+" "+ fd);
					if (line == lines) {
						if (fd == file_sz - 1) {
							continue;
						}
						break;
					}
					line = line + 1;
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return fd;
	}

	/**
	 * This is the method to fetch required K bytes from eof
	 * 
	 * @param file
	 * @param lines
	 *            - K lines from eof
	 * @return file pointer of Kth line
	 * @throws IOException
	 */
	public String fetchKBytesEnd(RandomAccessFile file, long bytes)
	throws IOException {

		long file_sz = file.length() - 1;
		StringBuffer sb = new StringBuffer();
		for (long fd = file_sz; fd != -1; fd--) {
			file.seek(fd);
			int curr_byte = file.readByte();
			if (bytes == 0) { // track K bytes
				break;
			}
			// line end feed track
			if (curr_byte == 0xA) {
				continue;
			} else if (curr_byte == 0xD) {
				continue;
			}
			sb.append((char) curr_byte);
			bytes--;
		}
		return sb.reverse().toString();
	}

	/**
	 * method to print lines till eof
	 * 
	 * @param file
	 * @return file pointer at eof
	 * @throws IOException
	 */
	public long printLines(RandomAccessFile file) throws IOException {
		String line = null;
		while ((line = file.readLine()) != null) {
			System.out.println(line);
		}
		return file.getFilePointer();
	}

	/**
	 * This method is used to stop monitoring on files
	 */

	@SuppressWarnings("deprecation")
	public void stopMonitors() {
		for (int i = 0; i < monitors.length; i++) {
			monitors[i].stop();
		}
	}

	/**
	 * This method is accessed if something left unclosed
	 */

	public void closeEverything() {
		for (RandomAccessFile raf: map.values()) {
			try {
				if (raf != null) raf.close();
			} catch (IOException e) {
				System.out.println(e.getMessage());
				System.exit(12);
			}
		}
	}
}

/**
 * This thread class will monitor the file for updates
 */

class PrintThread implements Runnable {

	private String filename;
	private long fd;

	public PrintThread(String file_name, long fd) {
		this.filename = file_name;
		this.fd = fd;
	}

	public void run() {
		String line = null;

		while (true) {
			RandomAccessFile raf = null;
			try {
				raf = new RandomAccessFile(filename, "r");
				raf.seek(fd);
				while ((line = raf.readLine()) != null) {
					System.out.println(line);
				}
				fd = raf.getFilePointer();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}

			if (line == null) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else System.out.println(line);
			try {
				raf.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
}

/**
 * This class is used to test TailCommand implementation Run : javac
 * FileTail.java java FileTail Inputs: tail -n [+k/k] [file_path] tail -f
 * [file_path] Note: You can give "quit"/"stop -f" to stop monitoring stop -f -
 * stops monitoring quit - quits application
 * 
 * @author sarath
 */

public class FileTail {
	public static void main(String[] args) {
		Scanner sc = new Scanner(System. in );
		TailCommand tc = new TailCommand();
		String option = "";
		boolean lines = false;
		boolean bytes = false;
		boolean plus = false;
		long K = 0;
		while (true) {
			try {
				ArrayList < String > files = new ArrayList < String > ();
				String command = sc.nextLine();
				String com[] = command.split("\\s+");

				if (command.equals("quit")) {
					System.out.println("Application closing...");
					System.exit(0);
				}
				if (command.equals("stop -f")) { // stops monitor thread
					tc.stopMonitors();
					System.out.println("stopped monitoring");
					continue;
				}
				if (!com[0].equals("tail")) {
					System.out.println("Invalid Command");
					continue;
				}

				option = com[1];
				int file_args = 0;
				if (option.equals("-n")) {
					if (com[2].contains("--lines")) { // set lines flag
						com[2] = com[2].substring(8);
						lines = true;
					} else if (com[2].contains("--bytes")) { // set bytes flag
						com[2] = com[2].substring(8);
						bytes = true;
					} else {
						lines = true; // default
					}

					if (com[2].charAt(0) == '+') {
						plus = true;
						com[2] = com[2].substring(1);
					}
					if (bytes) {
						com[2] = parseNumBytes(com[2]); // extract giga, kilo,
						// mega
					}
					K = Long.parseLong(com[2]);
					file_args = 3;
				} else if (option.equals("-f")) {
					K = 10;
					file_args = 2;
				} else System.out.println("This Option is out of scope of this project");

				for (int k = file_args; k < com.length; k++) {
					files.add(com[k]);
				}

				tc.tailCommand(option, files, K, lines, bytes, plus);
				tc.closeEverything();

			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	public static String parseNumBytes(String str) {
		str = str.toLowerCase();
		if (str.contains("g") || str.contains("m") || str.contains("k")) { // check
			// for
			// giga,
			// mega,
			// kilo
			long val = Long.parseLong(str.substring(0, str.length() - 1));
			if (str.contains("g")) {
				val *= 1000000000;
			} else if (str.contains("m")) {
				val *= 1000000;
			} else if (str.contains("k")) {
				val *= 1000;
			}
			return String.valueOf(val);
		}
		return str;
	}
}
