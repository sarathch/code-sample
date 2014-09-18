

/*
*	Oddwords Problem 
*	Note - No Extra buffer to carry words
*	     - In Place
*/

public class OddWords
{
	public static void main(String...args)
	{
		String line = "I'm very pissed at the moment on my girl friend";
		int gap_count =0;
		
		StringBuilder build = new StringBuilder(line);
		for (int i =0; i<line.length(); i++)
		{
			if(line.charAt(i) == ' ')
			{
				gap_count++;
				if((gap_count & 1) > 0)
				{
					int start = i+1,end =i;
					while(line.charAt(end+1) != ' '){
						end++;
						if(end == line.length() -1) // final letter
							break;
					}
					while(start<=end)
					{
						char c = line.charAt(start);
						build.setCharAt(start, line.charAt(end));
						build.setCharAt(end, c);
						start++; end--;
					}
				}
			}
		}
		System.out.println("Result: "+build);
	}
}
