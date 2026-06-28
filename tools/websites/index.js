// TILE DEFINITIONS & HELPERS
let TILE_DEFS = {};
let TILE_CATEGORIES = []; // Stores { id, name, tiles: [] }
let activeCategory = "all";

function hslToHex(h, s, l) {
  l /= 100;
  const a = (s * Math.min(l, 1 - l)) / 100;
  const f = (n) => {
    const k = (n + h / 30) % 12;
    const color = l - a * Math.max(Math.min(k - 3, 9 - k, 1), -1);
    return Math.round(255 * color)
      .toString(16)
      .padStart(2, "0");
  };
  return `#${f(0)}${f(8)}${f(4)}`;
}

function generateDeterministicColor(seedString) {
  let hash = 0;
  for (let i = 0; i < seedString.length; i++) {
    hash = seedString.charCodeAt(i) + ((hash << 5) - hash);
  }
  const h = Math.abs(hash) % 360;
  return hslToHex(h, 70, 45);
}

async function loadTileDefinitions() {
  try {
    const response = await fetch("../tile_map.json");
    if (!response.ok) throw new Error("Network response was not ok");
    const data = await response.json();

    const shortcuts = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "0"];
    let shortcutIdx = 0;

    // Initialize "All Tiles" as the default base category
    TILE_CATEGORIES = [{ id: "all", name: "All Tiles", tiles: [] }];
    let currentGroup = TILE_CATEGORIES[0];

    // Iterate over the keys (JS preserves insertion order for string keys)
    for (const key of Object.keys(data.TILE_MAP_META)) {
      if (key === "_legacy_aliases") continue;

      // Detect if this is a group header
      if (key.startsWith("_group_")) {
        // Extract the nice string name after the colon (e.g., "Core / System")
        const groupName =
          data.TILE_MAP_META[key].split(":")[1]?.trim() ||
          data.TILE_MAP_META[key];
        currentGroup = { id: key, name: groupName, tiles: [] };
        TILE_CATEGORIES.push(currentGroup);
        continue;
      }

      // Standard tile mapping
      const metaLabel = data.TILE_MAP_META[key];
      const baseColor = generateDeterministicColor(key + metaLabel);

      TILE_DEFS[key] = {
        label: metaLabel
          .replace(/_/g, " ")
          .replace(/\b\w/g, (l) => l.toUpperCase()),
        shortLabel: metaLabel.split("—")[0].trim().toLowerCase(),
        mapValue: data.TILE_MAP[key],
        key: shortcutIdx < shortcuts.length ? shortcuts[shortcutIdx] : "",
        bg: baseColor + "40",
        border: baseColor,
        textColor: "#ffffff",
      };

      // Add to "All Tiles" and the respective group
      TILE_CATEGORIES[0].tiles.push(key);
      if (currentGroup !== TILE_CATEGORIES[0]) {
        currentGroup.tiles.push(key);
      }

      shortcutIdx++;
    }
  } catch (error) {
    console.error(
      "Failed to load tile_map.json. Did you run a local server?",
      error,
    );
    alert(
      "Failed to load tile definitions from JSON. Check console for details.",
    );
  }
}

// UI GENERATORS
function buildCategorySelect() {
  const select = document.getElementById("category-select");
  if (!select) return;
  select.innerHTML = "";

  TILE_CATEGORIES.forEach((cat) => {
    // Hide empty groups (like Future Cutscenes) from the UI
    if (cat.tiles.length === 0) return;

    const opt = document.createElement("option");
    opt.value = cat.id;
    opt.textContent = cat.name;
    select.appendChild(opt);
  });

  select.value = activeCategory;

  select.addEventListener("change", (e) => {
    activeCategory = e.target.value;
    buildPalette();
  });
}

function buildPalette() {
  const el = document.getElementById("palette");
  el.innerHTML = "";

  // Find the selected category, default to "All Tiles"
  const cat =
    TILE_CATEGORIES.find((c) => c.id === activeCategory) || TILE_CATEGORIES[0];

  cat.tiles.forEach((k) => {
    const def = TILE_DEFS[k];
    if (!def) return;
    const btn = document.createElement("div");
    btn.className = "tile-btn" + (k === currentTool ? " active" : "");
    btn.id = `tb-${k}`;
    btn.style.cssText = `background:${def.bg}; border-color:${def.border}; color:${def.textColor}`;
    btn.innerHTML = `
        <div class="tile-key">[${def.key || "-"}]</div>
        <div class="tile-code">${k}</div>
        <div class="tile-name">${def.shortLabel}</div>
    `;
    btn.dataset.tip = `Paint: ${def.label}${def.key ? ` · shortcut [${def.key}]` : ""}`;
    btn.onclick = () => selectTool(k);
    el.appendChild(btn);
  });
}

// STATE
let params = {
  tileSize: 0.0625,
  offsetX: 0,
  offsetZ: 0,
  width: 0,
  height: 0,
  name: "map",
  scale: 1.0,
  centered: true,
  source_blender: false,
};
let tileData = new Array(0 * 0).fill("w");
let currentTool = "w";
let isPainting = false;
let paintErase = false;
let currentView = "top";

// Undo / Redo
let undoStack = [];
let redoStack = [];
let strokeBefore = null; // snapshot captured by beginStroke()

// Minimap
let minimapCanvas = null;
let minimapCtx = null;

