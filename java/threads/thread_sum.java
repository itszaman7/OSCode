import java.util.concurrent.*;

// This class defines a task that computes the sum of integers up to `upper`
class Summation implements Callable<Integer> {
    private int upper;

    public Summation(int upper) {
        this.upper = upper;
    }

    // The thread will execute this method
    public Integer call() {
        int sum = 0;
        for (int i = 1; i <= upper; i++) {
            sum += i;
        }
        return sum;
    }
}

public class Driver {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java Driver <positive-integer>");
            return;
        }

        int upper = Integer.parseInt(args[0]);

        ExecutorService pool = Executors.newSingleThreadExecutor();

        // Submit the task to the executor and get a Future object
        Future<Integer> result = pool.submit(new Summation(upper));

        try {
            // Retrieve the result of the computation
            System.out.println("sum = " + result.get());
        } catch (InterruptedException | ExecutionException ie) {
            System.err.println("Error occurred while computing the sum.");
            ie.printStackTrace();
        }

        // Always shut down the executor to free resources
        pool.shutdown();
    }
}
