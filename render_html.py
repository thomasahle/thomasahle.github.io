import sys

from jinja2 import Environment, FileSystemLoader
from data import Vars

env = Environment(loader=FileSystemLoader('.'))
template = env.get_template(sys.argv[1])
print(template.render(Vars.__dict__))

