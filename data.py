from collections import namedtuple, Counter
import datetime
from dataclasses import dataclass, field
from typing import List, Optional, Sequence

@dataclass
class Person:
    name: str = 'Anonymous'
    abrv: str = ''
    href: str = ''
    photo: Optional[str] = None
    email: Optional[str] = None

@dataclass
class Paper:
    tag: str
    title: str
    authors: List[Person]
    abstract: str
    year: int
    conference: Optional[str] = None
    comment: Optional[str] = None
    files: Sequence[str] = ()
    featured: bool = False
    new: bool = False
    img: Optional[str] = None

@dataclass
class Conference:
    name: str

@dataclass
class Journal:
    name: str

@dataclass
class Teaching:
    name: str
    abrv: str
    year: int
    description: str
    href: str

@dataclass
class File:
    format: str
    href: str

@dataclass
class Newspaper:
    author: Person
    name: str
    date: str
    title: str
    href: str
    description: str
    files: Sequence[File] = ()

@dataclass
class Award:
    name: str
    place: Optional[str] = None
    giver: Optional[str] = None
    date: Optional[str] = None
    description: Optional[str] = None

@dataclass
class Job:
    title: str
    company: str
    date: str
    description: str
    academic: bool = False



class Vars:
    now = datetime.datetime.now()
    # Whether to show all papers by default
    show_all = False

