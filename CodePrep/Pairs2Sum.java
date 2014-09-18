
/*
*	BuddyChallenge- 2Sum problem for ordered and unordered arrays
*/

import java.util.*;

public class Pairs2Sum
{
	public static void main(String args[])
	{
		Scanner sc = new Scanner(System.in);
		int size = sc.nextInt();
		int[] input = new int[size];

		for (int i=0; i< size; i++)
			input[i] = sc.nextInt();

		int tally = sc.nextInt();

		// Unordered array
		// Performance: O(n) - best and average
		//		O(nlogn) - worst
	
		Map<Integer, Boolean> map = new HashMap<Integer, Boolean>();
		for (int i=0; i<size; i++)
			map.put(input[i], true);
	
		System.out.println("Pairs::");
		for (int k =0; k<size; k++)
		{
			if(map.containsKey(tally - input[k]))
				System.out.println(input[k]+ " " + (tally - input[k]));
		}

		// Ordered array - no need of map
		// Performance : O(n) - all cases

		Arrays.sort(input);
		System.out.println("Pairs for sorted array");
		int begin = 0, end = size-1;
		while(begin<end)
		{
			if(input[begin] + input[end] < tally)
				begin++;
			else if(input[begin] + input[end] > tally)
				end--;
			else{
				System.out.println(input[begin] + " " + input[end]);
				begin++;
			}
		}
		
	}
}
