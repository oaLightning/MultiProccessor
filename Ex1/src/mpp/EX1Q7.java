package mpp;

import java.util.concurrent.locks.ReentrantLock;

public class EX1Q7 {
	static int ITERATIONS = 1000000;
	static ReentrantLock lock = new ReentrantLock();
	static int shared_counter = 0;
	
	static class MyThread extends java.lang.Thread {
		public void run() {
			for (int i = 0; i < ITERATIONS; i++) {
				lock.lock();
				try {
					int local_var = shared_counter;
					local_var++;
					shared_counter = local_var;
				} finally {
					lock.unlock();
				}
			}
		}
	}
	
	public static void main(int n) {
		shared_counter = 0;
		System.out.println("Running test for EX1Q7 with " + Integer.toString(n) + " threads");
		MyThread[] threads = new MyThread[n];
		long start_time = System.currentTimeMillis();
		
		for (int i = 0; i < n; i++) {
			threads[i] = new MyThread();
		}
		for (int i = 0; i < n; i++) {
			threads[i].start();
		}
		for (int i = 0; i < n; i++) {
			try {
				threads[i].join();
			} catch (InterruptedException e) {}
		}
		
		long end_time = System.currentTimeMillis();
		System.out.println("The counter is " + Integer.toString(shared_counter) + " and the time it took is " + Long.toString(end_time - start_time) + " miliseconds");
	}
}
