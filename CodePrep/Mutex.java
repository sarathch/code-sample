
/*
*	Petersen's Mutual Exclusion Algorithm for two threads
*	Will extend this in time 
*/

class MyThread extends Thread
{
	int MyId;
	Mutex lock;
	public MyThread(int id, Mutex lock)
	{
		MyId = id;
		this.lock = lock;
	}

	public void run()
	{
		lock.requestCS(MyId);
		System.out.println("Thread::"+MyId);
		lock.x += 1;	// critical region
		System.out.println(lock.x);
		lock.releaseCS(MyId);
	}
}

public class Mutex implements Lock
{
	int x =0;
	int turn = 0;
	boolean[] req = new boolean[2];
	public static void main(String...args)
	{
		Mutex lock = new Mutex();
		MyThread[] t = new MyThread[2];
		for (int i=0; i<2; i++){
			t[i] = new MyThread(i, lock);
			t[i].start();
		}
	}
	
	public void requestCS(int pid)
	{
		req[pid] = true;
		int next = (pid+1) % 2;
		turn = next;
		while(req[next] && turn == next){};
	}

	public void releaseCS(int pid)
	{
		req[pid] = false;
	}
}
