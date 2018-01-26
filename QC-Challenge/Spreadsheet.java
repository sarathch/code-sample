

/*
* QuantCast Challenge : Spread Sheet Calculator 
* @start: 9:45 Am
* @end: 12:40 Pm
* Review - Probably didn't meet their 3 hour time frame 
*        - Cycle detection don't work for duplicate cell references in an exp
*	 - Ex : A1 A1 + (Will exit with a cycle)
*	 - Need to find time to improve this
*/

import java.util.*;
import java.io.*;


public class Spreadsheet
{

	public static void main(String...args)
	{
		int n,m;
		String[][] input;
		try
		{
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			
			String limit = br.readLine();
			n = Integer.parseInt(""+limit.charAt(0));
			m = Integer.parseInt(""+limit.charAt(2));

			input = new String[m][n];
			
			ArrayList<Pair> cellRefList = new ArrayList<Pair>();
			for (int i = 0; i <m; i++)
			{
				for (int j=0; j<n; j++)
				{	
					String exp = br.readLine();
					input[i][j] = exp;
				}
			}
			
			// Evaluating each cell
			for (int i = 0; i <m; i++)
                        {
                                for (int j=0; j<n; j++)
                                {
                                        String exp = input[i][j];
                                        double val = evalPostFix(exp, input);
                                        input[i][j] = String.valueOf(val);
                                }
                        }

			display(input);
		}catch(Exception e)
		{
			e.printStackTrace();
		}

	}

	// Display the final spreadsheet
	public static void display(String[][] input)
	{
		System.out.println(input[0].length +" " + input.length);
		for (int p = 0; p <input.length; p++)
                {
                        for (int q=0; q<input[0].length; q++)
                        {
                                Double val = Double.parseDouble(input[p][q]);
				System.out.printf("%.5f\n",val);
                        }
                }
	}

	// Method to evaluate postfix expressions

	public static double evalPostFix (String expr, String[][] input)
   	{
		Stack<Double> stack = new Stack<Double>();
      		double op1, op2, result = 0;
		String token;
		ArrayList<String> arr = new ArrayList<String>();
		String[] str = expr.split("\\s+");
		for (String s: str)
			arr.add(s);
		Map<String, Boolean> marked = new HashMap<String, Boolean>();
		
		//Repeat until list is empty
      		while (!arr.isEmpty())
      		{
			
         		token = arr.get(0);

			// If Operator fetch operands and evaluate
         		if (isOperator(token))
         		{
            			op2 = stack.pop();
            			op1 = stack.pop();
            			result = evalSingleOp (token.charAt(0), op1, op2);
            			stack.push (new Double(result));
         		}
         		else
			{
				// If it is a cell reference build expression
				if(isCellRef(token))
				{
					int row = token.charAt(0) - 'A';
                			int col = Integer.parseInt(""+token.charAt(1))-1;
					String start_cell = token;
					// If this string is already visited it's a cycle else fetch new expression from that cell
					if( marked.containsKey(token))
					{
						System.out.println("Cycle found");
						System.exit(1);
					}else{
						String[] exp = input[row][col].split("\\s+");
						arr.remove(0);
						for (int i= exp.length-1; i >=0; i--)
						{
							arr.add(0, exp[i]);
						}
						marked.put(token, true);
						continue;
					}
				}else	// If operands push them on to the stack
            				stack.push (new Double(Double.parseDouble(token)));
			}
			arr.remove(0);
      		}

      		return stack.pop();
   	}	
	
    	//  Determines if the specified token is an operator.
	public static boolean isCellRef(String token)
	{
		
		return Character.isLetter(token.charAt(0));
	}

  
   	//  Determines if the specified token is an operator.

   	private static boolean isOperator (String token)
   	{
      		return ( token.equals("+") || token.equals("-") ||
               		token.equals("*") || token.equals("/") );
   	}

  
   	//  Evaluates a single expression consisting of the specified
   	//  operator and operands.
   	private static double evalSingleOp (char operation, double op1, double op2)
   	{
      		double result = 0;
		switch (operation)
      		{
         		case '+':
            			result = op1 + op2;
            			break;
         		case '-':
            			result = op1 - op2;
            			break;
         		case '*':
            			result = op1 * op2;
            			break;
         		case '/':
            			result = op1 / op2;
      		}

      return result;
   }
}
