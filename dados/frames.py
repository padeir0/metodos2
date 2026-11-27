#!/usr/bin/env python3
"""Salva uma grade de quadros estaticos da saida de wave_02.c, wave_03.c,
wave_04.c e wave_05.c (mesmo formato lido pelo animate2d.py).

Formato da entrada: uma linha de cabecalho
    # n_x = .., n_y = .., dt = .., stride = .., x = a b, y = a b
seguida de quadros de n_x linhas com n_y valores (u[i][j] em x_i, y_j),
separados por uma linha em branco. O C so imprime 1 passo de tempo a cada
`stride`, entao o tempo entre quadros impressos e dt * stride.

Os nos que valem exatamente 0 em todos os quadros sao os fixos (bordas e
obstaculos) e sao desenhados em cinza.

Escolha dos quadros:
  -t T1 T2 ...   instantes desejados (usa o quadro impresso mais proximo)
  -n N           N quadros igualmente espacados (padrao 4), se -t nao for dado

Escala de cor (as mesmas opcoes do animate2d.py, valem para todos os quadros):
  a escala padrao e +-clim * max|u| sobre a animacao inteira. Quando o
  movimento interessante e bem mais fraco que isso, use --scale-x e/ou --gamma
  (ex.: parede em x = 0.5 com a fonte na esquerda: --scale-x 0.55 1 --gamma 0.5).
  Valores alem da escala saturam (a barra de cores mostra setas quando isso
  acontece).

Exemplo:
  ./run wave_02.c | python3 ../dados/frames.py -o gota.png -t 0 0.3 0.6 1.0
"""
import argparse
import re
import sys

import matplotlib
matplotlib.use("Agg")  # nao precisa de display
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import FuncNorm, Normalize

p = argparse.ArgumentParser()
p.add_argument("input", nargs="?", default="-", help="arquivo de saida do C, ou - para stdin")
p.add_argument("-o", "--output", default="frames.png")
p.add_argument("-t", "--times", type=float, nargs="+", default=None,
               help="instantes a mostrar (quadro impresso mais proximo de cada um)")
p.add_argument("-n", "--num", type=int, default=4,
               help="numero de quadros igualmente espacados, se -t nao for dado (padrao 4)")
p.add_argument("--cols", type=int, default=None,
               help="colunas da grade (padrao: 2 se houver 4 quadros ou mais, senao o numero de quadros)")
p.add_argument("--dpi", type=int, default=150)
p.add_argument("--clim", type=float, default=0.5,
               help="escala de cor como fracao da amplitude de referencia")
p.add_argument("--scale-x", type=float, nargs=2, metavar=("A", "B"), default=None,
               help="amplitude de referencia so dos nos com A <= x <= B")
p.add_argument("--percentile", type=float, default=100.0,
               help="amplitude de referencia = este percentil de |u| na regiao (padrao 100 = o maximo)")
p.add_argument("--gamma", type=float, default=1.0,
               help="potencia com sinal sign(u)|u|^gamma nas cores; gamma < 1 realca ondas fracas")
args = p.parse_args()

text = sys.stdin.read() if args.input == "-" else open(args.input).read()
num = r"([\d.eE+-]+)"
header = re.search(
    rf"n_x = (\d+), n_y = (\d+), dt = {num}, stride = (\d+), "
    rf"x = {num} {num}, y = {num} {num}", text)
if header is None:
    sys.exit("cabecalho '# n_x = .., n_y = .., dt = .., stride = .., x = a b, y = a b' nao encontrado")
n_x, n_y = int(header.group(1)), int(header.group(2))
dt, stride = float(header.group(3)), int(header.group(4))
a_x, b_x, a_y, b_y = (float(header.group(k)) for k in (5, 6, 7, 8))

U = np.loadtxt(text.splitlines(), comments="#").reshape(-1, n_x, n_y)
n_frames = U.shape[0]
t = np.arange(n_frames) * dt * stride
fixed = np.abs(U).max(axis=0) == 0

if args.times is None:
    chosen = np.unique(np.round(np.linspace(0, n_frames - 1, args.num)).astype(int))
else:
    chosen = np.array([int(np.argmin(np.abs(t - tt))) for tt in args.times])
print("quadros escolhidos (t): " + ", ".join(f"{t[j]:.3f}" for j in chosen))
if args.times is not None and max(args.times) > t[-1] + dt * stride:
    print(f"aviso: ultimo quadro impresso em t = {t[-1]:.3f}", file=sys.stderr)

# escala de cor: igual a do animate2d.py
x = np.linspace(a_x, b_x, n_x)
if args.scale_x is None:
    in_region = np.ones(n_x, dtype=bool)
else:
    in_region = (x >= min(args.scale_x)) & (x <= max(args.scale_x))
region = np.abs(U[:, in_region, :][:, ~fixed[in_region]])
if region.size == 0:
    sys.exit("--scale-x nao seleciona nenhum no livre")
reference = np.percentile(region, args.percentile)
if reference == 0:
    sys.exit("o campo e identicamente 0 na regiao da escala")
vmax = args.clim * reference
print(f"escala de cor: +-{vmax:.4g} (referencia {reference:.4g}, max|u| = {np.abs(U).max():.4g})")

if args.gamma == 1.0:
    norm = Normalize(vmin=-vmax, vmax=vmax)
else:
    g = args.gamma
    norm = FuncNorm((lambda v: np.sign(v) * np.abs(v) ** g,
                     lambda v: np.sign(v) * np.abs(v) ** (1 / g)),
                    vmin=-vmax, vmax=vmax)
cmap = plt.get_cmap("RdBu_r").copy()
cmap.set_bad("0.35")

n = len(chosen)
cols = args.cols if args.cols else (n if n < 4 else 2)
rows = -(-n // cols)
aspect = (b_y - a_y) / (b_x - a_x)
panel = 3.3
fig, axes = plt.subplots(rows, cols, squeeze=False, layout="constrained",
                         figsize=(panel * cols + 1.2, panel * aspect * rows + 0.5 * rows + 0.3))
image = None
for k, ax in enumerate(axes.ravel()):
    if k >= n:
        ax.axis("off")
        continue
    j = chosen[k]
    # linhas de U percorrem x, colunas percorrem y: transpoe para x ficar na horizontal
    image = ax.imshow(np.ma.masked_where(fixed, U[j]).T, origin="lower",
                      extent=(a_x, b_x, a_y, b_y), cmap=cmap, norm=norm)
    ax.set_title(f"t = {t[j]:.2f}", fontsize=10)
    ax.set_xlabel("x" if k // cols == rows - 1 or k + cols >= n else "")
    ax.set_ylabel("y" if k % cols == 0 else "")
    if k % cols != 0:
        ax.set_yticklabels([])
fig.colorbar(image, ax=axes.ravel().tolist(), label="u(x, y, t)", shrink=0.8,
             extend="both" if vmax < np.abs(U).max() else "neither")
fig.savefig(args.output, dpi=args.dpi)
print(f"salvo {args.output} ({n} quadros)")
