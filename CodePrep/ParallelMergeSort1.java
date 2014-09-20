

/*
*	MultiThreaded Version of Merge Sort
*	Manipulate the array bounds by setting low and high local variables
*	At each Thread, Two threads work on front and back halfs of the array
* 	@Author: Sarath
*/

import java.util.*;

public class ParallelMergeSort1 extends Thread
{
	int[] input = { 5,6,1,3,9,2,4,7,8 };
	int low;
	int high;
	public ParallelMergeSort1(int low, int high)
	{
		this.low = low;
		this.high = high;
	}

	public void run()
	{
		if(low < high)
		{
			int mid = (low+high)/2;
			ParallelMergeSort1 front = new ParallelMergeSort1(low, mid);
			Thread t1 = new Thread(front);
			ParallelMergeSort1 back =  new ParallelMergeSort1(mid+1, high);
			Thread t2 = new Thread(back);
			t1.start();
			t2.start();
			try{
				t1.join();
				t2.join();
			}catch(Exception e)
			{
				e.printStackTrace();
			}
			ArrayList<Integer> A = front.returnList();
			ArrayList<Integer> B = back.returnList();
			merge(A,B);
		}
	}

	public ArrayList<Integer> returnList()
	{
		ArrayList<Integer> arr = new ArrayList<Integer>();
		for (int i=low; i<=high; i++)
		{
			arr.add(input[i]);
		}
		return arr;
	}

	public void merge(ArrayList<Integer> A, ArrayList<Integer> B)
	{
		int cntA = 0, cntB=0;
		for (int i=low; i<=high; i++)
		{
			if(cntA == A.size()){
				input[i] = B.get(cntB);
				cntB++;
			}else if(cntB == B.size()){
				input[i] = A.get(cntA);
				cntA++;
			}else
			{
				int n1 = A.get(cntA);
				int n2 = B.get(cntB);
				if(n1 < n2)
				{
					input[i] = n1;
					cntA++;
				}else{
					input[i] = n2;
					cntB++;
				}
			}
		}
	}

	public static void main(String...args)
	{
		ParallelMergeSort1 p = new ParallelMergeSort1(0, 8);
		Thread t = new Thread(p);
		t.start();
		try{
			t.join();
		}catch(Exception e){
			e.printStackTrace();
		}
	
		for (int i: p.input)
			System.out.print(i+" ");
		System.out.println();
	}
}
