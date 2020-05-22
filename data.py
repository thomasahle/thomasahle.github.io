from collections import namedtuple
import datetime

Person = namedtuple('Person', ['name', 'abrv', 'href', 'photo', 'email'])
Person.__new__.__defaults__ = ('Anonymous', '', '')
Paper = namedtuple(
    'Paper',
    ['tag', 'title', 'authors', 'abstract', 'year', 'conference', 'comment', 'files'])
Conference = namedtuple('Conference', ['name'])
Teaching = namedtuple('Teaching', ['name', 'abrv', 'year', 'description', 'href'])
File = namedtuple('File', ['format', 'href'])
Newspaper = namedtuple('Newspaper', ['author', 'name', 'date', 'title', 'href', 'description', 'files'])
Award = namedtuple('Award', ['name', 'place', 'giver', 'date', 'description'])
Job = namedtuple('Job', ['title', 'company', 'date', 'description', 'academic'])


class Vars:
    now = datetime.datetime.now()

    authors = {
        'thdy':
        Person('Thomas Dybdahl Ahle', 'TA', '/', 'static/potrait.jpg', 'thdy@itu.dk')
        , 'pagh':
        Person('Rasmus Pagh', 'R Pagh', 'https://www.itu.dk/people/pagh/')
        , 'ilya':
        Person('Ilya Razenshteyn', 'I Razenshteyn', 'http://www.ilyaraz.org/')
        , 'fran':
        Person('Francesco Silvestri', 'F Silvestri', 'http://www.dei.unipd.it/~silvestri/')
        , 'maau':
        Person('Martin Aumüller', 'M Aumüller', 'http://itu.dk/people/maau/')
        , 'jbtk':
        Person('Jakob Bæk Tejs Knudsen', 'J Knudsen', 'https://di.ku.dk/english/staff/?pure=en/persons/493157')
        , 'mika':
        Person('Michael Kapralov', 'M Kapralov', 'https://theory.epfl.ch/kapralov/')
        , 'amve':
        Person('Ameya Velingker', 'A Velingker', 'http://www.cs.cmu.edu/~avelingk/')
        , 'dawo':
        Person('David P. Woodruff', 'D Woodruff', 'http://www.cs.cmu.edu/~dwoodruf/')
        , 'amza':
        Person('Amir Zandieh', 'A Zandieh', 'https://people.epfl.ch/amir.zandieh')
    }
    me = authors['thdy']
    coauthors = [p for k, p in authors.items() if k != 'thdy']
    coauthors.sort(key=lambda p: p.name.split()[-1])

    conferences = {
        '': Conference('Not Published'),
        'arxiv': Conference('ArXiv'),
        'subm': Conference('Submitted'),
        'soda': Conference('ACM-SIAM Symposium on Discrete Algorithms'),
        'pods': Conference('ACM Symposium on Principles of Database Systems'),
        'focs': Conference('IEEE Symposium on Foundations of Computer Science'),
        'icalp': Conference('EATCS International Colloquium on Automata, Languages and Programming')
    }

    papers = [
        Paper(
            'supermajority',
            'Subsets and Supermajorities: Optimal Hashing-based Set Similarity Search',
            ['thdy'],
            open('abstracts/supermajority').read(),
            2019,
            'subm',
            '',
            files=[
                File('pdf', 'papers/supermajority.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1904.04045'),
                File('slides', 'https://docs.google.com/presentation/d/1qB4M7oEHmeRs8b0x1u0O3NS7PozydhVgqI059Bi_2QE'),
                File('blog post', 'https://thomasahle.com/blog/sets.html')
            ]),
        Paper(
            'tensorsketch-joint',
            'Oblivious Sketching of High-Degree Polynomial Kernels',
            ['thdy', 'mika', 'jbtk','pagh','amve','dawo','amza'],
            open('abstracts/tensorsketch-joint').read(),
            2019,
            'soda',
            '',
            files=[
                File('pdf', 'papers/tensorsketch-joint.pdf'),
                File('arxiv', 'https://arxiv.org/abs/1909.01410v3')
            ]),
        Paper(
            'lasvegas',
            'Optimal Las Vegas Locality Sensitive Data Structures',
            ['thdy'],
            open('abstracts/lasvegas').read(),
            2017,
            'focs',
            'Updated Jun 2018',
            files=[
                File('pdf', 'papers/lasvegas.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1704.02054'),
                File(
                    'slides',
                    'https://docs.google.com/presentation/d/1HgNCrDFdIZ_oh2RRedmd1qOHgWxk74HSkUCMGcuUbP8/edit'
                )
            ]),
        Paper(
            'output-sensitive',
            'Parameter-free Locality Sensitive Hashing for Spherical Range Reporting',
            ['thdy', 'maau', 'pagh'],
            open('abstracts/output-sensitive-lsh-for-knn').read(),
            2017,
            'soda',
            '',
            files=[
                File('pdf', 'papers/output-sensitive-lsh-for-knn.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1605.02673'),
                File(
                    'slides',
                    'https://docs.google.com/presentation/d/1UOFCN1Eujr4sCQdYqdwn_8D8pNCJHEyzw9Yd_BW6bh4'
                )
            ]),
        Paper(
            'mips',
            'On the Complexity of Inner Product Similarity Join',
            ['thdy', 'pagh', 'ilya', 'fran'],
            open('abstracts/mips').read(),
            2016,
            'pods',
            '',
            files=[
                File('pdf', 'papers/mips.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1510.02824'),
                File(
                    'slides',
                    'https://docs.google.com/presentation/d/1y_BZ5Ctyn67Vam8rWzrskMkrkc-yIelCQsFLz1g4_bM'
                )
            ])
    ]

    manuscripts = [
        Paper(
            'tensorsketch2',
            'Almost Optimal Tensor Sketch', ['thdy', 'jbtk'],
            open('abstracts/tensorsketch2').read(),
            2019,
            '',
            '',
            files=[
                File('arxiv', 'https://arxiv.org/abs/1909.01821'),
                File('pdf', 'papers/tensorsketch2.pdf'),
            ]),
        Paper(
            'verification',
            'It is NP-hard to verify an LSF on the sphere',
            ['thdy'],
            'We show a reduction from verifying that an LSF family `covers` the sphere, in the sense of Las Vegas LSF, to 3-sat.',
            2017,
            '',
            '',
            files=[
                File('pdf', 'papers/verification.pdf')
            ]),
        Paper(
            'minhash',
            'Minhash without false negatives',
            ['thdy'],
            open('abstracts/minhash').read(),
            2017,
            '', # Not published
            'Master thesis',
            files=[
                File('pdf', 'papers/minhash.pdf'),
                File(
                    'slides',
                    'https://docs.google.com/presentation/d/1oT-L5EON7ZCVScuYRoMzVHxf7x2Y6cig8deQXMmpP0I/edit?usp=sharing'
                )
            ]),
        Paper(
            'tails',
            'Asymptotic Tail Bounds and Applications',
            ['thdy'],
            open('abstracts/tails').read(),
            2017,
            '', # Not published
            '',
            files=[
                File('pdf', 'papers/tails.pdf'),
            ]),
    ]

    teachings = [
            Teaching('Practical Concurrent and Parallel Programming', 'pcpp', 2019, 'In this MSc course you learn how to write correct and efficient concurrent and parallel software, primarily using Java, on standard shared-memory multicore hardware. The course covers basic mechanisms such as threads, locks and shared memory as well as more advanced mechanisms such as parallel streams for bulk data, transactional memory, message passing, and lock-free data structures with compare-and-swap. It covers concepts such as atomicity, safety, liveness and deadlock. It covers how to measure and understand performance and scalability of parallel programs. It covers methods to find bugs in concurrent programs.', 'teaching/pcpp2019')
            ]

    media = [
        Newspaper(
            '',
            'Stibo', 'August 2016',
            'The Stibo-Foundation supports IT-talents',
            'http://www.stibo.com/da/2016/08/26/the-stibo-foundation-supports-it-talents/',
            'The announcement of my winning the Stibo Travel grant.'
            , files=[]
        ),
        Newspaper(
            'Bidwell, Jonni',
            'Linux Format', 'January 2016',
            'Python: Sunfish chess engine',
            'http://www.pressreader.com/australia/linux-format/20151222/282802125292910',
            'Article about my Sunfish chess software.'
            , files=[File('pdf','papers/sunfish.pdf')]
        ),
        Newspaper(
            '',
            'Computerworld', 'June 2015',
            'The National Team at the Programming World Cup',
            'http://www.computerworld.dk/art/234196',
            'Coverage of my teams participation in the ICPC World Finals.'
            , files=[]
            ),
        Newspaper(
            'Elkær, Mads',
            'Computerworld', 'October 2013',
            'Denmark\'s Three Greatest Programmers',
            'https://www.computerworld.dk/art/228544/her-er-danmarks-tre-bedste-programmoerer',
            '', files=[]
            )
    ]

    awards = [
        Award('Research Travel Award', '', 'Stibo-Foundation', 2016,
              'Given to just two Danish students a year, to collaborate in research abroad.'),
        Award('Northwestern Europe Regional Programming Contest', '1st',
              'Association for Computing Machinery', 2014,
              'With my team Lambdabamserne, becoming the first ever Danish team to qualify for the ACM wold finals.'),
        Award('Danish National Programming Champion', '1st',
              'Netcompany', '2013, 2014',
              'Algorithm competition known as "DM i Programmering" '),
        Award('Oxford Computer Science Competition', '1st',
              'University of Oxford', 2013,
              'For my Numberlink solving software, giving the first fixed parameter polynomial algorithm for the problem.'),
        Award('Demyship', '',
            'Magdalen College', '2010, 2011',
            'A historic scholarship awarded to the top students each year.'),
        Award('Les Trophées du Libre', '1st',
              'Free Software Foundation Europe', 2007,
              'For my work on the PyChess free software chess suite.'),
    ]

    jobs = [
        Job('Chief Machine Learning Officer', 'SupWiz', '2017 - 2018',
            '''I co-founded an NLP start-up with academics from University of Copenhagen.
               At SupWiz I lead a team of four in developing our chatbot software and putting it into production at 3 of the largest Danish IT companies. (Now many more.)
               In 2019 the chatbot won the most prestigious prize given by Innovation Fund Denmark.
               I was also responsible for our hiring efforts, interviewing dozens and employing 4 engineers over a 5 month period.
               ''', True),
        Job('Teaching', 'IT University of Copenhagen', '2015 - 2019',
            '''In 2019 I co-designed and taught the Parallel and Concurrent Programming course to 140 master students.
               Earlier years I assisted in various algorithms design classes.
            ''', True),
        Job('Teaching', 'University of Copenhagen', '2014',
            '''I assisted in teaching algorithms to more than 200 bachelor students.''',
            True),
        Job('Software Engineer', 'Sophion Bioscience', '2013 - 2014',
            '''I lead a project developing internal debugging tools for sifting through gigabytes of data/second on Sophion's ion channel screening machines.''',
            False),
        Job('Software Engineer Intern', 'Palantir', '2012',
            '''Ported the Metropolis ontological time-series system (now Foundry) to the web.
               Acted as coordinating hub for 10 people deciding API and network infrastructure.''',
               False),
        Job('Software Engineer', 'XION', '2010-2012',
            '''I Developed the most popular Danish TV-listings app for Android at the time.
               This included writing scrapers to gather TV information from 100s of TV-stations (consensually) and serving it on a public facing API.''',
               False),
    ]

    oss = [
            Job('Project Owner', 'PyChess', '2006 - current', 'Developed the most used chess client and engine for the Linux desktop. Currently the 7th most used interface on the Free Internet Chess Server. Translated to more than 35 languages. I lead a team of 4-8 developers and designers. In 2009 we won Les Trophées du Libre in Paris. The project is under the Gnu Public License and has been used by people all over the world for research projects and other experiments.',
                True),
            ]
