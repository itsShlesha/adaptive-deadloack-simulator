/* ══════════════════════════════════════════════════════════════
   BOOT SEQUENCE
══════════════════════════════════════════════════════════════ */
const bootLines = [
  'ADAPTIVE DEADLOCK MANAGEMENT SYSTEM v1.0',
  'Copyright (C) 2026. All rights reserved.',
  '',
  'Initializing memory subsystem........... OK',
  'Loading resource allocation tables...... OK',
  "Mounting Banker's Algorithm module....... OK",
  'Initializing RAG engine.................. OK',
  'DFS cycle detector ready................. OK',
  'Dynamic Max Update module loaded......... OK',
  '',
  'System ready.',
  '',
  'Press any key to continue_',
];

let bootEl     = document.getElementById('boot-text');
let bootScreen = document.getElementById('boot-screen');
let lineIdx = 0, charIdx = 0;
let bootDone = false;

function typeBoot() {
  if (lineIdx >= bootLines.length) { bootDone = true; return; }
  const line = bootLines[lineIdx];
  if (charIdx <= line.length) {
    bootEl.textContent = bootLines.slice(0, lineIdx).join('\n')
      + (lineIdx > 0 ? '\n' : '') + line.slice(0, charIdx);
    charIdx++;
    setTimeout(typeBoot, charIdx === 1 ? 60 : 18);
  } else {
    lineIdx++; charIdx = 0;
    setTimeout(typeBoot, lineIdx === bootLines.length ? 0 : 80);
  }
}

function dismissBoot() {
  if (!bootDone) {
    lineIdx = bootLines.length;
    bootEl.textContent = bootLines.join('\n');
    bootDone = true;
    return;
  }
  bootScreen.classList.add('done');
  document.getElementById('main-app').style.display = 'block';
  setTimeout(() => bootScreen.remove(), 700);
}

document.addEventListener('keydown', dismissBoot);
document.addEventListener('click',   dismissBoot);
typeBoot();

/* ══════════════════════════════════════════════════════════════
   GLOBAL STATE
══════════════════════════════════════════════════════════════ */
const S = {
  np: 0, nr: 0,
  total: [],
  alloc: [],
  maxN:  [],
  need:  [],
  avail: [],
  mode: '',             /* 'single' | 'multi' */
  allocCounter: 0,
  sweepCounter: 0,
  ready: false,
  SWEEP_INTERVAL: 3,
};

/* ══════════════════════════════════════════════════════════════
   TAB SWITCHING
══════════════════════════════════════════════════════════════ */
function switchTab(id) {
  document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
  document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
  document.getElementById('panel-' + id).classList.add('active');
  event.target.classList.add('active');

  if (id === 'state'   && S.ready) renderBlockViz();
  if (id === 'request' && S.ready) buildReqVector();
}

/* ══════════════════════════════════════════════════════════════
   TAB 1 — BUILD INPUT GRIDS
══════════════════════════════════════════════════════════════ */
function buildInputs() {
  const np = parseInt(document.getElementById('num-p').value);
  const nr = parseInt(document.getElementById('num-r').value);

  if (np < 1 || np > 5 || nr < 1 || nr > 5) {
    termWrite('term-init', '> ERROR: processes 1-5, resources 1-5.', 'err');
    return;
  }

  /* total instances row */
  let totHtml = '';
  for (let j = 0; j < nr; j++)
    totHtml += `<div style="display:flex;align-items:center;gap:4px;">
      <span style="font-family:var(--font-mono);font-size:13px;color:var(--green-dim)">R${j}:</span>
      <input type="number" id="tot-${j}" min="1" max="20" value="1" style="width:52px">
    </div>`;
  document.getElementById('total-inputs').innerHTML = totHtml;

  /* column headers helper */
  function colHeaders() {
    let h = `<div class="matrix-grid" style="grid-template-columns: 40px repeat(${nr}, 52px);">`;
    h += '<div></div>';
    for (let j = 0; j < nr; j++)
      h += `<div style="font-family:var(--font-mono);font-size:13px;color:var(--green-dim);text-align:center;">R${j}</div>`;
    return h;
  }

  /* matrix row inputs helper */
  function matrixRows(prefix) {
    let rows = '';
    for (let i = 0; i < np; i++) {
      rows += `<div class="matrix-row-label">P${i}</div>`;
      for (let j = 0; j < nr; j++)
        rows += `<input type="number" id="${prefix}-${i}-${j}" min="0" max="20" value="0">`;
    }
    return rows + '</div>';
  }

  document.getElementById('alloc-grid').innerHTML = colHeaders() + matrixRows('a');
  document.getElementById('max-grid').innerHTML   = colHeaders() + matrixRows('m');
  document.getElementById('matrix-inputs').style.display = 'block';
  termWrite('term-init',
    `> Matrices built for ${np} processes × ${nr} resources.\n> Fill in values and press INITIALIZE SYSTEM.`, 'ok');
}

