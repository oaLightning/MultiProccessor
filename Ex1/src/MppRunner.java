import mpp.Ex2q7;


public class MppRunner {
	final static int NUMBER_OF_RUNS = 10;
	
	public static void main(String[] args) {
		for (int type : new int[] {1, 2}) {
			for (int num_threads : new int[] {1, 2, 4, 8, 16, 32} ) {
				for (int i = 0; i < NUMBER_OF_RUNS; i++) {
					Ex2q7.main(num_threads, type);
				}
			}
		}
		
	}
}
