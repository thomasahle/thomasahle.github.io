/* Static contents links work without JS; enhance them from the article headings. */
(() => {
  'use strict';
  const headings = [...document.querySelectorAll('.post h2, .post h3')].filter(heading => !heading.hasAttribute('data-toc-skip'));
  if (!headings.length) return;
  document.querySelectorAll('[data-toc-list]').forEach(list => {
    const items = [];
    let groupElement, groupList;
    headings.forEach(heading => {
      const group = heading.closest('[data-toc-group]');
      if (group !== groupElement) {
        groupElement = group;
        groupList = null;
        if (group) {
          const item = document.createElement('li');
          const label = document.createElement('span');
          item.className = 'toc-group';
          label.className = 'toc-group-label';
          label.textContent = group.dataset.tocGroup;
          groupList = document.createElement('ol');
          item.append(label, groupList);
          items.push(item);
        }
      }
      const item = document.createElement('li');
      const link = document.createElement('a');
      item.className = 'toc-level-' + heading.tagName.slice(1);
      link.href = '#' + heading.id;
      link.textContent = heading.dataset.tocLabel || heading.textContent;
      item.append(link);
      if (groupList) groupList.append(item);
      else items.push(item);
    });
    list.replaceChildren(...items);
  });

  const nav = document.querySelector('.toc-desktop');
  const links = [...nav.querySelectorAll('a')];
  const sections = headings.map(heading => heading.closest('.post-section'));
  const desktop = window.matchMedia('(min-width: 1200px)');
  let observer, frame;
  const update = () => {
    const index = sections.findLastIndex(section => section.getBoundingClientRect().top <= 33);
    links.forEach((link, i) => {
      if (i === index) link.setAttribute('aria-current', 'location');
      else link.removeAttribute('aria-current');
    });
    // The short main article precedes these optional technical sections.
    if (index < 0) return;
    // Scroll only the rail; scrollIntoView would also move the article.
    const item = links[index].getBoundingClientRect();
    const rail = nav.getBoundingClientRect();
    const contentTop = rail.top + nav.querySelector('.toc-label').offsetHeight;
    if (item.top < contentTop) nav.scrollTop += item.top - contentTop - 4;
    else if (item.bottom > rail.bottom) nav.scrollTop += item.bottom - rail.bottom + 4;
  };
  const observe = () => {
    if (observer) observer.disconnect();
    links.forEach(link => link.removeAttribute('aria-current'));
    if (!desktop.matches || !('IntersectionObserver' in window)) return;
    // Observe continuous sections, so long tables/code and large jumps work.
    observer = new IntersectionObserver(update, {
      rootMargin: '-32px 0px -' + Math.max(0, window.innerHeight - 33) + 'px 0px',
      threshold: 0,
    });
    sections.forEach(section => observer.observe(section));
    update();
  };
  window.addEventListener('resize', () => {
    cancelAnimationFrame(frame);
    frame = requestAnimationFrame(observe);
  });
  observe();
  document.querySelector('.toc-mobile').addEventListener('click', event => {
    if (event.target.closest('a')) event.currentTarget.open = false;
  });

  // Keep scroll cues accurate for disclosures and responsive breakouts.
  document.querySelectorAll('.table-scroll').forEach(region => {
    const hint = document.createElement('p');
    hint.className = 'scroll-hint';
    hint.textContent = 'Scroll horizontally to see all columns →';
    region.before(hint);
    // Grid items need the same breakout width as their table.
    hint.classList.add('l-page');
    const updateTable = () => {
      const scrollable = region.clientWidth > 0 && region.scrollWidth > region.clientWidth + 1;
      region.classList.toggle('is-scrollable', scrollable);
      hint.hidden = !scrollable;
    };
    if (window.ResizeObserver) new ResizeObserver(updateTable).observe(region);
    updateTable();
  });

  // Existing chart fallback links should reveal the native disclosure too.
  const revealTable = () => {
    if (window.location.hash === '#chart-table-details') {
      document.getElementById('chart-table-details').open = true;
    }
  };
  window.addEventListener('hashchange', revealTable);
  revealTable();
})();

// Syntax-highlight the appendix pseudo-code line by line with a small self-contained tokenizer
// (the site's highlight.js bundle has no C grammar). Uses the theme's hljs-* class names; keeps line ids.
(function () {
  var KW = /^(if|else|for|while|do|return|const|static|inline|switch|case|break|continue|goto|sizeof|typedef|struct|require|input|output|let|in|to|step)$/;
  var TY = /^(u8|u16|u32|u64|u128|i64|uint8_t|uint16_t|uint32_t|uint64_t|unsigned|int|void|bool|char|size_t|word|byte|lane|state|seed|key|secret|packet)$/;
  var esc = function (t) { return t.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;'); };
  function tokenize(text) {
    var out = '', re = /(\/\/.*$|\/\*[\s\S]*?\*\/)|("(?:[^"\\]|\\.)*")|(0x[0-9a-fA-F_]+|\b\d+(?:\.\d+)?(?:e[+-]?\d+)?\b|\b2\^[0-9]+\b)|([A-Za-z_][A-Za-z0-9_]*)|(\S)|(\s+)/g, m;
    while ((m = re.exec(text)) !== null) {
      if (m[1]) out += '<span class="hljs-comment">' + esc(m[1]) + '</span>';
      else if (m[2]) out += '<span class="hljs-string">' + esc(m[2]) + '</span>';
      else if (m[3]) out += '<span class="hljs-number">' + esc(m[3]) + '</span>';
      else if (m[4]) out += KW.test(m[4]) ? '<span class="hljs-keyword">' + m[4] + '</span>' : TY.test(m[4]) ? '<span class="hljs-type">' + m[4] + '</span>' : (/\(/.test(text.slice(re.lastIndex, re.lastIndex + 1)) ? '<span class="hljs-title">' + m[4] + '</span>' : m[4]);
      else out += esc(m[5] || m[6] || '');
    }
    return out;
  }
  function run() {
    document.querySelectorAll('pre.pseudocode code .code-line').forEach(function (line) {
      if (line.querySelector('[class^="hljs-"]')) return;
      var num = line.querySelector('.line-number'); var numHtml = num ? num.outerHTML : '';
      var text = line.textContent; if (num) text = text.slice(num.textContent.length);
      line.innerHTML = numHtml + tokenize(text.replace(/^\s?/, ''));
    });
  }
  if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', run); else run();
})();
