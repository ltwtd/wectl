export module wectl.web.gl_eye;
import <string>;

export namespace wectl::web::gl {
	std::string get_eye_shader() {
		return R"GLSL(#version 300 es
precision highp float;
in vec2 v_uv;
out vec4 fragColor;
uniform vec2 u_res, u_mouse;
uniform float u_time, u_scroll, u_zoom;
uniform int u_mode;

mat2 rot(float a) { float s = sin(a), c = cos(a); return mat2(c, -s, s, c); }
float hash(vec2 p) { return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123); }

float noise(vec2 p) {
	vec2 i = floor(p), f = fract(p);
	f = f * f * (3.0 - 2.0 * f);
	return mix(mix(hash(i), hash(i + vec2(1.0, 0.0)), f.x), mix(hash(i + vec2(0.0, 1.0)), hash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// Тяжёлый 8-октавный FBM для сверхмелкой микротекстуры поверхности
float fbm8(vec2 p) {
	float v = 0.0, a = 0.5;
	mat2 m = rot(0.54);
	for (int i = 0; i < 8; i++) {
		v += a * noise(p);
		p = m * p * 2.02;
		a *= 0.5;
	}
	return v;
}

// Сферическая проекция микрорельефа (Bump Mapping без растяжений)
vec3 microSphereNormal(vec3 p3d, vec3 baseN, float px) {
	vec2 sUv = p3d.xy / (abs(p3d.z) + 0.85) * 9.5 + vec2(u_time * 0.014, -u_time * 0.009);
	float eps = 0.004;
	float h0 = fbm8(sUv) * 0.65 + fbm8(sUv * 3.8) * 0.35;
	float hx = fbm8(sUv + vec2(eps, 0.0)) * 0.65 + fbm8((sUv + vec2(eps, 0.0)) * 3.8) * 0.35;
	float hy = fbm8(sUv + vec2(0.0, eps)) * 0.65 + fbm8((sUv + vec2(0.0, eps)) * 3.8) * 0.35;
	vec3 grad = vec3((h0 - hx) / eps, (h0 - hy) / eps, 0.0) * 0.016;
	return normalize(baseN + grad);
}

// Чистое тонкое кольцо с точечными микро-импульсами без шлейфа
vec4 renderPureRing(vec2 el, float ang, float t, float spd, float px, float wBase, bool front) {
	float rEl = length(el);
	float d = abs(rEl - 1.0), aa = px * 1.4;
	float mask = smoothstep(wBase + aa, wBase - aa, d);
	if (mask <= 0.001) return vec4(0.0);

	// Резкие точечные узлы на кольце (без длинного шлейфа)
	float node1 = exp(-pow(sin((ang - t * spd) * 1.0), 2.0) * 420.0);
	float node2 = exp(-pow(cos((ang - t * spd) * 1.0), 2.0) * 520.0);
	float microTick = step(0.86, fract(ang * 24.0 - t * spd * 2.0)) * 0.35;
	float edgeLine = exp(-d / (px * 1.2));

	float lum = 0.06 + edgeLine * (0.28 + 0.85 * max(node1, node2)) + microTick * 0.25;
	if (!front) lum *= 0.42;
	return vec4(vec3(lum), clamp(mask * (0.55 + 0.45 * max(node1, node2)), 0.0, 0.96));
}

void main() {
	float px = 1.0 / min(u_res.x, u_res.y);
	vec2 rawUv = (gl_FragCoord.xy - 0.5 * u_res.xy) * px;
	vec2 uv = rawUv * (u_zoom * (1.0 + 0.045 * (u_zoom - 1.0) * dot(rawUv, rawUv)));
	vec2 m = (u_mouse - 0.5) * 0.48;
	uv += m * 0.22;

	float scroll = u_scroll * 0.00022;
	vec2 center = vec2(0.0, 0.06 - scroll * 0.26);
	vec3 lightDir = normalize(vec3(0.74 + m.x * 0.24, 0.46 + m.y * 0.24, 0.48));
	vec3 viewDir = vec3(0.0, 0.0, 1.0);

	// Глубокий чёрно-белый космос с мягкой перспективной глубиной
	float bgDepth = smoothstep(1.45, 0.10, length(uv - center));
	vec3 col = vec3(0.002 + 0.014 * bgDepth);
	col += vec3(0.06) * exp(-length(uv - vec2(0.88, 0.60)) * 2.1);

	// 5 слоёв сверхмелких точечных звёзд и микрочастиц вокруг шара
	for (int i = 0; i < 5; i++) {
		float fl = float(i);
		vec2 rel = (uv - center) * rot(u_time * (0.028 - fl * 0.004) * (mod(fl, 2.0) == 0.0 ? 1.0 : -1.0) + fl * 1.25) + m * (0.085 * (fl + 1.0));
		vec2 st = rel * (34.0 + fl * 26.0) + vec2(fl * 23.7, fl * 13.1);
		vec2 id = floor(st), gv = fract(st) - 0.5;
		float h = hash(id);
		if (h > 0.915) {
			vec2 j = vec2(sin(u_time * (0.55 + fract(h * 17.0)) + h * 45.0), cos(u_time * (0.48 + fract(h * 31.0)) + h * 29.0)) * 0.26;
			float d = length(gv - j);
			float tw = 0.55 + 0.45 * sin(u_time * 3.4 + h * 80.0);
			float dotSize = mix(0.032, 0.068, fract(h * 11.0));
			float pt = smoothstep(dotSize, dotSize * 0.15, d) * tw;
			col += vec3(pt * (0.28 + 0.72 * (fl / 4.0)));
		}
	}

	vec2 p = uv - center;
	float R = 0.46;
	float r = length(p);

	// Тонкая белая атмосферная корона по краю шара
	float rimCorona = exp(-max(0.0, r - R) * 22.0) * smoothstep(-0.2, 0.6, dot(normalize(vec3(p, 0.1)), lightDir));
	col += vec3(rimCorona * 0.42);

	// Два тонких чистых 3D-кольца вокруг шара
	float tA = -0.22 + sin(u_time * 0.28) * 0.07 + m.x * 0.20;
	vec2 pA = p * rot(tA);
	vec2 elA = vec2(pA.x / 0.88, pA.y / (0.16 + cos(u_time * 0.34) * 0.022 + m.y * 0.06));
	float angA = atan(elA.y, elA.x);
	bool frontA = elA.y <= 0.15;
	vec4 rA = renderPureRing(elA, angA, u_time, 0.85, px * 3.0, 0.013, frontA);

	float tB = 0.48 + cos(u_time * 0.24) * 0.08 - m.y * 0.20;
	vec2 pB = p * rot(tB);
	vec2 elB = vec2(pB.x / (0.24 + sin(u_time * 0.30) * 0.030 + m.x * 0.06), pB.y / 0.74);
	float angB = atan(elB.y, elB.x);
	bool frontB = elB.x >= -0.15;
	vec4 rB = renderPureRing(elB, angB, u_time, -1.05, px * 2.6, 0.015, frontB);

	if (!frontA && rA.a > 0.0) col = mix(col, rA.rgb, rA.a);
	if (!frontB && rB.a > 0.0) col = mix(col, rB.rgb, rB.a);

	// ИДЕАЛЬНО ГЛАДКИЙ ЧИСТЫЙ ЧЁРНЫЙ ШАР С МИКРОРЕЛЬЕФОМ И СВЕТОТЕНЬЮ
	float sMask = smoothstep(R + px * 1.2, R - px * 1.2, r);
	if (sMask > 0.0) {
		float z = sqrt(max(0.0, R * R - min(r, R) * min(r, R)));
		vec3 baseN = normalize(vec3(p, z));
		vec3 n = microSphereNormal(baseN, baseN, px);

		float ndotl = max(0.0, dot(n, lightDir));
		float terminator = smoothstep(-0.03, 0.35, dot(baseN, lightDir));
		vec3 halfVec = normalize(lightDir + viewDir);
		float ndoth = max(0.0, dot(n, halfVec));

		float specBroad = pow(ndoth, 24.0) * 0.35;
		float specSharp = pow(ndoth, 140.0) * 1.25;
		float fresnel = pow(1.0 - max(0.0, baseN.z), 4.8) * smoothstep(-0.15, 0.50, dot(baseN, lightDir));

		float shade = 0.001 + terminator * (ndotl * 0.14 + specBroad + specSharp) + fresnel * 1.15;
		vec3 sphereCol = vec3(shade);

		vec2 cP = p - vec2(0.0, -0.015);
		float cD = length(cP);
		float coreDot = smoothstep(0.026, 0.004, cD);
		float coreHalo = exp(-cD * 34.0) * 0.45;
		float coreRing = exp(-abs(cD - 0.038) / (px * 1.4)) * 0.65;
		sphereCol += vec3(coreDot * 1.35 + coreHalo + coreRing);

		float shA = frontA ? smoothstep(0.045, 0.003, abs(length(elA) - 1.0)) * 0.90 : 0.0;
		float shB = frontB ? smoothstep(0.052, 0.003, abs(length(elB) - 1.0)) * 0.90 : 0.0;
		sphereCol *= 1.0 - max(shA, shB);

		col = mix(col, sphereCol, sMask);
	}


	if (frontA && rA.a > 0.0) col = mix(col, rA.rgb, rA.a);
	if (frontB && rB.a > 0.0) col = mix(col, rB.rgb, rB.a);

	// Перспективный чёрно-белый горизонт
	float hY = -0.46 - scroll * 0.14 + m.y * 0.04;
	float dH = abs(uv.y - hY);
	float floorPersp = smoothstep(hY, -0.95, uv.y);
	col = mix(col, vec3(0.001 + 0.016 * floorPersp), smoothstep(hY + px, hY - px * 1.5, uv.y) * 0.97);
	col += vec3(exp(-dH / (px * 1.5)) * exp(-abs(uv.x) * 1.35) * 0.55);

	// Строгий монохромный выход (без каких-либо цветных оттенков)
	float mono = dot(col, vec3(0.3333)) * smoothstep(1.85, 0.22, length(rawUv));
	mono = pow(max(mono, 0.0), 0.94);
	fragColor = vec4(vec3(mono), 1.0);
}
)GLSL";
	}
}

