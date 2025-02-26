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

# Pre-process function to handle HTML entities before LaTeX escaping
def preprocess_html_entities(value):
    if not isinstance(value, str):
        return value
    # Replace common HTML entities first
    value = value.replace('&nbsp;', ' ')
    value = value.replace('&amp;', 'and')  # Convert &amp; to "and" for safety
    value = value.replace('&lt;', '<')
    value = value.replace('&gt;', '>')
    return value

LATEX_SUBS = (
    (re.compile(r'\\'), r'\\textbackslash'),
    (re.compile(r'([{}_#%$&])'), r'\\\1'),  # Added & back since we preprocess it 
    (re.compile(r'~'), r'\~{}'),
    (re.compile(r'\^'), r'\^{}'),
    (re.compile(r'"'), r"''"),
    (re.compile(r'\.\.\.+'), r'\\ldots'),
    (re.compile(r'/'), r'\/'),
    (re.compile(r'p₁⁻¹'), r'$p_1^{-1}$'),
    (re.compile(r'‑'), r'-')  # Replace non-breaking hyphens with regular hyphens
)

def escape_tex(value):
    if not isinstance(value, str):
        return value
    # First preprocess HTML entities
    value = preprocess_html_entities(value)
    # Then do LaTeX escaping
    for pattern, replacement in LATEX_SUBS:
        value = pattern.sub(replacement, value)
    return value

env.filters['tex'] = escape_tex

template = env.get_template(sys.argv[1])
print(template.render(Vars.__dict__))