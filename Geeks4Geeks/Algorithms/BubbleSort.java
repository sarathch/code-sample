
/**
 * Implemented simple bubble/sinking sort
 *
 * @author: sarath
 */

public class BubbleSort
{
	public static void main(String...args)
	{
		int[] arr = {10,2,3,5,7,10,1,9,3};

		for(int i=0; i<arr.length; i++)
			for(int j=0;j<arr.length-1;j++)
				if(arr[j] > arr[j+1])
					swap(arr, j, j+1);

		for(int p : arr)
			System.out.print(p + " ");
	}

	protected static void swap(int[] arr, int a, int b)
	{
		int temp = arr[a];
		arr[a] = arr[b];
		arr[b] = temp;
	}
}
