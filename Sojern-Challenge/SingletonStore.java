package rest.chandu.jersey.web;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.HashMap;
import java.util.Map;

/*
 * Storage Service 
 */

public class SingletonStore {

	//private static SymbolTable st;
	private static Map<String, String> st;
	static String f = "BackUP.txt";
	static File file = new File(f);
	static int count =0;
	
    public SingletonStore() {

    }

    /*
     * returns only instance of hash map
     */
    public static Map<String, String> getInstance()
    {
    	if (st == null)
        {
        	st = new HashMap<String, String>();
        	readFile();
        }
        return st;
    }
    
    /*
     * Insert backed up contents in file into Hash Table  
     */
    
    public static void readFile()
    {
		BufferedReader br;
		try {
			 if (!file.exists()) {
				 file.createNewFile();
			 }
			br = new BufferedReader(new FileReader(file));
			
			String line;
			while ((line = br.readLine()) != null) {
				// process the line.
				String[] tokens = line.split(" : ");
				System.out.println("file"+tokens.length);
				st.put(tokens[0], tokens[1]);
				/*count++;*/
			}
			br.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
    }
}

