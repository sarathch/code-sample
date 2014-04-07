import java.lang.*;
import java.io.*;

public class Get_Top10 {

   public static void main(String[] args) throws IOException{
	
	BufferedReader reader = new BufferedReader(new FileReader("/usr/local/hadoop/Output32"));
	String line ="";
	int var_size=Integer.parseInt(args[0]);
	int corr_val_size = 0;
	for(int i=var_size-1;i>0;i--)
		corr_val_size+=i;
	double[] corr_val = new double[corr_val_size];
	String[] sort = new String[corr_val_size];
	int count=0;
	
	while ((line = reader.readLine())!=null){
		sort[count] = line;
		corr_val[count]=Double.parseDouble(line.substring(4));
               System.out.println(sort[count]);
		count++;
       }
		System.out.println("----");	
	for(int i=0;i<count;i++){
		for(int j=i+1;j<count;j++){
		   if(corr_val[i]<corr_val[j]){
			 double temp=corr_val[i];
			 corr_val[i]=corr_val[j];
			 corr_val[j]=temp;
			 String temps=sort[i];
                         sort[i]=sort[j];
                         sort[j]=temps;
		   }
		}
	}
		
	
	for (int i=0;i<10;i++) {
		System.out.println(sort[i]);	
	}
   }
}

