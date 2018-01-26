

/*
*	Dijkstra's way of achieving Mutual Exclusion
*/

import java.util.*;

public class BinarySemaphore
{
	boolean value;

	public BinarySemaphore(boolean value)
	{
		this.value = value;
	}

	// sync prevents another thread from entering until the current one is done
	public synchronized void P()
	{
		
		while(!value){
			try{
				this.wait();	// Note: sync and this wait combined helps threads to wait out different times
			}catch(InterruptedException e)
			{
				e.printStackTrace();
			}
		}
		value = false;
	}

	public synchronized void V()
	{
		value = true;
		notify();
	}
}
