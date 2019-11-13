from collections import namedtuple
from datetime import date, timedelta

class Vars:
    Lecture = namedtuple('Lecture', ['title', 'desc', 'materials', 'learnit', 'date'])
    Break = namedtuple('Break', ['title', 'date'])
    startdate = date(2019, 8, 29)

    lectures = [
            Lecture(title = 'Surprises in Parallelism and Java'
                    , desc = 'Concurrent and parallel programming, why, what is so hard. Threads and locks in Java, shared mutable memory, mutual exclusion, visibility, volatile fields, atomic operations, avoiding sharing (thread confinement, stack confinement), immutability, final, safe publication.'
                    , materials = 'Goetz chapters 1, 2, 3; Sutter paper; McKenney chapter 2; Bloch item 66'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=104971'
                    , date = startdate
                    )
            , Lecture(title = 'Threads and Locks'
                    , desc = 'Designing thread-safe classes. Monitor pattern. Concurrent collections. Documenting thread-safety.'
                    , materials = 'Goetz chapters 4, 5; Bloch item 15; <a href="http://jcip.net/jcip-annotations.jar">jcip-annotations.jar</a>'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=105858'
                    , date = startdate + timedelta(weeks=1)
                    )
            , Lecture(title = 'Performance measurements'
                    , desc = ''
                    , materials = 'Sestoft: <a href="http://www.itu.dk/people/sestoft/papers/benchmarking.pdf">Microbenchmarks</a> and <a href="http://www.itu.dk/people/sestoft/javaprecisely/benchmarks-java.zip">supporting Java code</a>;  Optional: McKenney chapter 3, <a href="https://shipilev.net/talks/j1-Oct2011-21682-benchmarking.pdf">(The Art of) (Java) Benchmarking</a>'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=105939'
                    , date = startdate + timedelta(weeks=2)
                    )
            , Lecture(title = 'Streams'
                    , desc = 'Java 8 parallel streams for bulk data and parallel array prefix operations. Functional interfaces, lambda expressions, method reference expressions.'
                    , materials = 'Sestoft: Java Precisely <b>3rd edition</b> sections 11.13, 11.14, 23, 24, 25; get <a href="https://learnit.itu.dk/mod/resource/view.php?id=106234">draft on LearnIT</a> and <a href="https://www.itu.dk/people/sestoft/javaprecisely/javaprecisely3-examples.tgz">associated example code</a>. Also <a href="words.zip">File with English words</a>.'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106669'
                    , date = startdate + timedelta(weeks=3)
                    )
            , Lecture(title = 'Threads and Locks 2'
                    , desc = 'Tasks and the Java executor framework. Concurrent pipelines, wait() and notifyAll().'
                    , materials = 'Goetz chapters 5.3, 6, 8; Bloch items 68, 69'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106670'
                    , date = startdate + timedelta(weeks=4)
                    )
            , Break(title = 'Lecture Cancelled'
                    , date = startdate + timedelta(weeks=5)
                    )
            , Lecture(title = 'Threads and Locks 3'
                    , desc = 'Concurrent hash maps: performance and scalability case study.'
                    , materials = 'Goetz chapter 11, 13.5'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106673'
                    , date = startdate + timedelta(weeks=6)
                    )
            #, Lecture(title = 'GUI applications'
                    #, desc = 'Cache coherence and performance consequences.'
                    #, materials = 'Goetz chapters 9, 10.1-2; McKenney chapters 1-4'
                    #, learnit = '#'
                    #, date = startdate + timedelta(weeks=6)
                    #)
            , Break(title = 'Fall break'
                    , date = startdate + timedelta(weeks=7)
                    )
            , Lecture(title = 'Threads and Locks 4'
                    , desc = 'Testing concurrent programs.'
                    , materials = 'Goetz chapter 12; <a href="https://learnit.itu.dk/pluginfile.php/251401/mod_resource/content/1/herlihy-shavit-rfe-ch03.pdf">Herlihy and Shavit chapter 3</a>'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106675'
                    , date = startdate + timedelta(weeks=8)
                    )
            , Lecture(title = 'Transactional Memory'
                    , desc = 'Transactional memory with Multiverse: <a href="http://itu.dk/people/fbie/multiverse-javadoc/index.html?org/multiverse/api/package-summary.html">Multiverse API Javadoc</a> and file <a href="multiverse-core-0.7.0.jar">multiverse-core-0.7.0.jar</a>.'
                    , materials = '<a href="https://learnit.itu.dk/mod/resource/view.php?id=108890">Harris et al 2008 paper</a>; <a href="https://learnit.itu.dk/mod/resource/view.php?id=108891">Herlihy and Shavit</a> sections 18.1-18.2; <a href="https://queue.acm.org/detail.cfm?id=1454466">Cascaval et al 2008 paper</a>; <a href="https://learnit.itu.dk/mod/resource/view.php?id=108892">Eidenbenz, Wattenhofer 2011 paper</a>;'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106676'
                    , date = startdate + timedelta(weeks=9)
                    )
            , Lecture(title = 'Compare and Swap 1'
                    , desc = '<b>Notice: Lecture will be posted as video thursday afternoon.</b> There won\'t be anyone in the auditorium in the morning; Description: Optimistic concurrency, lock-free data structures, Treiber stack, compare-and-swap. Consensus number.'
                    , materials = 'Goetz chapter 15; <a href="https://learnit.itu.dk/pluginfile.php/252293/mod_resource/content/1/HerlihyShavit5_1-5_2.pdf">Herlihy and Shavit sections 5.1-5.2</a>'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106677'
                    , date = startdate + timedelta(weeks=10)
                    )
            , Lecture(title = 'Compare and Swap 2'
                    , desc = 'The Michael and Scott queue, progress concepts, Union Find, work-stealing queues.'
                    , materials = '<a href="http://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf">Michael and Scott paper</a> and <a href="http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.170.1097&rep=rep1&type=pdf">Chase and Lev paper</a> sections 1, 2 and 5'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106678'
                    , date = startdate + timedelta(weeks=11)
                    )
            , Lecture(title = 'Introduction to Message Passing Concurrency'
                    , desc = 'Introduction to message passing concurrency (no mutable shared memory), Introduction to Erlang, Introduction to Java+Akka.'
                    , materials = 'Erlang chapter 1, 2, 5'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106679'
                    , date = startdate + timedelta(weeks=12)
                    )
            , Lecture(title = 'More about Message Passing Concurrency'
                    , desc = 'Message passing (no mutable shared memory), Erlang, Java+Akka.'
                    , materials = 'Just the slides'
                    , learnit = 'https://learnit.itu.dk/mod/assign/view.php?id=106680'
                    , date = startdate + timedelta(weeks=13)
                    )
            , Lecture(title = 'Exam preparation'
                    , desc = ''
                    , materials = 'See Materials -> Archive'
                    , learnit = '#'
                    , date = startdate + timedelta(weeks=14)
                    )
            ]

    def ord(n):
        return str(n)+("th" if 4<=n%100<=20 else {1:"st",2:"nd",3:"rd"}.get(n%10, "th"))
    isinstance = isinstance
