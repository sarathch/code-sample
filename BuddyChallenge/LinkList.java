
/*
*Implementing Linked List API to my understanding
*@Author: Sarath
*@start time: 1:21 AM
*@end time : 2: 30 AM
*/

import java.util.*;

public class LinkList<Item extends Comparable<Item>> implements Iterable<Item>
{

	private class Node
	{
        	private Item item;
        	Node next;
        	public Node(Item item)
        	{
                this.item = item;
                next = null;
        	}
	}

	public Node head = null;
	public static int size = 0;

	// add item - append

	public void add(Item item)
	{
		Node newNode = new Node(item);
		newNode.next = head;
		head = newNode;
		size++;
	}

	// add item at index

	public void add(int index, Item item)
	{
		Node newNode = new Node(item);
		if(index < size)
		{
			int step = 0;
			Node temp = head;
			while(step!=index)
			{
				temp = temp.next;
				step++;
			}
			Item tempE = temp.item;
			temp.item = item;
			newNode.next = temp.next;
			temp.next = newNode;
			newNode.item = tempE;
			size++;
		}else
			System.out.println("Index Out of Bounds");	
	}

	// get item at index

	public Item get(int index)
	{
		if(index < size)
                {
                        int step = 0;
                        Node temp = head;
                        while(step!=index)
                        {
                                temp = temp.next;
                                step++;
                        }
			return temp.item;
		}else
			return null;
	}

	// clear contents of the list

	public void clear()
	{
		head = null;
	}

	// if list contains element check

	public boolean contains(Item item)
	{
		Node temp = head;
		while(temp!=null)
		{
			if(temp.item.compareTo(item)==0)
				return true;
			temp = temp.next;
		}
		return false;
	}

	// list size

	public int size()
	{
		return size;
	}
	
	// remove an item at index
	
	public Item remove(int index)
	{
		Item temp;
		if(index < size)
                {
			size--;
                        if(index ==0)
			{
				if (head == null)
					return null;
				if (head.next == null)
				{
					temp = head.item;
					head = null;
					return temp;
				}
				temp = head.item;
				head = head.next;
				return temp;
			}

                        int step = 0;
                        Node t = head;
                        while(step!=(index-1))
                        
{
                                t = t.next;
                                step++;
                        }
			temp = t.next.item;
			t.next = t.next.next;
			return temp;
		}else
			return null;
	}

	// add a list of items -append

	public void addAll(Collection<? extends Item> list)
	{
		for (Item i : list)
			add(i);
	}

	// add a list of items at index

	public void addAll(int index, Collection<? extends Item> list)
	{
		for(Item i: list)
		{
			add(index,i);
		}
	}

	// set an element at index

	public void set(int index, Item item)
	{
		if(index < size)
                {
                        int step = 0;
                        Node temp = head;
                        while(step!=index)
                        {
                                temp = temp.next;
                                step++;
                        }
			temp.item = item;
		}
		else
			System.out.println("Index Out of Bounds");
	}

	// Check if empty
	public boolean isEmpty()
	{
		return size == 0;
	}

 	// Iterable instance to itearate

	public Iterator<Item> iterator()
	{
		return new ListIterator();		
	}	

	private class ListIterator	implements	Iterator<Item>
	{
		private Node thisNode = head;

		// hasNext
		public boolean hasNext()
		{
			return thisNode!=null;
		}

		// remove
		public void remove()
		{
		}
		
		// next

		public Item next()
		{
			Item item = thisNode.item;
			thisNode = thisNode.next;
			return item;
		}
	}

	public void reverse()
	{
		Node result = null;
		Node current = head;
		
		while(current!=null)
		{
			Node next = current.next;
			current.next = result;
			result = current;
			current = next;
		}
		head = result;
	}

	public void reverse(int index)
	{
		if (index <= size)
		{
		
			Item junk = null;
			Node dummy = new Node(junk);
			Node start = head;
			dummy.next = start;
			Node t_dummy = dummy;
			int size_check = size;
			while(size_check >=index)
			{
				Node result = null;
				Node current = start;
				int rev_step = 0;
				while(rev_step != index)
				{
					Node next = current.next;
					current.next = result;
					result = current;
					current = next;
					rev_step++;
				}
				t_dummy.next = result;
				while(t_dummy.next!=null)
					t_dummy = t_dummy.next;
				size_check -= index;
				start = current;
			}
			t_dummy.next = start;
			head = dummy.next;
		}
		else
			System.out.println("IndexOutOfBounds");
	}

	public static void main(String args[])
	{
		LinkList<Integer> LL = new LinkList<Integer>();
		ArrayList<Integer> ele = new ArrayList<Integer>();
		for (int i=0; i<10; i++)
			ele.add(i+1);
		LL.addAll(ele);

		for (Integer p : LL)
			System.out.println(p);
		System.out.println("After Reverse");
		LL.reverse();
		for (Integer p : LL)
                        System.out.println(p);

		System.out.println("After Reverse each set");
                LL.reverse(4);
                for (Integer p : LL)
                        System.out.println(p);
	}
}