/* ══════════════════════════════════════════════════════════════
   TAB 1 — INITIALIZE SYSTEM
══════════════════════════════════════════════════════════════ */
function initSystem() {
  const np = parseInt(document.getElementById('num-p').value);
  const nr = parseInt(document.getElementById('num-r').value);

  S.np = np; S.nr = nr;
  S.total = []; S.alloc = []; S.maxN = []; S.need = []; S.avail = [];
  S.allocCounter = 0; S.sweepCounter = 0;

  /* read totals */
  for (let j = 0; j < nr; j++) {
    const v = parseInt(document.getElementById(`tot-${j}`).value);
    if (isNaN(v) || v < 1) {
      termWrite('term-init', '> ERROR: total instances must be >= 1.', 'err');
      return;
    }
    S.total.push(v);
  }

  /* read alloc and max */
  for (let i = 0; i < np; i++) {
    S.alloc.push([]); S.maxN.push([]);
    for (let j = 0; j < nr; j++) {
      const a = parseInt(document.getElementById(`a-${i}-${j}`).value) || 0;
      const m = parseInt(document.getElementById(`m-${i}-${j}`).value) || 0;
      if (a > m) {
        termWrite('term-init', `> ERROR: P${i} R${j} allocation > max.`, 'err');
        return;
      }
      S.alloc[i].push(a);
      S.maxN[i].push(m);
    }
  }

  calcNeedAvail();
  S.mode  = S.total.some(t => t > 1) ? 'multi' : 'single';
  S.ready = true;

  updateStatusBar();
  buildReqVector();

  let log  = `> System initialized.\n`;
      log += `> Processes: ${np}   Resources: ${nr}\n`;
      log += `> Mode: ${S.mode === 'multi' ? "MULTI-INSTANCE — Banker's primary" : 'SINGLE-INSTANCE — RAG+DFS primary'}\n`;
      log += `> Need matrix calculated. Available computed.\n`;
      log += `> Ready.`;
  termWrite('term-init', log, 'ok');
}

/* ── recalculate need and available ── */
function calcNeedAvail() {
  S.need = [];
  for (let i = 0; i < S.np; i++) {
    S.need.push([]);
    for (let j = 0; j < S.nr; j++)
      S.need[i].push(S.maxN[i][j] - S.alloc[i][j]);
  }
  S.avail = S.total.map((t, j) => {
    let used = 0;
    for (let i = 0; i < S.np; i++) used += S.alloc[i][j];
    return t - used;
  });
}

