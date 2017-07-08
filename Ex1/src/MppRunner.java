import mpp.EX4Q7;

public class MppRunner {	
	public static void main(String[] args) {
		String path = "C:\\Users\\omera_000\\Dropbox\\University\\MultiCpu\\HW4\\graph.txt";
		for (int num_threads : new int[] {1} ) {
		//for (int num_threads : new int[] {1, 2, 4, 8, 16, 32, 64} ) {
			EX4Q7.main(num_threads, path);
		}
	}
}