// Free-look
// The camera orbits `freeTarget` using spherical coordinates.
let isFreeLook = false;
let freeTheta = -Math.PI / 5; // azimuth (around Y)
let freePhi = Math.PI / 3.5; // polar   (from top)
const FREE_RADIUS = 15;
let freeTarget = null; // THREE.Vector3, set in initThree
let freeDragStartX = 0;
let freeDragStartY = 0;
let freeDragMoved = false;
let freeDragButton = 0; // which button started the current free-look drag

// Three.js
let scene, camera, renderer;
let rawModelGroup = null;
let modelGroup, gridGroup, tileGroup;
let groundRaycaster;
let groundMesh;
let tileCanvasEl, tileCanvasCtx, tileTex, tilePlane;
let wireMaterial, solidMaterial;
let showWire = true,
  showSolid = false;
let showGrid = true,
  showModel = true;
let viewSize = 2.5;
let panX = 0,
  panZ = 0;
let isSpaceDown = false,
  isPanning = false;
let lastMX = 0,
  lastMY = 0;

// THREE.JS INIT
function initThree() {
  const canvas = document.getElementById("three-canvas");
  renderer = new THREE.WebGLRenderer({ canvas, antialias: true });
  renderer.setPixelRatio(window.devicePixelRatio);
  renderer.setClearColor(0x0d0d0f, 1);
  renderer.sortObjects = true;

  scene = new THREE.Scene();

  const vp = document.getElementById("viewport");
  const asp = vp.clientWidth / vp.clientHeight;
  camera = new THREE.OrthographicCamera(
    -viewSize * asp,
    viewSize * asp,
    viewSize,
    -viewSize,
    -1000,
    1000,
  );

  // freeTarget needs THREE. Initialise here to world centre
  freeTarget = new THREE.Vector3(
    (params.width * params.tileSize) / 2 - params.offsetX,
    0,
    (params.height * params.tileSize) / 2 - params.offsetZ,
  );

  setView("top");

  scene.add(new THREE.AmbientLight(0xffffff, 0.7));
  const sun = new THREE.DirectionalLight(0xffffee, 0.6);
  sun.position.set(2, 5, 3);
  scene.add(sun);

  modelGroup = new THREE.Group();
  scene.add(modelGroup);
  gridGroup = new THREE.Group();
  scene.add(gridGroup);
  tileGroup = new THREE.Group();
  scene.add(tileGroup);

  wireMaterial = new THREE.MeshBasicMaterial({
    color: 0x6688aa,
    wireframe: true,
    transparent: true,
    opacity: 0.55,
  });
  solidMaterial = new THREE.MeshLambertMaterial({
    color: 0x7799bb,
    transparent: true,
    opacity: 0.45,
    side: THREE.DoubleSide,
  });

  groundMesh = new THREE.Mesh(
    new THREE.PlaneGeometry(10000, 10000),
    new THREE.MeshBasicMaterial({ visible: false, side: THREE.DoubleSide }),
  );
  groundMesh.rotation.x = -Math.PI / 2;
  scene.add(groundMesh);
  groundRaycaster = new THREE.Raycaster();

  setupEvents(canvas);
  rebuildGrid();
  rebuildTileLayer();
  resetCamera();

  window.addEventListener("resize", onResize);
  onResize();
  animate();
}

function animate() {
  requestAnimationFrame(animate);
  renderer.render(scene, camera);
}

function onResize() {
  const vp = document.getElementById("viewport");
  renderer.setSize(vp.clientWidth, vp.clientHeight);
  updateCameraFrustum();
}

// CAMERA
function updateCameraFrustum() {
  const vp = document.getElementById("viewport");
  const asp = vp.clientWidth / vp.clientHeight;

  if (isFreeLook) {
    camera.left = -viewSize * asp;
    camera.right = viewSize * asp;
    camera.top = viewSize;
    camera.bottom = -viewSize;
  } else {
    camera.left = -viewSize * asp + panX;
    camera.right = viewSize * asp + panX;
    camera.top = viewSize - panZ;
    camera.bottom = -viewSize - panZ;
  }
  camera.updateProjectionMatrix();
  redrawMinimap(); // keep minimap frustum rect in sync
}

function resetCamera() {
  const { tileSize, offsetX, offsetZ, width, height } = params;
  if (isFreeLook) {
    freeTarget.set(
      (width * tileSize) / 2 - offsetX,
      0,
      (height * tileSize) / 2 - offsetZ,
    );
    viewSize = Math.max(width * tileSize, height * tileSize) * 0.7;
    updateFreeLookCamera();
  } else {
    panX = (width * tileSize) / 2 - offsetX;
    panZ = (height * tileSize) / 2 - offsetZ;
    viewSize = Math.max(width * tileSize, height * tileSize) * 0.7;
    updateCameraFrustum();
  }
}