/* ══════════════════════════════════════════════════════════════
   TAB 2 — BLOCK VISUALIZER
══════════════════════════════════════════════════════════════ */
function renderBlockViz() {
  const viz = document.getElementById('block-viz');
  if (!S.ready) return;

  let html = '';
  for (let i = 0; i < S.np; i++) {
    html += `<div class="bv-process">
      <div class="bv-pid">P${i}</div>
      <div class="bv-resources">`;

    for (let j = 0; j < S.nr; j++) {
      const held = S.alloc[i][j];
      const need = S.need[i][j];

      html += `<div class="bv-row">
        <div class="bv-rlabel">R${j}</div>
        <div class="bv-blocks">`;

      for (let b = 0; b < held; b++)
        html += `<div class="block block-held" title="P${i} holds R${j}">█</div>`;
      for (let b = 0; b < need; b++)
        html += `<div class="block block-need" title="P${i} needs R${j}">░</div>`;

      html += `</div>
        <span style="font-family:var(--font-mono);font-size:12px;color:var(--green-dim);margin-left:6px">
          ${held} held / ${need} needed
        </span>
      </div>`;
    }
    html += `</div></div>`;
  }
  viz.innerHTML = html;

  document.getElementById('avail-display').textContent =
    S.avail.map((v, j) => `R${j}:${v}`).join('  ');
}

/* ══════════════════════════════════════════════════════════════
   BANKER'S ALGORITHM — CORE LOGIC
══════════════════════════════════════════════════════════════ */
function isSafe() {
  const work   = [...S.avail];
  const finish = Array(S.np).fill(false);
  const seq    = [];
  let count    = 0;

  while (count < S.np) {
    let found = false;
    for (let i = 0; i < S.np; i++) {
      if (finish[i]) continue;
      let canRun = true;
      for (let j = 0; j < S.nr; j++) {
        if (S.need[i][j] > work[j]) { canRun = false; break; }
      }
      if (canRun) {
        for (let j = 0; j < S.nr; j++) work[j] += S.alloc[i][j];
        finish[i] = true;
        seq.push(i);
        count++; found = true;
      }
    }
    if (!found) break;
  }
  return count === S.np ? seq : null;
}

/* ══════════════════════════════════════════════════════════════
   TAB 3 — RUN BANKER'S
══════════════════════════════════════════════════════════════ */
function runBanker() {
  if (!S.ready) { termWrite('term-banker', '> ERROR: Initialize system first.', 'err'); return; }

  const seq  = isSafe();
  const wrap = document.getElementById('seq-display-wrap');
  const disp = document.getElementById('seq-display');

  if (seq) {
    let html = '';
    seq.forEach((pid, idx) => {
      html += `<div class="seq-node" id="sn-${idx}">P${pid}</div>`;
      if (idx < seq.length - 1)
        html += `<div class="seq-arrow" id="sa-${idx}">→</div>`;
    });
    disp.innerHTML    = html;
    wrap.style.display = 'block';

    /* animate nodes lighting up */
    seq.forEach((_, idx) => {
      setTimeout(() => {
        document.getElementById(`sn-${idx}`).classList.add('lit');
        if (idx > 0) document.getElementById(`sa-${idx - 1}`).classList.add('lit');
      }, idx * 500);
    });

    termWrite('term-banker',
      `> Result: SAFE STATE\n> Safe sequence: ${seq.map(p => 'P' + p).join(' → ')}\n> All processes can complete without deadlock.`,
      'ok');
  } else {
    wrap.style.display = 'none';
    termWrite('term-banker',
      `> Result: UNSAFE STATE\n> No safe sequence exists.\n> System is at risk of deadlock.`,
      'err');
  }
}

/* ══════════════════════════════════════════════════════════════
   RAG — BUILD ADJACENCY MATRIX
══════════════════════════════════════════════════════════════ */
function buildRAGMatrix() {
  const total = S.np + S.nr;
  const rag   = Array.from({ length: total }, () => Array(total).fill(0));

  for (let i = 0; i < S.np; i++) {
    for (let j = 0; j < S.nr; j++) {
      const pn = i, rn = S.np + j;
      if (S.need[i][j]  > 0) rag[pn][rn] = 1;   /* request edge   P→R */
      if (S.alloc[i][j] > 0) rag[rn][pn] = 1;   /* assignment edge R→P */
    }
  }
  return rag;
}

