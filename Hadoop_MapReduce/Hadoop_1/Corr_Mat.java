package org.chandu;

import java.io.IOException;
import java.lang.*;
import java.io.*;
import java.util.*;
import java.util.StringTokenizer;

import org.chandu.IndexPairsWritable;
import org.chandu.ValuePairsWritable;

import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser; 
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;


public class Corr_Mat {

  public static class TokenizerMapper 
       	extends	Mapper<LongWritable, Text, IndexPairsWritable, ValuePairsWritable> {

	private static final String DELIM = ",";

	public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {

		String row = value.toString(); 
		String[] tokens = row.split(DELIM); 
		System.out.print("tokens size is:"+tokens.length);
		double[] arr_row = convDouble(tokens); 
		
		for(int i=0; i < arr_row.length; i++) {
			for(int j=i+1; j < arr_row.length; j++) {
				IndexPairsWritable k2 = new IndexPairsWritable(i, j);
				ValuePairsWritable v2 = new ValuePairsWritable(arr_row[i], arr_row[j]);
				context.write(k2, v2);
				System.out.print(k2.toString() + ", " + v2.toString());
			}
		}
	}

	public double[] convDouble(String[] tokens) {
		double[] arr_row = new double[tokens.length];
		for(int i=0; i < tokens.length; i++) {
			arr_row[i] = Double.parseDouble(tokens[i]);
		}
		return arr_row;

	}
  }
  
  public static class CalCorrReducer 
       extends Reducer<IndexPairsWritable, ValuePairsWritable, IndexPairsWritable, DoubleWritable> {
	
	private DoubleWritable result = new DoubleWritable();
	public void reduce(IndexPairsWritable key, Iterable<ValuePairsWritable> values, Context context) throws IOException, InterruptedException {

		double sum_x = 0.0d;
		double sum_y = 0.0d;
		double x_mean = 0.0d;
		double y_mean = 0.0d;
		double corr = 0.0d;

		int size = 0;
		int index =0;
		
		/** Retrieve OBS Size parameter passed to Configuration **/ 
		Configuration conf = context.getConfiguration();
 		String param = conf.get("Size");
		size = Integer.parseInt(param);

		double[] arr_x = new double[size];
                double[] arr_y = new double[size];
		
		/** Iterate over values list obtained for one Index Pair (x,y) *
		*  Correlation Formula::					*
		*        	Sigma:n [(x-x*)(y-y*)]				*
		*       -------	-------	----------------------------		*
		*   Sqrt(Sigma:n[(x-x*)^2] X Sqrt(Sigma:n[(y-y*)^2])		*/
		
		for(ValuePairsWritable pairs : values) {
			
			sum_x += pairs.getI();					//Calculate SUM of x obs and yobs 
			sum_y += pairs.getJ();
			arr_x[index] = pairs.getI();				//Place obs values of x and y in array for later use
			arr_y[index] = pairs.getJ();
			index +=1;
		}

		x_mean = sum_x / size;						//Compute Mean x* and y* 				
		y_mean = sum_y / size;
		
		double x_diff = 0.0d;
                double y_diff = 0.0d;
		double diff_prod = 0.0d;
                double x_diff_sqr = 0.0d;
                double y_diff_sqr = 0.0d;

		for(int i=0;i<size;i++) {			
                        x_diff = arr_x[i]-x_mean;				//Compute Diff values i.e x-x* and y-y*
                        y_diff = arr_y[i]-y_mean;
                       x_diff_sqr += Math.pow(x_diff, 2.0d);    		//Compute SUM of (x-x*)^2 and SUM of Sqr(y-y*)^2 
                        y_diff_sqr += Math.pow(y_diff, 2.0d);
                        diff_prod += (x_diff * y_diff);				//Compute SUM of (x-x*)(y-y*)
                }
		corr = diff_prod / (Math.sqrt(x_diff_sqr)*Math.sqrt(y_diff_sqr));//Compute Correlation
		result.set(corr);
		context.write(key, result);
	}
  }

  
  public static void main(String[] args) throws Exception {

    long start = new Date().getTime();
    Configuration conf = new Configuration();
    String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
    if (otherArgs.length != 3) {
      System.err.println("Usage: Corr_Mat <in> <out> <obs_size>");
      System.exit(2);
    }
    /* Pass Obs_Size parameter to hold on to Configuration for later use in reduce function */
    conf.set("Size", otherArgs[2]);
    /* Start Map Reduce Job */
    Job job = new Job(conf);
    job.setJarByClass(Corr_Mat.class);
    job.setMapperClass(TokenizerMapper.class);
    job.setReducerClass(CalCorrReducer.class);
    job.setMapOutputKeyClass(IndexPairsWritable.class);
    job.setMapOutputValueClass(ValuePairsWritable.class);
    job.setOutputKeyClass(IndexPairsWritable.class);
    job.setOutputValueClass(DoubleWritable.class);
    FileInputFormat.addInputPath(job, new Path(otherArgs[0]));
    FileOutputFormat.setOutputPath(job, new Path(otherArgs[1]));
    boolean status = job.waitForCompletion(true);            
    long end = new Date().getTime();
    System.out.println("Job took "+(end-start) + "milliseconds");
    System.exit(job.waitForCompletion(true) ? 0 : 1);
  }
}