function setView(view) {
  // Switching to a fixed view always exits free-look
  if (isFreeLook) {
    isFreeLook = false;
    const flBtn = document.getElementById("btn-freelook");
    if (flBtn) {
      flBtn.classList.remove("active-btn");
      flBtn.textContent = "Free Look";
    }
  }

  currentView = view;
  document
    .querySelectorAll(".view-btn")
    .forEach((b) => b.classList.remove("active-btn"));
  document.getElementById("btn-view-" + view).classList.add("active-btn");
  document.getElementById("paint-warning").style.display =
    view === "top" ? "none" : "block";

  if (view === "top") {
    camera.position.set(0, 50, 0);
    camera.up.set(0, 0, -1);
  } else if (view === "front") {
    camera.position.set(0, 0, 50);
    camera.up.set(0, 1, 0);
  } else if (view === "back") {
    camera.position.set(0, 0, -50);
    camera.up.set(0, 1, 0);
  } else if (view === "left") {
    camera.position.set(-50, 0, 0);
    camera.up.set(0, 1, 0);
  } else if (view === "right") {
    camera.position.set(50, 0, 0);
    camera.up.set(0, 1, 0);
  }
  camera.lookAt(0, 0, 0);
  updateCameraFrustum();
}

/**
 * Free-look: orbit the camera around freeTarget using spherical coordinates
 */
function updateFreeLookCamera() {
  const sinPhi = Math.sin(freePhi);
  const cosPhi = Math.cos(freePhi);
  const sinTheta = Math.sin(freeTheta);
  const cosTheta = Math.cos(freeTheta);

  camera.position.set(
    freeTarget.x + FREE_RADIUS * sinPhi * sinTheta,
    freeTarget.y + FREE_RADIUS * cosPhi,
    freeTarget.z + FREE_RADIUS * sinPhi * cosTheta,
  );
  camera.up.set(0, 1, 0);
  camera.lookAt(freeTarget);
  updateCameraFrustum();
}

function toggleFreeLook() {
  isFreeLook = !isFreeLook;
  const btn = document.getElementById("btn-freelook");
  btn.classList.toggle("active-btn", isFreeLook);
  btn.textContent = isFreeLook ? "Free Look ✓" : "Free Look";

  if (isFreeLook) {
    // Deactivate all fixed-view buttons
    document
      .querySelectorAll(".view-btn")
      .forEach((b) => b.classList.remove("active-btn"));
    // Painting is allowed in free-look (via click), so hide the warning
    document.getElementById("paint-warning").style.display = "none";
    // Orbit around the current world centre
    const { tileSize, offsetX, offsetZ, width, height } = params;
    freeTarget.set(
      (width * tileSize) / 2 - offsetX,
      0,
      (height * tileSize) / 2 - offsetZ,
    );
    freeTheta = -Math.PI / 5;
    freePhi = Math.PI / 3.5;
    updateFreeLookCamera();
  } else {
    setView("top");
  }
}

// GRID
function rebuildGrid() {
  gridGroup.clear();
  const {
    tileSize: ts,
    offsetX: ox,
    offsetZ: oz,
    width: W,
    height: H,
  } = params;
  const x0 = -ox,
    x1 = W * ts - ox;
  const z0 = -oz,
    z1 = H * ts - oz;
  const Y = 0.03;

  const verts = [];
  for (let x = 0; x <= W; x++) {
    const wx = x * ts - ox;
    verts.push(wx, Y, z0, wx, Y, z1);
  }
  for (let z = 0; z <= H; z++) {
    const wz = z * ts - oz;
    verts.push(x0, Y, wz, x1, Y, wz);
  }
  const geo = new THREE.BufferGeometry();
  geo.setAttribute("position", new THREE.Float32BufferAttribute(verts, 3));
  gridGroup.add(
    new THREE.LineSegments(
      geo,
      new THREE.LineBasicMaterial({ color: 0x2a2a3a }),
    ),
  );

  const bv = [
    x0,
    Y + 0.01,
    z0,
    x1,
    Y + 0.01,
    z0,
    x1,
    Y + 0.01,
    z0,
    x1,
    Y + 0.01,
    z1,
    x1,
    Y + 0.01,
    z1,
    x0,
    Y + 0.01,
    z1,
    x0,
    Y + 0.01,
    z1,
    x0,
    Y + 0.01,
    z0,
  ];
  const bgeo = new THREE.BufferGeometry();
  bgeo.setAttribute("position", new THREE.Float32BufferAttribute(bv, 3));
  gridGroup.add(
    new THREE.LineSegments(
      bgeo,
      new THREE.LineBasicMaterial({ color: 0xf0a030 }),
    ),
  );

  gridGroup.visible = showGrid;
}

// TILE LAYER
function rebuildTileLayer() {
  tileGroup.clear();
  const {
    tileSize: ts,
    offsetX: ox,
    offsetZ: oz,
    width: W,
    height: H,
  } = params;

  tileCanvasEl = document.createElement("canvas");
  tileCanvasEl.width = W;
  tileCanvasEl.height = H;
  tileCanvasCtx = tileCanvasEl.getContext("2d");

  tileTex = new THREE.CanvasTexture(tileCanvasEl);
  tileTex.magFilter = THREE.NearestFilter;
  tileTex.minFilter = THREE.NearestFilter;
  tileTex.flipY = true; // default=true; keeps canvas Z→pixel-Y in sync with world Z

  const ww = W * ts,
    wh = H * ts;
  const geo = new THREE.PlaneGeometry(ww, wh);
  const mat = new THREE.MeshBasicMaterial({
    map: tileTex,
    transparent: true,
    opacity: 0.72,
    depthTest: false,
    depthWrite: false,
  });
  tilePlane = new THREE.Mesh(geo, mat);
  tilePlane.rotation.x = -Math.PI / 2;
  tilePlane.position.set(ww / 2 - ox, 0.04, wh / 2 - oz);
  tilePlane.renderOrder = 1;
  tileGroup.add(tilePlane);

  redrawTiles();
}

