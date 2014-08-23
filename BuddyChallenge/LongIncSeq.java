
/*
*   BuddyChallenge - Longest Increasing Sequence
*   @Author - Sarath	
*/

import java.util.Scanner;
import java.util.ArrayList;

public class LongIncSeq
{
	private static int max=1;

	public static void main(String args[])
	{
		Scanner sc = new Scanner(System.in);
		
		int size = sc.nextInt();
		int[] input = new int[size];
		
		for (int i=0; i<size;i++)
			input[i] = sc.nextInt();
			
		arr= new ArrayList<Integer>();
		// Find LIS till every index
		int last= LIS(input, size);
		System.out.println("Longest Sequence Length::"+max);	
	}

	protected static int LIS(int[] input, int n)
	{
		if(n==1)
			return 1;

		int max_here = 1;

		for (int i=1; i<n; i++)
		{
			int res = LIS(input, i);
			if(input[i-1] < input[n-1] && max_here < res+1)
				max_here = res+1;
		}

		if(max_here > max)
			max = max_here;
		
		return max_here;
	}
}
