.pragma library
//"use strict";

//Object.defineProperty(exports, "__esModule", {
//  value: true
//});
//exports.thumbHashToRGBA = thumbHashToRGBA;
///**
// * Decodes a ThumbHash to an RGBA image. RGB is not be premultiplied by A.
// *
// * @param hash The bytes of the ThumbHash.
// * @returns The width, height, and pixels of the rendered placeholder image.
// */
//function thumbHashToRGBA(hash) {
//  var PI = Math.PI,
//    min = Math.min,
//    max = Math.max,
//    cos = Math.cos,
//    round = Math.round;

//  // Read the constants
//  var header24 = hash[0] | hash[1] << 8 | hash[2] << 16;
//  var header16 = hash[3] | hash[4] << 8;
//  var l_dc = (header24 & 63) / 63;
//  var p_dc = (header24 >> 6 & 63) / 31.5 - 1;
//  var q_dc = (header24 >> 12 & 63) / 31.5 - 1;
//  var l_scale = (header24 >> 18 & 31) / 31;
//  var hasAlpha = header24 >> 23;
//  var p_scale = (header16 >> 3 & 63) / 63;
//  var q_scale = (header16 >> 9 & 63) / 63;
//  var isLandscape = header16 >> 15;
//  var lx = max(3, isLandscape ? hasAlpha ? 5 : 7 : header16 & 7);
//  var ly = max(3, isLandscape ? header16 & 7 : hasAlpha ? 5 : 7);
//  var a_dc = hasAlpha ? (hash[5] & 15) / 15 : 1;
//  var a_scale = (hash[5] >> 4) / 15;

//  // Read the varying factors (boost saturation by 1.25x to compensate for quantization)
//  var ac_start = hasAlpha ? 6 : 5;
//  var ac_index = 0;
//  var decodeChannel = function (nx, ny, scale) {
//    var ac = [];
//    for (var cy = 0; cy < ny; cy++) for (var cx = cy ? 0 : 1; cx * ny < nx * (ny - cy); cx++) ac.push(((hash[ac_start + (ac_index >> 1)] >> ((ac_index++ & 1) << 2) & 15) / 7.5 - 1) * scale);
//    return ac;
//  };
//  var l_ac = decodeChannel(lx, ly, l_scale);
//  var p_ac = decodeChannel(3, 3, p_scale * 1.25);
//  var q_ac = decodeChannel(3, 3, q_scale * 1.25);
//  var a_ac = hasAlpha && decodeChannel(5, 5, a_scale);

//  // Decode using the DCT into RGB
//  var ratio = thumbHashToApproximateAspectRatio(hash);
//  var w = round(ratio > 1 ? 32 : 32 * ratio);
//  var h = round(ratio > 1 ? 32 / ratio : 32);
//  var rgba = new Uint8Array(w * h * 4),
//    fx = [],
//    fy = [];
//  for (var y = 0, i = 0; y < h; y++) {
//    for (var x = 0; x < w; x++, i += 4) {
//      var l = l_dc,
//        p = p_dc,
//        q = q_dc,
//        a = a_dc;

//      // Precompute the coefficients
//      for (var cx = 0, n = max(lx, hasAlpha ? 5 : 3); cx < n; cx++) fx[cx] = cos(PI / w * (x + 0.5) * cx);
//      for (var cy = 0, _n = max(ly, hasAlpha ? 5 : 3); cy < _n; cy++) fy[cy] = cos(PI / h * (y + 0.5) * cy);

//      // Decode L
//      for (var _cy = 0, j = 0; _cy < ly; _cy++) for (var _cx = _cy ? 0 : 1, fy2 = fy[_cy] * 2; _cx * ly < lx * (ly - _cy); _cx++, j++) l += l_ac[j] * fx[_cx] * fy2;

//      // Decode P and Q
//      for (var _cy2 = 0, _j = 0; _cy2 < 3; _cy2++) {
//        for (var _cx2 = _cy2 ? 0 : 1, _fy = fy[_cy2] * 2; _cx2 < 3 - _cy2; _cx2++, _j++) {
//          var f = fx[_cx2] * _fy;
//          p += p_ac[_j] * f;
//          q += q_ac[_j] * f;
//        }
//      }

//      // Decode A
//      if (hasAlpha) for (var _cy3 = 0, _j2 = 0; _cy3 < 5; _cy3++) for (var _cx3 = _cy3 ? 0 : 1, _fy2 = fy[_cy3] * 2; _cx3 < 5 - _cy3; _cx3++, _j2++) a += a_ac[_j2] * fx[_cx3] * _fy2;

//      // Convert to RGB
//      var b = l - 2 / 3 * p;
//      var r = (3 * l - b + q) / 2;
//      var g = r - q;
//      rgba[i] = max(0, 255 * min(1, r));
//      rgba[i + 1] = max(0, 255 * min(1, g));
//      rgba[i + 2] = max(0, 255 * min(1, b));
//      rgba[i + 3] = max(0, 255 * min(1, a));
//    }
//  }
//  return {
//    w: w,
//    h: h,
//    rgba: rgba
//  };
//}

///**
// * Extracts the approximate aspect ratio of the original image.
// *
// * @param hash The bytes of the ThumbHash.
// * @returns The approximate aspect ratio (i.e. width / height).
// */
//function thumbHashToApproximateAspectRatio(hash) {
//  var header = hash[3];
//  var hasAlpha = hash[2] & 0x80;
//  var isLandscape = hash[4] & 0x80;
//  var lx = isLandscape ? hasAlpha ? 5 : 7 : header & 7;
//  var ly = isLandscape ? header & 7 : hasAlpha ? 5 : 7;
//  return lx / ly;
//}