function redrawTiles() {
  if (!tileCanvasCtx) return;
  const { width: W, height: H } = params;
  const ctx = tileCanvasCtx;
  ctx.clearRect(0, 0, W, H);
  for (let z = 0; z < H; z++) {
    for (let x = 0; x < W; x++) {
      const type = tileData[z * W + x];
      if (type === "w") continue;
      const def = TILE_DEFS[type];
      if (!def) continue;
      ctx.fillStyle = def.border;
      ctx.fillRect(x, z, 1, 1);
    }
  }
  if (tileTex) tileTex.needsUpdate = true;
  redrawMinimap(); // keep minimap in sync whenever tiles change
}

// INPUT EVENTS
function setupEvents(canvas) {
  // Zoom (all views)
  canvas.addEventListener(
    "wheel",
    (e) => {
      e.preventDefault();
      viewSize *= e.deltaY > 0 ? 1.08 : 0.92;
      viewSize = Math.max(0.03, Math.min(30, viewSize));
      // updateFreeLookCamera calls updateCameraFrustum internally
      if (isFreeLook) {
        updateFreeLookCamera();
      } else {
        updateCameraFrustum();
      }
    },
    { passive: false },
  );

  // Mouse down
  canvas.addEventListener("mousedown", (e) => {
    lastMX = e.clientX;
    lastMY = e.clientY;

    if (isFreeLook) {
      // Record where this interaction started so we can distinguish
      // a short click (→ paint) from a drag (→ orbit).
      freeDragStartX = e.clientX;
      freeDragStartY = e.clientY;
      freeDragMoved = false;
      freeDragButton = e.button;
      paintErase = e.button === 2;
      return; // defer painting/orbiting to mousemove & mouseup
    }

    if (e.button === 1 || (e.button === 0 && isSpaceDown)) {
      isPanning = true;
      canvas.style.cursor = "grabbing";
      return;
    }
    if ((e.button === 0 || e.button === 2) && currentView === "top") {
      isPainting = true;
      paintErase = e.button === 2;
      beginStroke(); // snapshot tileData before first paint in stroke
      paintAt(e);
    }
  });

  // Mouse move
  canvas.addEventListener("mousemove", (e) => {
    if (isFreeLook) {
      const dx = e.clientX - lastMX;
      const dy = e.clientY - lastMY;
      lastMX = e.clientX;
      lastMY = e.clientY;

      const totalDist = Math.hypot(
        e.clientX - freeDragStartX,
        e.clientY - freeDragStartY,
      );
      if (totalDist > 5) freeDragMoved = true;

      if (e.buttons > 0 && freeDragMoved) {
        if (isSpaceDown) {
          // Pan freeTarget in the camera's screen plane.
          // We derive the screen-right and screen-up vectors from the
          // current spherical angles rather than from the camera matrix
          // (which may not be updated yet this frame).
          const vp = document.getElementById("viewport");
          const panScale = (viewSize * 2) / vp.clientHeight;
          const lookDir = new THREE.Vector3(
            -Math.sin(freePhi) * Math.sin(freeTheta),
            -Math.cos(freePhi),
            -Math.sin(freePhi) * Math.cos(freeTheta),
          );
          const worldUp = new THREE.Vector3(0, 1, 0);
          const right = new THREE.Vector3()
            .crossVectors(lookDir, worldUp)
            .normalize();
          const up = new THREE.Vector3()
            .crossVectors(right, lookDir)
            .normalize();
          freeTarget.addScaledVector(right, -dx * panScale);
          freeTarget.addScaledVector(up, dy * panScale);
          updateFreeLookCamera();
        } else if (e.buttons === 1) {
          // Left-drag → orbit
          freeTheta -= dx * 0.007;
          freePhi -= dy * 0.007;
          freePhi = Math.max(0.05, Math.min(Math.PI * 0.95, freePhi));
          updateFreeLookCamera();
        }
      }
      hoverAt(e);
      return; // don't run normal pan/paint logic below
    }

    // Normal (non-free-look) mousemove
    if (isPanning) {
      const dx = e.clientX - lastMX;
      const dy = e.clientY - lastMY;
      lastMX = e.clientX;
      lastMY = e.clientY;
      const vp = document.getElementById("viewport");
      const asp = vp.clientWidth / vp.clientHeight;
      panX -= (dx / vp.clientWidth) * viewSize * asp * 2;
      panZ += (dy / vp.clientHeight) * viewSize * 2;
      updateCameraFrustum();
    } else if (isPainting && currentView === "top") {
      paintAt(e);
    }
    hoverAt(e);
  });

  // Mouse up
  // Registered on `window` so it fires even if pointer leaves canvas.
  window.addEventListener("mouseup", (e) => {
    if (isFreeLook) {
      const totalDist = Math.hypot(
        e.clientX - freeDragStartX,
        e.clientY - freeDragStartY,
      );
      // Short click (no meaningful drag) → paint or erase at click position
      if (
        !freeDragMoved &&
        totalDist <= 5 &&
        (freeDragButton === 0 || freeDragButton === 2)
      ) {
        beginStroke();
        paintAt({ clientX: freeDragStartX, clientY: freeDragStartY });
        endStroke();
      }
      freeDragMoved = false;
      return;
    }

    endStroke(); // commit stroke to undo history if anything changed
    isPainting = false;
    isPanning = false;
    canvas.style.cursor = isSpaceDown ? "grab" : "crosshair";
  });

  canvas.addEventListener("contextmenu", (e) => e.preventDefault());

  // Keyboard
  window.addEventListener("keydown", (e) => {
    if (e.code === "Space" && !e.repeat) {
      isSpaceDown = true;
      canvas.style.cursor = "grab";
    }

    // Undo: Ctrl+Z
    if (e.ctrlKey && e.key === "z") {
      e.preventDefault();
      undo();
      return;
    }
    // Redo: Ctrl+Y  or  Ctrl+Shift+Z
    if (e.ctrlKey && (e.key === "y" || (e.shiftKey && e.key === "Z"))) {
      e.preventDefault();
      redo();
      return;
    }

    for (const [k, def] of Object.entries(TILE_DEFS)) {
      if (e.key === def.key) {
        selectTool(k);
        return;
      }
    }
    if (e.key === "g" || e.key === "G") toggleGrid();
  });
  window.addEventListener("keyup", (e) => {
    if (e.code === "Space") {
      isSpaceDown = false;
      canvas.style.cursor = "crosshair";
    }
  });

  canvas.style.cursor = "crosshair";
}

