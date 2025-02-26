import sys, re
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

LATEX_SUBS = (
    (re.compile(r'\\'), r'\\textbackslash'),
    (re.compile(r'([{}_#%$])'), r'\\\1'),  # Removed & from this group to handle below
    (re.compile(r'&nbsp;'), r' '),  # Replace HTML non-breaking spaces with regular spaces - MUST come before &amp; handling
    (re.compile(r'&amp;'), r'\\&'),  # Handle HTML entity for ampersand - MUST come before generic & handling
    (re.compile(r'&'), r'\\&'),      # Handle regular ampersands
    (re.compile(r'~'), r'\~{}'),
    (re.compile(r'\^'), r'\^{}'),
    (re.compile(r'"'), r"''"),
    (re.compile(r'\.\.\.+'), r'\\ldots'),
    (re.compile(r'/'), r'\/'),
    (re.compile(r'p₁⁻¹'), r'$p_1^{-1}$'),
    (re.compile(r'‑'), r'-')  # Replace non-breaking hyphens with regular hyphens
)

def escape_tex(value):
    newval = value
    for pattern, replacement in LATEX_SUBS:
        newval = pattern.sub(replacement, newval)
    return newval

env.filters['tex'] = escape_tex

template = env.get_template(sys.argv[1])
print(template.render(Vars.__dict__))