/* ══════════════════════════════════════════════════════════════
   TAB 4 — DRAW RAG ON CANVAS
══════════════════════════════════════════════════════════════ */
function drawRAG() {
  if (!S.ready) { termWrite('term-rag', '> ERROR: Initialize system first.', 'err'); return; }

  const canvas = document.getElementById('rag-canvas');
  const W = canvas.offsetWidth || 680;
  canvas.width = W; canvas.height = 380;
  const ctx = canvas.getContext('2d');
  ctx.clearRect(0, 0, W, 380);

  const rag   = buildRAGMatrix();
  const nodes = [];
  const topY  = 100, botY = 270;

  /* process nodes — top row */
  const pSpacing = W / (S.np + 1);
  for (let i = 0; i < S.np; i++)
    nodes.push({ x: pSpacing * (i + 1), y: topY, label: `P${i}`, type: 'process' });

  /* resource nodes — bottom row */
  const rSpacing = W / (S.nr + 1);
  for (let j = 0; j < S.nr; j++)
    nodes.push({ x: rSpacing * (j + 1), y: botY, label: `R${j}`, type: 'resource' });

  const cycleExists = hasCycleCheck(rag);
  const total       = S.np + S.nr;

  /* draw edges first (so nodes render on top) */
  for (let from = 0; from < total; from++) {
    for (let to = 0; to < total; to++) {
      if (!rag[from][to]) continue;
      const isAssign = from >= S.np;
      drawArrow(ctx, nodes[from], nodes[to],
        isAssign ? '#00ff41' : '#00882a', isAssign);
    }
  }

  /* draw nodes */
  nodes.forEach(n => {
    ctx.save();
    if (n.type === 'process') {
      ctx.beginPath();
      ctx.arc(n.x, n.y, 28, 0, Math.PI * 2);
      ctx.fillStyle   = '#001a06';
      ctx.fill();
      ctx.strokeStyle = '#00ff41';
      ctx.lineWidth   = 2;
      ctx.shadowColor = '#00ff41';
      ctx.shadowBlur  = 10;
      ctx.stroke();
    } else {
      ctx.beginPath();
      ctx.moveTo(n.x,      n.y - 28);
      ctx.lineTo(n.x + 28, n.y);
      ctx.lineTo(n.x,      n.y + 28);
      ctx.lineTo(n.x - 28, n.y);
      ctx.closePath();
      ctx.fillStyle   = '#001a06';
      ctx.fill();
      ctx.strokeStyle = '#00ff41';
      ctx.lineWidth   = 2;
      ctx.shadowColor = '#00ff41';
      ctx.shadowBlur  = 10;
      ctx.stroke();
    }
    ctx.restore();

    /* node label */
    ctx.fillStyle     = '#00ff41';
    ctx.font          = 'bold 16px "VT323", monospace';
    ctx.textAlign     = 'center';
    ctx.textBaseline  = 'middle';
    ctx.fillText(n.label, n.x, n.y);
  });

  /* canvas legend */
  ctx.font      = '13px "Share Tech Mono", monospace';
  ctx.fillStyle = '#00aa2a';
  ctx.textAlign = 'left';
  ctx.fillText('● Process   ◆ Resource   dim arrow = request   bright arrow = assignment', 14, 350);

  let log = `> RAG built. ${S.np} processes + ${S.nr} resources = ${S.np + S.nr} nodes.\n`;
  log += cycleExists
    ? `> CYCLE DETECTED in graph.\n> ${S.mode === 'single'
        ? 'DEADLOCK CONFIRMED (single-instance).'
        : "Potential deadlock (multi-instance — run Banker's to confirm)."}`
    : `> No cycle detected. Graph is acyclic.`;
  termWrite('term-rag', log, cycleExists ? 'warn' : 'ok');
}

