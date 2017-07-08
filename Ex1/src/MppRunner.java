import mpp.EX4Q7;

public class MppRunner {
	static final int TIMES_TO_RUN = 1; 
	public static void main(String[] args) {
		String path = "/tmp/random-graph.txt";
		for (int num_threads : new int[] {1, 2, 4, 8, /*16, 32, 64*/} ) {
			for (int i = 0; i < TIMES_TO_RUN; i++) {
				EX4Q7.main(num_threads, path);
				System.gc();
			}
		}
	}
}
