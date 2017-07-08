package mpp;

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.Random;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.lang.AutoCloseable;

public class EX4Q7 {
	
	// Since we aren't allowed to use any built-in type we'll use the Backoff lock from 
	// EX3Q8. This lock is good since we won't hold any lock for too long and that would mean
	// that the waiting thread will wake up pretty quickly. We changed the minDelay and
	// maxDelay to be constants to reduce a tiny bit of overhead (and since they will always
	// be the same for us her) and made the Backoff class a static class inside the lock to
	// encapsulate better. We also made the lock function return an AutoCloseable class 
	// to make the rest of the code cleaner
	static class BackoffLock implements AutoCloseable {
		private AtomicBoolean state;
		
		public BackoffLock() {
			this.state = new AtomicBoolean(false);
		}
		
		public AutoClose lock() {
			Backoff backoff = new Backoff();
			while (true) {
				while (state.get()) {};
				if (!state.getAndSet(true)) {
					return new AutoClose(this);
				} else {
					backoff.backoff();
				}
			}
		}
		
		@Override
		public void close() throws Exception {
			unlock();			
		}
		
		public void unlock() {
			state.set(false);
		}
		
		static class AutoClose implements AutoCloseable {
			BackoffLock lock;
			public AutoClose(BackoffLock lock) {
				this.lock = lock;
			}
			@Override
			public void close() {
				lock.unlock();
			}
		}
		
		static class Backoff {
			final int minDelay = 1;
			final int maxDelay = 5;
			int limit;
			final Random random;
			public Backoff() {
				limit = minDelay;
				random = new Random();
			}
			public void backoff() {
				int delay = random.nextInt(limit);
				limit = Math.min(maxDelay, 2 * limit);
				try {
					Thread.sleep(delay);
				} catch (InterruptedException e) {
					System.out.println("Didn't expect to actually be interrupted");
				}
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
			BackoffLock lock;
			
			Node(int key) {
				this.key = key;
				this.next = null;
				lock = new BackoffLock();
			}
		}
		
		static final int HEAD_KEY = -1;
		Node head;
		
		public OpportunisticList() {
			head = new Node(HEAD_KEY);
		}
		
		// The get function either returns the already existing node with the given key
		// or creates a new node and returns it
		T get(int key) {
			while(true) {
				// We have a special case for the chance that the list is still empty 
				if (null == head.next) {
					try (BackoffLock.AutoClose headLock = head.lock.lock()) {
						if (null == head.next) {
							head.next = new Node(key);
							return head.next.value;
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
						try (BackoffLock.AutoClose prevLock = prev.lock.lock()) {
							// We make sure that after the lock a new node still wasn't added
							if (null == prev.next) {
								prev.next = new Node(key);
								return prev.next.value;
							} else {
								continue;
							}
						}
					}
					// We check if we found a node with the given key already
					if (curr.key == key) {
						return curr.value;
					}
					try (BackoffLock.AutoClose prevLock = prev.lock.lock();
						 BackoffLock.AutoClose currLock = curr.lock.lock()) {
						// We check that after we took the locks there still isn't any
						// new node that came between the 2 locked nodes. We should notice
						// that if there is then due to the optimization we won't have to scan
						// the whole list again, just from "prev"
						if (prev.next == curr) {
							Node newNode = new Node(key);
							newNode.next = curr;
							prev.next = newNode;
							return newNode.value;
						} else {
							continue;
						}
					}
				}
			}
		}
	}
	
	
	static class EdgeList {
		void addEdge(Edge e) {
			edges.get(e.destinationNode).weight = e.weight;
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
	
	static class Node {
		EdgeList edges;
	}
	
	static class HashBucket {
		OpportunisticList<Node> nodes;
		
		public HashBucket() {
			nodes = new OpportunisticList<Node>();
		}
	}
	
	static class GraphHashTable {
		static final int SIZE_FACTOR = 8;
		public GraphHashTable(int numberOfNodes) {
			buckets = new HashBucket[numberOfNodes / SIZE_FACTOR];
		}
		
		HashBucket[] buckets;
		
		Node getNode(int key) {
			int bucketIndex = key % buckets.length;
			return buckets[bucketIndex].nodes.get(key);
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
	
	public static void main(int numberOfThreads, String inputPath) {
		System.out.println("Preparing test for EX4Q7 with " + Integer.toString(numberOfThreads) + " threads");
		assert readFile(inputPath);
		g_graph = new GraphHashTable(g_nodes);
		g_edgesProcessed = new AtomicInteger(0);
		
		System.out.println("Running test for EX4Q7 with " + Integer.toString(numberOfThreads) + " threads");
		WorkerThread[] threads = new WorkerThread[numberOfThreads];
		for (int i = 0; i < numberOfThreads; i++) {
			threads[i] = new WorkerThread();
		}
		
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
