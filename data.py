from collections import namedtuple
import datetime

Person = namedtuple('Person', ['name', 'abrv', 'href', 'photo', 'email'])
Person.__new__.__defaults__ = ('Anonymous', '', '')
Paper = namedtuple(
    'Paper',
    ['tag', 'title', 'authors', 'abstract', 'year', 'conference', 'comment', 'files', 'featured', 'new'])
Conference = namedtuple('Conference', ['name'])
Journal = namedtuple('Journal', ['name'])
Teaching = namedtuple('Teaching', ['name', 'abrv', 'year', 'description', 'href'])
File = namedtuple('File', ['format', 'href'])
Newspaper = namedtuple('Newspaper', ['author', 'name', 'date', 'title', 'href', 'description', 'files'])
Award = namedtuple('Award', ['name', 'place', 'giver', 'date', 'description'])
Job = namedtuple('Job', ['title', 'company', 'date', 'description', 'academic'])


class Vars:
    now = datetime.datetime.now()
    # Whether to show all papers by default
    show_all = False

# Whether to show all papers by default
    authors = {
        'thdy':
        Person('Thomas Dybdahl Ahle', 'TA', '/', 'static/potrait.jpg', 'thomas@ahle.dk')
        , 'pagh':
        Person('Rasmus Pagh', 'R Pagh', 'http://rasmuspagh.net/')
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
        , 'mtho':
        Person('Mikkel Thorup', 'M Thorup', 'http://hjemmesider.diku.dk/~mthorup/')
        , 'aama':
        Person('Anders Åmand', 'A Aamand', 'https://di.ku.dk/english/staff/?pure=en/persons/433494')
        , 'mabr':
        Person('Mikkel Abrahamsen', 'M Abrahamsen', 'https://sites.google.com/view/mikkel-abrahamsen')
        , 'pe2m':
        Person('Peter M. R. Rasmussen', 'P Rasmussen', 'https://di.ku.dk/english/staff/?pure=en/persons/462256')
        , 'sahark':
        Person('Sahar Karimi', 'S Karimi', 'https://scholar.google.com/citations?user=bPiE44QAAAAJ&hl=en')
        , 'ptpt':
        Person('Peter Tang', 'P Tang', 'https://dl.acm.org/profile/81452604699')
        , 'henry':
        Person('Henry Ling-Hei Tsang', 'H Tsang', 'https://math.osu.edu/people/tsang.79')
    }
    me = authors['thdy']
    coauthors = [p for k, p in authors.items() if k != 'thdy']
    coauthors.sort(key=lambda p: p.name.split()[-1])

    conferences = {
        '': Conference('Not Published'),
        'arxiv': Conference('ArXiv'),
        'subm': Conference('In Review'),
        'soda': Conference('ACM-SIAM Symposium on Discrete Algorithms'),
        'pods': Conference('ACM Symposium on Principles of Database Systems'),
        'sisap': Conference('International Conference on Similarity Search and Applications'),
        'focs': Conference('IEEE Symposium on Foundations of Computer Science'),
        'icalp': Conference('EATCS International Colloquium on Automata, Languages and Programming'),
        'socg': Conference('Symposium on Computational Geometry'),
        'Stat. Prob. Lett.': Journal('Statistics & Probability Letters'),
    }

    papers = [
        Paper(
            'cqr',
            'Clustering&nbsp;the&nbsp; Sketch&nbsp;: A Novel Approach to Embedding&nbsp;Table&nbsp;Compression',
            ['henry', 'thdy'],
            open('abstracts/cqr').read(),
            2022,
            'subm',
            'Updated Jan 2023',
            files=[
                File('arxiv', 'https://arxiv.org/abs/2210.05974'),
                File('pdf', 'papers/cqr.pdf'),
                ],
            featured=False,
            new=True
            ),
        Paper(
            'favour',
            'Fast Variance Operator for Uncertainty Rating',
            ['thdy', 'sahark', 'ptpt'],
            open('abstracts/favour').read(),
            2022,
            'subm',
            '',
            files=[
                File('pdf', 'papers/favour.pdf'),
                File('video', 'https://youtu.be/Ct63ikA2q-c'),
                ],
            featured=False,
            new=True
            ),
        Paper(
            'tiling',
            'Tiling with Squares and Packing Dominos in Polynomial Time',
            ['aama', 'mabr', 'thdy', 'pe2m'],
            open('abstracts/tiling').read(),
            2022,
            'socg',
            '',
            files=[
                File('arxiv', 'https://arxiv.org/abs/2011.10983'),
                File('pdf', 'papers/tiling.pdf'),
                File('slides', 'papers/tiling_mikkel.pdf'),
                ],
            featured=False,
            new=False
            ),
        Paper(
            'bi-moments',
            'Sharp and Simple Bounds for the raw Moments&nbsp;of&nbsp;the&nbsp;Binomial&nbsp;and&nbsp;Poisson Distributions',
            ['thdy'],
            open('abstracts/bi-moments').read(),
            2022,
            'Stat. Prob. Lett.',
            '',
            files=[
                File('journal', 'https://www.sciencedirect.com/science/article/abs/pii/S0167715221002662'),
                File('arxiv', 'https://arxiv.org/abs/2103.17027'),
                File('pdf', 'papers/bi-moments.pdf'),
                ],
            featured=False,
            new=False,
            ),
        Paper(
            'tcu',
            'Similarity Search with Tensor Core Units',
            ['thdy', 'fran'],
            open('abstracts/tcu').read(),
            2020,
            'sisap',
            '',
            files=[
                File('arxiv', 'https://arxiv.org/abs/2006.12608'),
                File('pdf', 'papers/tcu.pdf'),
                File('video', 'https://youtu.be/Ts8ZB-LsGQ4')
                ],
            featured=False,
            new=False
            ),
        Paper(
            'p1',
            'On the Problem of p₁⁻¹ in Locality‑Sensitive&nbsp;Hashing',
            ['thdy'],
            open('abstracts/p1').read(),
            2020,
            'sisap',
            '',
            files=[
                File('pdf', 'papers/p1.pdf'),
                File('arxiv', 'https://arxiv.org/abs/2005.12065'),
                File('video', 'https://youtu.be/o0jqXjUF_d4')
                ],
            featured=False,
            new=False
            ),
        Paper(
            'supermajority',
            'Subsets and Supermajorities: Optimal&nbsp;Hashing‑based Set&nbsp;Similarity&nbsp;Search',
            ['thdy', 'jbtk'],
            open('abstracts/supermajority').read(),
            2020,
            'focs',
            '',
            files=[
                File('pdf', 'papers/supermajority.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1904.04045'),
                File('slides', 'https://docs.google.com/presentation/d/1qB4M7oEHmeRs8b0x1u0O3NS7PozydhVgqI059Bi_2QE'),
                #File('short', 'papers/supshort.pdf'),
                File('blog post', 'https://thomasahle.com/blog/sets.html'),
                File('video', 'https://www.youtube.com/watch?v=Bp3afo2YYCQ')
                ],
            featured=True,
            new=False
            ),
        Paper(
            'tensorsketch-joint',
            'Oblivious Sketching of High‑Degree&nbsp;Polynomial&nbsp;Kernels',
            ['thdy', 'mika', 'jbtk','pagh','amve','dawo','amza'],
            open('abstracts/tensorsketch-joint').read(),
            2020,
            'soda',
            'Merged from "Almost Optimal Tensor&nbsp;Sketch"',
            files=[
                File('pdf', 'papers/tensorsketch-joint.pdf'),
                File('arxiv', 'https://arxiv.org/abs/1909.01410'),
                File('slides', 'papers/TensorSketch_Amir.pdf'),
                File('slides 2', 'https://docs.google.com/presentation/d/1cPMMaZ2kuVI1PEJrBjKxLq6k8GYP7JUh_Q23gBw1LGA/edit?usp=sharing'),
                ],
            featured=True,
            new=False
            ),
        Paper(
            'lasvegas',
            'Optimal Las Vegas Locality&nbsp;Sensitive&nbsp;Data&nbsp;Structures',
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
                ],
            featured=True,
            new=False
            ),
        Paper(
            'output-sensitive',
            'Parameter-free Locality‑Sensitive Hashing for Spherical&nbsp;Range&nbsp;Reporting',
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
                ],
            featured=False,
            new=False
            ),
        Paper(
            'mips',
            'On the Complexity of Inner&nbsp;Product&nbsp;Similarity&nbsp;Join',
            ['thdy', 'pagh', 'ilya', 'fran'],
            open('abstracts/mips').read(),
            2016,
            'pods',
            '',
            files=[
                File('pdf', 'papers/mips.pdf'),
                File('poster', 'papers/PodsPoster.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1510.02824'),
                File(
                    'slides',
                    'https://docs.google.com/presentation/d/1y_BZ5Ctyn67Vam8rWzrskMkrkc-yIelCQsFLz1g4_bM'
                )
                ],
            featured=True,
            new=False
            ),
    ]

    manuscripts = [
        Paper(
            'minner',
            'Minner: Improved Similarity Estimation and Recall on&nbsp;MinHashed&nbsp;Databases',
            ['thdy'],
            open('abstracts/minner').read(),
            2021,
            'subm',
            '',
            files=[
                File('pdf', 'papers/minner.pdf'),
                File('slides', 'https://docs.google.com/presentation/d/e/2PACX-1vTdvK58YN2UcDYbEPM-BOEUwtChKekUvu08Ezz07810dn54bJliaxSZbaapqtHmojHdD_aK-sa44mWp/pub?start=false&loop=false&delayms=5000')
                ],
            featured=True,
            new=False
            ),
        Paper(
            'mersenne',
            'The Power of Hashing with Mersenne&nbsp;Primes',
            ['thdy', 'jbtk', 'mtho'],
            open('abstracts/mersenne').read(),
            2021,
            'subm',
            'Updated May 2021',
            files=[
                File('arxiv', 'https://arxiv.org/abs/2008.08654'),
                File('pdf', 'papers/mersenne.pdf'),
                ],
            featured=True,
            new=False
            ),
        Paper(
            'tensorsketch2',
            'Almost Optimal Tensor Sketch', ['thdy', 'jbtk'],
            open('abstracts/tensorsketch2').read(),
            2019,
            '',
            'Merged into "Oblivious Sketching of High‑Degree Polynomial Kernels"',
            files=[
                File('arxiv', 'https://arxiv.org/abs/1909.01821'),
                File('pdf', 'papers/tensorsketch2.pdf'),
                ],
            featured=False,
            new=False,
            ),
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
                ],
            featured=False,
            new=False,
            ),
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
                ],
            featured=False,
            new=False,
            ),
        Paper(
            'tails',
            'Asymptotic Tail Bounds and Applications',
            ['thdy'],
            open('abstracts/tails').read(),
            2017,
            conference='', # Not published
            comment='',
            files=[
                File('pdf', 'papers/tails.pdf'),
                ],
            featured=False,
            new=False,
            ),
    ]
    for pubs in [papers, manuscripts]:
        for paper in pubs:
            paper.files.sort()
    has_featured = lambda articles: any(p.featured or p.new for p in articles)

    teachings = [
            Teaching('Practical Concurrent and Parallel Programming', 'pcpp', 2019, 'MSc course on correct and efficient concurrent and parallel software, primarily using Java, on standard shared-memory multicore hardware.', 'teaching/pcpp2019')
            ]

    media = [
        Newspaper(
            'Jon Lund',
            'Prosa', 'May 2021',
            'En ulv i fåreklæder',
            'https://www.prosa.dk/artikel/en-ulv-i-faareklaeder/',
            'I was interviewed by the IT workers union on Google\'s FLoC system and the use of SimHash.'
            , files=[]
        ),
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
            'Denmark\'s Three Greatest Programmers.',
            'https://www.computerworld.dk/art/228544/her-er-danmarks-tre-bedste-programmoerer',
            '', files=[]
            )
    ]

    awards = [
        Award('Research Travel Award', '', 'Stibo-Foundation', 2016,
              'Highly competitive scholarship, awarded to only two Danish students per year for research collaboration abroad.'),
        Award('Northwestern Europe Regional Programming Contest', '1st',
              'Association for Computing Machinery', 2014,
              'As a member of Lambdabamserne, my team and I made history by becoming the first ever Danish team to qualify for the ACM world finals after winning 1st place out of all universities in North Western Europe.'),
        Award('Danish National Programming Champion', '1st',
              'Netcompany', '2013, 2014',
              'Crowned Danish National Programming Champion twice, taking the top spot in the Algorithm competition known as "DM i Programmering"'),
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
        Job('Research Scientist', 'Meta', '2020 - Present',
            '''
I lead and co-founded the Machine Learning Efficiency group, focused on scaling AI across the organization.
We developed several new algorithms, including: (1) a dynamic clustering algorithm for embedding tables, which reduced the size of internal recommendation systems by 50%; (2) a fast inference algorithm for Bayesian Neural Networks based on sketching; and (3) lower memory and latency transformer architectures for Facebook Assistant, enabling mobile deployment of language models.
In 2023 I moved to CoreML, working on Ranking and Retrieval for Video Recommendation.
            ''', academic=False),
        Job('Researcher', 'Basic Algorithms Research Copenhagen (BARC)', '2019 - 2020',
            '''
Researcher at the Basic Algorithms Research Copenhagen (BARC), a leading center for fundamental algorithmic research, headed by Mikkel Thorup. I developed a number of new algorithms, publishing papers at the top algorithmic conferences, SODA and FOCS. Some selected results: (1) An optimal LSH data structure for set similarity search. (2) A very fast sketching based algorithm for high-degree polynomial machine learning kernels. (3) Methods for doing similarity search with tensor cores.
            ''', academic=False),
        Job('Chief Machine Learning Officer', 'SupWiz', '2017 - 2018',
            '''
I lead a team developing our chatbot software and putting it into production at three of Denmark’s largest IT firms. (There are now many more.) We used a combination of traditional symbolic AI and modern (at the time) sentence embeddings. The chatbot was awarded the most prestigious prize by the Innovation Fund Denmark in 2019. I was also in charge of our hiring efforts, interviewing dozens of candidates and hiring four engineers over the course of five months.
            ''', academic=False),
        Job('Teaching', 'IT University of Copenhagen', '2015 - 2019',
            '''In 2019 I co-designed and taught the Parallel and Concurrent Programming course to 140 master students.
               Earlier years I assisted in various algorithms design classes.
            ''', academic=True),
        Job('Teaching', 'University of Copenhagen', '2014',
            '''I assisted in teaching algorithms to more than 200 bachelor students.''',
            academic=True),
        Job('Software Engineer', 'Other: Sophion, Palantir, XION & Daintel', '2006 - 2014',
            '''
At Sophion Bioscience, I created debugging tools for ion channel screening machines, processing gigabytes of data per second.
At Palantir Technologies, I was responsible for the initial web port of Metropolis (now Foundry), an ontological time-series system.
At XION, I developed a TV-listing Android app serving 50,000 users, with a PHP backend aggregating information from over 100 sources, including websites, PDFs, and emails.
Lastly, at Daintel (now Cambio Healthcare Systems), I worked on ICU and medical drug handling software development in SQL and PHP.
            ''',
            academic=False),
#        Job('Software Engineer', 'Sophion Bioscience', '2013 - 2014',
#            '''
#Sophion Bioscience is a leading biotech company that specializes in developing advanced solutions for ion channel research. During my time at Sophion Bioscience, I worked on developing internal debugging tools that were used to sift through gigabytes of data per second on ion channel screening machines.Sophion Bioscience is a leading biotech company that specializes in developing advanced solutions for ion channel research. During my time at Sophion Bioscience, I worked on developing internal debugging tools that were used to sift through gigabytes of data per second on ion channel screening machines.
#            ''',
#            False),
#        Job('Software Engineer Intern', 'Palantir', '2012',
#            '''
#At Palantir Technologies, my primary responsibility was to port the Metropolis ontological time-series system (now known as Foundry) to the web, which involved a series of complex tasks that required innovative solutions and careful planning. Over the course of this project, I focused on designing data visualization tools, efficient processing pipelines, and improving the overall user experience.
#            ''',
#               False),
#        Job('Android/PHP Developer', 'XION', '2010-2012',
#            '''
#I developed robust scrapers for 100 TV station websites, extracting valuable information such as emails, PDFs, and Word documents. Utilizing this data, I designed and developed a user-centric TV-listing app for Android, catering to over 50,000 users. The app's design was based on in-depth UI research and user statistics, ensuring an intuitive and engaging user experience.
#            ''',
#               False),
    ]

    oss = [
            Job('Project Owner', 'PyChess', '2006 - current', 'I developed this chess engine and client for Linux desktop, which became the most popular way to play chess on the Free Internet Chess Server.  Through the years I have led a team of 4-8 developers and designers, as well as numerous other contributers.  Such as the volunteers who translated it to more than 35 languages.  In 2009 we won Les Trophées du Libre in Paris.',
                True),
            Job('Project Owner', 'Sunfish', '2012 - current', 'A 111 line python chess engine, which is nevertheless 2000+ rating on the online Lichess server.  Because of the simplicity and focus on teaching good AI techniques, it has become a popular project on Github with 2400+ stars and nearly 500 forks.  Sunfish was referenced in multiple early applications of neural networks to chess.  ',
                True),
            ]