// WORLD POSITION / TILE MAPPING
function getWorldPos(e) {
  const canvas = document.getElementById("three-canvas");
  const rect = canvas.getBoundingClientRect();
  const ndcX = ((e.clientX - rect.left) / rect.width) * 2 - 1;
  const ndcY = -((e.clientY - rect.top) / rect.height) * 2 + 1;
  groundRaycaster.setFromCamera(new THREE.Vector2(ndcX, ndcY), camera);
  const hits = groundRaycaster.intersectObject(groundMesh);
  return hits.length ? hits[0].point : null;
}

function worldToTile(wx, wz) {
  const {
    tileSize: ts,
    offsetX: ox,
    offsetZ: oz,
    width: W,
    height: H,
  } = params;
  const tx = Math.floor((wx + ox) / ts);
  const tz = Math.floor((wz + oz) / ts);
  return { tx, tz, valid: tx >= 0 && tx < W && tz >= 0 && tz < H };
}

function paintAt(e) {
  const pt = getWorldPos(e);
  if (!pt) return;
  const { tx, tz, valid } = worldToTile(pt.x, pt.z);
  if (!valid) return;
  const type = paintErase ? "w" : currentTool;
  const idx = tz * params.width + tx;
  if (tileData[idx] !== type) {
    tileData[idx] = type;
    redrawTiles();
  }
}

function hoverAt(e) {
  const pt = getWorldPos(e);
  if (!pt) {
    document.getElementById("s-tile").textContent = "—";
    document.getElementById("s-world").textContent = "—";
    document.getElementById("s-type").textContent = "—";
    return;
  }
  document.getElementById("s-world").textContent =
    `(${pt.x.toFixed(4)}, ${pt.z.toFixed(4)})`;
  const { tx, tz, valid } = worldToTile(pt.x, pt.z);
  if (!valid) {
    document.getElementById("s-tile").textContent = "out of bounds";
    document.getElementById("s-type").textContent = "—";
    return;
  }
  const type = tileData[tz * params.width + tx];
  document.getElementById("s-tile").textContent = `(${tx}, ${tz})`;
  document.getElementById("s-type").textContent =
    `${type} — ${TILE_DEFS[type]?.label || "?"}`;
}

// PALETTE UI

function selectTool(k) {
  if (!TILE_DEFS[k]) return;
  currentTool = k;
  document
    .querySelectorAll(".tile-btn")
    .forEach((b) => b.classList.remove("active"));
  const btn = document.getElementById(`tb-${k}`);
  if (btn) btn.classList.add("active");
  const def = TILE_DEFS[k];
  const indicator = document.getElementById("s-tool");
  indicator.textContent = k;
  indicator.style.background = def.border;
  indicator.style.color = def.bg;
}

// PARAMETERS
const paramInputs = document.querySelectorAll(
  "#p-offsetx, #p-offsetz, #p-width, #p-height, #p-name, #p-scale, #p-center, #p-source-blender",
);
paramInputs.forEach((input) => {
  input.addEventListener("input", () =>
    document.getElementById("btn-apply").classList.add("needs-apply"),
  );
  input.addEventListener("change", () =>
    document.getElementById("btn-apply").classList.add("needs-apply"),
  );
});

function readParamInputs() {
  return {
    tileSize: parseFloat(document.getElementById("p-tilesize").value) || 0.0625,
    offsetX: parseFloat(document.getElementById("p-offsetx").value) || 0,
    offsetZ: parseFloat(document.getElementById("p-offsetz").value) || 0,
    width: Math.max(
      1,
      parseInt(document.getElementById("p-width").value) || 56,
    ),
    height: Math.max(
      1,
      parseInt(document.getElementById("p-height").value) || 12,
    ),
    name: document.getElementById("p-name").value.trim() || "map",
    scale: parseFloat(document.getElementById("p-scale").value) || 1.0,
    centered: document.getElementById("p-center").checked,
    source_blender: document.getElementById("p-source-blender").checked,
  };
}

