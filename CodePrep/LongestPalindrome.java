
/*
* BuddyChallenge - Longest Palindrome
* @Author - Sarath
*/

import java.util.Scanner;

public class LongestPalindrome
{
	public static void main(String args[])
	{	
		Scanner sc = new Scanner(System.in);
		String input = sc.next();
		
		char[] inp = input.toCharArray();
		
		int max =0, start=0, end=0;
		for(int i=0; i<inp.length-1; i++)
		{
			for(int j=i+1; j<inp.length; j++)
			{
				if(inp[i] == inp[j])
				{
					int size = isPalendrome(inp, i, j);
					if(size > max)
					{
						max = size;
						end = j;
						start = i;
					}
				}
			}
		}

		for (int p = start; p<=end; p++)
			System.out.print(inp[p] + " ");
		System.out.println();
	}

	protected static int isPalendrome(char[] inp, int start, int end)
	{
		int index = (end - start)/2 +1;
		int j = end-1;
		for (int i=start+1; i< index; i++)
		{
			if(inp[i] != inp[j])
				return -1;
			j--;
		}
		return end - start;
	}
}
