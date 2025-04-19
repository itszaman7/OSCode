import java.util.concurrent.*;

public class ThreadPoolExample {
    public static void main(String[] args) {
        if (args.length < 1) {
            System.out.println("Usage: java ThreadPoolExample <number-of-tasks>");
            return;
        }

        int numTasks = Integer.parseInt(args[0].trim());
        
        /* Create the thread pool */
        ExecutorService pool = Executors.newCachedThreadPool();

        /* Run each task using a thread in the pool */
        for (int i = 0; i < numTasks; i++) {
            final int taskId = i;
            pool.execute(new Runnable() {
                @Override
                public void run() {
                    System.out.println("Task " + taskId + " executed by " 
                                     + Thread.currentThread().getName());
                    // Simulate some work
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            });
        }

        /* Shut down the pool and wait for termination */
        pool.shutdown();
        try {
            if (!pool.awaitTermination(1, TimeUnit.MINUTES)) {
                System.err.println("Not all tasks completed!");
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        
        System.out.println("All tasks completed!");
    }
}