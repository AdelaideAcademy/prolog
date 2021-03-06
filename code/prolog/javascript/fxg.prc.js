
var str = {
  BackgroundColour: '#00000088',
  tokens: [],
  viewports: []
};

studio . setResource (['prolog', 'fxg'],
function (root, directory) {
  var structure = str;
  var Location = directory . searchAtom ('Location');
  var Position = directory . searchAtom ('Position');
  var Size = directory . searchAtom ('Size');
  var Scaling = directory . searchAtom ('Scaling');
  var Mode = directory . searchAtom ('Mode');
  var BackgroundColour = directory . searchAtom ('BackgroundColour');
  var ForegroundColour = directory . searchAtom ('ForegroundColour');
  var Index = directory . searchAtom ('Index');
  var Indexing = directory . searchAtom ('Indexing');
  var Repaint = directory . searchAtom ('Repaint');
  var RotateBy = directory . searchAtom ('RotateBy');
  var MoveBy = directory . searchAtom ('MoveBy');
  var Deck = directory . searchAtom ('Deck');
  var Release = directory . searchAtom ('Release');
  var ReleaseRandom = directory . searchAtom ('ReleaseRandom');
  var Shuffle = directory . searchAtom ('Shuffle');
  var Roll = directory . searchAtom ('Roll');
  var Insert = directory . searchAtom ('Insert');
  var Side = directory . searchAtom ('Side');
  var repaints = [];
  var images = {};
  var atoms = [];
  var viewport_removers = [];
  var find_image = function (token) {
    var location = token . Text;
    var image = images [location];
    if (image !== undefined) return image;
    image = studio . readResource (location);
    if (image === null) {image = new Image (); image . src = location;}
    if (image === null) return null;
    images [location] = image;
    if (token . location . size === null) token . location . size = {x: image . width, y: image . height};
    return image;
  };
  var DrawSquareGrid = function (ctx, viewport, token, token_index) {
    var xx = token . location . size . x * token . scaling . x, yy = token . location . size . y * token . scaling . y;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    ctx . translate (token . location . size . x * -0.5, token . location . size . y * -0.5);
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fillRect (0, 0, xx * token . indexing . x, yy * token . indexing . y);}
    ctx . beginPath ();
    for (var ind = 0; ind <= token . indexing . x; ind ++) {ctx . moveTo (ind * xx, 0); ctx . lineTo (ind * xx, yy * token . indexing . y);}
    for (var ind = 0; ind <= token . indexing . y; ind ++) {ctx . moveTo (0, ind * yy); ctx . lineTo (xx * token . indexing . x, ind * yy);}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke ();
    if (token . index != null) {
      ctx . font = '12px arial'; ctx . textBaseline = 'top';
      ctx . fillStyle = token . ForegroundColour;
      for (var ind = 0; ind < token . indexing . x; ind ++) {
        for (var sub = 0; sub < token . indexing . y; sub ++)
          ctx . fillText (String (ind + token . index . x) . padStart (2, '0') + String (sub + token . index . y) . padStart (2, '0'), 2 + ind * xx, 2 + sub * yy);
      }
    }
    var pth = new Path2D ();
    pth . moveTo (0, 0);
    pth . lineTo (token . indexing . x * xx, 0);
    pth . lineTo (token . indexing . x * xx, token . indexing . y * yy);
    pth . lineTo (0, token . indexing . y * yy);
    pth . closePath ();
    if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
  };
  var h = Math . sqrt (3) * 0.5;
  var DrawHorizontalHexGrid = function (ctx, viewport, token, token_index, direction) {
    var xx = token . location . size . x * token . scaling . x * 0.5, yy = token . location . size . y * token . scaling . y * 0.5;
    var hx = xx * 0.5, hy = yy * 0.5, hg = yy * h, hg2 = hg + hg, hshift = xx + hx;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    // =============
    var pth = new Path2D ();
    var yyy = token . indexing . y * hg2 - hg;
    var xxx = - hx - hx;
    pth . moveTo (xxx + xx + hx, yyy); pth . lineTo (xxx + hx, yyy);
    for (var ind = 0; ind < token . indexing . y; ind ++) {yyy -= hg; pth . lineTo (xxx, yyy); yyy -= hg; pth . lineTo (xxx + hx, yyy);}
    xxx += xx + hx;
    pth . lineTo (xxx, yyy);
    var ind = token . indexing . x, hhg = direction ? hg : - hg;
    while (ind > 1) {yyy += hhg; xxx += hx; pth . lineTo (xxx, yyy); xxx += xx; pth . lineTo (xxx, yyy); ind --; hhg = - hhg;}
    for (var ind = 0; ind < token . indexing . y; ind ++) {yyy += hg; pth . lineTo (xxx + hx, yyy); yyy += hg; pth . lineTo (xxx, yyy);}
    ind = token . indexing . x;
    while (ind > 1) {xxx -= xx; pth . lineTo (xxx, yyy); xxx -= hx; yyy += hhg; pth . lineTo (xxx, yyy); ind --; hhg = - hhg;}
    pth . lineTo (100, 100);
    pth . closePath ();
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill (pth);}
    // =========
    ctx . beginPath ();
    xxx = 0;
    var vswitch = 0, vvswitch = direction ? hg : - hg;
    for (var ind = 0; ind < token . indexing . x; ind ++) {
      yyy = vswitch;
      for (var sub = 0; sub < token . indexing . y; sub ++) {
        ctx . moveTo (xxx - hx, yyy + hg); ctx . lineTo (xxx - xx, yyy); ctx . lineTo (xxx - hx, yyy - hg); ctx . lineTo (xxx + hx, yyy - hg);
        yyy += hg2;
      }
      xxx += hshift;
      vswitch = vswitch === 0 ? vvswitch : 0;
    }
    xxx -= xx; yyy = direction ? - vswitch : - vswitch - hg2; ctx . moveTo (xxx, yyy);
    for (var ind = 0; ind < token . indexing . y; ind ++) {yyy += hg; ctx . lineTo (xxx + hx, yyy); yyy += hg; ctx . lineTo (xxx, yyy);}
    ctx . lineTo (xxx - xx, yyy); xxx = direction ? hx : xx + xx;
    var dhg = direction ? 0 : - hg;
    for (var ind = direction ? 1 : 2; ind < token . indexing . x; ind += 2) {ctx . moveTo (xxx, dhg - hg); ctx . lineTo (xxx + hx, dhg); xxx += hshift + hshift;}
    xxx = - hx; vswitch = 0; yyy = token . indexing . y * hg2 - hg;
    for (var ind = 1; ind < token . indexing . x; ind ++) {
      ctx . moveTo (xxx, yyy + vswitch); ctx . lineTo (xxx + xx, yyy + vswitch); xxx += hshift; vswitch = vswitch === 0 ? vvswitch : 0;
    }
    xxx = direction ? xx + xx : hx; if (! direction) yyy -= hg;
    for (var ind = direction ? 2 : 1; ind < token . indexing . x; ind += 2) {ctx . moveTo (xxx, yyy + hg); ctx . lineTo (xxx + hx, yyy); xxx += hshift + hshift;}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke ();
    if (token . index != null) {
      xxx = - hx; yyy = 2 - hg;
      ctx . font = '8px arial'; ctx . textBaseline = 'top';
      ctx . fillStyle = token . ForegroundColour;
      vswitch = 0;
      for (var ind = 0; ind < token . indexing . x; ind ++) {
        for (var sub = 0; sub < token . indexing . y; sub ++)
          ctx . fillText (String (ind + token . index . x) . padStart (2, '0') + String (sub + token . index . y) . padStart (2, '0'), xxx + ind * hshift, yyy + sub * hg2 + vswitch);
        vswitch = vswitch === 0 ? vvswitch : 0;
      }
    }
    if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
  };
  var DrawVerticalHexGrid = function (ctx, viewport, token, token_index, direction) {
    var yy = token . location . size . y * token . scaling . y * 0.5, xx = token . location . size . x * token . scaling . x * 0.5;
    var hy = yy * 0.5, hx = yy * 0.5, hg = xx * h, hg2 = hg + hg, yshift = yy + hy;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    // =============
    var pth = new Path2D ();
    var xxx = token . indexing . x * hg2 - hg;
    var yyy = - hy - hy;
    pth . moveTo (xxx, yyy + yy + hy); pth . lineTo (xxx, yyy + hy);
    for (var ind = 0; ind < token . indexing . x; ind ++) {xxx -= hg; pth . lineTo (xxx, yyy); xxx -= hg; pth . lineTo (xxx, yyy + hy);}
    yyy += yy + hy;
    pth . lineTo (xxx, yyy);
    var ind = token . indexing . y, hhg = direction ? hg : - hg;
    while (ind > 1) {xxx += hhg; yyy += hy; pth . lineTo (xxx, yyy); yyy += yy; pth . lineTo (xxx, yyy); ind --; hhg = - hhg;}
    for (var ind = 0; ind < token . indexing . x; ind ++) {xxx += hg; pth . lineTo (xxx, yyy + hy); xxx += hg; pth . lineTo (xxx, yyy);}
    ind = token . indexing . y;
    while (ind > 1) {yyy -= yy; pth . lineTo (xxx, yyy); yyy -= hy; xxx += hhg; pth . lineTo (xxx, yyy); ind --; hhg = - hhg;}
    pth . closePath ();
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill (pth);}
    // =========
    ctx . beginPath ();
    yyy = 0;
    var vswitch = 0, vvswitch = direction ? hg : - hg;
    for (var ind = 0; ind < token . indexing . y; ind ++) {
      xxx = vswitch;
      for (var sub = 0; sub < token . indexing . x; sub ++) {
        ctx . moveTo (xxx + hg, yyy - hy); ctx . lineTo (xxx, yyy - yy); ctx . lineTo (xxx - hg, yyy - hy); ctx . lineTo (xxx - hg, yyy + hy);
        xxx += hg2;
      }
      yyy += yshift;
      vswitch = vswitch === 0 ? vvswitch : 0;
    }
    yyy -= yy; xxx = direction ? - vswitch : - vswitch - hg2; ctx . moveTo (xxx, yyy);
    for (var ind = 0; ind < token . indexing . x; ind ++) {xxx += hg; ctx . lineTo (xxx, yyy + hy); xxx += hg; ctx . lineTo (xxx, yyy);}
    ctx . lineTo (xxx, yyy - yy); yyy = direction ? hy : yy + yy;
    var dhg = direction ? 0 : - hg;
    for (var ind = direction ? 1 : 2; ind < token . indexing . y; ind += 2) {ctx . moveTo (dhg - hg, yyy); ctx . lineTo (dhg, yyy + hy); yyy += yshift + yshift;}
    yyy = - hy; vswitch = 0; xxx = token . indexing . x * hg2 - hg;
    for (var ind = 1; ind < token . indexing . y; ind ++) {
      ctx . moveTo (xxx + vswitch, yyy); ctx . lineTo (xxx + vswitch, yyy + yy); yyy += yshift; vswitch = vswitch === 0 ? vvswitch : 0;
    }
    yyy = direction ? yy + yy : hy; if (! direction) xxx -= hg;
    for (var ind = direction ? 2 : 1; ind < token . indexing . y; ind += 2) {ctx . moveTo (xxx + hg, yyy); ctx . lineTo (xxx, yyy + hy); yyy += yshift + yshift;}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke ();
    if (token . index != null) {
      yyy = - hy; xxx = 2 - hg;
      ctx . font = '8px arial'; ctx . textBaseline = 'top';
      ctx . fillStyle = token . ForegroundColour;
      vswitch = 0;
      for (var ind = 0; ind < token . indexing . y; ind ++) {
        for (var sub = 0; sub < token . indexing . x; sub ++)
          ctx . fillText (String (sub + token . index . x) . padStart (2, '0') + String (ind + token . index . y) . padStart (2, '0'), xxx + sub * hg2 + vswitch, yyy + ind * yshift);
        vswitch = vswitch === 0 ? vvswitch : 0;
      }
    }
    if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
  };
  var DrawRoundedRectangle = function (ctx, w, h, rx, ry) {
    ctx . moveTo (- w, - h + ry); ctx . ellipse (- w + rx, - h + ry, rx, ry, 0, Math . PI, Math . PI * 1.5);
    ctx . lineTo (w - rx, - h); ctx . ellipse (w - rx, - h + ry, rx, ry, 0, Math . PI * 1.5, Math . PI * 2);
    ctx . lineTo (w, h - ry); ctx . ellipse (w - rx, h - ry, rx, ry, 0, 0, Math . PI * 0.5);
    ctx . lineTo (- w + rx, h); ctx . ellipse (- w + rx, h - ry, rx, ry, 0, Math . PI * 0.5, Math . PI); ctx . closePath ();
  };
  var DrawDice = function (ctx, viewport, token, token_index) {
    var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    ctx . beginPath (); DrawRoundedRectangle (ctx, hw, hh, token . indexing . x, token . indexing . y);
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill ();}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke ();
    if (token_index !== null) ctx . addHitRegion ({id: token_index});
    var sx = token . location . size . x, sy = token . location . size . y;
    var radius = (sx + sy) * 0.0625; sx *= 0.28; sy *= 0.28;
    ctx . fillStyle = ctx . strokeStyle;
    switch (token . Side) {
      case 0: ctx . beginPath (); ctx . arc (0, 0, radius, 0, Math . PI * 2); ctx . fill (); break;
      case 1:
        ctx . beginPath (); ctx . arc (sx, - sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, sy, radius, 0, Math . PI * 2); ctx . fill ();
        break;
      case 2:
        ctx . beginPath (); ctx . arc (0, 0, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (sx, - sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, sy, radius, 0, Math . PI * 2); ctx . fill ();
        break;
      case 3:
        ctx . beginPath (); ctx . arc (sx, - sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (sx, sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, - sy, radius, 0, Math . PI * 2); ctx . fill ();
        break;
      case 4:
        ctx . beginPath (); ctx . arc (0, 0, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (sx, - sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (sx, sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, - sy, radius, 0, Math . PI * 2); ctx . fill ();
        break;
      case 5:
        ctx . beginPath (); ctx . arc (sx, 0, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, 0, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (sx, - sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (sx, sy, radius, 0, Math . PI * 2); ctx . fill ();
        ctx . beginPath (); ctx . arc (- sx, - sy, radius, 0, Math . PI * 2); ctx . fill ();
        break;
      default: break;
    }
  };
  var DiceValue = function (token) {
    var value = (token . Side + token . Shift) * token . Multiplier + '';
    if (token . Sides >= 9 && token . Multiplier !== 10 && (value . indexOf ('9') >= 0 || value . indexOf ('6') >= 0)) value += '.';
    if (token . Multiplier > 9) value = value . padStart (2, '0');
    return value;
  };
  var DrawTetrahedron = function (ctx, viewport, token, token_index) {
    var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    ctx . beginPath ();
    hw *= Math . sqrt (3) * 0.5;
    ctx . moveTo (0, - hh); ctx . lineTo (hw, hh * 0.5); ctx . lineTo (- hw, hh * 0.5); ctx . closePath ();
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill ();}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke ();
    if (token_index !== null) ctx . addHitRegion ({id: token_index});
    ctx . fillStyle = token . ForegroundColour;
    ctx . font = hh + 'px arial'; ctx . textBaseline = 'middle'; ctx . textAlign = 'center';
    ctx . fillText (DiceValue (token), 0, 0);
  };
  var DrawCube = function (ctx, viewport, token, token_index) {
    var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    ctx . beginPath (); DrawRoundedRectangle (ctx, hw, hh, token . indexing . x, token . indexing . y);
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill ();}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke ();
    if (token_index !== null) ctx . addHitRegion ({id: token_index});
    ctx . fillStyle = token . ForegroundColour;
    ctx . font = hh + 'px arial'; ctx . textBaseline = 'middle'; ctx . textAlign = 'center';
    ctx . fillText (DiceValue (token), 0, 0);
  };
  var DrawOctahedron = function (ctx, viewport, token, token_index) {
    var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    var pq = Math . sqrt (3) * 0.5;
    var pth = new Path2D ();
    pth . moveTo (0, - hh); pth . lineTo (hw * pq, hh * -0.5); pth . lineTo (hw * pq, hh * 0.5);
    pth . lineTo (0, hh); pth . lineTo (- hw * pq, hh * 0.5); pth . lineTo (- hw * pq, hh * -0.5); pth . closePath ();
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill (pth);}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke (pth);
    if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
    ctx . beginPath (); ctx . moveTo (0, - hh);
    ctx . lineTo (hw * pq, hh * 0.5); ctx . lineTo (- hw * pq, hh * 0.5); ctx . closePath ();
    ctx . stroke ();
    ctx . fillStyle = token . ForegroundColour;
    ctx . font = hh + 'px arial'; ctx . textBaseline = 'middle'; ctx . textAlign = 'center';
    ctx . fillText (DiceValue (token), 0, 0);
  };
  var DrawDeltohedron = function (ctx, viewport, token, token_index) {
    var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    var pq = Math . sqrt (3) * 0.5;
    var pth = new Path2D ();
    //============
    var alpha = 0.0168338 * Math . PI;
    var ratio = Math . cos (Math . PI / 5);
    var radius = Math . tan (Math . PI / 4 + alpha);
    var sub = Math . tan (Math . PI / 4 - alpha);
    var compensation = radius - sub;
    var prism_height = compensation / sub;
    var edge = Math . sqrt (1 + radius * radius);
    var across = Math . sqrt (radius * radius + (1 + prism_height) * (1 + prism_height));
    var centre = Math . sqrt (1 + sub * sub);
    var kite = radius * Math . sin (Math . PI / 5);
    var short_edge = radius * Math . sin (Math . PI / 10) * 2;
    var visible_edge_length = short_edge * Math . cos (radius) + prism_height * Math . sin (radius);
    var distance_to_edge = radius * Math . cos (Math . PI / 10);
    // console . log (radius, sub, compensation, prism_height);
    // console . log ('Edge', edge, 'Across', across, 'Kite', kite, 'Centre', centre, 'Visible edge', visible_edge_length, 'Width', distance_to_edge);
    sub *= sub;
    // console . log (sub, radius, ratio);
    //=============
    pth . moveTo (0, 0);
    pth . lineTo (hw * distance_to_edge, hh * 0.5 * (across - visible_edge_length));
    pth . lineTo (hw * distance_to_edge, hh * 0.5 * (across + visible_edge_length));
    pth . lineTo (0, hh * across);
    pth . lineTo (- hw * distance_to_edge, hh * 0.5 * (across + visible_edge_length));
    pth . lineTo (- hw * distance_to_edge, hh * 0.5 * (across - visible_edge_length));
    pth . closePath ();
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill (pth);}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke (pth);
    if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
    var prop = Math . sin (Math . PI * 3 / 10) * 2;
    var hhw = hw / prop, hhh = hh / prop;
    ctx . beginPath ();
    ctx . moveTo (- hw * kite, hh * centre);
    ctx . lineTo (0, 0);
    ctx . lineTo (hw * kite, hh * centre);
    ctx . stroke ();
    ctx . stroke ();
    ctx . fillStyle = token . ForegroundColour;
    ctx . font = (hh * 0.6) + 'px arial'; ctx . textBaseline = 'bottom'; ctx . textAlign = 'center';
    ctx . fillText (DiceValue (token), 0, hh * centre);
  };
  var DrawDodecahedron = function (ctx, viewport, token, token_index) {
    var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    var pq = Math . sqrt (3) * 0.5;
    var pth = new Path2D ();
    pth . moveTo (0, - hh);
    for (var ind = -1.5 * Math . PI / 5; ind < 4.25; ind += Math . PI / 5) pth . lineTo (hw * Math . cos (ind), hh * Math . sin (ind));
    pth . closePath ();
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill (pth);}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke (pth);
    if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
    var prop = Math . sin (Math . PI * 3 / 10) * 2;
    ctx . beginPath ();
    for (var ind = -0.5 * Math . PI; ind < 4.25; ind += Math . PI / 2.5) {
      ctx . moveTo (hw * Math . cos (ind), hh * Math . sin (ind));
      ctx . lineTo (hw * Math . cos (ind) / prop, hh * Math . sin (ind) / prop);
      ctx . lineTo (hw * Math . cos (ind + Math . PI / 2.5) / prop, hh * Math . sin (ind + Math . PI / 2.5) / prop);
    }
    ctx . stroke ();
    ctx . fillStyle = token . ForegroundColour;
    ctx . font = (hh / prop) + 'px arial'; ctx . textBaseline = 'middle'; ctx . textAlign = 'center';
    ctx . fillText (DiceValue (token), 0, 0);
  };
  var DrawIcosahedron = function (ctx, viewport, token, token_index) {
    var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
    ctx . translate (token . location . position . x, token . location . position . y);
    ctx . rotate (token . Rotation * Math . PI / 12);
    var pq = Math . sqrt (3) * 0.5;
    var pth = new Path2D ();
    pth . moveTo (0, - hh);
    for (var ind = -0.5 * Math . PI; ind < 5; ind += Math . PI / 3) pth . lineTo (hw * Math . cos (ind), hh * Math . sin (ind));
    pth . closePath ();
    if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill (pth);}
    ctx . strokeStyle = token . ForegroundColour;
    ctx . stroke (pth);
    if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
    var prop = Math . sin (Math . PI * 3 / 10) * 2;
    var hhw = hw / prop, hhh = hh / prop;
    ctx . beginPath ();
    for (var ind = -0.5 * Math . PI; ind < 5; ind += Math . PI / 1.5) {
      ctx . moveTo (hw * Math . cos (ind), hh * Math . sin (ind));
      ctx . lineTo (hhw * Math . cos (ind), hhh * Math . sin (ind));
      ctx . lineTo (hw * Math . cos (ind + Math . PI / 3), hh * Math . sin (ind + Math . PI / 3));
      ctx . lineTo (hhw * Math . cos (ind + Math . PI / 1.5), hhh * Math . sin (ind + Math . PI / 1.5));
      ctx . lineTo (hhw * Math . cos (ind), hhh * Math . sin (ind));
    }
    ctx . stroke ();
    ctx . fillStyle = token . ForegroundColour;
    ctx . font = (hh * 0.4) + 'px arial'; ctx . textBaseline = 'middle'; ctx . textAlign = 'center';
    ctx . fillText (DiceValue (token), 0, 0);
  };
  var draws = {
    Grid: function (ctx, viewport, token, token_index) {
      switch (token . Side) {
      case 1: DrawHorizontalHexGrid (ctx, viewport, token, token_index, true); break;
      case 2: DrawHorizontalHexGrid (ctx, viewport, token, token_index, false); break;
      case 3: DrawVerticalHexGrid (ctx, viewport, token, token_index, true); break;
      case 4: DrawVerticalHexGrid (ctx, viewport, token, token_index, false); break;
      default: DrawSquareGrid (ctx, viewport, token, token_index); break;
      }
    },
    Rectangle: function (ctx, viewport, token, token_index) {
      var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
      ctx . translate (token . location . position . x, token . location . position . y);
      ctx . rotate (token . Rotation * Math . PI / 12);
      ctx . beginPath ();
      ctx . moveTo (- hw, - hh); ctx . lineTo (hw, - hh); ctx . lineTo (hw, hh); ctx . lineTo (- hw, hh); ctx . closePath ();
      if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill ();}
      ctx . strokeStyle = token . ForegroundColour;
      ctx . stroke ();
      if (token_index !== null) ctx . addHitRegion ({id: token_index});
    },
    Circle: function (ctx, viewport, token, token_index) {
      var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
      ctx . translate (token . location . position . x, token . location . position . y);
      ctx . rotate (token . Rotation * Math . PI / 12);
      ctx . beginPath ();
      ctx . ellipse (0, 0, hw, hh, 0, 0, Math . PI + Math . PI);
      if (token . BackgroundColour != null) {ctx . fillStyle = token . BackgroundColour; ctx . fill ();}
      ctx . strokeStyle = token . ForegroundColour;
      ctx . stroke ();
      if (token_index !== null) ctx . addHitRegion ({id: token_index});
    },
    Picture: function (ctx, viewport, token, token_index) {
      //var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
      var image = find_image (token);
      ctx . translate (token . location . position . x, token . location . position . y);
      ctx . rotate (token . Rotation * Math . PI / 12);
      ctx . scale (token . scaling . x, token . scaling . y);
      var width = token . location . size . x, height = token . location . size .y;
      ctx . translate (width * -0.5, height * -0.5);
      if (token . index === undefined) {
        if (token . Sides === 1) ctx . drawImage (image, 0, 0, width, height);
        else {
          var fraction = image . height / token . Sides;
          ctx . drawImage (image, 0, fraction * token . Side, image . width, fraction, 0, 0, width, height);
        }
      } else {
        var fraction_x = image . width / token . indexing . x, fraction_y = image . height / token . indexing . y;
        ctx . drawImage (image, fraction_x * token . index . x, fraction_y * token . index . y, fraction_x, fraction_y, 0, 0, width, height);
      }
      var pth = new Path2D ();
      pth . moveTo (0, 0); pth . lineTo (width, 0); pth . lineTo (width, height); pth . lineTo (0, height); pth . closePath ();
      if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
    },
    Dice: function (ctx, viewport, token, token_index) {
      switch (token . Sides) {
        case 0: DrawDice (ctx, viewport, token, token_index); break;
        case 4: DrawTetrahedron (ctx, viewport, token, token_index); break;
        case 6: DrawCube (ctx, viewport, token, token_index); break;
        case 8: DrawOctahedron (ctx, viewport, token, token_index); break;
        case 10: DrawDeltohedron (ctx, viewport, token, token_index); break;
        case 12: DrawDodecahedron (ctx, viewport, token, token_index); break;
        case 20: DrawIcosahedron (ctx, viewport, token, token_index); break;
        default: DrawCube (ctx, viewport, token, token_index); break;
      }
    },
    Text: function (ctx, viewport, token, token_index) {
      if (token . Font === undefined) token . Font = token . location . size . y + 'px arial';
      ctx . font = token . Font;
      if (token . Text !== undefined) token . location . size . x = ctx . measureText (token . Text) . width;
      var hw = token . location . size . x * 0.5 * token . scaling . x, hh = token . location . size . y * 0.5 * token . scaling . y;
      ctx . translate (token . location . position . x, token . location . position . y);
      ctx . rotate (token . Rotation * Math . PI / 12);
      var pth = new Path2D ();
      pth . moveTo (- hw, - hh); pth . lineTo (hw, - hh); pth . lineTo (hw, hh); pth . lineTo (- hw, hh); ctx . closePath ();
      ctx . fillStyle = token . ForegroundColour;
      ctx . textAlign = 'center'; ctx . textBaseline = 'middle';
      ctx . fillText (token . Text, 0, 0);
      if (token_index !== null) ctx . addHitRegion ({path: pth, id: token_index});
    }
  };
  var rollDice = function (dice) {
    dice . Side = Math . floor (Math . random () * (dice . Sides === 0 ? 6 : dice . Sides));
    var current_rotation = dice . Rotation % 4;
    while (current_rotation === (dice . Rotation % 4)) dice . Rotation = Math . floor (Math . random () * 24);
  };
  var viewport = function (atom, viewport, name, x, y, width, height) {
    if (viewport !== null) {
      name = viewport . name;
      x = viewport . location . x; y = viewport . location . y;
      width = viewport . size . x; height = viewport . size . y;
    }
    var content = document . createElement ('canvas');
    if (width === null) width = content . width; else content . width = width;
    if (height === null) height = content . height; else content . height = height;
    if (x === null) x = 0; if (y === null) y = 0;
    if (name === null) name = atom . name;
    if (viewport === null) viewport = {atom: atom . name, name: name, location: {x: x, y: y}, position: {x: 0, y: 0}, size: {x: width, y: height}, scaling: {x: 1, y: 1}, Mode: 'move'};
    structure . viewports . push (viewport);
    var bar = document . createElement ('div'); bar . style . background = 'yellow'; bar . appendChild (document . createTextNode (name)); bar . style ['font-family'] = 'arial';
    var close = document . createElement ('input'); close . type = 'button'; close . value = String . fromCharCode (0xd7); close . style . float = 'right';
    bar . appendChild (close);
    var ctx = content . getContext ('2d');
    var info = document . createElement ('div'); info . style . background = 'yellow'; info . appendChild (document . createTextNode ('info')); info . style ['font-family'] = 'arial';
    var resize = document . createElement ('input'); resize . type = 'button'; resize . value = String . fromCharCode (0x21f2); resize . style . float = 'right';
    info . appendChild (resize);
    var radio_side = document . createElement ('input'); radio_side . type = 'radio'; radio_side . name = `${atom . name}@mode`; radio_side . style .float = 'right';
    radio_side . onmousedown = function () {viewport . Mode = 'side';}; info . appendChild (radio_side);
    var radio_rotate = document . createElement ('input'); radio_rotate . type = 'radio'; radio_rotate . name = `${atom . name}@mode`; radio_rotate . style . float = 'right';
    radio_rotate . onmousedown = function () {viewport . Mode = 'rotate';}; info . appendChild  (radio_rotate);
    var radio_select = document . createElement ('input'); radio_select . type = 'radio'; radio_select . name = `${atom . name}@mode`; radio_select . style . float = 'right';
    radio_select . onmousedown = function () {viewport . Mode = 'select';}; info . appendChild (radio_select);
    var radio_move = document . createElement ('input'); radio_move . type = 'radio'; radio_move . name = `${atom . name}@mode`; radio_move . checked = true; radio_move . style . float = 'right';
    radio_move . onmousedown = function () {viewport . Mode = 'move';}; info . appendChild (radio_move);
    var save_button = document . createElement ('input'); save_button . type = 'button'; save_button . value = 'SAVE'; save_button . style . float = 'right';
    save_button . onmousedown = function () {
      var location = prompt ('File Name:');
      if (location == null) return;
      studio . writeFile (location, JSON . stringify (structure));
    };
    info . appendChild (save_button);
    var div = document . createElement ('div');
    var mode = 'navigate';
    div . appendChild (bar);
    div . appendChild (content);
    div . appendChild (info);
    div . style . position = 'absolute'; div . style . top = viewport . location . y; div . style . left = viewport . location . x;
    var selected = [];
    var repaint = function () {
      ctx . clearHitRegions ();
      ctx . fillStyle = viewport . BackgroundColour || structure . BackgroundColour;
      ctx . clearRect (0, 0, viewport . size . x, viewport . size . y);
      ctx . fillRect (0, 0, viewport . size . x, viewport . size . y);
      ctx . save ();
      ctx . scale (viewport . scaling . x, viewport . scaling . y);
      ctx . translate (- viewport . position . x, - viewport . position . y);
      for (var ind in structure . tokens) {
        ctx . save ();
        var index = selected . indexOf (structure . tokens [ind]) >= 0 ? null : ind;
        draws [structure . tokens [ind] . type] (ctx, viewport, structure . tokens [ind], index);
        ctx . restore ();}
      ctx . restore ();
    };
    var for_double_click = null;
    document . oncontextmenu = function (e) {return false;};
    var mouseup = function (e) {
      document . onmouseup = null; document . onmousemove = null;
      if (viewport . Mode === 'select' && e . region . length > 0) {
        var ind = Number (e . region);
        var deck = structure . tokens [ind];
        if (deck != null && deck . deck !== undefined) {
          for (var sub in selected) {
            if (deck !== selected [sub]) {
              structure . tokens . splice (structure . tokens . indexOf (selected [sub]), 1);
              deck . deck . push (selected [sub]);
            }
          }
        }
      }
      for_double_click = selected;
      selected = []; repaint ();
    };
    var mousemove = function (e) {
      viewport . location . x += e . movementX; viewport . location . y += e . movementY;
      div . style . top = viewport . location . y; div . style . left = viewport . location . x;
    };
    var mousesize = function (e) {
      viewport . size . x += e . movementX; viewport . size . y += e . movementY;
      content . width = viewport . size . x; content . height = viewport . size . y;
      repaint ();
    };
    var canvas_move = function (e) {
      switch (viewport . Mode) {
      case 'move':
        viewport . position . x -= e . movementX / viewport . scaling . x;
        viewport . position . y -= e . movementY / viewport . scaling . y;
        repaint ();
        break;
      case 'select':
        if (selected . length > 0) {
          for (var ind in selected) {
            selected [ind] . location . position . x += e . movementX;
            selected [ind] . location . position . y += e . movementY;
          }
          repaint ();
        }
        break;
      default: break;
      }
    };
    content . onwheel = function (e) {
      e . preventDefault ();
      var delta = e . deltaX + e . deltaY;
      var fraction = Math . pow (1.0625, - delta);
      switch (viewport . Mode) {
      case 'move': viewport . scaling . x *= fraction; viewport . scaling . y *= fraction; repaint (); break;
      case 'select':
        if (selected . length > 0) {
          for (var ind in selected) selected [ind] . Rotation += delta;
          repaint ();
        }
        break;
      case 'side':
        if (selected . length > 0) {
          for (var ind in selected) {
            selected [ind] . Side -= delta;
            if (selected [ind] . Side < 0) selected [ind] . Side = 0;
            if (selected [ind] . Side >= selected [ind] . Sides) selected [ind] . Side = selected [ind] . Sides - 1;
          }
          repaint ();
        }
        break;
      default: break;
      }
    };
    bar . onmousedown = function (e) {document . onmouseup = mouseup; document . onmousemove = mousemove;};
    resize . onmousedown = function (e) {document . onmouseup = mouseup; document . onmousemove = mousesize;};
    document . body . appendChild (div);
    var remove_viewport = function () {
      div . parentElement . removeChild (div);
      repaints . splice (repaints . indexOf (repaint, 1));
      structure . viewports . splice (structure . viewports . indexOf (viewport), 1);
      return atom . setMachine (null);
    };
    viewport_removers . push (remove_viewport);
    close . onmousedown = function (e) {
      viewport_removers . splice (viewport_removers . indexOf (remove_viewport), 1);
      remove_viewport ();
    };
    content . ondblclick = function (e) {
      if (for_double_click === null) return;
      for (var ind in for_double_click) {
        if (for_double_click [ind] . type === 'Dice') rollDice (for_double_click [ind]);
        if (for_double_click [ind] . deck != null) {
          var target = for_double_click [ind] . deck . pop ();
          if (target == null) return;
          target . location . position . x = for_double_click [ind] . location . position . x;
          target . location . position . y = for_double_click [ind] . location . position . y;
          structure . tokens . push (target);
        }
      }
      repaint ();
    };
    content . onmousedown = function (e) {
      e . preventDefault ();
      document . onmouseup = mouseup; document . onmousemove = canvas_move;
      if (e . region . length > 0) {
        var ind = Number (e . region);
        var token = structure . tokens [ind];
        structure . tokens . splice (ind, 1);
        structure . tokens . push (token);
        selected . push (token);
        if (token . type === 'Dice' && e . which > 1) rollDice (token);
        if (token . deck != null && e . which > 1) {
          if (confirm (`Shuffle deck [${token . atom}] ?`)) studio . random_permutation (token . deck);
          else {
            var target = studio . random_pop (token . deck);
            if (target != null) {
              target . location . position . x = token . location . position . x;
              target . location . position . y = token . location . position . y;
              structure . tokens . push (target);
            }
          }
        }
        repaint ();
      }
    };
    repaint ();
    repaints . push (repaint);
    this . code = function (el) {
      if (el . type === 0) return remove_viewport ();
      if (el . type !== 1) return false;
      var selector = el . left; el = el . right;
      if (selector . type === 3) {
        switch (selector . left) {
          case Location:
            if (el . type === 2) {
              el = el . setNativePair (viewport . location . x);
              el = el . setNativePair (viewport . location . y);
              el = el . setNativePair (viewport . size . x);
              el . setNativePair (viewport . size . y);
              return true;
            }
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . location . x = el . left . left; div . style . left = viewport . location . x; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . location . y = el . left . left; div . style . top = viewport . location . y; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return true; viewport . size . x = el . left . left; content . width = viewport . size . x; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . size . y = el . left . left; content . height = viewport . size . y;
            return true;
          case Position:
            if (el . type === 2) {el = el . setNativePair (viewport . position . x); el . setNativePair (viewport . position . y); return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . position . x = el . left . left; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . position . y = el . left . left;
            return true;
          case Size:
            if (el . type === 2) {el = el . setNativePair (viewport . size . x); el . setNativePair (viewport . size . y); return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . size . x = el . left . left; content . width = viewport . size . x; el = el . right;
            if (el . type === 0) {viewport . size . y = viewport . size . x; return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . size . y = el . left . left; content . height = viewport . size . y;
            return true;
          case Scaling:
            if (el . type === 2) {el = el . setNativePair (viewport . scaling . x); el . setNativePair (viewport . scaling . y); return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . scaling . x = el . left . left; el = el . right;
            if (el . type === 0) {viewport . scaling . y = viewport . scaling . x; return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; viewport . scaling . y = el . left . left;
            return true;
          case Repaint: repaint (); return true;
          default:
            if (el . type === 1) el = el . left;
            if (el . type === 2) {if (! viewport [selector . left . name]) return false; el . setNative (viewport [selector . left . name]); return true;}
            if (el . type === 6) {viewport [selector . left . name] = el . left; return true;}
            return true;
        }
      }
      return false;
    };
  };
  var Viewport = {
    code: function (el) {
      var atom = null, name = null, x = null, y = null, width = null, height = null;
      while (el . type === 1) {
        var e = el . left;
        if (e . type === 2 || e . type === 3) atom = e;
        if (e . type === 6) {
          if (typeof (e . left) === 'number') {
            if (x === null) x = e . left;
            else if (y === null) y = e . left;
            else if (width === null) width = e . left;
            else height = e . left;
          } else name = e . left;
        }
        el = el . right;
      }
      if (atom === null) return false;
      if (atom . type === 2) atom . setAtom (new prolog . Atom ());
      if (atom . left . machine !== null) return false;
      return atom . left . setMachine (new viewport (atom . left, name, null, x, y, width, height));
    }
  };
  var ColourFunction = function (colour_type) {
    this . code = function (el) {
      if (el . type === 2) {el . setNative (structure [`${colour_type}Colour`]); return true;}
      if (el . type !== 1) return false;
      var selector = el . left;
      if (selector . type === 2) {selector . setNative (structure [`${colour_type}Colour`]); return true;}
      if (selector . type === 6) {structure [`${colour_type}Colour`] = selector . left; return true;}
      if (selector . type !== 3) return false;
      el = el . right; if (el . type === 1) el = el . left;
      if (el . type === 2) {el . setNative (structure [`${selector . left . name}${colour_type}Colour`]); return true;}
      if (el . type === 6) {structure [`${selector . left . name}${colour_type}Colour`] = el . left; return true;}
      return false;
    }
  };
  var grid_token_position = function (grid, token, x, y) {
    if (grid . index != null) {x -= grid . index . x; y -= grid . index . y;}
    x *= grid . location . size . x * grid . scaling . x;
    y *= grid . location . size . y * grid . scaling . y;
    var sn = Math . sin (grid . Rotation * Math . PI / 12);
    var cs = Math . cos (grid . Rotation * Math . PI / 12);
    token . location . position . x = grid . location . position . x + x * cs - y * sn;
    token . location . position . y = grid . location . position . y + y * cs + x * sn;
  };
  var token = function (atom, token, type) {
    if (token !== null) type = token . type;
    else token = {
      atom: atom . name, type: type, location: {position: {x: 0, y: 0}, size: type === "Picture" ? null : {x: 128, y: 128}},
      scaling: {x: 1, y: 1}, Rotation: 0,
      indexing: type === "Dice" ? {x: 16, y: 16} : {x: 4, y: 4},
      ForegroundColour: 'white',
      Sides: type === "Grid" ? 5 : type === "Dice" ? 0 : 1, Side: 0,
      Shift: type === "Dice" ? 1 : 0, Multiplier: 1
    };
    this . token = token;
    structure . tokens . push (token);
    this . code = function (el) {
      if (el . type === 0) {
        structure . tokens . splice (structure . tokens . indexOf (token), 1);
        atoms . splice (atoms . indexOf (atom), 1);
        return atom . setMachine (null);
      }
      if (el . type !== 1) return false;
      var selector = el . left; el = el . right;
      if (selector . type === 3) {
        var random_pop = false;
        switch (selector . left) {
          case Location:
            if (el . type === 2) {
              el = el . setNativePair (token . location . position . x);
              el = el . setNativePair (token . location . position . y);
              if (token . location . size === null) return true;
              el = el . setNativePair (token . location . size . x);
              el . setNativePair (token . location . size . y);
              return true;
            }
            if (el . type !== 1 || el . left . type !== 6) return false; token . location . position . x = el . left . left; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; token . location . position . y = el . left . left; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return true;
            if (token . location . size === null) token . location . size = {};
            token . location . size . x = el . left . left; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; token . location . size . y = el . left . left;
            return true;
          case Position:
            if (el . type === 2) {el = el . setNativePair (token . location . position . x); el . setNativePair (token . location . position . y); return true;}
            if (el . type === 1 && el . left . type === 3) {
              var target = el . left . left . machine;
              if (target !== null) target = target . token;
              if (target === null) return false;
              el = el . right;
              if (el . type !== 1 || el . left . type !== 6) return false;
              var x = el . left . left; el = el . right;
              if (el . type !== 1 || el . left . type !== 6) return false;
              var y = el . left . left;
              if (token . type === "Grid") grid_token_position (token, target, x, y);
              else if (target . type === "Grid") grid_token_position (target, token, x, y);
              return true;
            }
            if (el . type !== 1 || el . left . type !== 6) return false; token . location . position . x = el . left . left; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; token . location . position . y = el . left . left;
            return true;
          case Size:
            if (el . type === 2) {el = el . setNativePair (token . location . size . x); el . setNativePair (token . location . size . y); return true;}
            if (el . type !== 1 || el . left . type !== 6) return false;
            if (token . location . size === null) token . location . size = {};
            token . location . size . x = el . left . left; el = el . right;
            if (el . type === 0) {token . location . size . y = token . location . size . x; return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; token . location . size . y = el . left . left;
            return true;
          case Scaling:
            if (el . type === 2) {el = el . setNativePair (token . scaling . x); el . setNativePair (token . scaling . y); return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; token . scaling . x = el . left . left; el = el . right;
            if (el . type === 0) {token . scaling . y = token . scaling . x; return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; token . scaling . y = el . left . left;
            return true;
          case Indexing:
            if (el . type === 2) {el = el . setNativePair (token . indexing . x); el . setNativePair (token . indexing . y); return true;}
            if (el . type !== 1 || el . left . type !== 6) return false; token . indexing . x = el . left . left; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; token . indexing . y = el . left . left;
            return true;
          case Index:
            if (el . type === 2) {
              if (token . index === undefined) return false;
              el = el . setNativePair (token . index . x); el . setNativePair (token . index . y);
              return true;
            }
            if (el . type === 0) {delete token . index; return true;}
            token . index = {};
            if (el . type !== 1 || el . left . type !== 6) return false; token . index . x = el . left . left; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; token . index . y = el . left . left;
            return true;
          case RotateBy: if (el . type !== 1 || el . left . type !== 6) return false; token . Rotation += el . left . left; return true;
          case MoveBy:
            if (el . type !== 1 || el . left . type !== 6) return false; token . location . position . x += el . left . left; el = el . right;
            if (el . type !== 1 || el . left . type !== 6) return false; token . location . position . y += el . left . left;
            return true;
          case Deck: if (token . deck === undefined) token . deck = []; return true;
          case ReleaseRandom: random_pop = true;
          case Release:
            if (token . deck === undefined || token . deck . length < 1) return false;
            var target = random_pop ? studio . random_pop (token . deck) : token . deck . pop ();
            target . location . position . x = token . location . position . x;
            target . location . position . y = token . location . position . y;
            structure . tokens . push (target);
            if (el . type === 1) el = el . left;
            if (el . type === 2) {for (var ind in atoms) {if (atoms [ind] . machine !== null && atoms [ind] . machine . token === target) {el . setAtom (atoms [ind]); return true;}}}
            return true;
          case Shuffle: case Roll:
            if (token . deck !== undefined) {studio . random_permutation (token . deck); return true;}
            if (token . Sides > 1) rollDice (token);
            if (el . type === 1) el = el . left; if (el . type === 2) el . setNative ((token . Side + token . Shift) * token . Multiplier);
            return true;
          case Insert:
            if (token . deck !== undefined) {
              while (el . type === 1) {
                var target = el . left;
                if (target . type === 3) {
                  target = target . left;
                  target = target && target . machine && target . machine . token;
                  if (! target) return false;
                  var index = structure . tokens . indexOf (target);
                  if (index < 0) return false;
                  structure . tokens . splice (index, 1);
                  token . deck . push (target);
                }
                el = el . right;
              }
              return true;
            }
            if (el . type === 1) el = el . left; if (el . type !== 3) return false;
            var target = el . left;
            target = target && target . machine && target . machine . token && target . machine . token . deck;
            if (! target) return false;
            var index = structure . tokens . indexOf (token); if (index < 0) return false;
            structure . tokens . splice (index, 1);
            target . push (token);
            return true;
          default:
            if (el . type === 1) el = el . left;
            if (el . type === 2) {if (token [selector . left . name] === undefined) return false; el . setNative (token [selector . left . name]); return true;}
            if (el . type === 6) {token [selector . left . name] = el . left; return true;}
            return true;
        }
      }
      return false;
    };
  };
  var Token = {
    code: function (el) {
      var atom = null, type = null;
      while (el . type === 1) {
        if (el . left . type === 2) atom = el . left;
        if (el . left . type === 3) {if (atom === null) atom = el . left; else type = el . left . left . name;}
        if (el . left . type === 6) type = el . left . left;
        el = el . right;
      }
      if (atom === null || type === null) return false;
      if (atom . type === 2) atom . setAtom (new prolog . Atom ());
      if (atom . left . machine !== null) return false;
      atoms . push (atom . left);
      return atom . left . setMachine (new token (atom . left, null, type));
    }
  };
  var SaveBoard = {
    code: function (el) {
      if (el . type !== 1) return false;
      el = el . left;
      if (el . type !== 6) return false;
      studio . writeFile (el . left, JSON . stringify (structure));
      return true;
    }
  };
  var erase_board = function () {for (var ind in atoms) atoms [ind] . setMachine (null); atoms = []; structure . tokens = [];};
  var erase = function () {erase_board (); for (var ind in viewport_removers) viewport_removers [ind] (); viewport_removers = [];};
  var EraseBoard = {code: function (el) {erase_board (); return true;}};
  var Erase = {code: function (el) {erase (); return true;}};
  var load_json = function (el) {
    if (el . type === 1) el = el . left; if (el . type !== 6) return false;
    var json = studio . readFile (el . left);
    if (! json) return false;
    try {json = JSON . parse (json);} catch (e) {return null;}
    return json;
  };
  var load_tokens = function (json) {
    for (var ind in json . tokens) {
      var element = json . tokens [ind];
      var atom = root . searchC (element . atom);
      atom . setMachine (new token (atom, element));
    }
  };
  var load_viewports = function (json) {
    for (var ind in json . viewports) {
      var view = json . viewports [ind];
      var atom = root . searchC (view . atom);
      atom . setMachine (new viewport (atom, view));
    }
  };
  var LoadBoard = {
    code: function (el) {
      var json = load_json (el); if (json === null) return false;
      erase_board ();
      load_tokens (json);
      return true;
    }
  };
  var Load = {
    code: function (el) {
      var json = load_json (el); if (json === null) return false;
      erase ();
      load_tokens (json);
      load_viewports (json);
      return true;
    }
  };
  this . getNativeCode = function (name) {
    switch (name) {
      case 'Viewport': return Viewport;
      case 'BackgroundColour': return new ColourFunction ('Background');
      case 'ForegroundColour': return new ColourFunction ('Foreground');
      case 'Token': return Token;
      case 'Repaint': return {code: function (el) {for (var ind in repaints) repaints [ind] (); return true;}};
      case 'SaveBoard': return SaveBoard;
      case 'Erase': return Erase;
      case 'EraseBoard': return EraseBoard;
      case 'LoadBoard': return LoadBoard;
      case 'Load': return Load;
      default: break;
    }
    return null;
  };
}
);

var rep = function rep (command) {return res ((command || '') + '[Repaint]');};

studio . setResource (['fxg.prc'], `
program fxg #machine := 'prolog . fxg'
  [
    Viewport
    Token Rectangle Circle Picture Dice Grid Text Deck
    Location Position Size Scaling Rotation Side Sides Text Index Indexing Mode
    RotateBy MoveBy Release ReleaseRandom Shuffle Roll Insert
    BackgroundColour ForegroundColour
    SaveBoard EraseBoard Erase LoadBoard Load
    Repaint
  ]

#machine Viewport := 'Viewport'
#machine BackgroundColour := 'BackgroundColour'
#machine ForegroundColour := 'ForegroundColour'
#machine Token := 'Token'
#machine Repaint := 'Repaint'
#machine SaveBoard := 'SaveBoard'
#machine EraseBoard := 'EraseBoard'
#machine Erase := 'Erase'
#machine LoadBoard := 'LoadBoard'
#machine Load := 'Load'

end .
`);
