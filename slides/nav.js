// nav.js — compartilhado por todos os slides.
const SLIDES = [               // ÚNICO lugar a editar para inserir/remover/mover slides
  ["01-titulo", "Título"],
  ["02-sumario", "Sumário"],
  ["03-edp", "A EDP da onda 2D"],
  ["04-discretizacao", "Discretização da EDP"],
  ["04b-stencil", "Stencil"],
  ["05-api", "API de de_solveWave"],
  ["06-corpo-variaveis", "Corpo: variáveis alocadas"],
  ["07-corpo-loop", "Corpo: loop principal"],
  ["07b-corpo-retorno", "Corpo: avanço e retorno"],
  ["08-gota-codigo", "Gota: argumentos"],
  ["09-gota-gif", "Gota: animação"],
  ["10-fenda-codigo", "Fenda dupla: argumentos"],
  ["11-fenda-gif", "Fenda dupla: animação"],
  ["12-refletor-codigo", "Refletor: argumentos"],
  ["13-refletor-gif", "Refletor: animação"],
  ["14-refletor-maximo", "Refletor: máximo em cada nó"],
  ["15-outras-formas", "Outras formas da EDP"],
  ["16-conclusao", "Conclusão"],
  ["17-referencias", "Referências"],
];
const TEX_MACROS = {};         // ex.: {R: "\\mathbb{R}", E: "\\operatorname{E}"}
const MATHJAX_URL = "https://cdn.jsdelivr.net/npm/mathjax@3.2.2/es5/tex-svg.js";

const cssVar = n => getComputedStyle(document.documentElement).getPropertyValue(n).trim();
const vals = P => Object.fromEntries(Object.entries(P).map(([k, o]) => [k, o.value]));
function loadParams(P) {       // aplica ?p={"chave":valor} (enviado por lab.html)
  try {
    const o = JSON.parse(new URLSearchParams(location.search).get("p") || "{}");
    for (const k in o) if (P[k]) P[k].value = o[k];
  } catch (e) {}
  return P;
}

(() => {
  if (window.NO_NAV) return;
  const idx = SLIDES.findIndex(([d]) => location.pathname.includes("/" + d + "/"));
  const base = idx >= 0 ? "../" : "./";
  const inLab = new URLSearchParams(location.search).has("p");
  const go = i => SLIDES[i] && location.assign(base + SLIDES[i][0] + "/index.html");
  const help = () => document.getElementById("help-overlay").classList.toggle("open");

  addEventListener("keydown", e => {
    if (!inLab && e.key === "ArrowRight") go(idx + 1);
    if (!inLab && e.key === "ArrowLeft") go(idx - 1);
    if (e.key.toLowerCase() === "h") help();
    if (e.key === "Escape") document.getElementById("help-overlay").classList.remove("open");
  });

  addEventListener("DOMContentLoaded", () => {
    if (typeof PARAMS !== "undefined")
      document.querySelectorAll("[data-param]").forEach(el => {
        const o = PARAMS[el.dataset.param]; if (o) el.textContent = o.value;
      });
    document.querySelectorAll("[data-toc]").forEach(el => el.innerHTML =
      SLIDES.map(([d, t], i) => `<a href="${base}${d}/index.html">${i + 1}. ${t}</a>`).join("<br>"));
    const extra = (window.HELP_EXTRA || []).map(([k, t]) => `<li><b>${k}</b> — ${t}</li>`).join("");
    document.body.insertAdjacentHTML("beforeend", `<div id="help-overlay"><div class="help-box">
      <div class="help-header"><span>Teclas</span><button id="help-close">×</button></div>
      <ul><li><b>h</b> — mostra/esconde esta ajuda</li><li><b>→</b> — próximo slide</li>
      <li><b>←</b> — slide anterior</li>${extra}</ul></div></div>`);
    document.getElementById("help-close").onclick = help;
    if (/\$|\\\(|\\\[/.test(document.body.textContent)) {   // só carrega MathJax se houver LaTeX
      window.MathJax = { tex: { inlineMath: [["$", "$"], ["\\(", "\\)"]], macros: TEX_MACROS },
                         svg: { fontCache: "global" } };
      const s = document.createElement("script"); s.src = MATHJAX_URL; s.async = true;
      document.head.append(s);
    }
  });
})();
