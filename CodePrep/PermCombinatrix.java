/*
*	BuddyChallenge - Permutation & Combination
* 	@Author- Sarath
*/

public class PermCombinatrix
{
	public static void main(String[] args)
	{
		PermCombinatrix scramble = new PermCombinatrix();
		String s = "123456789";
		char[] input = s.toCharArray();
		int comb_bound = 4;
		char[] data = new char[comb_bound];
		scramble.combination(input, data, 0, 0,comb_bound);
	}

	protected void combination(char[] inp, char[] data, int start, int index, int r)
	{
		// base case- if data array is full
		if(index == r)
		{
			permute(data, 0);
			return;
		}
		for (int i= start; i<inp.length ;i++)
		{
			data[index] = inp[i];
			combination(inp, data, i+1, index+1, r);
		}
	}

	protected void permute(char[] inp, int index)
	{
		if(index < inp.length-1)
		{
			for (int i= index; i<inp.length; i++)
			{
				inp = swap(inp, index, i);
				permute(inp,index+1);
				inp = swap(inp,index,i);
			}
		}else
			System.out.println(inp);
	}

	protected char[] swap(char[] inp, int left, int right)
	{
		char temp = inp[left];
		inp[left] = inp[right];
		inp[right] = temp;
		return inp;
	}
}