function applyParams() {
  const p = readParamInputs();
  const newTiles = new Array(p.width * p.height).fill("w");
  const copyW = Math.min(p.width, params.width);
  const copyH = Math.min(p.height, params.height);
  for (let z = 0; z < copyH; z++) {
    for (let x = 0; x < copyW; x++) {
      newTiles[z * p.width + x] = tileData[z * params.width + x] || "w";
    }
  }

  const requiresTransformUpdate =
    p.scale !== params.scale ||
    p.centered !== params.centered ||
    p.source_blender !== params.source_blender;
  params = p;
  tileData = newTiles;

  // Map dimensions may have changed → old snapshots are invalid
  undoStack = [];
  redoStack = [];
  strokeBefore = null;
  updateUndoRedoUI();

  rebuildGrid();
  rebuildTileLayer();
  updateMinimapSize(); // resize minimap canvas to new aspect ratio

  if (requiresTransformUpdate && rawModelGroup) {
    applyModelTransforms();
  }

  // Keep free-look orbit centred on the new world centre
  if (freeTarget) {
    freeTarget.set(
      (p.width * p.tileSize) / 2 - p.offsetX,
      0,
      (p.height * p.tileSize) / 2 - p.offsetZ,
    );
  }

  updateMapInfo();
  updateComputed();
  document.getElementById("btn-apply").classList.remove("needs-apply");
}

function autoSize() {
  const p = readParamInputs();
  const w = Math.round((p.offsetX * 2) / p.tileSize);
  const h = Math.round((p.offsetZ * 2) / p.tileSize);
  document.getElementById("p-width").value = w;
  document.getElementById("p-height").value = h;
}

function updateComputed() {
  const { tileSize: ts, width: W, height: H } = params;
  document.getElementById("world-computed").textContent =
    `${W}×${H} tiles  ·  ${(W * ts).toFixed(4)}×${(H * ts).toFixed(4)} world units`;
}

function updateMapInfo() {
  document.getElementById("map-info").innerHTML =
    `<strong>${params.name}</strong> · ${params.width}×${params.height}`;
}

// TOGGLES
function toggleGrid() {
  showGrid = !showGrid;
  gridGroup.visible = showGrid;
  const btn = document.getElementById("grid-btn");
  btn.textContent = `Grid: ${showGrid ? "ON" : "OFF"}`;
  btn.classList.toggle("active-btn", showGrid);
}
function toggleModel() {
  showModel = !showModel;
  modelGroup.visible = showModel;
  document.getElementById("model-btn").textContent =
    `Model: ${showModel ? "ON" : "OFF"}`;
}
function toggleWire() {
  showWire = !showWire;
  modelGroup.traverse((c) => {
    if (c.isMesh && c._isWire) c.visible = showWire;
  });
  document.getElementById("wire-btn").textContent =
    `Wire: ${showWire ? "ON" : "OFF"}`;
}
function toggleSolid() {
  showSolid = !showSolid;
  modelGroup.traverse((c) => {
    if (c.isMesh && c._isSolid) c.visible = showSolid;
  });
  document.getElementById("solid-btn").textContent =
    `Solid: ${showSolid ? "ON" : "OFF"}`;
}

function fillAll(type) {
  beginStroke(); // snapshot before fill
  tileData.fill(type);
  endStroke(); // commit to history if anything changed
  redrawTiles();
}

// MODEL LOADING
document.getElementById("obj-input").addEventListener("change", (e) => {
  const file = e.target.files[0];
  if (!file) return;
  const url = URL.createObjectURL(file);
  document.getElementById("no-model-hint").style.display = "none";

  new THREE.OBJLoader().load(
    url,
    (obj) => {
      rawModelGroup = obj;
      applyModelTransforms();
      URL.revokeObjectURL(url);
      e.target.value = "";
    },
    undefined,
    (err) => {
      alert("Failed to load .obj: " + err.message);
      URL.revokeObjectURL(url);
    },
  );
});

function applyModelTransforms() {
  modelGroup.clear();

  // Convert Blender Z-up to NDS Y-up via a proper rotation:
  //   new_x =  old_x
  //   new_y =  old_z  (Blender's up becomes NDS up)
  //   new_z = -old_y  (negation preserves right-handedness)
  // det = +1 - this is a rotation, not a reflection, so winding is preserved.
  const blender = params.source_blender;
  const swapMat = blender
    ? new THREE.Matrix4().set(1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1)
    : null;

  rawModelGroup.traverse((child) => {
    if (!child.isMesh) return;
    // Clone geometry when blender flag is on so the swap is non-destructive
    const geo = blender
      ? child.geometry.clone().applyMatrix4(swapMat)
      : child.geometry;
    const wire = new THREE.Mesh(geo, wireMaterial.clone());
    const solid = new THREE.Mesh(geo, solidMaterial.clone());
    wire._isWire = true;
    solid._isSolid = true;
    wire.visible = showWire;
    solid.visible = showSolid;
    modelGroup.add(wire);
    modelGroup.add(solid);
  });

  const s = params.scale;
  modelGroup.scale.set(s, s, s);

  if (params.centered) {
    // Compute the bounding box directly from the (possibly swapped) geometry
    // position attributes. Identical to Python's compute_bounds() which runs
    // after convert_blender_zup. We read raw attribute data so modelGroup.scale
    // is not factored in, keeping the math consistent with the Python pipeline:
    //   ox = (xmin+xmax)/2,  oy = ymin,  oz = (zmin+zmax)/2
    //   position = -offset * scale
    const bbox = new THREE.Box3();
    modelGroup.traverse((c) => {
      if (c.isMesh && c._isWire) {
        const pos = c.geometry.attributes.position;
        for (let i = 0; i < pos.count; i++) {
          bbox.expandByPoint(
            new THREE.Vector3(pos.getX(i), pos.getY(i), pos.getZ(i)),
          );
        }
      }
    });
    modelGroup.position.set(
      (-(bbox.min.x + bbox.max.x) / 2) * s,
      -bbox.min.y * s,
      (-(bbox.min.z + bbox.max.z) / 2) * s,
    );
  } else {
    modelGroup.position.set(0, 0, 0);
  }
}

