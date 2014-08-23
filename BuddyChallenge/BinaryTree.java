
/*
*    Fun Code- Implementing a Binary Tree...O wait! Not BST..just Btree ;D
*    BuddyChallenge- Lowest Common Ancestor(line-100) || Vertical Node Sum(line-117)
*    @StartTime-2:53 PM
*    @EndTime- 4 PM
*/

import java.util.Queue;
import java.util.LinkedList;
import java.util.Scanner;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class BinaryTree<Item extends Comparable<Item>>
{
	// Tree Node Class
	private class Node
	{
		private Item item;
		private Node left;
		private Node right;
		private int distance ; 	// For vertical sum problem
		
		public Node(Item item)
		{
			this.item = item;
			left = null;
			right = null;
		}
	}

	private Node root = null;
	private int size = 0;
	// Queue to hold parent nodes
	Queue<Node> parents = new LinkedList<Node>();
	
	// Map to hold vertically aligned nodes
	private Map<Integer, ArrayList<Item>> map = null;

	// Adding node - level by level
	public void add(Item item)
	{
		size++;
		Node newNode = new Node(item);
		parents.add(newNode);
		if(root == null)
		{
			root = newNode;
		}else
		{
	 		Node current = parents.peek();
			if (current.left !=null && current.right!=null)
			{
				Node done = parents.remove();
				current = parents.peek();
			}
			if(current.left == null)
				current.left = newNode;
			else if(current.right == null)
				current.right = newNode;
		}
	}

	// Removing node - last insert
	public Item delete()
	{
		size--;
		Node current = parents.peek();
		Item item;
		if(current.right!= null)
		{
			item = current.right.item;
			current.right = null;
		}
		else
		{
			item = current.left.item;
			current.left = null;
		}
		return item;
	}

	// Traversals - In Order
	public void InOrderDisplay(Node root)
	{
		// Base Case
		if(root==null)
			return;
		else
		{
		InOrderDisplay(root.left);
		System.out.print(root.item + "->" +root.distance+ "***");
		InOrderDisplay(root.right);
		}
	}

	// return Lowest Common Ancestor
	public Item LCA(Node root, int flag, Item[] items)
	{

		if(root == null)
			return null;
		if(root.item.compareTo(items[0])==0 || root.item.compareTo(items[1]) == 0)
			return root.item;
		Item num1 = LCA(root.left, flag, items);
		Item num2 = LCA(root.right,flag,items);
		if(num1!=null && num2 != null)
			return root.item;
		if(num1!=null)
			return num1;
		return num2;
	}

	// Find Vertical Sum
	public void verticalSum()
	{
		map = new HashMap<Integer, ArrayList<Item>>();
		findDistance(root, size);
		map = alignedNodes(map, root);
		// Moved below code to Main Method to do numeric calculations(summation) - Generic type summations need Numeric Interface
		/*int line =0;
		for (Integer i : map.keySet())
		{
			line++;
			System.out.println(line+"::line -");
			for(Item item : map.get(i))
			{
				System.out.print(item+"---");
			}
			System.out.println();
		}*/
	}

	// Iterate over the tree and add nodes with same distance
	public Map<Integer, ArrayList<Item>> alignedNodes(Map<Integer, ArrayList<Item>> map, Node root)
	{
		if (root == null)
			return map;
		if(map.get(root.distance) == null)
		{
			ArrayList<Item> curr = new ArrayList<Item>();
			curr.add(root.item);
			map.put(root.distance, curr);
		}else
			map.get(root.distance).add(root.item);
		map = alignedNodes(map, root.left);
		map = alignedNodes(map, root.right);
		return map;
	}

	// Find horizontal distance
	public void findDistance(Node root, int dist)
	{
		if(root == null)
			return;
		root.distance = dist;
		findDistance(root.left, dist+1);
		findDistance(root.right, dist-1);
	}

	// Unit testing above tree

	public static void main(String args[])
	{
		BinaryTree<Integer> tree = new BinaryTree<Integer>();
		Scanner sc = new Scanner(System.in);
		
		for(;;)
		{
			int i = sc.nextInt();
			if ( i==999)
				break;
			tree.add(i);
		}
		System.out.println("Enter values to find LCA");
		Integer[] num = new Integer[2];
		num[0] = sc.nextInt();
		num[1] = sc.nextInt();
		System.out.println("LCA:-"+tree.LCA(tree.root, 0, num));
		tree.verticalSum();
		// Displaying vertical line sum
		int line =0;
                for (Integer i : tree.map.keySet())
                {
                        line++;
                        System.out.print(line+"::line -");
			int sum =0;
                        for(Integer p : tree.map.get(i))
                        {
				sum += p;
                        }
			System.out.print(sum);
                        System.out.println();
                }
		tree.InOrderDisplay(tree.root);
	}
	
}
