/*
* BuddyChallenge - Top K words in a String
* @Author - Sarath 
*/

import java.io.*;
import java.util.*;

public class TopK
{
	private static Map<String, Integer> map;
	public static void main(String args[])
	{
		BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		String input = null;
		int k = 0;
		try
		{
		input = br.readLine();
		// Enter K value - Top K words in the string
		k = Integer.parseInt(br.readLine());
		}catch (IOException e)
		{
			e.printStackTrace();
		}
		String[] words = input.split("\\s+");
		map = new HashMap<String, Integer>();
		for (String s : words)
		{
			if(map.get(s) == null)
				map.put(s,1) ;
			else
				map.put(s,map.get(s)+1);
		}
		
		// Sort the map based on Values
		ArrayList<Map.Entry<String,Integer>> list = sortByValues();
		for(Map.Entry<String, Integer> entry : list)
		{
			if (k>0)
				System.out.println(entry.getKey());
			k--;
		}
	}

	protected static ArrayList<Map.Entry<String, Integer>> sortByValues()
	{
		Comparator<Map.Entry<String, Integer>> byMapValues = new Comparator<Map.Entry<String, Integer>>() {
      	
        		public int compare(Map.Entry<String, Integer> left, Map.Entry<String, Integer> right) {
            			return right.getValue().compareTo(left.getValue());
        		}
    		};

		// Now Create a list of Map Entries and sort
		ArrayList<Map.Entry<String, Integer>> listmap = new ArrayList<Map.Entry<String, Integer>>();
		listmap.addAll(map.entrySet());
		Collections.sort(listmap, byMapValues);
		return listmap;
	}
}
