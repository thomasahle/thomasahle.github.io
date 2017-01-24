from jinja2 import Environment, FileSystemLoader
from collections import namedtuple

Person = namedtuple('Person', ['name', 'abrv', 'website', 'photo'])
Person.__new__.__defaults__ = ('Anonymous', '', '')
Paper = namedtuple('Paper', ['title', 'authors', 'abstract', 'year', 'conference', 'comment', 'files'])
Conference = namedtuple('Conference', ['name'])
File = namedtuple('File', ['format', 'href'])
Newspaper = namedtuple('Newspaper', ['name', 'date', 'title', 'href'])

class Vars:
    coauthors = {
        'thdy': Person('Thomas Dybdahl Ahle', 'TA', '/', 'me.png'),
        'pagh': Person('Rasmus Pagh', 'R Pagh', 'https://www.itu.dk/people/pagh/'),
        'ilya': Person('Ilya Razenshteyn', 'I Razenshteyn', 'http://www.ilyaraz.org/'),
        'fran': Person('Francesco Silvestri', 'F Silvestri', 'http://itu.dk/people/fras/'),
        'maau': Person('Martin Aumüller', 'M Aumüller', 'http://itu.dk/people/maau/'),
    }
    me = coauthors['thdy']

    conferences = {
        '': Conference('Not Published'),
        'arxiv': Conference('ArXiv'),
        'subm': Conference('Submitted'),
        'soda': Conference('Proceedings of Symposium on Discrete Algorithms'),
        'pods': Conference('Symposium on Principles of Database Systems')
    }

    papers = [
        Paper(
            'Parameter-free Locality Sensitive Hashing for Spherical Range Reporting',
            ['thdy', 'maau', 'pagh'],
            open('abstracts/output-sensitive-lsh-for-knn').read(),
            2017, 'soda', '', files=[
                File('pdf', 'papers/output-sensitive-lsh-for-knn.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1605.02673'),
                File('slides', 'https://docs.google.com/presentation/d/1UOFCN1Eujr4sCQdYqdwn_8D8pNCJHEyzw9Yd_BW6bh4')
            ]),
        Paper(
            'On the Complexity of Inner Product Similarity Join',
            ['thdy', 'pagh', 'ilya', 'fran'],
            open('abstracts/mips').read(),
            2016, 'pods', '', files=[
                File('pdf', 'papers/mips/paper.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1510.02824'),
                File('slides', 'https://docs.google.com/presentation/d/1y_BZ5Ctyn67Vam8rWzrskMkrkc-yIelCQsFLz1g4_bM')
            ])
    ]

    media = [
        Newspaper(
            'Stibo', 'August 2016',
            'The Stibo-Foundation supports IT-talents',
            'http://www.stibo.com/da/2016/08/26/the-stibo-foundation-supports-it-talents/'
            ),
        Newspaper(
            'Computerworld', 'Juni 2015',
            'Med landsholdet i programmering til VM: Kodesport gør dig mere præcis',
            'http://www.computerworld.dk/art/234196'
            ),
    ]

env = Environment(loader=FileSystemLoader('.'))
template = env.get_template('template.html')
print(template.render(Vars.__dict__))

