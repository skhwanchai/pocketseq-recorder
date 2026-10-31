// =============================================================
//  หน้า "ทำดนตรี" — คลังสูตรเพลงและเมโทรนอม + เสียงตัวอย่างในเว็บ
// =============================================================
(() => {
  const RECIPES = window.PS_RECIPES || [];
  const view = document.getElementById('viewMusic');
  if (!view) return;

  const GENRES = {
    song: [
      ['all', 'ทั้งหมด'], ['lofi', 'Lofi'], ['hiphop', 'Hip-Hop · R&B'], ['electronic', 'Electronic'],
      ['band', 'Band · Groove'], ['thai', 'ไทย'], ['game', 'เกม'], ['latin', 'ละติน'],
      ['xmas', 'คริสต์มาส'], ['guitar', 'แบ็กกิ้งกีตาร์'],
    ],
    metro: [
      ['all', 'ทั้งหมด'], ['basic', 'พื้นฐาน'], ['style', 'ตามสไตล์'], ['waltz', '3/4 วอลซ์'], ['practice', 'ฝึกรักษาจังหวะ'],
    ],
  };
  const TAG_LABEL = Object.fromEntries([...GENRES.song, ...GENRES.metro].filter(([k]) => k !== 'all'));
  const TRACKS = [['kick', 'KICK'], ['snare', 'SNARE'], ['hat', 'HAT'], ['bass', 'BASS']];
  const KEY_NOTE = ['C4', 'D4', 'E4', 'F4', 'G4', 'A4', 'B4', 'C5', 'D5', 'E5', 'F5', 'G5', 'A5', 'B5', 'C6', 'D6'];
  const DOREMI = { C: 'โด', D: 'เร', E: 'มี', F: 'ฟา', G: 'ซอล', A: 'ลา', B: 'ที' };

  // ---------- ค่าที่จำไว้ในเบราว์เซอร์ ----------
  const store = {
    get(k, d) { try { const v = localStorage.getItem('ps-' + k); return v === null ? d : JSON.parse(v); } catch (e) { return d; } },
    set(k, v) { try { localStorage.setItem('ps-' + k, JSON.stringify(v)); } catch (e) {} },
  };
  const ui = { kind: store.get('kind', 'song'), genre: store.get('genre', { song: 'all', metro: 'all' }), q: '' };

  const esc = s => String(s).replace(/[&<>"']/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));
  const byId = id => RECIPES.find(r => r.id === id);

  // =============================================================
  //  เครื่องเสียงจำลอง (Web Audio)
  // =============================================================
  const Engine = {
    ctx: null, timer: null, recipe: null, bpm: 100, step: 0, bar: 0, nextTime: 0,
    mutes: new Set(), playScore: true, queue: [], onStep: null, onStop: null, noise: null,

    ensure() {
      if (this.ctx) return;
      const AC = window.AudioContext || window.webkitAudioContext;
      this.ctx = new AC();
      const c = this.ctx;
      this.comp = c.createDynamicsCompressor();
      this.comp.threshold.value = -14; this.comp.ratio.value = 4;
      this.master = c.createGain(); this.master.gain.value = 0.8;
      this.comp.connect(this.master).connect(c.destination);
      const len = c.sampleRate;
      this.noise = c.createBuffer(1, len, c.sampleRate);
      const d = this.noise.getChannelData(0);
      for (let i = 0; i < len; i++) d[i] = Math.random() * 2 - 1;
    },

    // เส้นทางเสียง: กลองกับเมโลดี้แยกกัน เพื่อใส่ FX เฉพาะเมโลดี้ (♫ KEYS) หรือทุกเสียง (FX:ALL)
    buildBuses(r) {
      const c = this.ctx;
      if (this.buses) { try { this.buses.forEach(n => n.disconnect()); } catch (e) {} }
      const drum = c.createGain(), mel = c.createGain(), fxIn = c.createGain();
      const nodes = [drum, mel, fxIn];
      mel.connect(fxIn);
      (r.fxAll ? drum.connect(fxIn) : drum.connect(this.comp));
      const amt = (r.fxAmt || 0) / 100;
      if (r.fx === 'REV') {
        const conv = c.createConvolver();
        const n = Math.floor(c.sampleRate * 2.2), ir = c.createBuffer(2, n, c.sampleRate);
        for (let ch = 0; ch < 2; ch++) { const x = ir.getChannelData(ch); for (let i = 0; i < n; i++) x[i] = (Math.random() * 2 - 1) * Math.pow(1 - i / n, 3); }
        conv.buffer = ir;
        const wet = c.createGain(); wet.gain.value = amt * 0.9;
        fxIn.connect(this.comp); fxIn.connect(conv).connect(wet).connect(this.comp);
        nodes.push(conv, wet);
      } else if (r.fx === 'DLY') {
        const dl = c.createDelay(2), fb = c.createGain(), wet = c.createGain();
        dl.delayTime.value = 3 * 15 / this.bpm;          // 3 ช่อง = เขบ็ต 1 ชั้นประจุด
        fb.gain.value = 0.25 + 0.4 * amt; wet.gain.value = 0.3 + 0.5 * amt;
        fxIn.connect(this.comp); fxIn.connect(dl); dl.connect(fb).connect(dl); dl.connect(wet).connect(this.comp);
        nodes.push(dl, fb, wet); this.delayNode = dl;
      } else if (r.fx === 'OD' || r.fx === 'DIST') {
        const sh = c.createWaveShaper(), k = r.fx === 'OD' ? 2 + amt * 20 : 10 + amt * 80;
        const curve = new Float32Array(1024);
        for (let i = 0; i < 1024; i++) { const x = i / 512 - 1; curve[i] = Math.tanh(k * x) / Math.tanh(k); }
        sh.curve = curve;
        const post = c.createGain(); post.gain.value = r.fx === 'OD' ? 0.7 : 0.5;
        fxIn.connect(sh).connect(post).connect(this.comp);
        nodes.push(sh, post);
      } else {
        fxIn.connect(this.comp);
      }
      this.buses = nodes; this.drumBus = drum; this.melBus = mel;
    },

    stepDur(s) {
      const base = 15 / this.bpm, sw = (this.recipe.swing || 0) * 0.0033;
      return s % 2 === 0 ? base * (1 + sw) : base * (1 - sw);
    },

    // ---------- กลอง ----------
    kick(t) {
      const c = this.ctx, o = c.createOscillator(), g = c.createGain();
      o.frequency.setValueAtTime(150, t); o.frequency.exponentialRampToValueAtTime(45, t + 0.12);
      g.gain.setValueAtTime(1, t); g.gain.exponentialRampToValueAtTime(0.001, t + 0.35);
      o.connect(g).connect(this.drumBus); o.start(t); o.stop(t + 0.4);
    },
    snare(t) {
      const c = this.ctx, n = c.createBufferSource(), hp = c.createBiquadFilter(), g = c.createGain();
      n.buffer = this.noise; hp.type = 'highpass'; hp.frequency.value = 1200;
      g.gain.setValueAtTime(0.55, t); g.gain.exponentialRampToValueAtTime(0.001, t + 0.18);
      n.connect(hp).connect(g).connect(this.drumBus); n.start(t, Math.random() * 0.5); n.stop(t + 0.2);
      const o = c.createOscillator(), og = c.createGain();
      o.type = 'triangle'; o.frequency.value = 190;
      og.gain.setValueAtTime(0.35, t); og.gain.exponentialRampToValueAtTime(0.001, t + 0.1);
      o.connect(og).connect(this.drumBus); o.start(t); o.stop(t + 0.12);
    },
    hat(t) {
      const c = this.ctx, n = c.createBufferSource(), hp = c.createBiquadFilter(), g = c.createGain();
      n.buffer = this.noise; hp.type = 'highpass'; hp.frequency.value = 7000;
      g.gain.setValueAtTime(0.28, t); g.gain.exponentialRampToValueAtTime(0.001, t + 0.05);
      n.connect(hp).connect(g).connect(this.drumBus); n.start(t, Math.random() * 0.5); n.stop(t + 0.06);
    },
    bass(t) {
      const c = this.ctx, o = c.createOscillator(), lp = c.createBiquadFilter(), g = c.createGain();
      o.type = 'sawtooth'; o.frequency.value = 110;                     // เบสเครื่องเล่นโน้ต A2
      lp.type = 'lowpass'; lp.frequency.setValueAtTime(900, t); lp.frequency.exponentialRampToValueAtTime(180, t + 0.25);
      g.gain.setValueAtTime(0.5, t); g.gain.exponentialRampToValueAtTime(0.001, t + 0.4);
      o.connect(lp).connect(g).connect(this.drumBus); o.start(t); o.stop(t + 0.45);
    },

    // ---------- เครื่องดนตรีเมโลดี้ (เลียนแบบเครื่องจริง) ----------
    freq(name) {
      const m = /^([A-G])(\d)$/.exec(name); if (!m) return 0;
      const semi = { C: 0, D: 2, E: 4, F: 5, G: 7, A: 9, B: 11 }[m[1]];
      return 440 * Math.pow(2, (12 * (+m[2] + 1) + semi - 69) / 12);
    },
    voice(inst, f, t, gate) {
      const c = this.ctx, out = c.createGain(), env = out.gain, end = t + gate;
      out.connect(this.melBus);
      const osc = (type, fr, lvl, dest = out) => {
        const o = c.createOscillator(), g = c.createGain();
        o.type = type; o.frequency.value = fr; g.gain.value = lvl;
        o.connect(g).connect(dest); o.start(t); return o;
      };
      let oscs = [], stopAt = end + 0.3;
      const sustained = (atk, rel, lvl) => {
        env.setValueAtTime(0, t); env.setTargetAtTime(lvl, t, atk); env.setTargetAtTime(0, end, rel); stopAt = end + rel * 6;
      };
      switch (inst) {
        case 'PIANO':
          oscs = [osc('sine', f, 1), osc('sine', 2 * f, 0.4), osc('sine', 3 * f, 0.15)];
          env.setValueAtTime(0.3, t); env.setTargetAtTime(0.12, t, 1.0); env.setTargetAtTime(0, end, 0.065); stopAt = end + 0.4; break;
        case 'BELL': {
          const mod = c.createOscillator(), mg = c.createGain(), car = c.createOscillator();
          mod.frequency.value = 3.5 * f; mg.gain.setValueAtTime(f * 3 * 3.5 * 0.3, t); mg.gain.setTargetAtTime(0, t, 0.15);
          car.frequency.value = f; mod.connect(mg).connect(car.frequency); car.connect(out);
          mod.start(t); car.start(t); oscs = [mod, car];
          env.setValueAtTime(0.28, t); env.setTargetAtTime(0.18, t, 1.5); env.setTargetAtTime(0, end, 0.45); stopAt = end + 2.7; break;
        }
        case 'PLUCK': {
          const lp = c.createBiquadFilter(); lp.type = 'lowpass'; lp.frequency.setValueAtTime(f * 8, t); lp.frequency.setTargetAtTime(f * 1.5, t, 0.15);
          lp.connect(out); oscs = [osc('sawtooth', f, 1, lp)];
          env.setValueAtTime(0.35, t); env.setTargetAtTime(0, end, 0.05); stopAt = end + 0.3; break;
        }
        case 'MARIMBA': {
          const g4 = c.createGain(); g4.gain.setValueAtTime(0.35, t); g4.gain.setTargetAtTime(0, t, 0.03); g4.connect(out);
          oscs = [osc('sine', f, 1), osc('sine', 4 * f, 1, g4)];
          env.setValueAtTime(0.4, t); env.setTargetAtTime(0, t, 0.1); stopAt = t + 0.6; break;
        }
        case 'CHIP': oscs = [osc('square', f, 1)]; sustained(0.001, 0.005, 0.1); break;
        case 'SYNTH': {
          const lp = c.createBiquadFilter(); lp.type = 'lowpass'; lp.frequency.value = Math.min(6000, f * 6); lp.connect(out);
          oscs = [osc('sawtooth', f, 0.5, lp), osc('sawtooth', f * 1.007, 0.5, lp)]; sustained(0.002, 0.015, 0.22); break;
        }
        case 'LEAD': {
          const lp = c.createBiquadFilter(); lp.type = 'lowpass'; lp.frequency.value = Math.min(8000, f * 10); lp.connect(out);
          oscs = [osc('square', f, 0.5, lp), osc('sawtooth', f * 1.01, 0.5, lp)]; sustained(0.0015, 0.008, 0.18); break;
        }
        case 'BRASS': {
          const lp = c.createBiquadFilter(); lp.type = 'lowpass';
          lp.frequency.setValueAtTime(f * 1.2, t); lp.frequency.setTargetAtTime(f * 5, t, 0.04); lp.connect(out);
          oscs = [osc('sawtooth', f, 1, lp)]; sustained(0.011, 0.015, 0.3); break;
        }
        case 'ORGAN': oscs = [osc('sine', f, 1), osc('sine', 2 * f, 0.6), osc('sine', 4 * f, 0.35)]; sustained(0.0045, 0.011, 0.14); break;
        case 'FLUTE': oscs = [osc('sine', f, 1), osc('sine', 2 * f, 0.15)]; sustained(0.018, 0.015, 0.3); break;
        default: oscs = [osc('triangle', f, 1)]; sustained(0.003, 0.05, 0.25);
      }
      oscs.forEach(o => o.stop(stopAt));
    },

    // ---------- ตัวนับจังหวะ ----------
    load(r, bpm) {
      this.recipe = r; this.bpm = bpm;
      this.patMap = Object.fromEntries(r.patterns.map(p => [p.name, p]));
    },
    async start(r, bpm) {
      this.ensure();
      if (this.ctx.state === 'suspended') await this.ctx.resume();
      this.stop(true);
      this.load(r, bpm);
      this.buildBuses(r);
      this.step = 0; this.bar = 0; this.queue = [];
      this.nextTime = this.ctx.currentTime + 0.06;
      this.timer = setInterval(() => this.tick(), 25);
      this.tick();
      this.raf();
    },
    setBpm(b) { this.bpm = b; if (this.delayNode) this.delayNode.delayTime.value = 3 * 15 / b; },
    stop(silent) {
      if (this.timer) { clearInterval(this.timer); this.timer = null; }
      if (this.buses) { const old = this.buses; setTimeout(() => old.forEach(n => { try { n.disconnect(); } catch (e) {} }), 50); this.buses = null; }
      this.queue = [];
      if (!silent && this.onStop) this.onStop();
    },
    get playing() { return !!this.timer; },
    tick() {
      const r = this.recipe, ahead = this.ctx.currentTime + 0.12;
      while (this.nextTime < ahead) {
        const t = this.nextTime, s = this.step;
        const chainPos = this.bar % r.chain.length, pat = this.patMap[r.chain[chainPos]];
        for (const [k] of TRACKS) {
          if (!this.mutes.has(k) && pat.drums[k] && pat.drums[k].includes(s + 1)) this[k](t);
        }
        (r.melody || []).forEach(l => {
          const n = l.notes[s];
          if (!this.mutes.has('L' + l.loop) && /^[A-G]\d$/.test(n)) this.voice(l.inst, this.freq(n), t, this.stepDur(s));
        });
        if (r.score && this.playScore) {
          const bar = r.score[this.bar % r.score.length];
          bar.forEach(([st, len, key]) => {
            if (st * 2 === s) {
              let g = 0; for (let k = 0; k < len * 2; k++) g += this.stepDur(s + k);
              this.voice('BELL', this.freq(KEY_NOTE[key - 1]), t, g * 0.92);
            }
          });
        }
        this.queue.push({ t, s, bar: this.bar, chainPos });
        this.nextTime += this.stepDur(s);
        this.step = (s + 1) % 16;
        if (this.step === 0) this.bar++;
      }
    },
    raf() {
      const loop = () => {
        if (!this.timer) return;
        const now = this.ctx.currentTime;
        let last = null;
        while (this.queue.length && this.queue[0].t <= now) last = this.queue.shift();
        if (last && this.onStep) this.onStep(last);
        requestAnimationFrame(loop);
      };
      requestAnimationFrame(loop);
    },
  };

  // =============================================================
  //  หน้าจอ
  // =============================================================
  let current = null;          // สูตรที่เปิดดูอยู่
  let playingId = null;

  function bpmOf(r) { return store.get('bpm-' + r.id, r.bpm || 100); }

  async function play(r) {
    const bpm = bpmOf(r);
    playingId = r.id;
    if (!current || current.id !== r.id) Engine.mutes = new Set();
    Engine.playScore = store.get('score-' + r.id, true);
    await Engine.start(r, bpm);
    refreshPlayButtons();
  }
  function stopPlay() { Engine.stop(); }
  Engine.onStop = () => { playingId = null; refreshPlayButtons(); clearPlayhead(); setStatus(''); };
  Engine.onStep = ({ s, bar, chainPos }) => {
    if (!current || current.id !== playingId) return;
    const r = current;
    view.querySelectorAll('.now').forEach(el => el.classList.remove('now'));
    const pname = r.chain[chainPos];
    view.querySelectorAll(`[data-pat="${pname}"] [data-s="${s}"], .mel [data-s="${s}"]`).forEach(el => el.classList.add('now'));
    if (r.chain.length > 1) view.querySelectorAll(`.chain [data-c="${chainPos}"]`).forEach(el => el.classList.add('now'));
    if (r.score) view.querySelectorAll(`.score [data-b="${bar % r.score.length}"]`).forEach(el => el.classList.add('now'));
    if (r.meter === '3/4') {
      const beat = Math.floor(((bar % r.chain.length) * 16 + s) / 2);
      setStatus(`ห้อง ${Math.floor(beat / 3) + 1}/8 · จังหวะ ${beat % 3 + 1} · PTRN ${pname}`);
      return;
    }
    const total = r.chain.length > 1 ? r.chain.length : (r.score ? r.score.length : 0);
    setStatus(total ? `ห้อง ${(bar % total) + 1}/${total} · PTRN ${pname} · ช่อง ${s + 1}` : `PTRN ${pname} · ช่อง ${s + 1}`);
  };
  function clearPlayhead() { view.querySelectorAll('.now').forEach(el => el.classList.remove('now')); }
  function setStatus(t) { const el = view.querySelector('#mStatus'); if (el) el.textContent = t || 'กดเล่นเพื่อฟังเสียงตัวอย่าง'; }

  function refreshPlayButtons() {
    view.querySelectorAll('[data-play]').forEach(b => {
      const on = b.dataset.play === playingId;
      b.classList.toggle('on', on);
      b.setAttribute('aria-label', (on ? 'หยุด ' : 'ลองฟัง ') + (byId(b.dataset.play) || {}).title);
      const big = b.classList.contains('bigplay');
      b.innerHTML = on ? icon('stop') + (big ? '<span>หยุด</span>' : '') : icon('play') + (big ? '<span>ลองฟัง</span>' : '');
    });
  }
  function icon(n) {
    return n === 'play'
      ? '<svg viewBox="0 0 24 24" width="18" height="18" aria-hidden="true"><path d="M7 5l12 7-12 7z" fill="currentColor"/></svg>'
      : '<svg viewBox="0 0 24 24" width="18" height="18" aria-hidden="true"><rect x="6" y="6" width="12" height="12" rx="2" fill="currentColor"/></svg>';
  }

  // ---------- หน้ารายการ ----------
  function renderList() {
    current = null;
    const g = ui.genre[ui.kind] || 'all';
    const q = ui.q.trim().toLowerCase();
    const items = RECIPES.filter(r => r.kind === ui.kind && (g === 'all' || (r.tags || []).includes(g)) &&
      (!q || (r.id + ' ' + r.title + ' ' + r.sub).toLowerCase().includes(q)));
    const chips = GENRES[ui.kind].map(([k, label]) => {
      const n = k === 'all' ? RECIPES.filter(r => r.kind === ui.kind).length : RECIPES.filter(r => r.kind === ui.kind && (r.tags || []).includes(k)).length;
      return `<button class="fchip" aria-pressed="${k === g}" data-genre="${k}">${esc(label)} <small>${n}</small></button>`;
    }).join('');
    view.innerHTML = `
      <div class="card mtop">
        <div class="seg" role="group" aria-label="เลือกประเภท">
          <button data-kind="song" aria-pressed="${ui.kind === 'song'}">${icon('play')}เพลง</button>
          <button data-kind="metro" aria-pressed="${ui.kind === 'metro'}"><svg viewBox="0 0 24 24" width="18" height="18" aria-hidden="true"><path d="M9 3h6l4 18H5z M12 17l5-11" fill="none" stroke="currentColor" stroke-width="2" stroke-linejoin="round" stroke-linecap="round"/></svg>เมโทรนอม</button>
        </div>
        <div class="fchips" role="group" aria-label="กรองตามแนว">${chips}</div>
        <label class="search"><span class="sr">ค้นหา</span><input id="mSearch" type="search" placeholder="ค้นหาชื่อเพลง เช่น lofi, jingle, ร็อก" value="${esc(ui.q)}"></label>
      </div>
      <div class="mcount">${items.length} สูตร · กด ▶ ฟังตัวอย่าง · กดการ์ดเพื่อดูวิธีกดบนเครื่อง</div>
      <div class="rgrid">${items.map(cardHtml).join('') || '<div class="empty">ไม่พบสูตรที่ตรงกับคำค้น</div>'}</div>`;
    refreshPlayButtons();
  }
  function cardHtml(r) {
    const tags = (r.tags || []).map(t => `<span class="tag">${esc(TAG_LABEL[t] || t)}</span>`).join('');
    const bpm = r.range ? `BPM ${r.range[0]}–${r.range[1]}` : `BPM ${r.bpm}`;
    return `<article class="rc">
      <a class="rclink" href="#music/${encodeURIComponent(r.id)}">
        <span class="rno">${esc(r.id)}</span>
        <b class="rtitle">${esc(r.title)}</b>
        <span class="rsub">${esc(r.sub)}</span>
        <span class="rmeta"><span class="mono">${bpm}</span>${r.meter ? ' · ' + r.meter : ''}${r.meter === '3/4' ? ' · 8 ห้อง' : r.chain.length > 1 ? ' · ' + r.chain.length + ' ห้อง' : ''}</span>
        <span class="tags">${tags}</span>
      </a>
      <button class="miniplay" data-play="${esc(r.id)}"></button>
    </article>`;
  }

  // ---------- หน้ารายละเอียด ----------
  function gridRow(label, cells, cls = '', attrs = '') {
    return `<div class="g16 ${cls}" ${attrs}><b>${label}</b>${cells.join('')}</div>`;
  }
  function renderDetail(r) {
    current = r;
    if (playingId !== r.id) Engine.mutes = new Set();
    const bpm = bpmOf(r);
    const tracksUsed = TRACKS.filter(([k]) => r.patterns.some(p => p.drums[k] !== undefined));
    const beatRow = gridRow('', r.beatLabels.map((l, i) => `<i data-s="${i}">${esc(l)}</i>`), 'beats');
    const pats = r.patterns.map(p => {
      const marks = p.marks ? gridRow('คอร์ด', Array.from({ length: 16 }, (_, i) => `<i>${p.marks[i + 1] ? `<span class="cmark">${esc(p.marks[i + 1])}</span>` : ''}</i>`), 'marks') : '';
      const rows = tracksUsed.map(([k, L]) => gridRow(L,
        Array.from({ length: 16 }, (_, i) => `<i data-s="${i}" class="${(p.drums[k] || []).includes(i + 1) ? 'x' : ''}"></i>`), 'drum t-' + k));
      const head = r.patterns.length > 1 ? `<div class="phead"><span class="pl">${p.name}</span><b>PTRN ${p.name}</b>${p.label ? `<span>${esc(p.label)}</span>` : ''}</div>` : '';
      return `<div class="pat" data-pat="${p.name}">${head}${marks}${beatRow}${rows.join('')}</div>`;
    }).join('');
    const mel = (r.melody || []).length ? `<section class="card"><h2>เมโลดี้ ${r.melody.length} ลูป <small>-- = เว้น (กด BPM) · · = หยุดพิมพ์</small></h2>
      <div class="mel scrollx">${gridRow('', r.beatLabels.map((l, i) => `<i data-s="${i}">${esc(l)}</i>`), 'beats')}
      ${r.melody.map(l => gridRow(`<u>${l.loop}</u>${esc(l.inst)}`, l.notes.map((n, i) =>
        `<i data-s="${i}" class="${n === '-' ? 'z' : n === '.' ? 'e' : 'nt'}">${n === '-' ? '--' : n === '.' ? '·' : esc(n)}</i>`), 'note')).join('')}</div></section>` : '';
    const chain = r.chain.length > 1 ? `<section class="card"><h2>${r.meter === '3/4' ? `chain ${r.chain.length} แพตเทิร์น <small>ต่อกัน = 8 ห้องของ 3/4</small>` : `chain ${r.chain.length} ห้อง`}</h2>
      <div class="chain">${[...r.chain].map((p, i) => `<div data-c="${i}" class="cbx${r.barChords && r.barChords[i] === 'C' ? ' home' : ''}"><small>${i + 1}</small><b class="pp">${p}</b>${r.barChords ? `<b class="cc">${esc(r.barChords[i])}</b>` : ''}</div>`).join('')}</div>
      <div class="small">ค้าง PTRN + ${[...r.chain].map(p => 'S' + ('ABCD'.indexOf(p) + 1)).join(' ')}</div></section>` : '';
    const score = r.score ? `<section class="card"><h2>ทำนองกดสด <small>เลข = ปุ่ม S · กล่องยาว = กดค้าง</small></h2>
      <label class="toggle"><input type="checkbox" id="mScore" ${store.get('score-' + r.id, true) ? 'checked' : ''}> ให้เว็บเล่นทำนองนี้ด้วยตอนลองฟัง</label>
      <div class="score">${r.score.map((bar, bi) => `<div class="sbar" data-b="${bi}"><div class="sbh"><b>${bi + 1}</b><span>${r.chain[bi % r.chain.length]}</span>${r.barChords ? `<b class="sc">${esc(r.barChords[bi])}</b>` : ''}</div>
        <div class="sev">${bar.map(([st, len, key]) => `<div style="grid-column:${st + 1} / span ${len}" class="${len === 1 ? 'n1' : ''}"><b>${key}</b>${len > 1 ? `<small>${DOREMI[KEY_NOTE[key - 1][0]]}</small>` : ''}</div>`).join('')}</div>
        <div class="sly">${bar.map(([st, len, , w]) => `<span style="grid-column:${st + 1} / span ${len}">${esc(w)}</span>`).join('')}</div></div>`).join('')}</div></section>` : '';
    const mutes = [...tracksUsed.map(([k, L]) => [k, L]), ...(r.melody || []).map(l => ['L' + l.loop, 'L' + l.loop + ' ' + l.inst])];
    view.innerHTML = `
      <a class="back" href="#music">← กลับไปรายการ</a>
      <section class="card dhead">
        <div class="dtop"><span class="rno">${esc(r.id)}</span><span class="tags">${(r.tags || []).map(t => `<span class="tag">${esc(TAG_LABEL[t] || t)}</span>`).join('')}</span></div>
        <h2 class="dtitle">${esc(r.title)}</h2>
        <div class="dsub">${esc(r.sub)} <span class="small">· ${esc(r.src)}</span></div>
        <div class="chips">${r.chips.map(c => `<span class="c${r.hi.includes(c) ? ' hi' : ''}">${esc(c)}</span>`).join('')}</div>
        ${r.guitar.map(g => `<div class="gt">${esc(g)}</div>`).join('')}
      </section>
      <section class="card player">
        <div class="prow">
          <button class="bigplay" data-play="${esc(r.id)}"></button>
          <div class="bpmbox" role="group" aria-label="ความเร็ว">
            <button class="sbtn" data-bpm="-5" aria-label="ลด 5 BPM">−5</button>
            <button class="sbtn" data-bpm="-1" aria-label="ลด 1 BPM">−1</button>
            <span class="bpmv"><b id="mBpm">${bpm}</b><small>BPM</small></span>
            <button class="sbtn" data-bpm="1" aria-label="เพิ่ม 1 BPM">+1</button>
            <button class="sbtn" data-bpm="5" aria-label="เพิ่ม 5 BPM">+5</button>
          </div>
        </div>
        <div id="mStatus" class="state" aria-live="polite">กดเล่นเพื่อฟังเสียงตัวอย่าง</div>
        <div class="mutes" role="group" aria-label="ปิดเสียงบางแทร็ก"><span class="small mlab">แตะเพื่อปิด/เปิดเสียง</span>${mutes.map(([k, L]) => `<button class="fchip" data-mute="${k}" aria-pressed="${!Engine.mutes.has(k)}">${esc(L)}</button>`).join('')}</div>
        <div class="small">เสียงตัวอย่างจากเว็บใกล้เคียงเครื่องจริง แต่ไม่เหมือน 100% · BPM ที่ปรับจำไว้ให้แต่ละสูตร${r.range ? ` · ช่วงแนะนำ ${r.range[0]}–${r.range[1]}` : ''}</div>
      </section>
      ${chain}
      <section class="card"><h2>กลอง${r.patterns.length > 1 ? ` ${r.patterns.length} แพตเทิร์น` : ''} <small>■ = กดเปิดช่องนั้น</small></h2><div class="scrollx pats">${pats}</div></section>
      ${mel}
      ${score}
      <section class="card"><h2>เคล็ดลับ</h2>${r.tips.map(t => `<p class="tip">${esc(t)}</p>`).join('')}
        <details><summary>วิธีพิมพ์สูตรเข้าเครื่อง</summary><ol>
          <li>ล้างของเดิม: โหมดบีต SELECT ค้าง · โหมดเมโลดี้ SELECT ค้างทีละลูป</li>
          <li>BPM: หมุนปุ่ม A · SWING: ค้าง BPM + หมุนปุ่ม A</li>
          <li>FX: กด FX จนชื่อขึ้น · ความแรง: ค้าง FX + หมุน A · ♫ KEYS / ALL: ค้าง FX 1 วิ</li>
          <li>กลอง: หยุดเครื่อง · SOUND เลือกแทร็ก · กดช่องที่เป็น ■</li>
          <li>เมโลดี้: REC → PTRN เลือกลูป → SOUND เลือกเครื่องดนตรี → พิมพ์โน้ต (-- = กด BPM แล้วปล่อย)</li>
          <li>เซฟ: REC ค้างจนขึ้น SAVED · chain ไม่เซฟ ตั้งใหม่ทุกครั้งที่เปิดเครื่อง</li>
          <li>อยากเก็บเป็นไฟล์: ไปแท็บ <a href="#rec">อัดเสียง</a></li>
        </ol></details>
      </section>`;
    refreshPlayButtons();
    if (playingId === r.id) Engine.load(r, Engine.bpm);
  }

  // ---------- เหตุการณ์ ----------
  view.addEventListener('click', e => {
    const b = e.target.closest('button'); if (!b) return;
    if (b.dataset.play) {
      const r = byId(b.dataset.play);
      if (playingId === r.id) stopPlay(); else play(r);
    } else if (b.dataset.kind) {
      ui.kind = b.dataset.kind; store.set('kind', ui.kind); ui.q = ''; renderList();
    } else if (b.dataset.genre) {
      ui.genre[ui.kind] = b.dataset.genre; store.set('genre', ui.genre); renderList();
    } else if (b.dataset.bpm && current) {
      const lo = 40, hi = 240;
      const v = Math.max(lo, Math.min(hi, bpmOf(current) + +b.dataset.bpm));
      store.set('bpm-' + current.id, v);
      view.querySelector('#mBpm').textContent = v;
      if (playingId === current.id) Engine.setBpm(v);
    } else if (b.dataset.mute && current) {
      const k = b.dataset.mute;
      if (Engine.mutes.has(k)) Engine.mutes.delete(k); else Engine.mutes.add(k);
      b.setAttribute('aria-pressed', String(!Engine.mutes.has(k)));
    }
  });
  view.addEventListener('input', e => {
    if (e.target.id === 'mSearch') {
      ui.q = e.target.value;
      const pos = e.target.selectionStart;
      renderList();
      const s = view.querySelector('#mSearch'); s.focus(); s.setSelectionRange(pos, pos);
    }
  });
  view.addEventListener('change', e => {
    if (e.target.id === 'mScore' && current) {
      store.set('score-' + current.id, e.target.checked);
      Engine.playScore = e.target.checked;
    }
  });

  // ---------- เส้นทางหน้า (#music, #music/ID) ----------
  window.PSMusic = {
    route(hash) {
      const m = /^#music\/(.+)$/.exec(hash);
      const r = m && byId(decodeURIComponent(m[1]));
      if (r) { renderDetail(r); window.scrollTo(0, 0); }
      else renderList();
    },
    stop: stopPlay,
  };
})();
