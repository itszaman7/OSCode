import java.util.concurrent.*;

public class ThreadPoolExample {

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java ThreadPoolExample <numTasks>");
            return;
        }

        int numTasks = Integer.parseInt(args[0].trim());

        // Create a dynamic thread pool
        ExecutorService pool = Executors.newCachedThreadPool();

        // Submit tasks to the pool
        for (int i = 0; i < numTasks; i++) {
            pool.execute(new Task(i + 1)); // Pass task ID
        }

        // Shutdown the pool after all tasks complete
        pool.shutdown();
    }
}

// This class defines the task each thread will execute
class Task implements Runnable {
    private final int taskId;

    public Task(int id) {
        this.taskId = id;
    }

    public void run() {
        System.out.println("Task " + taskId + " is running in thread: " + Thread.currentThread().getName());

        // Simulate work
        try {
            Thread.sleep(1000); // 1 second
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("Task " + taskId + " completed.");
    }
}