/* ── draw a single directed arrow on the canvas ── */
function drawArrow(ctx, from, to, color, bright) {
  const dx  = to.x - from.x, dy = to.y - from.y;
  const len = Math.sqrt(dx * dx + dy * dy);
  const ux  = dx / len, uy = dy / len;
  const r   = 30;
  const sx  = from.x + ux * r, sy = from.y + uy * r;
  const ex  = to.x   - ux * r, ey = to.y   - uy * r;

  ctx.save();
  ctx.strokeStyle = color;
  ctx.lineWidth   = bright ? 2 : 1;
  ctx.shadowColor = color;
  ctx.shadowBlur  = bright ? 10 : 4;
  ctx.setLineDash(bright ? [] : [5, 4]);
  ctx.beginPath();
  ctx.moveTo(sx, sy);
  ctx.lineTo(ex, ey);
  ctx.stroke();

  /* arrowhead */
  const angle = Math.atan2(ey - sy, ex - sx);
  ctx.setLineDash([]);
  ctx.beginPath();
  ctx.moveTo(ex, ey);
  ctx.lineTo(ex - 10 * Math.cos(angle - 0.4), ey - 10 * Math.sin(angle - 0.4));
  ctx.lineTo(ex - 10 * Math.cos(angle + 0.4), ey - 10 * Math.sin(angle + 0.4));
  ctx.closePath();
  ctx.fillStyle = color;
  ctx.fill();
  ctx.restore();
}

/* ══════════════════════════════════════════════════════════════
   DFS CYCLE DETECTION
══════════════════════════════════════════════════════════════ */
function hasCycleCheck(rag) {
  const total    = rag.length;
  const visited  = Array(total).fill(false);
  const recStack = Array(total).fill(false);

  function dfs(node) {
    visited[node]  = true;
    recStack[node] = true;
    for (let nb = 0; nb < total; nb++) {
      if (!rag[node][nb]) continue;
      if (!visited[nb] && dfs(nb)) return true;
      if (recStack[nb]) return true;
    }
    recStack[node] = false;
    return false;
  }

  for (let i = 0; i < total; i++)
    if (!visited[i] && dfs(i)) return true;
  return false;
}

/* ══════════════════════════════════════════════════════════════
   TAB 5 — DETECT DEADLOCK
══════════════════════════════════════════════════════════════ */
function detectDeadlock() {
  if (!S.ready) { termWrite('term-detect', '> ERROR: Initialize system first.', 'err'); return; }

  const rag   = buildRAGMatrix();
  const cycle = hasCycleCheck(rag);

  if (cycle) {
    if (S.mode === 'single') {
      termWrite('term-detect',
        `> Cycle detected in RAG.\n> Mode: SINGLE-INSTANCE\n> Result: DEADLOCK CONFIRMED\n> In single-instance systems, a cycle guarantees deadlock.`,
        'err');
    } else {
      termWrite('term-detect',
        `> Cycle detected in RAG.\n> Mode: MULTI-INSTANCE\n> Result: POTENTIAL DEADLOCK (not guaranteed)\n> Run Banker's Algorithm to confirm.`,
        'warn');
    }
  } else {
    termWrite('term-detect',
      `> No cycle detected in RAG.\n> Result: NO DEADLOCK\n> System appears safe.`,
      'ok');
  }
}

/* ══════════════════════════════════════════════════════════════
   TAB 6 — REQUEST RESOURCE
══════════════════════════════════════════════════════════════ */
function buildReqVector() {
  if (!S.ready) return;
  let html = '';
  for (let j = 0; j < S.nr; j++)
    html += `<div style="display:flex;align-items:center;gap:4px;">
      <span style="font-family:var(--font-mono);font-size:13px;color:var(--green-dim)">R${j}:</span>
      <input type="number" id="rv-${j}" min="0" max="20" value="0" style="width:52px">
    </div>`;
  document.getElementById('req-vector').innerHTML = html;
}

