
package org.chandu;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

import org.apache.hadoop.io.Writable;

public class ValuePairsWritable implements Writable {

	private double i;
	private double j;

	public ValuePairsWritable() {
		this(0.0d, 0.0d);
	}
	
	public ValuePairsWritable(double i, double j) {
		this.i = i;
		this.j = j;
	}
	
	/** Read and Write ith and jth column Values**/
	public void readFields(DataInput in) throws IOException {
		i = in.readDouble();
		j = in.readDouble();
	}

	public void write(DataOutput out) throws IOException {
		out.writeDouble(getI());
		out.writeDouble(getJ());
	}
	
	/** Gets and Sets ith and jth column Values**/
	public double getI() {
		return i;
	}

	public void setI(double i) {
		this.i = i;
	}

	public double getJ() {
		return j;
	}

	public void setJ(double j) {
		this.j = j;
	}

}
