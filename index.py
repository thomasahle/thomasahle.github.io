from jinja2 import Environment, FileSystemLoader
from collections import namedtuple

Person = namedtuple('Person', ['name', 'website', 'photo'])
Person.__new__.__defaults__ = ('Anonymous', '', '')
Paper = namedtuple('Paper', ['title', 'authors', 'year', 'conference', 'comment', 'files'])
Conference = namedtuple('Conference', ['name'])
File = namedtuple('File', ['format', 'href'])

class Vars:
    coauthors = {
        'thdy': Person('Thomas Dybdahl Ahle', '/', 'me.png'),
        'pagh': Person('Rasmus Pagh', 'https://www.itu.dk/people/pagh/'),
        'ilya': Person('Ilya Razenshteyn', 'http://www.ilyaraz.org/'),
        'fran': Person('Francesco Silvestri', 'http://itu.dk/people/fras/')
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
            2015, 'subm', '', files=[
                File('pdf', 'papers/mips/paper.pdf'),
                File('arxiv', 'http://arxiv.org/abs/1510.02824')
            ])
    ]

    join_authors = lambda ps: ', '.join(Vars.coauthors[p].name for p in ps)

env = Environment(loader=FileSystemLoader('templates'))
template = env.get_template('index.html')
print(template.render(Vars.__dict__))

