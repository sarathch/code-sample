/*
*	In place Quicksort
* 	@Author Sarath
*/

import java.io.*;
import java.util.*;

public class QuickSort
{
	public static void main(String args[])
	{
		new QuickSort().run();
	}

	public static void run()
	{
		int[] input = new int[20];

		
		for ( int i =0; i<= 19; i++)
		{
			input[i] = (int)(Math.random() * 100);
		}

		//Display Input matrix

		for (int i=0; i<=19;i++)
			System.out.print(input[i] + "\t");
		System.out.println();

		qSort(input, 0, 19);
		System.out.println("After Sort");
		for (int i=0; i<=19;i++)
                        System.out.print(input[i] + "\t");
                System.out.println();

	}

	public static void qSort(int[] inp, int start, int end)
	{
	    try  
            {  
		if (end > start)
		{
			int r = partition(inp, start, end);
			qSort(inp, start, r-1);
			qSort(inp, r+1, end);
			
		}
	    }catch(Exception e)
		{
		}
	}

	public static int partition( int[] inp, int start, int end)
	{
		int pivot = inp[start];
		int i = start;
		for (int j= start+1; j<= end; j++)
		{
			if(inp[j] <= pivot)
			{
				i++;
				int temp = inp[i];
				inp[i] = inp[j];
				inp[j] = temp;
			}
		}

		int temp = inp[start];
		inp[start] = inp[i];
		inp[i] = temp;
		return i;
	}

}
