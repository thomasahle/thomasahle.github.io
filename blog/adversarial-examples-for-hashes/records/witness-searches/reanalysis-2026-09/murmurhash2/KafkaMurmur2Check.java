// KafkaMurmur2Check.java -- runs Apache Kafka's Utils.murmur2 (copied verbatim from
// clients/src/main/java/org/apache/kafka/common/utils/Utils.java, Apache-2.0; commit in
// SOURCES.md) on the 8-byte MurmurHash2 pair.  Run: java KafkaMurmur2Check.java (JDK 11+).
// Expected output: b626e14b b626e14b
import java.lang.invoke.*; import java.nio.ByteOrder;
public class KafkaMurmur2Check {
    private static final VarHandle INT_HANDLE = MethodHandles.byteArrayViewVarHandle(int[].class, ByteOrder.LITTLE_ENDIAN);
    @SuppressWarnings("fallthrough")
    public static int murmur2(final byte[] data) {
        int length = data.length;
        int seed = 0x9747b28c;
        // 'm' and 'r' are mixing constants generated offline.
        // They're not really 'magic', they just happen to work well.
        final int m = 0x5bd1e995;
        final int r = 24;

        // Initialize the hash to a random value
        int h = seed ^ length;
        int length4 = length >> 2;

        for (int i = 0; i < length4; i++) {
            final int i4 = i << 2;
            int k = (int) INT_HANDLE.get(data, i4);
            k *= m;
            k ^= k >>> r;
            k *= m;
            h *= m;
            h ^= k;
        }

        // Handle the last few bytes of the input array
        int index = length4 << 2;
        switch (length - index) {
            case 3:
                h ^= (data[index + 2] & 0xff) << 16;
            case 2:
                h ^= (data[index + 1] & 0xff) << 8;
            case 1:
                h ^= data[index] & 0xff;
                h *= m;
        }

        h ^= h >>> 13;
        h *= m;
        h ^= h >>> 15;

        return h;
    }

    public static void main(String[] a){
        byte[] x={0x6d,0x75,0x72,0x6d,0x75,0x72,0x32,0x21};
        byte[] y={(byte)0xed,0x53,(byte)0xff,(byte)0xba,(byte)0xf5,0x50,(byte)0xbf,0x6e};
        System.out.printf("%08x %08x%n", murmur2(x), murmur2(y));
    }
}