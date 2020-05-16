import sys

from jinja2 import Environment, FileSystemLoader
from importlib import import_module

# from data import Vars
data = import_module(sys.argv[1])

env = Environment(loader=FileSystemLoader('.'))
template = env.get_template(sys.argv[2])
print(template.render(data.Vars.__dict__))

