package mpp;

import java.util.Random;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;

public class EX3Q8 {
	static interface Lock {
		void lock();
		void unlock();
	}
	
	static class Backoff {
		final int minDelay, maxDelay;
		int limit;
		final Random random;
		public Backoff(int min, int max) {
			minDelay = min;
			maxDelay = max;
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
	
	static class BackoffLock implements Lock {
		private AtomicBoolean state;
		private int minDelay;
		private int maxDelay;
		
		public BackoffLock(int minDelay, int maxDelay) {
			this.minDelay = minDelay;
			this.maxDelay = maxDelay;
			this.state = new AtomicBoolean(false);
		}
		
		public void lock() {
			Backoff backoff = new Backoff(minDelay, maxDelay);
			while (true) {
				while (state.get()) {};
				if (!state.getAndSet(true)) {
					return;
				} else {
					backoff.backoff();
				}
			}
		}
		
		public void unlock() {
			state.set(false);
		}
	}
	
	static class QNode {
		volatile boolean locked = true;
		public QNode(boolean value) {
			locked = value;
		}
	}
	
	static class CLHLock implements Lock {
		AtomicReference<QNode> tail;
		ThreadLocal<QNode> myPred;
		ThreadLocal<QNode> myNode;
		
		public CLHLock() {
			tail = new AtomicReference<QNode>(new QNode(false));
			myNode = new ThreadLocal<QNode>() {
				protected QNode initialValue() {
					return new QNode(true);
				}
			};
			myPred = new ThreadLocal<QNode>() {
				protected QNode initialValue() {
					return null;
				}
			};
		}
		
		public void lock() {
			QNode qnode = myNode.get();
			qnode.locked = true;
			QNode pred = tail.getAndSet(qnode);
			myPred.set(pred);
			while (pred.locked) {}
		}	
		
		public void unlock() {
			QNode qnode = myNode.get();
			qnode.locked = false;
			myNode.set(myPred.get());
		}
	}
	
	static Lock g_lock;
	static long g_runs;
	static boolean g_shouldRun;
	static class MyThread extends java.lang.Thread {
		public void run() {
			while (g_shouldRun) {
				g_lock.lock();
				try {
					g_runs++;
				} finally {
					g_lock.unlock();
				}
			}
		}
	}
	
	final static int SECONDS_TO_TEST = 5;
	final static int MILLISECONDS_TO_TEST = SECONDS_TO_TEST * 1000;
	
	public static void main(int numberOfThreads, int lockType) {
		String lockTypeString; 
		if (1 == lockType) {
			g_lock = new CLHLock();
			lockTypeString = "CLHLock";
		} else if (2 == lockType) {
			g_lock = new BackoffLock(1, 10);
			lockTypeString = "Short BackoffLock";
		} else if (3 == lockType) {
			g_lock = new BackoffLock(1, 100);
			lockTypeString = "Long BackoffLock";
		} else {
			System.out.println("Bad lock type");
			return;
		}
		
		g_runs = 0;
		
		MyThread[] threads = new MyThread[numberOfThreads];
		
		for (int i = 0; i < numberOfThreads; i++) {
			threads[i] = new MyThread();
		}
		
		g_shouldRun = true;
		for (int i = 0; i < numberOfThreads; i++) {
			threads[i].start();
		}
		try {
			Thread.sleep(MILLISECONDS_TO_TEST);
		} catch (InterruptedException e) {
			System.out.println("Interrupted in main thread?");
		}
		g_shouldRun = false;
		
		for (int i = 0; i < numberOfThreads; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {}
		}
		
		long throughput = g_runs / SECONDS_TO_TEST;
		String message = String.format("Threads: %d, Type: %s, Throughput: %d, Ops: %d", numberOfThreads, lockTypeString, throughput, g_runs);
		System.out.println(message);
	}
}
