package BasicDesign;// For week 2
// sestoft@itu.dk * 2014-08-25
// thdy@itu.dk * 2019

import net.jcip.annotations.GuardedBy;

import java.io.IOException;


// From JSR 305 jar file jsr305-3.0.0.jar:

public class ThreadsafeLongCounter {
    public static void main(String[] args) throws IOException {
        final LongCounter lc = new LongCounter();
        Thread t = new Thread(() -> {
            while (true)        // Forever call increment
                lc.increment();
        });
        t.start();
        System.out.println("Press Enter to get the current value:");
        while (true) {
            System.in.read();         // Wait for enter key
            System.out.println(lc.get());
        }
    }

    static class LongCounter {
        @GuardedBy("this")
        private long count = 0;

        public synchronized void increment() {
            count++;
        }

        public synchronized long get() {
            return count;
        }
    }
}

