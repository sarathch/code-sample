
/*
* Given a array, rearrange the array such that, for some value as input, elements of the array with lesser value come first, elements of equal * value next and elements of greater value after that.
* @Author Sarath
*/
import java.util.Scanner;

public class Shuffler
{
	private static int bound=0;
	private static int flag =0;

	public static void main(String args[])
	{
		Scanner sc = new Scanner(System.in);
		
		int size = sc.nextInt();
		int[] num = new int[size];

		for (int i=0; i<size; i++)
			num[i] = sc.nextInt();
		
		int pivot = sc.nextInt();

		// Arrange elements <= pivot to one side
		num = arrange(num, pivot, size);
		// If equal elements exist then push them to middle
		System.out.println("Bound"+bound);
		if(flag ==1)
		{
			pivot--;
			num = arrange(num, pivot, bound);
		}
		
		for (int p=0; p<size;p++)
			System.out.print(num[p] + "->");
	}

	protected static int[] arrange(int[] num, int pivot, int index)
	{
		int low =0;
		for (int i=0; i<index; i++)
                {
                        if(num[i] <= pivot)
                        {
                                if(num[i] == pivot)     // Indicate if array has equal elements
                                        flag =1;
                                num=swap(num,low,i);
                                low++;
                        }
                }
		bound = low;
		return num;
        }

	protected static int[] swap(int[] num, int low, int high)
	{
		int temp = num[low];
		num[low] = num[high];
		num[high] = temp;
		return num;
	}
}
