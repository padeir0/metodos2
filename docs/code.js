// code.js — realce de sintaxe (highlight.js) para <pre><code class="language-c">.
(() => {
  const CDN = "https://cdn.jsdelivr.net/gh/highlightjs/cdn-release@11.9.0/build/";
  const css = document.createElement("link");
  css.rel = "stylesheet"; css.href = CDN + "styles/xcode.min.css";
  document.head.append(css);
  const s = document.createElement("script");
  s.src = CDN + "highlight.min.js";
  s.onload = () => {
    if (document.readyState === "loading") addEventListener("DOMContentLoaded", () => hljs.highlightAll());
    else hljs.highlightAll();
  };
  document.head.append(s);
})();