function requestResource() {
  if (!S.ready) { termWrite('term-request', '> ERROR: Initialize system first.', 'err'); return; }

  const pid = parseInt(document.getElementById('req-pid').value);
  if (pid < 0 || pid >= S.np) {
    termWrite('term-request', '> ERROR: Invalid process ID.', 'err');
    return;
  }

  const req = [];
  for (let j = 0; j < S.nr; j++)
    req.push(parseInt(document.getElementById(`rv-${j}`).value) || 0);

  let log = `> Request by P${pid}: [ ${req.join(' ')} ]\n`;

  /* check available */
  for (let j = 0; j < S.nr; j++) {
    if (req[j] > S.avail[j]) {
      log += `> DENIED — R${j} requested ${req[j]} but only ${S.avail[j]} available.\n> Process P${pid} must wait.`;
      termWrite('term-request', log, 'err');
      return;
    }
  }

  /* check need — detect dynamic case */
  const exceedsNeed = req.some((v, j) => v > S.need[pid][j]);
  const oldMax      = [...S.maxN[pid]];
  const oldNeed     = [...S.need[pid]];

  if (exceedsNeed) {
    log += `> Request exceeds declared Need.\n> Attempting dynamic Max update...\n`;
    for (let j = 0; j < S.nr; j++) {
      if (req[j] > S.need[pid][j]) {
        S.maxN[pid][j] = S.alloc[pid][j] + req[j];
        S.need[pid][j] = req[j];
      }
    }
    log += `> Max updated for P${pid}: [ ${S.maxN[pid].join(' ')} ]\n`;
  } else {
    log += `> Request within declared Need. Proceeding...\n`;
  }

  /* temp allocate */
  for (let j = 0; j < S.nr; j++) {
    S.avail[j]      -= req[j];
    S.alloc[pid][j] += req[j];
    S.need[pid][j]  -= req[j];
  }

  /* run Banker's */
  const seq = isSafe();

  if (seq) {
    log += `> Banker's result: SAFE\n> Allocation confirmed.\n> Safe sequence: ${seq.map(p => 'P' + p).join(' → ')}`;
    termWrite('term-request', log, 'ok');
    S.allocCounter++;
    updateStatusBar();
    if (S.allocCounter % S.SWEEP_INTERVAL === 0)
      periodicSweep('term-request');
  } else {
    /* rollback */
    for (let j = 0; j < S.nr; j++) {
      S.avail[j]      += req[j];
      S.alloc[pid][j] -= req[j];
    }
    if (exceedsNeed) { S.maxN[pid] = oldMax; S.need[pid] = oldNeed; }
    else { for (let j = 0; j < S.nr; j++) S.need[pid][j] += req[j]; }

    log += `> Banker's result: UNSAFE\n> Rolled back. P${pid} must wait.`;
    termWrite('term-request', log, 'err');
  }

  if (document.getElementById('panel-state').classList.contains('active'))
    renderBlockViz();
}

