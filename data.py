from collections import namedtuple
import datetime

Person = namedtuple('Person', ['name', 'abrv', 'href', 'photo', 'email'])
Person.__new__.__defaults__ = ('Anonymous', '', '')
Paper = namedtuple(
    'Paper',
    ['tag', 'title', 'authors', 'abstract', 'year', 'conference', 'comment', 'files'])
Conference = namedtuple('Conference', ['name'])
File = namedtuple('File', ['format', 'href'])
Newspaper = namedtuple('Newspaper', ['name', 'date', 'title', 'href', 'description'])
Award = namedtuple('Award', ['name', 'place', 'giver', 'date'])
Job = namedtuple('Job', ['title', 'company', 'date'])


class Vars:
    now = datetime.datetime.now()

    authors = {
        'thdy':
        Person('Thomas Dybdahl Ahle', 'TA', '/', 'static/thomas_farve.png', 'thdy@itu.dk')
        , 'pagh':
        Person('Rasmus Pagh', 'R Pagh', 'https://www.itu.dk/people/pagh/')
        , 'ilya':
        Person('Ilya Razenshteyn', 'I Razenshteyn', 'http://www.ilyaraz.org/')
        , 'fran':
        Person('Francesco Silvestri', 'F Silvestri', 'http://www.dei.unipd.it/~silvestri/')
        , 'maau':
        Person('Martin Aumüller', 'M Aumüller', 'http://itu.dk/people/maau/')
        , 'jbtk':
        Person('Jakob Bæk Tejs Knudsen', 'J Knudsen', 'https://di.ku.dk/english/staff/?pure=en/persons/493157'),
    }
    me = authors['thdy']
    coauthors = [p for k, p in authors.items() if k != 'thdy']
    coauthors.sort(key=lambda p: p.name.split()[-1])

    conferences = {
        '': Conference('Not Published'),
        'arxiv': Conference('ArXiv'),
        'subm': Conference('Submitted'),
        'soda': Conference('Proceedings of Symposium on Discrete Algorithms'),
        'pods': Conference('Symposium on Principles of Database Systems'),
        'focs': Conference('Foundations of Computer Science'),
        'icalp': Conference('International Colloquium on Automata, Languages and Programming')
    }

    papers = [
        Paper(
            'supermajority',
            'Subsets and Supermajorities: Unifying Hashing-based Set Similarity Search', ['thdy'],
            open('abstracts/supermajority').read(),
            2019,
            'subm',
            '',
            files=[
                File('pdf', 'papers/supermajority.pdf'),
            ]),
        Paper(
            'tensorsketch2',
            'High Probability Tensor Sketch', ['thdy', 'jbtk'],
            open('abstracts/tensorsketch2').read(),
            2019,
            'subm',
            '',
            files=[
                File('pdf', 'papers/tensorsketch2.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1904.04045'),
            ]),
        Paper(
            'lasvegas',
            'Optimal Las Vegas Locality Sensitive Data Structures', ['thdy'],
            open('abstracts/lasvegas').read(),
            2017,
            'focs',
            'Updated Jun 2018',
            files=[
                File('pdf', 'papers/lasvegas.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1704.02054'),
                File(
                    'slides',
                    'https://docs.google.com/presentation/d/1HgNCrDFdIZ_oh2RRedmd1qOHgWxk74HSkUCMGcuUbP8/edit?usp=sharing'
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
                File('pdf', 'papers/mips/paper.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1510.02824'),
                File(
                    'slides',
                    'https://docs.google.com/presentation/d/1y_BZ5Ctyn67Vam8rWzrskMkrkc-yIelCQsFLz1g4_bM'
                )
            ])
    ]

    manuscripts = [
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
            'Asymptotic Tail Bound and Applications',
            ['thdy'],
            open('abstracts/tails').read(),
            2017,
            '', # Not published
            '',
            files=[
                File('pdf', 'papers/tails.pdf'),
            ]),
    ]

    media = [
        Newspaper(
            'Stibo', 'August 2016', 'The Stibo-Foundation supports IT-talents',
            'http://www.stibo.com/da/2016/08/26/the-stibo-foundation-supports-it-talents/',
            'The announcement of my winning the Stibo Travel grant.'
        ),
        Newspaper(
            'Pressreader', 'December 2015', 'Python: Sunfish chess engine',
            'http://www.pressreader.com/australia/linux-format/20151222/282802125292910',
            'A article about my Sunfish chess software.'
        ),
        Newspaper(
            'Computerworld', 'June 2015',
            'Med landsholdet i programmering til VM: Kodesport gør dig mere præcis',
            'http://www.computerworld.dk/art/234196',
            'Coverage of my teams participation in the ICPC World Finals.'
            ),
    ]

    awards = [
        Award('Travel Grant', '', 'Stibo-Foundation', 2016),
        Award('Northwestern Europe Regional Contest', '1st',
              'Association for Computing Machinery', 2014),
        Award('Oxford Computer Science Competition', '1st',
              'University of Oxford', 2013),
        Award('Les Trophées du Libre', '1st',
              'Free Software Foundation Europe', 2007),
    ]

    jobs = [
        Job('Chief Machine Learning Officer', 'SupWiz', '2017 - 2018'),
        Job('Teaching', 'IT University of Copenhagen', '2015 - 2016'),
        Job('Teaching', 'University of Copenhagen', '2014'),
        Job('Software Engineer', 'Sophion Bioscience', '2013 - 2014'),
        Job('Software Engineer Intern', 'Palantir', '2012'),
        Job('Software Engineer', 'XION', '2010-2012'),
    ]
