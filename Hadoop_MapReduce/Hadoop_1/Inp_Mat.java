
import java.lang.*;
import java.io.*;

public class Inp_Mat{

public static void main(String[] args) throws Exception {
    
    int ivar=Integer.parseInt(args[0]);
    int iobs=Integer.parseInt(args[1]);
    String File_Name = "Mat_";
    File_Name=File_Name+ivar+"_"+iobs+".txt";
    PrintWriter writer = new PrintWriter(File_Name, "UTF-8");
    String Value = "";
    for( int i=0;i<iobs;i++){
        for(int j=0;j<ivar;j++){
		if(j!=ivar-1)
		  Value = Math.random() * 100+", ";
		else
		  Value = Math.random() * 100+"";
		  writer.print(Value);
                }
	writer.println("");
        Value = "" ;
        }
	writer.close();
   }
}
