function matchDefine(text, pattern) {
  const m = text.match(pattern);
  return m ? parseFloat(m[1]) : null;
}

// JMAP PARSING
document.getElementById("jmap-input").addEventListener("change", (e) => {
  const file = e.target.files[0];
  if (!file) return;
  const reader = new FileReader();
  reader.onload = (ev) => {
    parseJmap(ev.target.result);
    e.target.value = "";
  };
  reader.readAsText(file);
});

function parseJmap(text) {
  const lines = text.split("\n");
  const dataLines = [];
  (w = 0), (h = 0);

  for (const line of lines) {
    const t = line.trim();
    if (!t || t.startsWith("#")) {
      const sizeM = t.match(/(\d+)x(\d+)/);
      if (sizeM) {
        w = parseInt(sizeM[1]);
        h = parseInt(sizeM[2]);
      }

      // Auto-create a tile def for any unknown type found in the header
      const tileMatch = t.match(
        /#\s+([a-z0-9])\s*=\s*([^()]+)\s*(?:\(([^)]+)\))?/i,
      );

      if (tileMatch && tileMatch[1] !== "w") {
        const code = tileMatch[1];
        if (!TILE_DEFS[code]) {
          const parsedLabel = tileMatch[3] || tileMatch[2].trim();
          const baseColor = generateDeterministicColor(code + parsedLabel);
          TILE_DEFS[code] = {
            label: parsedLabel,
            shortLabel: tileMatch[2].trim(),
            key: "",
            bg: baseColor + "40",
            border: baseColor,
            textColor: "#ffffff",
          };
          // NEW: Ensure newly discovered tiles get injected into "All Tiles"
          if (TILE_CATEGORIES.length > 0) {
            TILE_CATEGORIES[0].tiles.push(code);
          }
        }
      }
      continue;
    }
    dataLines.push(t.split(",").map((s) => s.trim()));
  }

  if (dataLines.length === 0) {
    alert("No tile data found in .jmap");
    return;
  }
  const actualH = dataLines.length;
  const actualW = dataLines[0].length;

  document.getElementById("p-width").value = actualW;
  document.getElementById("p-height").value = actualH;

  applyParams(); // also clears undo/redo, rebuilds grid, resizes minimap
  buildPalette();

  for (let z = 0; z < actualH; z++) {
    for (let x = 0; x < actualW; x++) {
      const type = dataLines[z][x] || "w";
      if (TILE_DEFS[type] || type === "w") tileData[z * actualW + x] = type;
    }
  }
  redrawTiles();
  alert(`Loaded .jmap: ${actualW}×${actualH} tiles`);
}

// EXPORT
function exportJmap() {
  const { width: W, height: H, name } = params;
  const lines = [];
  lines.push(`# ${name} collision map  ${W}x${H}`);
  lines.push(`#`);
  lines.push(`#`);
  lines.push(`# Tile key:`);
  for (const [k, def] of Object.entries(TILE_DEFS)) {
    lines.push(`#   ${k} = ${def.shortLabel} (${def.label})`);
  }
  lines.push(``);
  for (let z = 0; z < H; z++) {
    const row = [];
    for (let x = 0; x < W; x++) row.push(tileData[z * W + x] || "w");
    lines.push(row.join(", "));
  }
  downloadFile(`${name}.jmap`, lines.join("\n"), "text/plain");
}

function downloadFile(filename, text, mime) {
  const a = document.createElement("a");
  a.href = URL.createObjectURL(new Blob([text], { type: mime }));
  a.download = filename;
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
}

// UNDO / REDO
/**
 * Call at the START of any operation that might change tileData
 * (mousedown for painting, fillAll). Snapshots the current state.
 */
function beginStroke() {
  strokeBefore = [...tileData];
}

/**
 * Call at the END of an operation (mouseup, or after fillAll).
 * Compares current tileData against the snapshot; pushes to
 * undoStack only if something actually changed.
 */
function endStroke() {
  if (!strokeBefore) return;
  let changed = false;
  for (let i = 0; i < tileData.length; i++) {
    if (tileData[i] !== strokeBefore[i]) {
      changed = true;
      break;
    }
  }
  if (changed) {
    undoStack.push(strokeBefore);
    redoStack = [];
    if (undoStack.length > 50) undoStack.shift(); // cap history at 50 strokes
    updateUndoRedoUI();
  }
  strokeBefore = null;
}

function undo() {
  if (undoStack.length === 0) return;
  redoStack.push([...tileData]);
  tileData = undoStack.pop();
  redrawTiles();
  updateUndoRedoUI();
}

function redo() {
  if (redoStack.length === 0) return;
  undoStack.push([...tileData]);
  tileData = redoStack.pop();
  redrawTiles();
  updateUndoRedoUI();
}

