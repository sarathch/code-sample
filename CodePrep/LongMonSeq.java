

import java.util.*;

public class LongMonSeq
{
	public static void main(String args[])
	{
		int[] arr = { 1,2,3,15,4,13,14,16,17};
		System.out.println("Max Sequence");
		int[] table= LMS(arr, arr.length);
		int max = Integer.MIN_VALUE;
		for (int i =0; i<table.length; i++)
			if(max < table[i])
				max = table[i];
		int[] list = new int[max];
		for (int p : table)
			System.out.print(p+" ");
		System.out.println();
		for(int i=0; i< list.length;i++)
		{
			for (int j =0; j<table.length;j++)
			{
				if(table[j] == i+1)
					list[i] = arr[j];
			}
		}

		for (Integer i: list)
			System.out.print(i+" ");
		System.out.println();
	}

	public static int[] LMS(int[] arr, int n)
	{	
		int[] track = new int[n];
		Arrays.fill(track,1);	

		for (int i = 1; i< n; i++)
		{
			for (int j =0; j<i; j++)
			{
				if(arr[j] < arr[i] && track[i] <= track[j])
					track[i] = 1 + track[j];
			}
		}	
		return track;
	}
}
