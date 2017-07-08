package mpp;

import java.io.BufferedReader;
import java.io.FileReader;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.lang.AutoCloseable;

public class EX4Q7 {
	
	// Since we aren't allowed to use any built-in type we'll use a simple lock that in stead of
	// spinning will delay for a short while (since the locks aren't expected to be held for a lot of time).
	// We make the lock's "lock" function return an "AutoClose" variable to make it's use simpler
	static class SimpleLock {
		private static final int DELAY = 1; 
		private AtomicBoolean state;
		
		public SimpleLock() {
			this.state = new AtomicBoolean(false);
		}
		
		public AutoClose lock() {
			while (true) {
				while (state.get()) {};
				if (!state.getAndSet(true)) {
					return new AutoClose(this);
				} else {
					try {
						Thread.sleep(DELAY);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			}
		}
		
		public void unlock() {
			state.set(false);
		}
		
		static class AutoClose implements AutoCloseable {
			SimpleLock lock;
			public AutoClose(SimpleLock lock) {
				this.lock = lock;
			}
			@Override
			public void close() {
				lock.unlock();
			}
		}
	}
	
	// We use a variation of the opportunistic sorted list from lecture 8
	// This is because we notice that during the building of the graph
	// we never remove any item, so our list doesn't need to support the 
	// remove function at all. 
	static class OpportunisticList<T> {
		class Node {
			int key;
			T value;
			Node next;
			SimpleLock lock;
			SimpleLock dataLock;
			
			Node(int key) {
				this.key = key;
				this.next = null;
				lock = new SimpleLock();
				dataLock = new SimpleLock();
			}
		}
		
		static final int HEAD_KEY = -1;
		Node head;
		
		public OpportunisticList() {
			head = new Node(HEAD_KEY);
		}
		
		// The get function either returns the already existing node with the given key
		// or creates a new node and returns it
		Node get(int key) {
			while(true) {
				// We have a special case for the chance that the list is still empty 
				if (null == head.next) {
					try (SimpleLock.AutoClose headLock = head.lock.lock()) {
						if (null == head.next) {
							head.next = new Node(key);
							return head.next;
						} else {
							continue;
						}
					}
				}
				// The standard case where the list isn't empty
				// We have a small optimization here since elements can't be removed
				// that if we need to scan a couple of times we will start from the
				// largest node we node that is still smaller than "key"
				Node prev = head;
				while (true) {
					Node curr = prev.next;
					while ((null != curr) && (curr.key > key)) {
						prev = curr;
						curr = curr.next;
					}
					// If we reached the end of the list then we need to add a new node
					if (null == curr) {
						try (SimpleLock.AutoClose prevLock = prev.lock.lock()) {
							// We make sure that after the lock a new node still wasn't added
							if (null == prev.next) {
								prev.next = new Node(key);
								return prev.next;
							} else {
								continue;
							}
						}
					}
					// We check if we found a node with the given key already
					if (curr.key == key) {
						return curr;
					}
					try (SimpleLock.AutoClose prevLock = prev.lock.lock();
						 SimpleLock.AutoClose currLock = curr.lock.lock()) {
						// We check that after we took the locks there still isn't any
						// new node that came between the 2 locked nodes. We should notice
						// that if there is then due to the optimization we won't have to scan
						// the whole list again, just from "prev"
						if (prev.next == curr) {
							Node newNode = new Node(key);
							newNode.next = curr;
							prev.next = newNode;
							return newNode;
						} else {
							continue;
						}
					}
				}
			}
		}
	}
	
	
	static class EdgeList {
		EdgeList() {
			edges = new OpportunisticList<Edge>();
		}
		
		void addEdge(Edge e) {
			edges.get(e.destinationNode).value = e;
		}
		OpportunisticList<Edge> edges;
	}
	
	static String[] g_input;
	static int g_nodes;
	static int g_edges;
	static AtomicInteger g_edgesProcessed;
	static GraphHashTable g_graph;
	
	static boolean readFile(String filePath) {
		try (BufferedReader br = new BufferedReader(new FileReader(filePath))) {
		    String firstLine = br.readLine();
		    assert null != firstLine;
		    
		    String[] parts = firstLine.split(" ");
		    assert(parts[0].equals("p"));
		    assert(parts[0].equals("sp"));
		    g_nodes = Integer.parseInt(parts[2]);
		    g_edges = Integer.parseInt(parts[3]);
		    g_input = new String[g_edges];
		    
		    
		    for (int i = 0; i < g_edges; i++) {
		    	g_input[i] = br.readLine();
		    	assert null != g_input[i];
		    }
		    String extraLines = br.readLine();
		    assert null == extraLines;
		    return true;
		} catch (Exception e) {
			System.out.println("Found a problem " + e.toString());
			return false;
		}
	}
	
	// Returns the source node in the return value and the parsed edge in the outEdge parameter
	static int parse(String line, Edge outEdge) {
		String[] parts = line.split(" ");
	    assert(parts[0].equals("a"));
	    int sourceNode = Integer.parseInt(parts[1]);
	    int destinationNode = Integer.parseInt(parts[2]);
	    int weight = Integer.parseInt(parts[3]);
	    outEdge.set(weight, destinationNode);
	    return sourceNode;
	}
	
	static class Edge {
		int weight;
		int destinationNode;
		
		public void set(int weight, int destinationNode) {
			this.weight = weight;
			this.destinationNode = destinationNode;
		}
	}
	
	static class GraphNode {
		EdgeList edges;
		
		public GraphNode() {
			edges = new EdgeList();
		}
	}
	
	static class HashBucket {
		OpportunisticList<GraphNode> nodes;
		
		public HashBucket() {
			nodes = new OpportunisticList<GraphNode>();
		}
	}
	
	static class GraphHashTable {
		static final int SIZE_FACTOR = 8;
		public GraphHashTable(int numberOfNodes) {
			buckets = new HashBucket[numberOfNodes / SIZE_FACTOR];
			for (int i = 0; i < buckets.length; i++) {
				buckets[i] = new HashBucket();
			}
		}
		
		HashBucket[] buckets;
		
		GraphNode getNode(int key) {
			int bucketIndex = key % buckets.length;
			OpportunisticList<GraphNode>.Node node = buckets[bucketIndex].nodes.get(key);
			// Here we set the internal value (the graph node inside the list node) if it wasn't set before
			if (null != node.value) {
				return node.value;
			}
			try (SimpleLock.AutoClose nodeLock = node.dataLock.lock()) {
				if (null != node.value) {
					return node.value;
				}
				node.value = new GraphNode();
				return node.value;
			}
		}
	}
	
	
	static class WorkerThread extends java.lang.Thread {
		public void run() {
			while (true) {
				int nextEdge = g_edgesProcessed.getAndIncrement();
				if (nextEdge >= g_edges) {
					return;
				}
				
				Edge currentEdge = new Edge();
				int sourceNode = parse(g_input[nextEdge], currentEdge);
				g_graph.getNode(sourceNode).edges.addEdge(currentEdge);
			}
		}
		
		int threadIndex;
	}
	
	static String getCurrentTimeStamp() {
	    return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date());
	}
	
	public static void main(int numberOfThreads, String inputPath) {
		//System.out.println(getCurrentTimeStamp() + " Preparing test for EX4Q7 with " + Integer.toString(numberOfThreads) + " threads");
		readFile(inputPath);
		g_graph = new GraphHashTable(g_nodes);
		g_edgesProcessed = new AtomicInteger(0);
		
		WorkerThread[] threads = new WorkerThread[numberOfThreads];
		for (int i = 0; i < numberOfThreads; i++) {
			threads[i] = new WorkerThread();
		}
		
		//System.out.println(getCurrentTimeStamp() + " Running test for EX4Q7 with " + Integer.toString(numberOfThreads) + " threads");
		long start_time = System.currentTimeMillis();
		
		for (int i = 0; i < numberOfThreads; i++) {
			threads[i].start();
		}
		for (int i = 0; i < numberOfThreads; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {}
		}
		
		long end_time = System.currentTimeMillis();
		System.out.println("The time it took is " + Long.toString(end_time - start_time) + " miliseconds for " + Integer.toString(numberOfThreads) + " threads");
	}
}
