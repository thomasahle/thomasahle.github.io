# Robustness figure

Run `python3 illustrations/build_robustness.py` from the article directory to
regenerate all three standalone SVG layouts and their inline block in
`index.html`. Edit the category text and geometry in that generator.
Keep each category to a short definition and relevant applications. Do not
add hash names, measurements, or longer consequence explanations to the figure.

The page deliberately embeds SVG, rather than using an image element, so
readers can select and copy the text. Each layout has unique IDs; responsive
visibility is controlled by the `.robustness-graphic` rules in `publication.css`.

All four categories, including few-way collisions, appear in one row. The
three SVG assets use the same layout; narrow screens scroll horizontally
within the figure so its text remains readable. Keep the headings top-aligned,
with a short gap before each definition and a normal-weight “Use for:” paragraph.
There are no internal divider lines. The cards progress from gold to red, with
matching light backgrounds. There is no severity arrow: set size and the
fraction of affected keys are separate properties, explained in each definition.
All inputs are chosen without knowing the key.
