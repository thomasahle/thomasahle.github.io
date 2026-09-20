# Robustness figure

Run `python3 illustrations/build_robustness.py` from the article directory to
regenerate all three standalone SVG layouts and their inline block in
`index.html`. Edit the category text and geometry in that generator.
Keep each category to a short definition and relevant applications. Do not
add hash names, measurements, or longer consequence explanations to the figure.

The page deliberately embeds SVG, rather than using an image element, so
readers can select and copy the text. Each layout has unique IDs; responsive
visibility is controlled by the `.robustness-graphic` rules in `publication.css`.

The figure is a matrix: rows distinguish a fixed pair from a large fixed set;
columns distinguish a fraction of keys from every key. The inputs in every
cell are chosen without knowing the key. Few-way collisions are a note between
the rows. Desktop and tablet show a 2×2 grid; mobile keeps the row groups and
repeats the column labels above stacked cells. Color emphasizes the two
dimensions together, with the same color for the two off-diagonal cells.
Do not restore a single severity arrow or describe a pair that works for a
fraction of keys as requiring knowledge of the key.