/* ══════════════════════════════════════════════════════════════
   TAB 7 — RANDOM SIMULATION
══════════════════════════════════════════════════════════════ */
function runSimulation() {
  if (!S.ready) { termWrite('term-sim', '> ERROR: Initialize system first.', 'err'); return; }

  const n = parseInt(document.getElementById('sim-rounds').value);
  if (n < 1) { termWrite('term-sim', '> ERROR: Enter at least 1 round.', 'err'); return; }

  let log  = `> Starting ${n}-round random simulation...\n`;
      log += `> Sweep interval: every ${S.SWEEP_INTERVAL} allocations\n\n`;

  for (let round = 1; round <= n; round++) {
    const pid = Math.floor(Math.random() * S.np);
    const req = [];
    let allZero = true;

    for (let j = 0; j < S.nr; j++) {
      const v = S.need[pid][j] > 0
        ? Math.floor(Math.random() * (S.need[pid][j] + 1))
        : 0;
      req.push(v);
      if (v > 0) allZero = false;
    }

    log += `> Round ${round}: P${pid} requests [ ${req.join(' ')} ]\n`;

    if (allZero) { log += `  → SKIPPED (zero request)\n\n`; continue; }

    const enough = req.every((v, j) => v <= S.avail[j]);
    if (!enough) { log += `  → DENIED (insufficient resources)\n\n`; continue; }

    /* temp allocate */
    for (let j = 0; j < S.nr; j++) {
      S.avail[j]      -= req[j];
      S.alloc[pid][j] += req[j];
      S.need[pid][j]  -= req[j];
    }

    const seq = isSafe();
    if (seq) {
      log += `  → APPROVED | sequence: ${seq.map(p => 'P' + p).join('→')}\n\n`;
      S.allocCounter++;
      updateStatusBar();

      if (S.allocCounter % S.SWEEP_INTERVAL === 0) {
        const rag    = buildRAGMatrix();
        const cycle  = hasCycleCheck(rag);
        const safe2  = isSafe();
        log += `  [SWEEP] Periodic detection fired (alloc #${S.allocCounter})\n`;
        log += cycle ? `  [SWEEP] WARNING: cycle detected in RAG.\n` : `  [SWEEP] RAG: no cycle.\n`;
        if (S.mode === 'multi')
          log += safe2 ? `  [SWEEP] Banker's: SAFE.\n` : `  [SWEEP] Banker's: UNSAFE — review state.\n`;
        log += '\n';
        S.sweepCounter++;
        updateStatusBar();
      }
    } else {
      for (let j = 0; j < S.nr; j++) {
        S.avail[j]      += req[j];
        S.alloc[pid][j] -= req[j];
        S.need[pid][j]  += req[j];
      }
      log += `  → DENIED (unsafe state)\n\n`;
    }
  }

  log += `> Simulation complete.\n> Total confirmed allocations: ${S.allocCounter}`;
  termWrite('term-sim', log, 'ok');
}

/* ══════════════════════════════════════════════════════════════
   PERIODIC SWEEP
══════════════════════════════════════════════════════════════ */
function periodicSweep(termId) {
  const rag   = buildRAGMatrix();
  const cycle = hasCycleCheck(rag);
  const safe2 = isSafe();
  S.sweepCounter++;
  updateStatusBar();

  let log  = `\n  [SWEEP] Periodic detection — alloc count: ${S.allocCounter}\n`;
      log += cycle ? `  [SWEEP] WARNING: cycle in RAG.\n` : `  [SWEEP] RAG: clean, no cycle.\n`;
  if (S.mode === 'multi')
    log += safe2 ? `  [SWEEP] Banker's: SAFE.\n` : `  [SWEEP] Banker's: UNSAFE.\n`;

  const el = document.getElementById(termId);
  el.textContent += log;
  el.scrollTop    = el.scrollHeight;
}

/* ══════════════════════════════════════════════════════════════
   HELPERS
══════════════════════════════════════════════════════════════ */
function termWrite(id, msg, type) {
  const cls = type === 'err'  ? 't-err'
            : type === 'warn' ? 't-warn'
            : type === 'info' ? 't-info'
            : 't-ok';
  const el = document.getElementById(id);
  el.innerHTML  = `<span class="${cls}">${msg}</span>`;
  el.scrollTop  = el.scrollHeight;
}

function updateStatusBar() {
  document.getElementById('sb-mode').textContent  = S.ready ? (S.mode === 'multi' ? 'MULTI' : 'SINGLE') : '—';
  document.getElementById('sb-p').textContent     = S.ready ? S.np : '—';
  document.getElementById('sb-r').textContent     = S.ready ? S.nr : '—';
  document.getElementById('sb-alloc').textContent = S.allocCounter;
  document.getElementById('sb-sweep').textContent = S.sweepCounter;
}
