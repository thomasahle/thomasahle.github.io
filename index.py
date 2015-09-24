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
        'subm': Conference('Submitted'),
        'soda': Conference('Proceedings of Symposium on Discrete Algorithms')
    }

    papers = [
        Paper('On the Complexity of Maximum Inner Product Search',
            ['thdy', 'pagh', 'ilya', 'fran'], 2015, 'subm', '', files=[])
            #files=[File('pdf','papers/SODA_2016_v1.pdf')])
    ]

    join_authors = lambda ps: ', '.join(Vars.coauthors[p].name for p in ps)

env = Environment(loader=FileSystemLoader('templates'))
template = env.get_template('index.html')
print(template.render(Vars.__dict__))

