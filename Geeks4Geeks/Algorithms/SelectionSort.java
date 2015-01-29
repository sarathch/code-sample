/*
 * Implementing simple selection sort algorithm
 *
 * @Author: sarath
 *
 */

import java.util.*;

public class SelectionSort
{
	public static void main(String...args)
	{
		Scanner sc = new Scanner(System.in);
		int[] inp = new int[10];
		for(int i=0; i<10; i++)
			inp[i] = sc.nextInt();

		System.out.println("Before Sorting");
		printArray(inp);

		selectionSort(inp);

		printArray(inp);
	}

	public static void selectionSort(int[] arr)
	{
		for(int i=0;i<arr.length;i++)
		{
			int min_pos= i;
			for(int j= i; j<arr.length; j++)
			{
				if(arr[j] < arr[min_pos])
				{
					min_pos = j;
				}
			}
			int temp = arr[i];
			arr[i] = arr[min_pos];
			arr[min_pos] = temp;
		}
	}

	public static void printArray(int[] arr)
	{
		for(int p : arr)
			System.out.print(p + " ");
		System.out.println();
	}
}



