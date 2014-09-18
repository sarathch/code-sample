/*
*	Find minimum element from Sorted Array which is Rotated
* 	@Author: Sarath
*/


public class SortRotMin
{
	public static void main(String args[])
	{
		int[] arr = { 5, 6, 7, 8,9,2,3,4};
		System.out.println("Min is"+"::"+ binarySearchedGet(arr, 0, 7));
	}


	public static int binarySearchedGet(int[] arr, int low, int high)
	{
		System.out.println(low +"low:"+high+":high");
		int mid = low+((high - low) / 2);
		if(mid > low)
		{
			if(arr[mid] > arr[low])
				return binarySearchedGet(arr, mid+1, high);
			else 
				return binarySearchedGet(arr, low, mid);
		}
		return (arr[low] >arr[high] ? arr[high] : arr[low]); 
	}
}
