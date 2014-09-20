
/*
*	Implemented classic Dining Philosopher Problem
*	@Mutual Exclusion achieved by BinarySemaphore class
*	@Philosopher - a light weighted process continuously try to access shared resource 
*/

class Philosopher implements Runnable
{
	int myid;
	DiningPhilosopher dp;
	Thread t;
	public volatile boolean isActive = true;	

	public Philosopher(int myid, DiningPhilosopher dp)
	{
		this.myid = myid;
		this.dp = dp;
		new Thread(this).start();
	}

	public void run()
	{
		while(isActive)
		{
			try
			{
				System.out.println("Philosopher" + myid+" is Thinking");
				Thread.sleep(30);
				System.out.println("Philosopher" + myid+" is hungry");
				if(dp.acquire(myid))
				{
					System.out.println("Philosopher" + myid+" is eating");
					Thread.sleep(40);
				}else{
					System.out.println("Philosopher" + myid+" left");
					kill();
				}
				dp.release(myid);
			}catch(InterruptedException e)	
			{
				e.printStackTrace();
			}
		}
	}

	public void kill()
	{
		isActive = false;
	}
}

public class DiningPhilosopher
{
	int n;
	int sphagetti_sz = 20;
	BinarySemaphore[] fork = null;

	public DiningPhilosopher(int num)
	{
		n = num;
		fork = new BinarySemaphore[n];
		for(int i=0; i< n; i++)
			fork[i] = new BinarySemaphore(true);
	}
	
	public boolean acquire(int pid)
	{
		if(sphagetti_sz <=0)
		{
			System.out.println("Chef can't feed you no more");
			return false;
		}
		fork[pid].P();	
		fork[(pid+1)%n].P();
		return true;
	}

	public void release(int pid)
	{
		fork[pid].V();
		fork[(pid+1)%n].V();
		sphagetti_sz--;
	}

	public static void main(String...args)
	{
		int num_phil = 10;
		DiningPhilosopher dp = new DiningPhilosopher(num_phil);
		for (int i=0; i<num_phil; i++)
			new Philosopher(i, dp);
	}
}
