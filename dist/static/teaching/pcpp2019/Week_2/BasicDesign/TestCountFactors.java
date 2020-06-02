package BasicDesign;// For week 2
// sestoft@itu.dk * 2014-08-29

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

class TestCountFactors {
    public static void main(String[] args) {
        final int range = 5_000_000;
        int count = 0;
        for (int p = 0; p < range; p++)
            count += countPrimeFactors(p);
        System.out.printf("Total number of factors is %9d%n", count);
        System.out.println(countPrimeFactorsParallel(5_000_000, 10));
    }

    public static int countPrimeFactors(int p) {
        if (p < 2)
            return 0;
        int factorCount = 1, k = 2;
        while (p >= k * k) {
            if (p % k == 0) {
                factorCount++;
                p /= k;
            } else {
                k++;
            }
        }
        return factorCount;
    }

    // Just some code I wrote because I misunderstood the exercises.
    // This counts the factors of a single number using multiple threads.
    // You won't need this for anything.
    public static int countFactorsParallel(final int p, int n) {
        final int sqrt = (int) Math.sqrt(p);
        final AtomicInteger count = new AtomicInteger();
        List<Thread> threads = new ArrayList<>();
        for (int i = 0; i < n; i++) {
            final int ti = i;
            threads.add(new Thread(() -> {
                for (int v = ti*sqrt/n+1; v <= (ti+1)*sqrt/n; v++) {
                    if (p % v == 0) {
                        if (v*v == p) count.addAndGet(1);
                        else count.addAndGet(2);
                    }
                }
            }));
            threads.get(threads.size()-1).start();
        }
        for (Thread t : threads) {
            try {
                t.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return count.get();
    }
}
