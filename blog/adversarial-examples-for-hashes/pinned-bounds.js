/* Add the pinned-length comparison to existing evidence cards. Plot data and rendering are unchanged. */
(() => {
  'use strict';
  const card = document.getElementById('chart-tooltip');
  if (!card) return;
  const values = new Map();
  document.querySelectorAll('[data-pinned-id]').forEach(value => {
    values.set(value.dataset.pinnedId, value);
  });
  const extend = () => {
    const metrics = card.querySelector('.tooltip-metrics');
    if (!metrics || metrics.querySelector('[data-pinned-metric]')) return;
    const point = document.querySelector('#bits-vs-speed [data-point-id][aria-expanded="true"]');
    const value = values.get(point?.dataset.pointId);
    const row = document.createElement('div');
    row.dataset.pinnedMetric = '';
    const label = document.createElement('dt');
    label.textContent = 'Bound at 1 GB';
    const result = document.createElement('dd');
    const strong = document.createElement('strong');
    strong.textContent = value?.textContent || '—';
    result.append(strong);
    row.append(label, result);
    metrics.firstElementChild.after(row);
    const formula = document.createElement('p');
    formula.className = 'tooltip-pinned-formula';
    formula.textContent = value ? value.title.split('\n')[1] :
      'A fixed short pair implies nothing at 1 GB; the existing heuristic cap is unchanged.';
    metrics.after(formula);
    if (value) {
      const scope = document.createElement('p');
      scope.textContent = value.title.split('\n')[2];
      card.querySelector('.tooltip-details').append(scope);
    }
  };
  new MutationObserver(extend).observe(card, {childList: true, subtree: true});
  extend();
})();
