import sys
from jinja2 import Environment, FileSystemLoader

from data import Vars

env = Environment(
    block_start_string = '\BLOCK{',
    block_end_string = '}',
    variable_start_string = '\VAR{',
    variable_end_string = '}',
    comment_start_string = '\#{',
    comment_end_string = '}',
    line_statement_prefix = '%%',
    line_comment_prefix = '%#',
    trim_blocks = True,
    autoescape = False,
    loader = FileSystemLoader('.')
)

template = env.get_template(sys.argv[1])
print(template.render(Vars.__dict__))

