/* The interactive view uses the generated publication SVGs. */
(() => {
  'use strict';
  const root = document.querySelector('.feature-chart');
  const chart = document.getElementById('bits-vs-speed');
  if (!root || !chart) return;
  const assetRoot = new URL('.', document.currentScript.src);
  const revision = new URL(document.currentScript.src).search;
  const select = root.querySelector('#hash-inspect');
  const detail = root.querySelector('.figure-inspector');
  const status = root.querySelector('.figure-status');
  const peek = root.querySelector('.figure-peek');
  const stage = root.querySelector('.figure-stage');
  const scaleSelect = root.querySelector('#score-scale');
  const cache = new Map();
  const reducedMotion = matchMedia('(prefers-reduced-motion: reduce)');
  const hostButtons = [...root.querySelectorAll('[data-host]')];
  const hostIndicator = document.createElement('span');
  hostIndicator.className = 'figure-host-indicator';
  hostIndicator.setAttribute('aria-hidden', 'true');
  root.querySelector('.figure-hosts').prepend(hostIndicator);
  let data, host = 'm2', desiredHost = 'm2', selectedId = '', currentLayout = '', requestId = 0, lastTrigger, motion;
  let scale = 'sqrt', desiredScale = 'sqrt';
  scaleSelect.value = 'sqrt';
  const scaleNames = {linear: 'Linear', sqrt: 'Square root', quadratic: 'Quadratic', log: 'Logarithmic'};
  const figureFile = (host, scale, layout) => host + (scale === 'linear' ? '' : '-' + scale) + (layout === 'desktop' ? '' : '-' + layout) + '.svg';
  const resource = file => new URL(file + revision, assetRoot);
  const get = file => {
    if (!cache.has(file)) cache.set(file, fetch(resource(file)).then(response => {
      if (!response.ok) throw new Error('The figure could not be loaded.');
      return response.text();
    }).catch(error => {cache.delete(file); throw error;}));
    return cache.get(file);
  };
  const rows = () => data.hosts[host].rows;
  const rowFor = id => rows().find(row => row.id === id);
  const pretty = value => new Intl.NumberFormat('en', {maximumFractionDigits: 2}).format(value);
  const kindLabel = row => row.kind === 'proof' ? 'Proved minimum score' : row.kind === 'claim' ? 'Unresolved score claim' : 'Score limited by a colliding pair';
  const clearPeek = () => {peek.hidden = true;};
  function showPeek(row, node) {
    if (motion || matchMedia('(pointer: coarse)').matches) return;
    peek.replaceChildren();
    const name = document.createElement('strong'); name.textContent = row.name;
    const values = document.createElement('span'); values.textContent = `${row.score} bits · ${pretty(row.speed)} B/cycle`;
    peek.append(name, values); peek.hidden = false;
    const box = node.getBoundingClientRect(), bounds = root.getBoundingClientRect();
    peek.style.left = Math.max(4, Math.min(box.left - bounds.left, bounds.width - peek.offsetWidth - 4)) + 'px';
    peek.style.top = Math.max(0, box.top - bounds.top - peek.offsetHeight - 12) + 'px';
  }
  function markSelected() {
    chart.querySelectorAll('[data-point-id]').forEach(node => {
      const active = node.dataset.pointId === selectedId;
      node.classList.toggle('is-active', active);
      node.setAttribute('aria-pressed', String(active));
    });
  }
  function inspect(row, trigger) {
    if (!row) return;
    if (trigger) lastTrigger = trigger;
    const profile = data.profiles[row.profile];
    const changed = selectedId !== row.id;
    selectedId = row.id; select.value = row.id; markSelected(); clearPeek();
    detail.hidden = false; detail.dataset.kind = row.kind;
    detail.querySelector('.inspector-kind').textContent = kindLabel(row);
    detail.querySelector('.inspector-name').textContent = row.name;
    detail.querySelector('.inspector-authors').textContent = 'By ' + profile.authors;
    detail.querySelector('.inspector-background').textContent = profile.background;
    detail.querySelector('.inspector-summary').textContent = row.summary;
    detail.querySelector('.inspector-score').textContent = row.score + ' bits';
    detail.querySelector('.inspector-speed').textContent = pretty(row.speed) + ' B/cycle';
    detail.querySelector('.inspector-host').textContent = host === 'm2' ? 'Apple M2 Pro' : 'Intel Xeon';
    detail.querySelector('.inspector-output').textContent = row.output_bits + ' bits';
    detail.querySelector('.inspector-evidence-label').textContent = row.kind === 'witness' ? 'Collision evidence' : row.kind === 'claim' ? 'Claimed bound (unresolved)' : 'Proved bound';
    detail.querySelector('.inspector-evidence').textContent = row.reader_notes.evidence;
    detail.querySelector('.inspector-key').textContent = row.reader_notes.key;
    detail.querySelector('.inspector-scope').textContent = row.reader_notes.scope;
    detail.querySelector('.inspector-section').href = '#' + row.anchor;
    // Each profile provides one curated set of public documents. Keep the
    // article navigation below without repeating code and measurement links.
    for (const selector of ['.inspector-code', '.inspector-source']) {
      const link = detail.querySelector(selector);
      link.hidden = true;
      link.removeAttribute('href');
    }
    detail.querySelector('.inspector-benchmark-note').textContent = data.hosts[host].provisional
      ? 'M2 timings are provisional; corrected ARM benchmarks are pending. Results concern the named version and API.'
      : 'Results apply to the version and function studied here. Later releases may behave differently.';
    const links = detail.querySelector('[aria-label="Hash project and background"]');
    links.replaceChildren(...profile.links.map(({label, url}) => {
      const link = document.createElement('a'); link.textContent = label; link.href = url; return link;
    }));
    if (changed) detail.querySelector('.inspector-qualifications').open = false;
    status.textContent = `${row.name}. ${kindLabel(row)}: ${row.score} bits. ${pretty(row.speed)} bytes per cycle.`;
  }
  function dismiss() {
    selectedId = ''; select.value = ''; detail.hidden = true; clearPeek(); markSelected();
    if (detail.contains(document.activeElement)) lastTrigger?.focus({preventScroll: true});
  }
  function options() {
    select.replaceChildren(new Option('Choose a hash…', ''));
    for (const [kind, label] of [['proof','Proved guarantees'],['claim','Unresolved claims'],['witness','Pairs that expose a limit']]) {
      const group = document.createElement('optgroup'); group.label = label;
      for (const row of rows().filter(row => row.kind === kind).sort((a,b) => a.name.localeCompare(b.name))) {
        group.append(new Option(row.name, row.id));
      }
      if (group.children.length) select.append(group);
    }
  }
  function updateHostControl() {
    const button = hostButtons.find(button => button.dataset.host === host);
    hostButtons.forEach(button => button.setAttribute('aria-pressed', String(button.dataset.host === host)));
    hostIndicator.style.width = button.offsetWidth + 'px';
    hostIndicator.style.transform = `translateX(${button.offsetLeft}px)`;
    root.querySelector('.figure-hosts').classList.add('has-indicator');
  }

  // Keep the exported SVG as the final frame. Match semantic IDs rather than
  // Matplotlib's incidental group numbers, including labels and their leaders.
  function motionNodes(svg) {
    svg.querySelectorAll('[data-point-id]').forEach(node => {node.dataset.motionKey = 'point:' + node.dataset.pointId;});
    svg.querySelectorAll('[data-label-for]').forEach(node => {node.dataset.motionKey = 'label:' + node.dataset.labelKey;});
    svg.querySelectorAll('[data-leader-for]').forEach(node => {node.dataset.motionKey = 'leader:' + node.dataset.leaderFor;});
    svg.querySelectorAll('#host-name, #host-note').forEach(node => {node.dataset.motionKey = 'host:' + node.id;});
    return [...svg.querySelectorAll('[data-motion-key]')];
  }
  function snapshot(svg) {
    const result = new Map();
    if (!svg) return result;
    const inverse = svg.getScreenCTM().inverse();
    for (const node of motionNodes(svg)) {
      const key = node.dataset.motionKey;
      const point = node.querySelector('.point-hit');
      const box = node.getBBox();
      const local = point ? new DOMPoint(+point.getAttribute('cx'), +point.getAttribute('cy')) : new DOMPoint(box.x, box.y);
      const origin = local.matrixTransform(inverse.multiply(node.getScreenCTM()));
      result.set(key, {node, origin, opacity: +getComputedStyle(node).opacity,
        path: key.startsWith('leader:') ? node.querySelector('path').getAttribute('d') : null,
        text: node.textContent});
    }
    return result;
  }
  function interpolatePath(from, to) {
    const number = /-?\d*\.?\d+(?:e[-+]?\d+)?/gi;
    const a = from.match(number)?.map(Number), b = to.match(number)?.map(Number);
    if (!a || a.length !== b?.length || from.replace(number, '#') !== to.replace(number, '#')) return null;
    return t => {let i = 0; return to.replace(number, () => {const n = i++; return a[n] + (b[n] - a[n]) * t;});};
  }
  function makeExit(record, svg, key) {
    const clone = record.node.cloneNode(true);
    // Make departing glyphs self-contained before the old SVG is discarded.
    // This avoids duplicate IDs or references to another host's marker defs.
    clone.querySelectorAll('use').forEach(use => {
      const href = use.getAttribute('href') || use.getAttributeNS('http://www.w3.org/1999/xlink', 'href');
      const shape = record.node.ownerSVGElement.querySelector(href)?.cloneNode(true);
      if (!shape) return;
      shape.setAttribute('transform', `translate(${use.getAttribute('x') || 0} ${use.getAttribute('y') || 0})`);
      shape.setAttribute('style', `${shape.getAttribute('style') || ''};${use.getAttribute('style') || ''}`);
      use.replaceWith(shape);
    });
    clone.querySelectorAll('defs').forEach(node => node.remove());
    [clone, ...clone.querySelectorAll('*')].forEach(node => {
      for (const name of ['id', 'tabindex', 'role', 'aria-label', 'aria-controls', 'aria-pressed', 'data-point-id', 'data-label-for', 'data-label-key', 'data-leader-for', 'clip-path']) node.removeAttribute(name);
    });
    clone.setAttribute('aria-hidden', 'true');
    clone.style.pointerEvents = 'none';
    clone.dataset.motionExit = '';
    if (key) clone.dataset.motionKey = key;
    else clone.removeAttribute('data-motion-key');
    svg.append(clone);
    return clone;
  }
  function transition(svg, before, animate) {
    motion?.cancel();
    if (!animate || reducedMotion.matches || !before.size) {
      motion = null; chart.dataset.motion = 'idle'; return;
    }
    const after = snapshot(svg), frames = [], exits = [];
    for (const [key, target] of after) {
      const prior = before.get(key), node = target.node;
      if (prior && key.startsWith('host:') && prior.text !== target.text) {
        const ghost = makeExit(prior, svg);
        exits.push(ghost);
        frames.push(t => {ghost.style.opacity = prior.opacity * Math.max(0, 1 - t * 3); node.style.opacity = Math.min(1, t * 2);});
      } else if (prior && key.startsWith('leader:')) {
        const path = node.querySelector('path'), interpolate = interpolatePath(prior.path, target.path);
        frames.push(t => {if (interpolate) path.setAttribute('d', interpolate(t)); node.style.opacity = prior.opacity + (1 - prior.opacity) * t;});
      } else if (prior) {
        const dx = prior.origin.x - target.origin.x, dy = prior.origin.y - target.origin.y;
        frames.push(t => {node.setAttribute('transform', `translate(${dx * (1 - t)} ${dy * (1 - t)})`); node.style.opacity = prior.opacity + (1 - prior.opacity) * t;});
      } else {
        frames.push(t => {node.style.opacity = Math.max(0, (t - .18) / .82);});
      }
    }
    for (const [key, prior] of before) {
      if (after.has(key)) continue;
      const ghost = makeExit(prior, svg, key);
      exits.push(ghost);
      frames.push(t => {ghost.style.opacity = prior.opacity * Math.max(0, 1 - t * 2);});
    }
    let frame, started;
    const finish = () => {
      cancelAnimationFrame(frame);
      for (const target of after.values()) {target.node.removeAttribute('transform'); target.node.style.removeProperty('opacity'); if (target.path) target.node.querySelector('path').setAttribute('d', target.path);}
      exits.forEach(node => node.remove());
      motion = null; chart.dataset.motion = 'idle';
    };
    motion = {cancel: () => cancelAnimationFrame(frame), finish};
    chart.dataset.motion = 'running';
    frames.forEach(render => render(0));
    const tick = now => {
      started ??= now;
      const progress = Math.min(1, (now - started) / 720);
      // Smooth acceleration and braking keeps dense clusters easy to follow.
      const eased = progress < .5 ? 4 * progress ** 3 : 1 - (-2 * progress + 2) ** 3 / 2;
      frames.forEach(render => render(eased));
      if (progress < 1) frame = requestAnimationFrame(tick);
      else finish();
    };
    frame = requestAnimationFrame(tick);
  }
  reducedMotion.addEventListener('change', () => {if (reducedMotion.matches) motion?.finish();});
  function bind(svg) {
    const nodes = [...svg.querySelectorAll('[data-point-id]')];
    svg.setAttribute('role','group');
    svg.setAttribute('aria-describedby','figure-keyboard-help');
    for (const [index, node] of nodes.entries()) {
      const row = rowFor(node.dataset.pointId);
      node.setAttribute('tabindex',index === 0 ? '0' : '-1');
      node.setAttribute('aria-controls','figure-inspector');
      node.addEventListener('click', () => inspect(row, node));
      node.addEventListener('pointerenter', () => showPeek(row,node));
      node.addEventListener('pointerleave',clearPeek);
      node.addEventListener('focus',() => {nodes.forEach(n => n.setAttribute('tabindex', n === node ? '0':'-1')); showPeek(row,node);});
      node.addEventListener('blur',clearPeek);
      node.addEventListener('keydown', event => {
        if (['Enter',' '].includes(event.key)) {event.preventDefault(); inspect(row,node);}
        if (['ArrowRight','ArrowDown','ArrowLeft','ArrowUp','Home','End'].includes(event.key)) {
          event.preventDefault();
          const next = event.key === 'Home' ? 0 : event.key === 'End' ? nodes.length-1 :
            (index + (['ArrowRight','ArrowDown'].includes(event.key) ? 1 : -1) + nodes.length) % nodes.length;
          nodes[next].focus();
        }
      });
    }
    svg.querySelectorAll('[data-label-for]').forEach(label => {
      const row = rowFor(label.dataset.labelFor);
      if (!row) return;
      label.addEventListener('click',() => inspect(row, nodes.find(n=>n.dataset.pointId===row.id)));
      label.addEventListener('pointerenter',() => showPeek(row,label));
      label.addEventListener('pointerleave',clearPeek);
    });
  }
  async function draw(force = false, nextHost = desiredHost, nextScale = desiredScale) {
    if (!data) return;
    const layout = chart.clientWidth < 540 ? 'mobile' : chart.clientWidth < 860 ? 'compact' : 'desktop';
    const key = nextHost + '-' + nextScale + '-' + layout;
    if (!force && key === currentLayout) return;
    const request = ++requestId;
    chart.setAttribute('aria-busy', 'true');
    try {
      const source = await get(figureFile(nextHost, nextScale, layout));
      if (request !== requestId) return;
      const doc = new DOMParser().parseFromString(source,'image/svg+xml');
      if (doc.querySelector('parsererror')) throw new Error('The figure could not be read.');
      const previousSvg = chart.querySelector('svg');
      const animate = previousSvg && (host !== nextHost || scale !== nextScale) && currentLayout.endsWith('-' + layout);
      const before = animate ? snapshot(previousSvg) : new Map();
      const focusedId = document.activeElement?.dataset.pointId;
      const previousTriggerId = lastTrigger?.dataset.pointId;
      host = nextHost;
      scale = nextScale;
      const svg = document.importNode(doc.documentElement,true);
      svg.removeAttribute('width'); svg.removeAttribute('height');
      svg.classList.add('publication-plot'); bind(svg);
      // Insert the new view before sampling its coordinates. The old detached
      // SVG remains available for resolving any departing marker definitions.
      chart.replaceChildren(svg);
      transition(svg, before, animate);
      chart.dataset.axis = data.hosts[host].key; chart.dataset.points = rows().length;
      chart.dataset.scale = scale;
      stage.dataset.layout = layout;
      root.querySelector('.figure-toolbar').hidden = false;
      scaleSelect.value = scale;
      currentLayout = key;
      updateHostControl();
      options();
      const previous = rowFor(selectedId);
      if (previous) inspect(previous);
      else {selectedId = ''; detail.hidden = true; markSelected();}
      const pointFor = id => [...svg.querySelectorAll('[data-point-id]')].find(node => node.dataset.pointId === id);
      if (previousTriggerId) lastTrigger = pointFor(previousTriggerId) || select;
      if (focusedId) (pointFor(focusedId) || select).focus({preventScroll: true});
      status.textContent = `${data.hosts[host].name}: ${rows().length} results. ${scaleNames[scale]} score scale.${scale === 'log' ? ' Linear from zero to one bit.' : ''} ${data.hosts[host].provisional ? 'Provisional measurements.' : ''}`;
      select.disabled = false;
      chart.setAttribute('aria-busy', 'false');
      // Warm the other host so the first toggle responds as quickly as later ones.
      get(figureFile(host === 'm2' ? 'xeon' : 'm2', scale, layout)).catch(() => {});
    } catch (error) {
      if (request !== requestId) return;
      desiredHost = host; desiredScale = scale; scaleSelect.value = scale;
      chart.setAttribute('aria-busy', 'false');
      status.textContent = error.message + ' The static figure and data table remain available.';
    }
  }
  hostButtons.forEach(button => button.addEventListener('click',() => {
    if (desiredHost === button.dataset.host) return;
    desiredHost = button.dataset.host;
    clearPeek(); draw(true);
  }));
  scaleSelect.addEventListener('change', () => {
    desiredScale = scaleSelect.value;
    clearPeek(); draw(true);
  });
  select.addEventListener('change',() => select.value ? inspect(rowFor(select.value),select) : dismiss());
  detail.querySelector('.inspector-close').addEventListener('click',dismiss);
  root.addEventListener('keydown',event => {if (event.key === 'Escape') dismiss();});
  new ResizeObserver(() => {clearPeek(); updateHostControl(); draw();}).observe(chart);
  get('data.json').then(source => {data=JSON.parse(source); draw(true);}).catch(error => {
    status.textContent = error.message + ' The static figure and data table remain available.';
  });
})();
