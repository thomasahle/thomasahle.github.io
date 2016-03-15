from jinja2 import Environment, FileSystemLoader
from collections import namedtuple

Person = namedtuple('Person', ['name', 'abrv', 'website', 'photo'])
Person.__new__.__defaults__ = ('Anonymous', '', '')
Paper = namedtuple('Paper', ['title', 'authors', 'year', 'conference', 'comment', 'files'])
Conference = namedtuple('Conference', ['name'])
File = namedtuple('File', ['format', 'href'])

class Vars:
    coauthors = {
        'thdy': Person('Thomas Dybdahl Ahle', 'TA', '/', 'me.png'),
        'pagh': Person('Rasmus Pagh', 'R Pagh', 'https://www.itu.dk/people/pagh/'),
        'ilya': Person('Ilya Razenshteyn', 'I Razenshteyn', 'http://www.ilyaraz.org/'),
        'fran': Person('Francesco Silvestri', 'F Silvestri', 'http://itu.dk/people/fras/')
    }
    me = coauthors['thdy']

    conferences = {
        'arxiv': Conference('ArXiv'),
        'subm': Conference('Submitted'),
        'soda': Conference('Proceedings of Symposium on Discrete Algorithms'),
        'pods': Conference('Symposium on Principles of Database Systems')
    }

    papers = [
        Paper(
            'On the Complexity of Inner Product Similarity Join',
            ['thdy', 'pagh', 'ilya', 'fran'],
            2016, 'pods', '', files=[
                File('pdf', 'papers/mips/paper.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1510.02824'),
                File('slides', 'https://docs.google.com/presentation/d/1y_BZ5Ctyn67Vam8rWzrskMkrkc-yIelCQsFLz1g4_bM/edit?usp=sharing')
            ])
    ]

env = Environment(loader=FileSystemLoader('.'))
template = env.get_template('template.html')
print(template.render(Vars.__dict__))

