#!/usr/bin/env python3
"""Animate the output of naiveHyperbolic_01.c.

Input format: one row per time step t_j = j*k, one column per node x_i.
"""
import argparse
import sys

import matplotlib
matplotlib.use("Agg")  # no display needed
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation, PillowWriter

p = argparse.ArgumentParser()
p.add_argument("input", nargs="?", default="-", help="output file, or - for stdin")
p.add_argument("-o", "--output", default="wave.gif")
p.add_argument("--k", type=float, required=True, help="time step used in the C program")
p.add_argument("--fps", type=int, default=20)
p.add_argument("--exact", action="store_true",
               help="overlay sin(pi x) cos(pi t) (only valid for this test problem)")
args = p.parse_args()

src = sys.stdin if args.input == "-" else open(args.input)
U = np.loadtxt(src, comments="#")      # shape (n_t, n_x)
n_t, n_x = U.shape
x = np.linspace(0.0, 1.0, n_x)
t = np.arange(n_t) * args.k

fig, ax = plt.subplots(figsize=(7, 4))
pad = 0.1 * max(np.abs(U).max(), 1e-12)
ax.set_xlim(0, 1)
ax.set_ylim(U.min() - pad, U.max() + pad)
ax.set_xlabel("x")
ax.set_ylabel("u(x, t)")
ax.grid(alpha=0.3)

(line,) = ax.plot(x, U[0], "o-", ms=3, label="solução numérica")
if args.exact:
    (ex_line,) = ax.plot(x, np.sin(np.pi * x) * np.cos(np.pi * t[0]),
                         "--", color="red", label="solução exata")
ax.legend(loc="upper right")
title = ax.set_title("")

def update(j):
    line.set_ydata(U[j])
    if args.exact:
        ex_line.set_ydata(np.sin(np.pi * x) * np.cos(np.pi * t[j]))
    title.set_text(f"t = {t[j]:.3f}")
    return (line, title)

anim = FuncAnimation(fig, update, frames=n_t, blit=False)
anim.save(args.output, writer=PillowWriter(fps=args.fps))
print(f"saved {args.output} ({n_t} frames)")
