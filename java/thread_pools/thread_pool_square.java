import java.util.concurrent.*;

public class ThreadPoolSquare {

    public static void main(String[] args) {
        if (args.length != 1) {
            System.out.println("Usage: java ThreadPoolSquare <numTasks>");
            return;
        }

        int numTasks = Integer.parseInt(args[0].trim());

        ExecutorService pool = Executors.newFixedThreadPool(4); // Use fixed pool (more realistic)

        for (int i = 0; i < numTasks; i++) {
            pool.execute(new Task(i + 1)); // Pass task ID
        }

        pool.shutdown();
    }
}

class Task implements Runnable {
    private final int taskId;

    public Task(int id) {
        this.taskId = id;
    }

    public void run() {
        int square = taskId * taskId;

        System.out.println("Task " + taskId + " is running on " + Thread.currentThread().getName());
        System.out.println("Square of " + taskId + " = " + square);

        try {
            Thread.sleep(500); // Simulate some work
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("Task " + taskId + " completed.\n");
    }
}
