#!/usr/bin/env python3
"""Uso: ./wave_max > max.txt && python3 plot_max.py max.txt maximo.png"""
import sys
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

M = np.loadtxt(sys.argv[1])              # M[i, j] em (x_i, y_j)
cmap = plt.get_cmap("inferno").copy()
cmap.set_bad("0.35")                      # nós fixos (bordas e espelho)
fig, ax = plt.subplots(figsize=(6.4, 5.6), dpi=150)
im = ax.imshow(np.ma.masked_equal(M, 0).T, origin="lower", extent=(0, 1, 0, 1), cmap=cmap)
ax.plot([0.62], [0.5], marker="+", color="cyan", markersize=12, markeredgewidth=1.5)
ax.set_xlabel("x"); ax.set_ylabel("y")
fig.colorbar(im, ax=ax, label="máximo de |u| no tempo")
fig.tight_layout()
fig.savefig(sys.argv[2])
