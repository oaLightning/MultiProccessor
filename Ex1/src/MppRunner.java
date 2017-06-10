import mpp.EX3Q8;

public class MppRunner {
	final static int NUMBER_OF_RUNS = 3;
	
	public static void main(String[] args) {
		for (int num_threads : new int[] {1, 2, 4, 8, 16, 32} ) {
			for (int type : new int[] {1, 2, 3}) {
				for (int i = 0; i < NUMBER_OF_RUNS; i++) {
					EX3Q8.main(num_threads, type);
				}
			}
		}
		
	}
}
