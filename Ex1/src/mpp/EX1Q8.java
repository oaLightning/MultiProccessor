package mpp;

import java.util.concurrent.atomic.AtomicInteger;

public class EX1Q8 {
	static int ITERATIONS = 1000000;
	static AtomicInteger shared_counter = new AtomicInteger(0); 
	
	static class MyThread extends java.lang.Thread {
		public void run() {
			for (int i = 0; i < ITERATIONS; i++) {
				shared_counter.incrementAndGet();
			}
		}
	}
	
	public static void test(int n) {
		shared_counter = new AtomicInteger(0);
		System.out.println("Running test for EX1Q8 with " + Integer.toString(n) + " threads");
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
		System.out.println("The counter is " + shared_counter.toString() + " and the time it took is " + Long.toString(end_time - start_time) + " miliseconds");
	}
}
