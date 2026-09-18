The article uses locally hosted KaTeX 0.18.7. No npm or network step is needed to
build or view the post. `math.js` typesets explicit `data-tex` elements and emits
both HTML and accessible MathML. Source code and demos stay literal.

Inline math:

```html
<span class="math" data-tex="\frac{L-1}{q}">(L−1)/q</span>
```

Displayed math:

```html
<div class="math-display l-body" data-tex="\varepsilon \le L/q">ε ≤ L/q</div>
```

Keep a readable expression inside each element as the no-JavaScript fallback.
Escape `&`, `<`, `>`, and double quotes in HTML attributes as usual. Use `\text{}`
for prose, `\operatorname{}` for named operators, and genuine subscripts/exponents.
The renderer reports invalid TeX in the console and preserves the fallback.
For a long display, `data-tex-compact` can provide the same equation with explicit
line breaks at phone widths. The collision-score definition uses this layout.
