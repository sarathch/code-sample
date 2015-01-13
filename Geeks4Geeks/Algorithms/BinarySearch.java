

import java.util.*;

public class BinarySearch
{
	public static void main(String...args)
	{
		Scanner sc = new Scanner(System.in);
		ArrayList<Integer> input = new ArrayList<Integer>();
		int ele = sc.nextInt();
		while(ele!= 999)
		{
			input.add(ele);
			ele = sc.nextInt();
		}
		Collections.sort(input);
		int find = sc.nextInt();
		System.out.println("ElementFound? = "+binarySearch(input,find,0,input.size()-1));
	}

	public static boolean binarySearch(ArrayList<Integer> input, int find, int start, int end)
	{
		if(start>=end)
			return false;

		int mid = (start+end)/2;
		if(input.get(mid) == find)
			return true;
		else if(input.get(mid) > find)
			return binarySearch(input, find, start, mid-1);
		return binarySearch(input, find, mid+1, end);
	}
}
