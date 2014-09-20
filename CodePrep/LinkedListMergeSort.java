

/*
*	Merge sorting a linked list and in place list reversal
*	Note: Unlike arrays, splitting a linked list is not space inefficient
*/

class Node
{
       int data;
       Node next;
       public Node(int data)
       {
               this.data = data;
               this.next = null;
       }
}

public class LinkedListMergeSort
{
	
	private static Node r = null;

	public static void main(String...args)
	{
		int[] arr = {5,4,2,7,8,1,3,6};
		Node head = new Node(arr[0]);
		Node curr = head;
		for(int i=1; i< arr.length; i++)
		{
			Node newNode = new Node(arr[i]);
			curr.next = newNode;
			curr = curr.next;
		}
		displayList(head);
		head = mergeSort(head);
		displayList(head);
		reverseList(head);
		displayList(r);
	}

	public static Node mergeSort(Node head)
	{
		if (head == null || head.next == null)
			return head;

		Node slow = head, fast = head;
                while(fast != null)
                {
                        slow = slow.next;
                        fast = fast.next.next;
                }

                Node front = head;
                Node current = front;
                while(current.next != slow)
                {
                        current = current.next;
                }
                current.next = null;
                Node back = slow;
	
		Node head1 = mergeSort(front);
		Node head2 = mergeSort(back);
		head = sortedMerge(head1, head2);
		return head;
	}


	public static Node sortedMerge(Node n1, Node n2)
	{
		Node dummy = new Node(-1);
		Node curr = dummy;
		while(n1!=null || n2!= null)
		{
			if (n1 == null)
			{
				curr.next = n2;
				n2 = n2.next;
			}
			else if (n2 == null)
			{
				curr.next = n1;
				n1 = n1.next;
			}
			else if (n1.data < n2.data)
			{
				curr.next = n1;
				n1 = n1.next;
			}else
			{
				curr.next = n2;
				n2 = n2.next;
			}
			curr = curr.next;
		}		
		return dummy.next;
	}

	public static void displayList(Node head)
	{
		while(head!= null)
		{
			System.out.print(head.data+ " ->");
			head = head.next;
		}
		System.out.println();
	}


	public static void reverseList(Node head)
	{
		if (head == null)
			return;
		Node temp = head.next;
		head.next = r;
		r = head;
		reverseList(temp);
	}

}
