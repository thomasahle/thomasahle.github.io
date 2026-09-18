/* Shared main-thread controller and module worker. No hash is implemented in JS. */
const assetRoot = new URL('.', import.meta.url);

if (typeof document === 'undefined') {
  let module, definition, runToken = 0;
  const send = value => self.postMessage(value);
  const fail = (id, error) => send({type: 'error', id, message: String(error.message || error)});
  const checked = value => {
    if (value < 0) throw new Error('The verification program rejected this operation.');
    return value;
  };
  const keyAt = pointer => {
    const view = new DataView(module.HEAPU8.buffer);
    return Array.from({length: 4}, (_, i) => view.getBigUint64(pointer + i * 8, true).toString(16).padStart(16, '0'));
  };
  const writeKey = (pointer, words) => {
    const view = new DataView(module.HEAPU8.buffer);
    for (let i = 0; i < 4; ++i) view.setBigUint64(pointer + i * 8, BigInt('0x' + (words[i] || '0')), true);
  };
  const describe = pair => {
    const i = pair.pair, p = module._scratch();
    checked(module._example_key(i, p + 32));
    return {...pair, keys: keyAt(p + 32).slice(0, pair.keyWords), messages: [0, 1].map(which => {
      const len = checked(module._pair_len(i, which));
      const start = module._pair_bytes(i, which);
      if (!start || !len) throw new Error('The verification program returned an empty pair.');
      return Array.from(module.HEAPU8.slice(start, start + len));
    })};
  };
  self.onmessage = async ({data}) => {
    try {
      if (data.type === 'load') {
        definition = data.definition;
        const factory = (await import(new URL('wasm/' + definition.module + '.js', assetRoot))).default;
        module = await factory({print: () => {}, printErr: () => {}});
        // Every worker instance validates, even when another widget shares its module.
        if (module._init() !== 1) throw new Error('Validation failed. No hash outputs or tallies will be shown.');
        for (const pair of definition.pairs) {
          if (pair.pair >= module._pair_count() || module._seed_bits(pair.pair) !== pair.seedBits ||
              module._key_words(pair.pair) !== pair.keyWords ||
              !!module._has_class(pair.pair) !== !!pair.classText) {
            throw new Error('The module and widget metadata do not agree.');
          }
        }
        send({type: 'ready', pairs: definition.pairs.map(describe)});
        return;
      }
      if (!module) throw new Error('The verification program is not ready.');
      const p = module._scratch();
      if (data.type === 'hash') {
        writeKey(p + 32, data.keys);
        const result = checked(module._hash_key(data.pair, p + 32, p + 64));
        send({type: 'hash', id: data.id, collides: !!result,
          outputs: [module.UTF8ToString(p + 64), module.UTF8ToString(p + 129)]});
      } else if (data.type === 'sample') {
        // A separate RNG state keeps interactive samples out of the batch stream.
        module._rng_init(data.random[0], data.random[1], p + 208);
        checked(module._sample_seed(data.pair, data.mode, p + 208, p + 32));
        send({type: 'sample', id: data.id, keys: keyAt(p + 32)});
      } else if (data.type === 'cancel') {
        ++runToken;
      } else if (data.type === 'batch') {
        const token = ++runToken;
        module._rng_init(data.random[0], data.random[1], p);
        let done = 0, collisions = 0, chunkSize = 256, lastUpdate = 0;
        const chunk = () => {
          if (token !== runToken) return;
          try {
            const n = Math.min(chunkSize, data.trials - done);
            const start = performance.now();
            collisions += checked(module._run_batch(data.pair, n, p, data.mode));
            done += n;
            const elapsed = performance.now() - start;
            // Yield between chunks for cancellation and individual-seed requests.
            chunkSize = Math.max(32, Math.min(4096, Math.floor(n * 20 / Math.max(1, elapsed))));
            const finished = done === data.trials;
            if (finished || performance.now() - lastUpdate > 150) {
              send({type: 'batch', id: data.id, collisions, trials: done, finished});
              lastUpdate = performance.now();
            }
            if (!finished) setTimeout(chunk, 0);
          } catch (error) { fail(data.id, error); }
        };
        chunk();
      }
    } catch (error) { fail(data.id, error); }
  };
} else {
  const format = new Intl.NumberFormat('en-US');
  const randomState = () => Array.from(crypto.getRandomValues(new Uint32Array(2)));
  const create = (tag, className, text) => {
    const node = document.createElement(tag);
    if (className) node.className = className;
    if (text !== undefined) node.textContent = text;
    return node;
  };
  let manifestPromise;
  const manifest = () => manifestPromise ||= fetch(new URL('wasm/manifest.json', assetRoot))
    .then(response => {
      if (!response.ok) throw new Error('The demo manifest could not be loaded.');
      return response.json();
    });

  class HashDemo {
    constructor(element) {
      this.element = element;
      this.status = element.querySelector('.demo-status');
      this.badge = element.querySelector('.demo-badge');
      this.content = element.querySelector('.demo-content');
      this.sequence = 0;
      this.tallies = new Map();
      element.hidden = false;
      element.addEventListener('toggle', () => {
        if (element.open && !this.started) this.load();
      });
    }
    async load() {
      if (this.started) return;
      this.started = true;
      this.element.dataset.state = 'loading';
      this.badge.textContent = 'Validating…';
      this.status.textContent = 'Loading and validating the verification program…';
      try {
        const config = await manifest();
        const definition = config.entries[this.element.dataset.demo];
        this.worker = new Worker(new URL('post-demo.js', assetRoot), {type: 'module'});
        this.worker.onerror = event => { event.preventDefault(); this.fail('The verification worker could not run.'); };
        this.worker.onmessage = ({data}) => this.receive(data);
        this.worker.postMessage({type: 'load', definition});
      } catch (error) { this.fail(error.message); }
    }
    fail(message) {
      this.worker?.terminate();
      this.element.dataset.state = 'failed';
      this.badge.textContent = 'Unavailable';
      this.content.hidden = true;
      this.content.replaceChildren();
      this.status.textContent = message + ' No results are displayed. Run the linked verification package locally.';
    }
    receive(data) {
      if (data.type === 'error') { this.fail(data.message); return; }
      if (data.type === 'ready') {
        this.pairs = data.pairs;
        this.element.dataset.state = 'ready';
        this.badge.textContent = 'Validated';
        this.content.hidden = false;
        this.render();
      } else if (data.type === 'hash' && data.id === this.hashId) {
        this.outputs.forEach((node, i) => { node.textContent = data.outputs[i]; });
        this.verdict.textContent = data.collides ? 'collide' : 'differ';
        this.verdict.dataset.verdict = data.collides ? 'collide' : 'differ';
        this.element.dataset.verdict = data.collides ? 'collide' : 'differ';
        this.result.hidden = false;
      } else if (data.type === 'sample' && data.id === this.sampleId) {
        this.seed.value = data.keys.slice(0, this.pair.keyWords).map(k => this.pair.seedBits === 32 ? k.slice(-8) : k).join(' ');
        this.hash();
      } else if (data.type === 'batch' && data.id === this.batchId) {
        const tally = this.tallies.get(this.tallyKey());
        tally.collisions = this.base.collisions + data.collisions;
        tally.trials = this.base.trials + data.trials;
        this.updateTally();
        if (data.finished) {
          this.running = false;
          this.runButton.disabled = false;
          this.stopButton.hidden = true;
          this.element.dataset.running = 'false';
        }
      }
    }
    render() {
      const id = this.element.dataset.demo;
      this.pair = this.pairs[0];
      const fieldset = create('fieldset', 'demo-controls');
      fieldset.append(create('legend', 'demo-sr-only', 'Hash collision experiment'));
      if (this.pairs.length > 1) {
        const label = create('label', 'demo-label', 'Output / variant');
        const select = create('select', 'demo-variant');
        select.id = id + '-variant'; label.htmlFor = select.id;
        this.pairs.forEach((p, i) => { const option = create('option', '', p.name); option.value = i; select.append(option); });
        select.addEventListener('change', () => { this.stop(); this.pair = this.pairs[Number(select.value)]; this.showPair(); });
        fieldset.append(label, select);
      }
      this.messageArea = create('div', 'demo-messages');
      this.note = create('p', 'demo-note');
      this.classText = create('p', 'demo-note');
      this.classText.id = id + '-class';
      this.seedLabel = create('label', 'demo-label');
      this.seed = create('input', 'demo-seed');
      this.seed.id = id + '-seed'; this.seed.type = 'text'; this.seed.spellcheck = false;
      this.seed.autocomplete = 'off'; this.seed.autocapitalize = 'off';
      this.seedLabel.htmlFor = this.seed.id;
      this.seedHelp = create('p', 'demo-hint'); this.seedHelp.id = id + '-seed-help';
      this.seed.setAttribute('aria-describedby', this.seedHelp.id);
      this.seed.addEventListener('input', () => {
        ++this.sequence; this.sampleId = null; this.hashId = null;
        this.result.hidden = true;
        this.seed.setCustomValidity(''); this.seed.removeAttribute('aria-invalid');
      });
      this.seed.addEventListener('keydown', event => { if (event.key === 'Enter') { event.preventDefault(); this.hash(); } });
      const buttons = create('div', 'demo-actions');
      const hashButton = create('button', '', 'Hash this seed'); hashButton.type = 'button';
      hashButton.addEventListener('click', () => this.hash());
      const randomButton = create('button', 'demo-random', 'Random seed'); randomButton.type = 'button';
      randomButton.addEventListener('click', () => this.sample(0));
      this.classButton = create('button', 'demo-class', 'Class seed'); this.classButton.type = 'button';
      this.classButton.setAttribute('aria-describedby', this.classText.id);
      this.classButton.addEventListener('click', () => this.sample(1));
      this.exampleButton = create('button', 'demo-example', 'Published seed'); this.exampleButton.type = 'button';
      this.exampleButton.addEventListener('click', () => {
        this.sampleId=null;
        this.seed.value=this.pair.keys.map(k=>this.pair.seedBits===32?k.slice(-8):k).join(' ');
        this.hash();
      });
      buttons.append(hashButton, randomButton, this.classButton, this.exampleButton);
      this.result = create('div', 'demo-result');
      this.result.setAttribute('role', 'status'); this.result.setAttribute('aria-live', 'polite'); this.result.setAttribute('aria-atomic', 'true');
      this.outputs = [0, 1].map(i => {
        const line = create('div', 'demo-output');
        const code = create('code'); line.append(create('span', '', i ? 'H(B)' : 'H(A)'), code);
        this.result.append(line); return code;
      });
      this.verdict = create('strong', 'demo-verdict'); this.result.append(this.verdict);
      const trials = create('div', 'demo-trial-controls');
      this.modeLabel = create('label', 'demo-mode-label', 'Sample ');
      this.mode = create('select', 'demo-mode');
      this.mode.id=id+'-mode'; this.modeLabel.htmlFor=this.mode.id;
      for (const [value, label] of [[0, 'Uniform seeds'], [1, 'Class seeds']]) {
        const option = create('option', '', label); option.value=value; this.mode.append(option);
      }
      this.mode.addEventListener('change', () => { this.stop(); this.updateTally(); });
      this.modeLabel.append(this.mode);
      this.runButton=create('button', 'demo-run'); this.runButton.type='button';
      this.runButton.addEventListener('click', () => this.run());
      this.stopButton=create('button', 'demo-stop', 'Stop'); this.stopButton.type='button'; this.stopButton.hidden=true;
      this.stopButton.addEventListener('click', () => this.stop());
      trials.append(this.modeLabel,this.runButton,this.stopButton);
      this.tally=create('div','demo-tally'); this.tally.setAttribute('role','status');
      this.tally.setAttribute('aria-live','polite'); this.tally.setAttribute('aria-atomic','true');
      this.comparison=create('p','demo-comparison');
      fieldset.append(this.messageArea,this.note,this.classText,this.seedLabel,this.seed,this.seedHelp,buttons,this.result,trials,this.tally,this.comparison);
      this.content.append(fieldset);
      this.showPair();
    }
    showPair() {
      const p=this.pair;
      this.sampleId=null;
      this.status.textContent=p.validation + '. The published witness also passed.';
      this.messageArea.replaceChildren();
      p.messages.forEach((bytes, which) => {
        const label=which?'B':'A';
        this.messageArea.append(create('p','demo-message-label',label+' · '+format.format(bytes.length)+' bytes'));
        const code=create('code','demo-hex'); code.tabIndex=0;
        code.setAttribute('role','region'); code.setAttribute('aria-label','Message '+label+' in hexadecimal, in byte order');
        bytes.forEach((byte, offset) => {
          const differs=byte!==p.messages[1-which][offset];
          const span=create(differs?'mark':'span', differs?'demo-diff':'',byte.toString(16).padStart(2,'0'));
          if (differs) span.title='Byte '+offset+' differs';
          code.append(span);
        });
        this.messageArea.append(code);
      });
      this.messageArea.append(create('p','demo-hint','Orange marks differing bytes, including bytes present in only one message.'));
      this.seedLabel.textContent=p.keyWords===4?'Seed / key (four 64-bit hex words, in API order)':'Seed ('+p.seedBits+'-bit hexadecimal)';
      this.seedHelp.textContent=p.keyWords===4?'Each random key uses four independent words, matching the published experiment.':
        'Enter up to '+(p.seedBits/4)+' hex digits; an optional 0x prefix is accepted.';
      this.seed.value=p.keys.map(k=>p.seedBits===32?k.slice(-8):k).join(' ');
      this.seed.setCustomValidity(''); this.seed.removeAttribute('aria-invalid');
      this.classText.hidden=!p.classText; this.classText.textContent=p.classText?'Seed class: '+p.classText:'';
      this.classButton.hidden=!p.classText; this.modeLabel.hidden=!p.classText;
      this.mode.value='0';
      this.runButton.textContent='Run '+format.format(p.batch)+' seeds';
      this.note.textContent=p.keyFree?'This pair is key-free: it collides for every seed, so the counter stays at 100%.':
        p.log2Rate < -20 ? 'A browser run will normally see no collisions: '+format.format(p.batch)+' trials is far fewer than the roughly '+format.format(Math.round(2**(-p.log2Rate)))+' needed per collision at the published rate.'+(p.classText?' Try class seeds below.':' Use the published seed to see the recorded witness.'):
        p.log2Rate < -15 ? 'This collision is rare. A short browser run will usually see none; the published seed reproduces a known collision.':'Each trial hashes both fixed messages under a fresh pseudorandom seed.';
      if (p.id.startsWith('highway')) this.note.textContent+=' Even within the class, the conditional rate is about 2⁻²⁴·²², so a short class run will usually also see none.';
      this.hash(); this.updateTally();
    }
    hash() {
      const words=this.seed.value.trim().split(/\s+/);
      const digits=this.pair.seedBits/4;
      if (words.length!==this.pair.keyWords || words.some(w=>!new RegExp('^(?:0x)?[0-9a-fA-F]{1,'+digits+'}$').test(w))) {
        this.seed.setCustomValidity('Enter '+this.pair.keyWords+' hexadecimal '+(this.pair.keyWords===1?'seed':'key words')+', each at most '+digits+' digits.');
        this.seed.setAttribute('aria-invalid','true'); this.seed.reportValidity(); this.result.hidden=true; return;
      }
      this.seed.setCustomValidity(''); this.seed.removeAttribute('aria-invalid');
      this.result.hidden=true;
      this.hashId=++this.sequence;
      this.worker.postMessage({type:'hash',id:this.hashId,pair:this.pair.pair,keys:words.map(w=>w.replace(/^0x/,''))});
    }
    sample(mode) {
      this.sampleId=++this.sequence;
      this.hashId=null;
      this.result.hidden=true;
      this.worker.postMessage({type:'sample',id:this.sampleId,pair:this.pair.pair,mode,random:randomState()});
    }
    tallyKey() { return this.pair.pair+':'+this.mode.value; }
    updateTally() {
      const p=this.pair, classMode=this.mode.value==='1';
      const key=this.tallyKey();
      if (!this.tallies.has(key)) this.tallies.set(key,{collisions:0,trials:0});
      const {collisions:c,trials:n}=this.tallies.get(key);
      this.element.dataset.collisions=c; this.element.dataset.trials=n;
      const label=classMode?'Within the class':'Uniform sample';
      this.tally.textContent=label+': '+format.format(c)+' / '+format.format(n)+' collisions / trials. '+
        (n ? c ? (100*c/n).toFixed(3)+'%; log₂(rate) = '+Math.log2(c/n).toFixed(4)+'.' : 'No collisions observed; log₂(rate) is not estimated from zero hits.' : 'Run a batch to measure the rate.');
      const target=classMode?p.conditionalLog2:p.log2Rate;
      this.comparison.textContent=classMode?'Published conditional contribution: log₂(rate) ≈ '+target.toFixed(4)+'. Class density: log₂ = '+p.classLog2.toFixed(4)+'.':
        'Published rate: '+p.rate+'; log₂ ≈ '+target.toFixed(4)+'.';
      if (classMode && c) this.comparison.textContent+=' Measured class contribution to uniform collisions: log₂ ≈ '+(Math.log2(c/n)+p.classLog2).toFixed(4)+'. This is a sufficient class, not a total-rate estimate.';
      if (n && !c) this.comparison.textContent+=' Zero observed does not mean zero probability.';
    }
    run() {
      if (this.running) return;
      this.running=true; this.element.dataset.running='true';
      this.runButton.disabled=true; this.stopButton.hidden=false;
      this.base={...this.tallies.get(this.tallyKey())};
      this.batchId=++this.sequence;
      this.worker.postMessage({type:'batch',id:this.batchId,pair:this.pair.pair,mode:Number(this.mode.value),trials:this.pair.batch,random:randomState()});
    }
    stop() {
      this.batchId=null;
      this.worker?.postMessage({type:'cancel'});
      this.running=false; this.element.dataset.running='false';
      if (this.runButton) this.runButton.disabled=false;
      if (this.stopButton) this.stopButton.hidden=true;
    }
  }

  const demos=[...document.querySelectorAll('[data-demo]')].map(element=>new HashDemo(element));
  if ('IntersectionObserver' in window) {
    const byElement=new Map(demos.map(demo=>[demo.element,demo]));
    const observer=new IntersectionObserver(entries=>entries.forEach(entry=>{
      if (entry.isIntersecting) { observer.unobserve(entry.target); byElement.get(entry.target).load(); }
    }));
    demos.forEach(demo=>observer.observe(demo.element));
  } else {
    // The native disclosure's toggle handler remains an explicit lazy fallback.
    demos.forEach(demo=>{ demo.badge.textContent='Open to load'; });
  }
}
