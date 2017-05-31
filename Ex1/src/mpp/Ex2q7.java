package mpp;

import java.util.ArrayList;
import java.util.Random;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.ReentrantLock;

public class Ex2q7
{
	final static long SECOND_TO_SLEEP = 10;
	final static long MILLISECONDS_TO_SLEEP = 1000 * SECOND_TO_SLEEP;
	
	public static class Consensus<T>
	{		
		private AtomicReference<T> decision = new AtomicReference<T>();
		public T decide(T value)
		{
			decision.compareAndSet(null, value);
			return decision.get();
		}
	}
	
	public interface Operation<T>
	{
		public Object apply(T ds);
	}
	
	public static class QueueEnq implements Operation<MyQueue> {
		int valueToPush;
		
		public QueueEnq(int valueToPush) {
			this.valueToPush = valueToPush;
		}
		
		public Object apply(MyQueue queue) {
			queue.enqueue(valueToPush);
			return null;
		}
	}
	
	public static class QueueDeq implements Operation<MyQueue> {
		public QueueDeq() {}
		
		public Object apply(MyQueue queue) {
			return queue.dequeue();
		}
	}
	
	public static class NoopOperation<T> implements Operation<T> {
		public Object apply(T obj) {
			return null;
		}
	}
	
	public static class UniversalConstruction<T extends Copiable<T>>
	{
		// We would rather use UCNode<T>[] for these, but there are problems in java
		// trying to create generic arrays
		private ArrayList<UCNode<T>> announce;
		private ArrayList<UCNode<T>> head;
		private ArrayList<UCNode<T>> tail;
		private ArrayList<T> baseStructures;
		
		public UniversalConstruction(int numThreads, T base) {
			baseStructures = new ArrayList<>(numThreads);
			announce = new ArrayList<>(numThreads);
			head = new ArrayList<>(numThreads);			
			tail = new ArrayList<>(numThreads);
			
			UCNode<T> first = new UCNode<T>(new NoopOperation<T>());
			first.seq = 1;
			for (int i = 0; i < numThreads; i++) {
				baseStructures.add(base.copy());
				tail.add(first);
				head.add(first);
				announce.add(first);
			}
		}

		public Object execute(Operation<T> op)
		{
			int tid = threadId.get();
			announce.set(tid, new UCNode<>(op));
			head.set(tid, maxHead());
			
			while (0 == announce.get(tid).seq) {
				UCNode<T> before = head.get(tid);
				UCNode<T> help = announce.get((before.seq + 1) % announce.size());
				UCNode<T> prefer = (help.seq == 0) ? (help) : (announce.get(tid));
				UCNode<T> after = before.decideNext.decide(prefer);
				before.next = after;
				after.seq = before.seq + 1;
				head.set(tid, after);
			}
			
			UCNode<T> current = tail.get(tid);
			T structure = baseStructures.get(tid);
			while (current != announce.get(tid) && current.next != null) {
				current.operation.apply(structure);
				current = current.next;
			}
			tail.set(tid, current);
			baseStructures.set(tid, structure);
			return current.operation.apply(structure);
		}
		
		private UCNode<T> maxHead() {
			UCNode<T> max = head.get(0);
			for (UCNode<T> elem : head) {
				if (max.seq < elem.seq) {
					max = elem;
				}
			}
			return max;
		}
	}
	
	public static class UCNode<T> implements java.lang.Comparable<UCNode<T>> {
		public Operation<T> operation;
		public Consensus<UCNode<T>> decideNext;
		public UCNode<T> next;
		public int seq;
		public UCNode(Operation<T> op) {
			operation = op;
			decideNext = new Consensus<UCNode<T>>();
			seq = 0;
		}
		
		@Override
		public int compareTo(UCNode<T> o) {
			return Integer.compare(this.seq, o.seq);
		}
	}
	
	public interface IQueue {
		void enqueue(int num);
		int dequeue();
	}
	
	public interface Copiable<T> {
		T copy();
	}
	
	// Note - Using this implementation because the exercise said to use our own custom
	// 		  one without using any built in ones. I assumed that meant we couldn't use
	//        the built in LinkedList either which would have made it much simpler
	public static class MyQueue implements IQueue, Copiable<MyQueue> {
		private static class Node {
			public Node next;
			public int value;
			
