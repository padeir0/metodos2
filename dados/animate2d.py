#!/usr/bin/env python3
"""Animate the output of wave2d_02.c, wave2d_03.c and wave2d_04.c.

Input format: a header line
    # n_x = .., n_y = .., dt = .., stride = .., x = a b, y = a b
then frames of n_x lines with n_y values (u[i][j] at x_i, y_j), separated by
a blank line. Only one frame out of `stride` time steps is printed by the C
program, so the time between frames is dt * stride.

Nodes that are exactly 0 in every frame are the fixed ones (boundary and
walls) and are drawn in gray.
"""
import argparse
import re
import sys

import matplotlib
matplotlib.use("Agg")  # no display needed
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation, PillowWriter

p = argparse.ArgumentParser()
p.add_argument("input", nargs="?", default="-", help="output file, or - for stdin")
p.add_argument("-o", "--output", default="wave2d.gif")
p.add_argument("--fps", type=int, default=20)
p.add_argument("--clim", type=float, default=0.5,
               help="color scale as a fraction of max|u| (smaller shows ripples better)")
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

vmax = args.clim * np.abs(U).max()
cmap = plt.get_cmap("RdBu_r").copy()
cmap.set_bad("0.35")
fig, ax = plt.subplots(figsize=(7, 7 * (b_y - a_y) / (b_x - a_x) + 0.8))
image = ax.imshow(frame(0), origin="lower", extent=(a_x, b_x, a_y, b_y),
                  cmap=cmap, vmin=-vmax, vmax=vmax)
fig.colorbar(image, ax=ax, label="u(x, y, t)", shrink=0.8)
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