function updateUndoRedoUI() {
  const u = document.getElementById("btn-undo");
  const r = document.getElementById("btn-redo");
  if (u) u.disabled = undoStack.length === 0;
  if (r) r.disabled = redoStack.length === 0;
}

// MINIMAP
function initMinimap() {
  minimapCanvas = document.getElementById("minimap");
  minimapCtx = minimapCanvas.getContext("2d");
  updateMinimapSize();

  // Click on minimap, then pan the main top-down view to that world position
  minimapCanvas.addEventListener("click", (e) => {
    if (currentView !== "top" || isFreeLook) return;
    const rect = minimapCanvas.getBoundingClientRect();
    const mx = (e.clientX - rect.left) / rect.width;
    const my = (e.clientY - rect.top) / rect.height;
    const {
      width: W,
      height: H,
      tileSize: ts,
      offsetX: ox,
      offsetZ: oz,
    } = params;
    // Map [0,1] minimap coordinates to world position of the clicked tile centre
    panX = mx * W * ts - ox;
    panZ = my * H * ts - oz;
    updateCameraFrustum();
  });
}

/**
 * Resize the minimap canvas to match the current map aspect ratio,
 * capped at 160×100 px. Call this after applyParams().
 */
function updateMinimapSize() {
  if (!minimapCanvas) return;
  const { width: W, height: H } = params;
  const aspect = W / H;
  const MAX_W = 160,
    MAX_H = 100;
  let mw, mh;
  if (aspect >= MAX_W / MAX_H) {
    mw = MAX_W;
    mh = Math.max(4, Math.round(MAX_W / aspect));
  } else {
    mh = MAX_H;
    mw = Math.max(4, Math.round(MAX_H * aspect));
  }
  minimapCanvas.width = mw;
  minimapCanvas.height = mh;
  minimapCanvas.style.width = mw + "px";
  minimapCanvas.style.height = mh + "px";
  minimapCtx = minimapCanvas.getContext("2d");
  redrawMinimap();
}

/**
 * Repaint the minimap canvas.
 *
 * Tile colours:
 *   'w' walkable = dark green background (#1a2a1a)
 *   anything else = the tile's `border` colour from TILE_DEFS
 */
function redrawMinimap() {
  if (!minimapCtx || !minimapCanvas) return;
  const {
    width: W,
    height: H,
    tileSize: ts,
    offsetX: ox,
    offsetZ: oz,
  } = params;
  const mw = minimapCanvas.width;
  const mh = minimapCanvas.height;
  const cellX = mw / W;
  const cellY = mh / H;

  // Background
  minimapCtx.fillStyle = "#0d0d0f";
  minimapCtx.fillRect(0, 0, mw, mh);

  // Draw every tile
  for (let z = 0; z < H; z++) {
    for (let x = 0; x < W; x++) {
      const type = tileData[z * W + x];
      if (type === "w") {
        minimapCtx.fillStyle = "#1a1b2a";
      } else {
        const def = TILE_DEFS[type];
        if (!def) continue;
        minimapCtx.fillStyle = def.border;
      }
      minimapCtx.fillRect(
        Math.floor(x * cellX),
        Math.floor(z * cellY),
        Math.max(1, Math.ceil(cellX)),
        Math.max(1, Math.ceil(cellY)),
      );
    }
  }

  if (currentView === "top" && !isFreeLook && camera) {
    const wx0 = camera.left; // visible world X min
    const wx1 = camera.right; // visible world X max
    const wz0 = -camera.top; // visible world Z min  (see docstring above)
    const wz1 = -camera.bottom; // visible world Z max

    // → tile space
    const tx0 = (wx0 + ox) / ts;
    const tx1 = (wx1 + ox) / ts;
    const tz0 = (wz0 + oz) / ts;
    const tz1 = (wz1 + oz) / ts;

    // minimap pixels
    const px0 = tx0 * cellX;
    const py0 = tz0 * cellY;
    const pw = (tx1 - tx0) * cellX;
    const ph = (tz1 - tz0) * cellY;

    minimapCtx.strokeStyle = "#f0a030";
    minimapCtx.lineWidth = 1.5;
    minimapCtx.strokeRect(
      Math.round(px0) + 0.5,
      Math.round(py0) + 0.5,
      Math.round(pw),
      Math.round(ph),
    );
  }
}

// TOOLTIPS
function initTooltips() {
  const tip = document.getElementById("tooltip");
  if (!tip) return;

  document.body.addEventListener("mouseover", (e) => {
    const el = e.target.closest("[data-tip]");
    if (el) {
      tip.textContent = el.dataset.tip;
      tip.style.display = "block";
    } else {
      tip.style.display = "none";
    }
  });

  document.body.addEventListener("mousemove", (e) => {
    if (tip.style.display === "none") return;
    tip.style.left = Math.min(e.clientX + 14, window.innerWidth - 240) + "px";
    tip.style.top = Math.min(e.clientY + 18, window.innerHeight - 52) + "px";
  });
}

// BOOT
async function init() {
  await loadTileDefinitions();
  buildCategorySelect(); // Call this immediately after loading definitions
  buildPalette();

  if (TILE_DEFS["w"]) {
    selectTool("w");
  } else {
    const firstTool = Object.keys(TILE_DEFS)[0];
    if (firstTool) selectTool(firstTool);
  }

  updateComputed();
  updateMapInfo();
  initThree();
  initMinimap();
  initTooltips();
}

init();