# Whether to show all papers by default
    authors = {
        'thdy':
        Person('Thomas Dybdahl Ahle', 'TA', '/', 'static/ThomasFace.png', 'thomas@ahle.dk')
        , 'pagh': Person('Rasmus Pagh', 'R Pagh', 'http://rasmuspagh.net/')
        , 'ilya': Person('Ilya Razenshteyn', 'I Razenshteyn', 'http://www.ilyaraz.org/')
        , 'fran': Person('Francesco Silvestri', 'F Silvestri', 'http://www.dei.unipd.it/~silvestri/')
        , 'maau': Person('Martin Aumüller', 'M Aumüller', 'http://itu.dk/people/maau/')
        , 'jbtk': Person('Jakob Bæk Tejs Knudsen', 'J Knudsen', 'https://di.ku.dk/english/staff/?pure=en/persons/493157')
        , 'mika': Person('Michael Kapralov', 'M Kapralov', 'https://theory.epfl.ch/kapralov/')
        , 'amve': Person('Ameya Velingker', 'A Velingker', 'http://www.cs.cmu.edu/~avelingk/')
        , 'dawo': Person('David P. Woodruff', 'D Woodruff', 'http://www.cs.cmu.edu/~dwoodruf/')
        , 'amza': Person('Amir Zandieh', 'A Zandieh', 'https://people.epfl.ch/amir.zandieh')
        , 'mtho': Person('Mikkel Thorup', 'M Thorup', 'http://hjemmesider.diku.dk/~mthorup/')
        , 'aama': Person('Anders Åmand', 'A Aamand', 'https://di.ku.dk/english/staff/?pure=en/persons/433494')
        , 'mabr': Person('Mikkel Abrahamsen', 'M Abrahamsen', 'https://sites.google.com/view/mikkel-abrahamsen')
        , 'pe2m': Person('Peter M. R. Rasmussen', 'P Rasmussen', 'https://di.ku.dk/english/staff/?pure=en/persons/462256')
        , 'sahark': Person('Sahar Karimi', 'S Karimi', 'https://scholar.google.com/citations?user=bPiE44QAAAAJ&hl=en')
        , 'ptpt': Person('Peter Tang', 'P Tang', 'https://dl.acm.org/profile/81452604699')
        , 'henry': Person('Henry Ling-Hei Tsang', 'H Tsang', 'https://scholars.croucher.org.hk/scholars/ling-hei-tsang')
        , 'maxa': Person('Maxwell Aifer', 'M Aifer', 'https://quthermo.umbc.edu/group-members/graduate-students/maxwell-aifer/')
        , 'kaed': Person('Kaelan Donatella', 'K Donatella', 'https://scholar.google.com/citations?user=iM2qRCkAAAAJ&hl=en')
        , 'maxg': Person('Max Hunter Gordon', 'M H Gordon', 'https://scholar.google.co.uk/citations?user=I2W8xCoAAAAJ&hl=en')
        , 'dans': Person('Daniel Simpson', 'D Simpson', 'https://dpsimpson.github.io/')
        , 'gavc': Person('Gavin E. Crooks', 'G E Crooks', 'https://threeplusone.com/')
        , 'patc': Person('Patrick J. Coles', 'P J Coles', 'https://patcoles.com/')
        , 'sduf': Person('Sam Duffield', 'S Duffield', 'https://twitter.com/Sam_Duffield')
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
        'neurips': Conference('Advances in Neural Information Processing Systems (NeurIPS)'),
        'Stat. Prob. Lett.': Journal('Statistics & Probability Letters'),
        'natcom': Journal('Nature Communications'),
        'unconv': Journal('Unconventional Computing'),
    }

    papers = [
        Paper(
            'pcb',
            'Thermodynamic Computing System for AI Applications',
            ['thdy', 'maxa', 'kaed', 'patc'],  # Add other authors if needed
            open('abstracts/pcb').read(),
            2025,
            'natcom',
            '',
            files=[
                File('journal', 'https://www.nature.com/articles/s41467-025-59011-x'),
                File('arxiv', 'https://arxiv.org/pdf/2312.04836'),
            ],
            featured=True,
            new=True,
            img='pcb.png',  # Add image file if available
            ),
        Paper(
            'tla',
            'Thermodynamic Linear Algebra',
            ['maxa', 'kaed', 'maxg', 'thdy', 'dans', 'gavc', 'patc'],
            open('abstracts/tla').read(),
            2024,
            'unconv',
            '',
            files=[
                File('arxiv', 'https://arxiv.org/abs/2308.05660'),
                ],
            featured=False,
            new=True,
            img='tla.png',
            ),
        Paper(
            'cqr',
            'Clustering&nbsp;the&nbsp;Sketch: Dynamic&nbsp;Compression for Embedding&nbsp;Tables',
            ['henry', 'thdy'],
            open('abstracts/cqr').read(),
            2023,
            'neurips',
            #'Updated Oct 2023',
            '',
            files=[
                File('arxiv', 'https://arxiv.org/abs/2210.05974'),
                File('pdf', 'papers/cqr.pdf'),
                File('website', 'https://thomasahle.github.io/cce'),
                File('github', 'https://github.com/thomasahle/cce'),
                File('poster', 'papers/cce_poster.pdf'),
                ],
            featured=True,
            new=False,
            #img='dalle.png',
            img='cqr.png',
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
            new=False,
            img='domino.png',
            ),
        Paper(
            'bi-moments',
            'Sharp and Simple Bounds for the raw Moments of the Binomial and Poisson Distributions',
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
            img='urn.png',
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
            new=False,
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
            new=False,
            img='sets.png',
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
            new=False,
            #img='obliv.png',
            img='cube.png',
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
            new=False,
            img='necklace.png',
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
            featured=False,
            new=False,
            img='mips.png',
            ),
    ]

    manuscripts = [
        Paper(
            'expm',
            'Thermodynamic Matrix Exponentials and Thermodynamic Parallelism',
            ['maxa', 'sduf', 'thdy', 'patc'],
            open('abstracts/expm').read(),
            2023,
            '',
            '',
            files=[
                File('arxiv', 'https://arxiv.org/abs/2311.12759'),
                ],
            featured=False,
            new=True,
            img='expm.png',
            ),
        Paper(
            'favour',
            'Fast Variance Operator for Uncertainty Rating',
            ['thdy', 'sahark', 'ptpt'],
            open('abstracts/favour').read(),
            2022,
            'arxiv',
            '',
            files=[
                File('pdf', 'papers/favour.pdf'),
                File('video', 'https://youtu.be/Ct63ikA2q-c'),
                ],
            featured=False,
            new=False,
            ),
        Paper(
            'minner',
            'Minner: Improved Similarity Estimation and Recall on&nbsp;MinHashed&nbsp;Databases',
            ['thdy'],
            open('abstracts/minner').read(),
            2021,
            '',
            '',
            files=[
                File('pdf', 'papers/minner.pdf'),
                File('slides', 'https://docs.google.com/presentation/d/e/2PACX-1vTdvK58YN2UcDYbEPM-BOEUwtChKekUvu08Ezz07810dn54bJliaxSZbaapqtHmojHdD_aK-sa44mWp/pub?start=false&loop=false&delayms=5000')
                ],
            featured=True,
            new=False,
            img='minner.png',
            ),
        Paper(
            'mersenne',
            'The Power of Hashing with Mersenne&nbsp;Primes',
            ['thdy', 'jbtk', 'mtho'],
            open('abstracts/mersenne').read(),
            2021,
            '',
            'Updated May 2021',
            files=[
                File('arxiv', 'https://arxiv.org/abs/2008.08654'),
                File('pdf', 'papers/mersenne.pdf'),
                ],
            featured=True,
            new=False,
            img='mersenne.png',
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
        Paper(
            'donkey',
            'Donkeys to Monoids: A Computational&nbsp;Framework for Dynamic&nbsp;Semantics',
            ['thdy'],
            open('abstracts/donkey').read(),
            2013,
            conference='', # Not published
            comment='Bachelor thesis',
            files=[
                File('pdf', 'papers/donkey.pdf'),
                File('github', 'https://github.com/thomasahle/donkey'),
                ],
            featured=False,
            new=False,
            ),
    ]
    for pubs in [papers, manuscripts]:
        for paper in pubs:
            paper.files = sorted(paper.files, key=lambda file: file.format)
    has_featured = lambda articles: any(p.featured or p.new for p in articles)

    # auth_cnt = Counter({a:0 for a in coauthors})
    # auth_cnt += Counter([x[key] for p in papers for key in p.authors])
    # sorted_coauthors = [a for a, _cnt in auth_cnt.most_common()]

    teachings = [
            Teaching('Practical Concurrent and Parallel Programming', 'pcpp', 2019, 'MSc course on correct and efficient concurrent and parallel software, primarily using Java, on standard shared-memory multicore hardware.', 'teaching/pcpp2019')
            ]

    media = [
        Newspaper(
            'Brian Bailey',
            'Semiconductor Engineering', 'July 2025',
            'Multi-Modal AI In EDA Development Flows',
            'https://semiengineering.com/multi-modal-ai-in-eda-development-flows/',
            'Interview on how our company, Normal Computing, is using AI to improve chip design and development.'
            , files=[]
        ),
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
            'Denmark\'s Three Greatest Programmers',
            'https://www.computerworld.dk/art/228544/her-er-danmarks-tre-bedste-programmoerer',
            '', files=[]
            )
    ]

    awards = [
        Award('Research Travel Award', '', 'Stibo-Foundation', 2016,
              'Highly competitive scholarship, given to just two Danish students a year to collaborate in research abroad.'),
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
        Job('Head of Machine Learning', 'Normal Computing', '2023 - Present',
            '''
I developed the machine learning strategy for Normal Computing, which had previously been focused on blue sky research in thermodynamic computing.
I built the team from scratch, hiring 10 engineers and researchers from top companies and universities.
We developed an AI system for the US's largest chip manufacturer, which allowed creating formal models for new chips, which previously took months, in just a few hours.
While at Normal Computing I published five papers on algorithms for Thermodynamic computing as well as one about a new approach to large context language models.
            ''', False),
        Job('Research Scientist', 'Meta', '2020 - 2023',
            '''
I lead and co-founded the Machine Learning Efficiency group, a five-person internal applied research group focused on scaling AI across the organization.
During the first year, we developed a new hashing-based algorithm that reduced the size of the internal recommendation systems by 50%.
In another project, we devised a new inference algorithm for Bayesian Neural Networks, allowing the Integrity team to deploy well-calibrated models directly on customer devices.
Both projects also resulted in publications at top-tier conferences.
Finally, we worked on scaling transformer models, lowering the memory requirement and inference time of Facebook Assistant through the use of embedding table compression, smart attention, and a variety of other tricks.
            ''', False),
        Job('Chief Machine Learning Officer', 'SupWiz', '2017 - 2018',
            '''
            I co-founded an NLP start-up with University of Copenhagen academics.
At SupWiz, I lead a four-person team in developing our chatbot software and putting it into production at three of Denmark's largest IT firms.
(There are now many more.) We used a combination of traditional symbolic AI and modern (at the time) sentence embeddings.
The chatbot was awarded the most prestigious prize by the Innovation Fund Denmark in 2019.
I was also in charge of our hiring efforts, interviewing dozens of candidates and hiring four engineers over the course of five months.
            ''', False),
        Job('Teaching', 'IT University of Copenhagen', '2015 - 2019',
            '''In 2019 I co-designed and taught the Parallel and Concurrent Programming course to 140 master students.
               Earlier years I assisted in various algorithms design classes.
            ''', True),
        Job('Teaching', 'University of Copenhagen', '2014',
            '''I assisted in teaching algorithms to more than 200 bachelor students.''',
            True),
        Job('Software Engineer', 'Other: Sophion, Palantir and XION', '2010 - 2014',
            '''
            Through various software engineering jobs, I have gained broad exposure to the different areas of software development. At Sophion Bioscience, I developed internal debugging tools for sifting through gigabytes of data/second on ion channel screening machines. At Palantir, I ported the Metropolis ontological time-series system (now Foundry) to the web, designing data visualization and efficient processing pipelines. At XION, I Developed the most popular Danish TV listings app for Android at the time, based on data scraped (consensually) from hundreds of TV-station websites.
            ''',
            False),
        # Job('Software Engineer', 'Sophion Bioscience', '2013 - 2014',
        #     '''I lead a project developing internal debugging tools for sifting through gigabytes of data/second on Sophion's ion channel screening machines.''',
        #     False),
        # Job('Software Engineer Intern', 'Palantir', '2012',
        #     '''Ported the Metropolis ontological time-series system (now Foundry) to the web.
        #        Acted as coordinating hub for 10 people deciding API and network infrastructure.''',
        #        False),
        # Job('Software Engineer', 'XION', '2010-2012',
        #     '''I Developed the most popular Danish TV-listings app for Android at the time.
        #        This included writing scrapers to gather TV information from 100s of TV-stations (consensually) and serving it on a public facing API.''',
        #        False),
    ]

    oss = [
            Job('Project Owner', 'PyChess', '2006 - current', 'I developed this chess engine and client for Linux desktop, which became the most pupular way to play chess on the Free Internet Chess Server.  Through the years I have lead a team of 4-8 developers and designers, as well as numerous other contributers.  Such as the volunteers who translated it to more than 35 languages.  In 2009 we won Les Trophées du Libre in Paris.',
                True),
            Job('Project Owner', 'Sunfish', '2012 - current', 'A 111 line python chess engine, which is nevertheless 2000+ rating on the online Lichess server.  Because of the simplicity and focus on teaching good AI techniques, it has become a popular project on Github with 2400+ stars and nearly 500 forks.  Sunfish was referenced in multiple early applications of neural networks to chess.  ',
                True),
            ]
