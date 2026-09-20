/* Explicit TeX keeps code examples literal and readable fallbacks in the HTML. */
(() => {
  'use strict';
  if (!window.katex) return;
  const narrow = window.matchMedia('(max-width: 600px)');
  const render = element => {
    try {
      const tex = narrow.matches && element.dataset.texCompact || element.dataset.tex;
      const markup = katex.renderToString(tex, {
        displayMode: element.classList.contains('math-display'),
        output: 'htmlAndMathml',
        throwOnError: true,
        strict: 'error',
        trust: false,
      });
      element.innerHTML = markup;
    } catch (error) {
      // Keep the original readable expression if an author makes a TeX error.
      console.error('Could not typeset formula:', element.dataset.tex, error);
    }
  };
  document.querySelectorAll('#article [data-tex]').forEach(render);
  narrow.addEventListener('change', () => {
    document.querySelectorAll('#article [data-tex-compact]').forEach(render);
  });
})();
