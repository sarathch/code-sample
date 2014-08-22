
package org.chandu;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import org.apache.hadoop.io.WritableComparable;

public class IndexPairsWritable implements WritableComparable<IndexPairsWritable> {

	private long i;
	private long j;

	public IndexPairsWritable() {
		this(0, 0);
	}

	public IndexPairsWritable(long i, long j) {
		this.i = i;
		this.j = j;
	}
	
	public String toString() {
		return (new StringBuilder())
				.append(getI())
				.append(',')
				.append(getJ())
				.toString();
	}

	public void readFields(DataInput in) throws IOException {
		i = in.readLong();
		j = in.readLong();

	}

	public void write(DataOutput out) throws IOException {
		out.writeLong(getI());
		out.writeLong(getJ());
	}
	
	/** Get and Set ith and jth Column Values**/
	public long getI() {
		return i;
	}

	public long getJ() {
		return j;
	}

	public void setI(long i) {
		this.i = i;
	}

	public void setJ(long j) {
		this.j = j;
	}
	
	/** Override Writable compareTo Method**/
	public int compareTo(IndexPairsWritable object) {
		Long i1 = getI();
		Long j1 = getJ();
		Long i2 = object.getI();
		Long j2 = object.getJ();
		
		int result = i1.compareTo(i2);
		if(result == 0) {
			return j1.compareTo(j2);
		}
		
		return result;
	}

}
