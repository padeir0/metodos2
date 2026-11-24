#!/usr/bin/env python3
"""Animate the output of wave2d_02.c, wave2d_03.c and wave2d_04.c.

Input format: a header line
    # n_x = .., n_y = .., dt = .., stride = .., x = a b, y = a b
then frames of n_x lines with n_y values (u[i][j] at x_i, y_j), separated by
a blank line. Only one frame out of `stride` time steps is printed by the C
program, so the time between frames is dt * stride.

Nodes that are exactly 0 in every frame are the fixed ones (boundary and
walls) and are drawn in gray.

Color scale: by default it is +-clim * max|u| over the whole animation, which
is dominated by whatever region/frame has the largest amplitude (typically the
initial bump, or the side of a wall where the source is). When the interesting
motion is much weaker than that, narrow the reference and/or compress the
colors, e.g. for a wall at x = 0.5 with the source on the left:
    --scale-x 0.55 1 --gamma 0.5
Values beyond the scale saturate (the colorbar shows arrows when that happens).
"""
import argparse
import re
import sys

import matplotlib
matplotlib.use("Agg")  # no display needed
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation, PillowWriter
from matplotlib.colors import FuncNorm, Normalize

p = argparse.ArgumentParser()
p.add_argument("input", nargs="?", default="-", help="output file, or - for stdin")
p.add_argument("-o", "--output", default="wave2d.gif")
p.add_argument("--fps", type=int, default=20)
p.add_argument("--clim", type=float, default=0.5,
               help="color scale as a fraction of the reference amplitude "
                    "(smaller shows ripples better)")
p.add_argument("--scale-x", type=float, nargs=2, metavar=("A", "B"), default=None,
               help="take the reference amplitude only from nodes with A <= x <= B "
                    "(e.g. the weak side of a wall); stronger values elsewhere saturate")
p.add_argument("--percentile", type=float, default=100.0,
               help="reference amplitude = this percentile of |u| in the scale region "
                    "(default 100 = the max; ~99.5 ignores isolated spikes)")
p.add_argument("--gamma", type=float, default=1.0,
               help="signed power law sign(u)|u|^gamma on the colors; gamma < 1 boosts "
                    "weak ripples (default 1 = linear)")
args = p.parse_args()

text = sys.stdin.read() if args.input == "-" else open(args.input).read()
num = r"([\d.eE+-]+)"
header = re.search(
    rf"n_x = (\d+), n_y = (\d+), dt = {num}, stride = (\d+), "
    rf"x = {num} {num}, y = {num} {num}", text)
if header is None:
    sys.exit("header '# n_x = .., n_y = .., dt = .., stride = .., x = a b, y = a b' not found")
n_x, n_y = int(header.group(1)), int(header.group(2))
dt, stride = float(header.group(3)), int(header.group(4))
a_x, b_x, a_y, b_y = (float(header.group(k)) for k in (5, 6, 7, 8))

U = np.loadtxt(text.splitlines(), comments="#").reshape(-1, n_x, n_y)
n_frames = U.shape[0]
t = np.arange(n_frames) * dt * stride
fixed = np.abs(U).max(axis=0) == 0

def frame(j):
    # rows of U run along x, columns along y: transpose so x is horizontal
    return np.ma.masked_where(fixed, U[j]).T

x = np.linspace(a_x, b_x, n_x)
if args.scale_x is None:
    in_region = np.ones(n_x, dtype=bool)
else:
    in_region = (x >= min(args.scale_x)) & (x <= max(args.scale_x))
# free nodes inside the scale region, all frames: shape (n_frames, n_nodes)
region = np.abs(U[:, in_region, :][:, ~fixed[in_region]])
if region.size == 0:
    sys.exit("--scale-x selects no free nodes")
reference = np.percentile(region, args.percentile)
if reference == 0:
    sys.exit("the field is identically 0 in the scale region")
vmax = args.clim * reference
print(f"color scale: +-{vmax:.4g} (reference {reference:.4g}, max|u| = {np.abs(U).max():.4g})")

if args.gamma == 1.0:
    norm = Normalize(vmin=-vmax, vmax=vmax)
else:
    g = args.gamma
    norm = FuncNorm((lambda v: np.sign(v) * np.abs(v) ** g,
                     lambda v: np.sign(v) * np.abs(v) ** (1 / g)),
                    vmin=-vmax, vmax=vmax)
cmap = plt.get_cmap("RdBu_r").copy()
cmap.set_bad("0.35")
fig, ax = plt.subplots(figsize=(7, 7 * (b_y - a_y) / (b_x - a_x) + 0.8))
image = ax.imshow(frame(0), origin="lower", extent=(a_x, b_x, a_y, b_y),
                  cmap=cmap, norm=norm)
fig.colorbar(image, ax=ax, label="u(x, y, t)", shrink=0.8,
             extend="both" if vmax < np.abs(U).max() else "neither")
ax.set_xlabel("x")
ax.set_ylabel("y")
title = ax.set_title("")

def update(j):
    image.set_data(frame(j))
    title.set_text(f"t = {t[j]:.3f}")
    return (image, title)

anim = FuncAnimation(fig, update, frames=n_frames, blit=False)
anim.save(args.output, writer=PillowWriter(fps=args.fps))
print(f"saved {args.output} ({n_frames} frames)")
