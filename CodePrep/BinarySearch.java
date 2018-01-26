/*
*	BuddyChallenge - Binary Search
*	@Author - Sarath
*/

import java.util.Arrays;

public class BinarySearch
{
	public static void main(String[] args)
	{
		int[] input = {5,4,2,6,8,1,3,10,20,15,13,7};
		int element = 7;
		int start = 0, end = input.length-1;
		Arrays.sort(input);
		if(BinarySearch(input, element, start, end))
			System.out.println("Found");
		else
			System.out.println("Not Found");
	}

	protected static boolean BinarySearch(int[] inp, int e, int start, int end)
	{
		if(end < start)
			return false;
		int mid = start+((end - start) /2);
		System.out.println(inp[mid]);
		if(inp[mid] == e)
			return true;
		else if(inp[mid] < e)
			return(BinarySearch(inp,e, mid+1, end));
		else
			return(BinarySearch(inp,e,start, mid-1));	
	}
}
