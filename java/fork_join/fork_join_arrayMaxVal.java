import java.util.concurrent.*;

public class MaxVal extends RecursiveTask<Integer> {
    static final int THRESHOLD = 1000;
    private int begin;
    private int end;
    private int[] array;

    public MaxVal(int begin, int end, int[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    protected Integer compute() {
        if (end - begin < THRESHOLD) {
            int highest = Integer.MIN_VALUE; // Use this for safety
            for (int i = begin; i <= end; i++) {
                if (array[i] > highest) {
                    highest = array[i]; // Compare array[i] not index i
                }
            }
            return highest;
        } else {
            int mid = (begin + end) / 2;
            MaxVal leftTask = new MaxVal(begin, mid, array);
            MaxVal rightTask = new MaxVal(mid + 1, end, array);

            // Fork tasks
            leftTask.fork();
            int rightResult = rightTask.compute(); // Direct compute for one side
            int leftResult = leftTask.join();      // Wait for forked one

            return Math.max(leftResult, rightResult); // Combine results
        }
    }
}
