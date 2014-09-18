/* Fun Code - Implemented Graph API - Directed
*  BuddyChallenge - Breadth First Search a vertex and display its Connected Components if exists(line-97)	
* @Author- Sarath
*/

import java.util.ArrayList;
import java.util.Scanner;
import java.util.Collection;
import java.util.Queue;
import java.util.LinkedList;
public class Graph<Vertex extends Comparable<Vertex>>
{

	private ArrayList<ArrayList<Vertex>> bag = new ArrayList<ArrayList<Vertex>>();
	private ArrayList<Vertex> vertices = new ArrayList<Vertex>();
	private static int size;
	private Vertex source;
	
	public void addAll(Collection<? extends Vertex> V)
	{
		for (Vertex v : V)
			add(v);
	}

	// Add vertex to the graph
	public void add(Vertex v)
	{
	    if(!contains(v)){	
		size++;
		vertices.add(v);
		ArrayList<Vertex> bucket = new ArrayList<Vertex>();
		bag.add(bucket);
	    }else
		System.out.println("Vertex already is part of the graph");
	}

	// Connect two vertices
	public void connect(Vertex s, Vertex d)
	{
		if (contains(s) && contains(d))
		{
			int i = getIndex(s);
			bag.get(i).add(d);
		}else
			System.out.println("Vertex not found");
	}

	// Check if vertex exists
	public boolean contains(Vertex v)
	{
		for (Vertex x : vertices)
		{
			if(v.compareTo(x) == 0)
				return true;
		}
		return false;
	}	

	// Return size of the graph
	public int size()
	{
		return size;
	}

	// remove a vertex;
	public boolean remove(Vertex v)
	{
		if (getIndex(v) > 0){
			size--;
			int index = getIndex(v);
			vertices.remove(index);
			bag.remove(index);
			return true;
		}
		return false;
	}

	// Return Iterable instance of connected components of a vertex 
	public Iterable<Vertex> fetchBag(Vertex v)
	{
		int index = getIndex(v);
		return bag.get(index);
	}

	// fetch index from verices list to pass on to bag list
	public int getIndex(Vertex v)
	{
		for (int i =0; i<size; i++)
                {
                      	if(vertices.get(i).compareTo(v)==0)
                      		return i;
                }
		return -1;
	}

	// Breadth First Search
	public boolean BFS(Vertex dest)
	{
		boolean marked[] = new boolean[size];
		Queue<Vertex> queue = new LinkedList<Vertex>();
		queue.add(source);
		
		while(!queue.isEmpty())
		{
			Vertex curr = queue.remove();
			int index = getIndex(curr);
			
			for(Vertex v: bag.get(index))
			{
				int i = getIndex(v);
				if(!marked[i])
				{
					if(v.compareTo(dest) == 0){
						System.out.println("Found");
						return true;
					}
					queue.add(v);
					marked[i] = true;
				}
			}
		}
		return false;
	}

	// Unit testing Graph API
	public static void main(String args[])
	{
		Graph<Integer> graph = new Graph<Integer>();
		Scanner sc = new Scanner(System.in);
		ArrayList<Integer> vertex = new ArrayList<Integer>();
		// Vertices numbered 1..2..size
		for (int i=0; i<10;i++)
			vertex.add(i);

		graph.addAll(vertex);

		graph.connect(3,4);
		graph.connect(3,5);
		graph.connect(2,3);
		graph.connect(1,2);
		graph.connect(1,4);
		graph.connect(5,6);
		graph.connect(5,9);
		graph.source = 1;
		if(graph.BFS(5))
		{
			System.out.println("Element Found and It's Conected Components are");
			for(Integer v: graph.fetchBag(5))
				System.out.print(v+"->");
			System.out.println();
		}
		for(Integer i: graph.fetchBag(1))
			System.out.print(i + "->");	
	}
}