			public Node(int val) {
				next = null;
				value = val;
			}
			
			public Node copy() {
				return new Node(value);
			}
		}
		Node head;
		Node tail;
		
		MyQueue() {
			head = null;
			tail = null;
		}

		@Override
		public void enqueue(int num) {
			Node new_node = new Node(num);
			if (null == tail) {
				assert (null == head);
				head = new_node;
				tail = new_node;
			} else {
				tail.next = new_node;
				tail = new_node;
			}
		}

		@Override
		public int dequeue() {
			if (null == tail) {
				assert (null == head);
				return -1;
			}
			int value = head.value;
			head = head.next; 
			if (null == head) {
				tail = null;
			}
			return value;
		}
		
		@Override
		public MyQueue copy() {
			MyQueue copyQueue = new MyQueue();
			if (null != head) {
				Node current = head;
				Node previous = null;
				while (current != null) {
					Node copy = current.copy();
					if (previous == null) {
						copyQueue.head = copy;
					} else {
						previous.next = copy;
					}
					previous = copy;
					current = current.next;
				}
				copyQueue.tail = previous;
			}
			return copyQueue;
		}
	}
	
	public static class LockFreeQueue implements IQueue {
		private UniversalConstruction<MyQueue> universalConstruction;
		
		public LockFreeQueue(int numberOfThreads) {
			universalConstruction = new UniversalConstruction<Ex2q7.MyQueue>(numberOfThreads, new MyQueue());
		}
		
		@Override
		public void enqueue(int num) {
			QueueEnq op = new QueueEnq(num);
			universalConstruction.execute(op);
		}

		@Override
		public int dequeue() {
			QueueDeq op = new QueueDeq();
			return (int)universalConstruction.execute(op);
		}
	}
	
	public static class DeadLockFreeQueue implements IQueue {
		private MyQueue queue;
		private ReentrantLock lock;
		
		public DeadLockFreeQueue() {
			queue = new MyQueue();
			lock = new ReentrantLock();
		}

		@Override
		public void enqueue(int num) {
			lock.lock();
			try {
				queue.enqueue(num);
			}
			finally {
				lock.unlock();
			}
		}

		@Override
		public int dequeue() {
			lock.lock();
			try {
				return queue.dequeue();
			}
			finally {
				lock.unlock();
			}
		}	
	}
	
	static boolean shouldStop = false;
	static IQueue queue = null;
	static AtomicLong totalCounter = new AtomicLong(0);
	static ThreadLocal<Integer> threadId = new ThreadLocal<Integer>();
	static AtomicInteger threadIdCounter = new AtomicInteger(0); 
	
	static class MyThread extends java.lang.Thread {
		public void run() {
			threadId.set(threadIdCounter.getAndIncrement());
			
			long operations = 0;
			Random rand = new Random(Thread.currentThread().getId());
			
			while (!shouldStop) {
				if (1 == (operations & 1)) { // It's an odd number
					queue.dequeue();
				} else {
					queue.enqueue(rand.nextInt());
				}
				operations++;
			}
			
			totalCounter.addAndGet(operations);
		}
	}
	
	public static void main(int threadCount, int queueType)
	{
		System.out.println(String.format("Type: %d Threads: %d", queueType, threadCount));
		threadIdCounter.set(0);
		totalCounter = new AtomicLong(0);
		shouldStop = false;
		MyThread[] threads = new MyThread[threadCount];
		if (1 == queueType) {
			queue = new LockFreeQueue(threadCount);
		} else if (2 == queueType) {
			queue = new DeadLockFreeQueue();
		} else {
			System.out.println("You entered the wrong type of queue");
			return;
		}
		
		for (int i = 0; i < threadCount; i++) {
			threads[i] = new MyThread();
			threads[i].start();
		}
		
		try {
			Thread.sleep(MILLISECONDS_TO_SLEEP);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
		shouldStop = true;
		
		for (int i = 0; i < threadCount; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {}
		}
		
		long operations = totalCounter.get();
		double throuput = operations / ((double)SECOND_TO_SLEEP);
		String message = String.format("Type: %d Threads: %d Operations: %d Throughput: %f", queueType, threadCount, operations, throuput);
		System.out.println(message);
	}
}
