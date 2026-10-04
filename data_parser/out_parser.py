#!/usr/bin/env python3
"""
Procesa un log de ejecuciones y calcula la media de las métricas para cada
combinación de (compiler, flags, N, p). Genera:
  - 1 tabla de errores (Max juntos, luego RMS juntos)
  - 1 tabla de tiempos por cada combinación de parámetros (N, p), con las
    filas de los distintos compiladores contiguas para cada flag.

Uso:
    python procesar_log.py log.txt
    python procesar_log.py log.txt -o resultados   # guarda resultados_errores.csv
                                                   # y resultados_tiempos.csv
    cat log.txt | python procesar_log.py -
"""
import argparse
import math
import re
import sys

import pandas as pd

RE_HEADER = re.compile(
    r'Compiler:\s*(\S+)\s+Flags:\s*"([^"]*)"\s+Parameters:\s*(\d+)\s+(\d+)'
)
RE_CHECK = re.compile(r'Check\s+(gaussian_elimination|gaussian_jordan)_solve')
RE_ERR = re.compile(r'(Max|RMS)\s*\|beta - beta_true\|:\s*([-+0-9.eE]+)')
RE_TIME = re.compile(r'Time taken by\s+(\w+):\s*([-+0-9.eE]+)\s*s')

TIMES_WANTED = {
    "compute_XtX": "t_XtX",
    "compute_Xty": "t_Xty",
    "gaussian_elimination": "t_GE",
    "gaussian_jordan": "t_GJ",
}

ERR_COLS = {
    "gaussian_elimination Max": "GE_Max",
    "gaussian_jordan Max": "GJ_Max",
    "gaussian_elimination RMS": "GE_RMS",
    "gaussian_jordan RMS": "GJ_RMS",
}
TIME_COLS = ["t_XtX", "t_Xty", "t_GE", "t_GJ"]
KEYS = ["compiler", "flags", "N", "p"]


# ----------------------------------------------------------------- parseo
def parse_log(lines):
    """Devuelve una lista de dicts, uno por ejecución."""
    runs, current, section = [], None, None
    for line in lines:
        m = RE_HEADER.search(line)
        if m:
            current = {"compiler": m.group(1), "flags": m.group(2),
                       "N": int(m.group(3)), "p": int(m.group(4))}
            runs.append(current)
            section = None
            continue
        if current is None:
            continue
        m = RE_CHECK.search(line)
        if m:
            section = m.group(1)
            continue
        m = RE_ERR.search(line)
        if m and section:
            current[f"{section} {m.group(1)}"] = float(m.group(2))
            continue
        m = RE_TIME.search(line)
        if m and m.group(1) in TIMES_WANTED:
            current[TIMES_WANTED[m.group(1)]] = float(m.group(2))
    return runs


def average(runs):
    df = pd.DataFrame(runs)
    for c in list(ERR_COLS) + TIME_COLS:
        if c not in df.columns:
            df[c] = float("nan")
    g = df.groupby(KEYS, sort=False)
    out = g[list(ERR_COLS) + TIME_COLS].mean().rename(columns=ERR_COLS)
    out.insert(0, "n_runs", g.size())
    return out.reset_index()


# --------------------------------------------------------------- impresión
def print_table(headers, rows, double_before=(), title=None):
    """
    headers: lista de cabeceras. rows: lista de listas de str (o None para
    una línea separadora). double_before: índices de columna antes de los que
    se dibuja un separador doble '‖'.
    """
    data_rows = [r for r in rows if r is not None]
    widths = [max(len(str(h)), *(len(r[i]) for r in data_rows))
              for i, h in enumerate(headers)]

    def fmt(cells):
        parts = []
        for i, c in enumerate(cells):
            sep = "" if i == 0 else (" ‖ " if i in double_before else " | ")
            parts.append(sep + (str(c).ljust(widths[i]) if i < 2
                                else str(c).rjust(widths[i])))
        return "".join(parts)

    header = fmt(headers)
    line = "-" * len(header)
    if title:
        print(f"\n{title}")
    print(line)
    print(header)
    print(line)
    for r in rows:
        print(line if r is None else fmt(r))
    print(line)


# ------------------------------------------------------------ tabla errores
def error_table(avg):
    cols = ["GE_Max", "GJ_Max", "GE_RMS", "GJ_RMS"]
    # Orden de aparición: parámetros, flags, compiler
    df = avg.sort_values(["N", "p"], kind="stable")
    df = df.copy()
    df["diff"] = [method_diff(r.GE_Max, r.GJ_Max, r.GE_RMS, r.GJ_RMS)
                  for r in df.itertuples()]
    headers = ["Compiler", "Flags", "N", "p"] + cols + ["Diff GE/GJ"]
    rows = [[r.compiler, r.flags, str(r.N), str(r.p)] +
            [f"{getattr(r, c):.6f}" for c in cols] + [r.diff]
            for r in df.itertuples()]
    print_table(headers, rows, double_before={6, 8},
                title="=== ERRORES (media) — Max juntos | RMS juntos | "
                      "¿difieren los métodos? "
                      "(GE = gauss elimination, GJ = gauss-jordan) ===")
    return df[KEYS + cols + ["diff"]]


def method_diff(ge_max, gj_max, ge_rms, gj_rms):
    """'No' si ambos métodos dan el mismo error; si no, indica en qué."""
    diffs = []
    if not math.isclose(ge_max, gj_max, rel_tol=1e-9, abs_tol=1e-12):
        diffs.append("Max")
    if not math.isclose(ge_rms, gj_rms, rel_tol=1e-9, abs_tol=1e-12):
        diffs.append("RMS")
    return "Sí (" + ", ".join(diffs) + ")" if diffs else "No"


# ------------------------------------------------------------ tabla tiempos
def cell(t, total):
    pct = f"{100 * t / total:5.1f}%" if total > 0 else "  n/a "
    return f"{t:.2f} ({pct})"


def speedup(ref_total, total):
    """Speed-up respecto a gcc: t_gcc / t_compilador (None si no se puede)."""
    if ref_total is None or total <= 0:
        return None
    return ref_total / total


def fmt_sp(sp):
    return "x?" if sp is None else f"x{sp:.2f}"


def time_tables(avg):
    # Orden de aparición de cada parámetro / flag / compiler
    param_order = list(dict.fromkeys(zip(avg["N"], avg["p"])))
    flags_order = list(dict.fromkeys(avg["flags"]))
    comp_order = list(dict.fromkeys(avg["compiler"]))

    headers = ["Compiler", "Flags",
               "XtX", "Xty", "GE", "Total GE",
               "XtX ", "Xty ", "GJ", "Total GJ"]
    all_rows = []

    for N, p in param_order:
        sub = avg[(avg["N"] == N) & (avg["p"] == p)]
        rows = []
        for fl in flags_order:
            block = sub[sub["flags"] == fl]
            if block.empty:
                continue
            if rows:
                rows.append(None)  # línea entre grupos de flags
            # Totales de gcc en este bloque (referencia del speed-up)
            ref = block[block["compiler"].str.lower() == "gcc"]
            if ref.empty:
                ref_ge = ref_gj = None
            else:
                g = ref.iloc[0]
                ref_ge = g.t_XtX + g.t_Xty + g.t_GE
                ref_gj = g.t_XtX + g.t_Xty + g.t_GJ
            for comp in comp_order:
                for r in block[block["compiler"] == comp].itertuples():
                    tot_ge = r.t_XtX + r.t_Xty + r.t_GE
                    tot_gj = r.t_XtX + r.t_Xty + r.t_GJ
                    sp_ge = speedup(ref_ge, tot_ge)
                    sp_gj = speedup(ref_gj, tot_gj)
                    rows.append([
                        r.compiler, r.flags,
                        cell(r.t_XtX, tot_ge), cell(r.t_Xty, tot_ge),
                        cell(r.t_GE, tot_ge), f"{tot_ge:.2f} s ({fmt_sp(sp_ge)})",
                        cell(r.t_XtX, tot_gj), cell(r.t_Xty, tot_gj),
                        cell(r.t_GJ, tot_gj), f"{tot_gj:.2f} s ({fmt_sp(sp_gj)})",
                    ])
                    all_rows.append({
                        "N": N, "p": p, "compiler": r.compiler,
                        "flags": r.flags, "t_XtX": r.t_XtX,
                        "t_Xty": r.t_Xty, "t_GE": r.t_GE, "t_GJ": r.t_GJ,
                        "total_GE": tot_ge, "total_GJ": tot_gj,
                        "speedup_GE_vs_gcc": sp_ge,
                        "speedup_GJ_vs_gcc": sp_gj,
                    })
        print_table(headers, rows, double_before={6},
                    title=f"=== TIEMPOS (media) — N={N}, p={p} "
                          f"(tiempo, % del total de cada solver y speed-up vs gcc) ===")
    return pd.DataFrame(all_rows)


# -------------------------------------------------------------------- main
def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("logfile", help="Ruta del log ('-' para stdin)")
    ap.add_argument("-o", "--output",
                    help="Prefijo para guardar CSVs (<prefijo>_errores.csv, "
                         "<prefijo>_tiempos.csv)")
    args = ap.parse_args()

    if args.logfile == "-":
        lines = sys.stdin.read().splitlines()
    else:
        with open(args.logfile, encoding="utf-8", errors="replace") as f:
            lines = f.read().splitlines()

    runs = parse_log(lines)
    if not runs:
        sys.exit("No se encontraron ejecuciones en el log.")

    avg = average(runs)
    n = avg["n_runs"]
    if n.nunique() > 1:
        print("Aviso: no todas las combinaciones tienen el mismo nº de "
              "ejecuciones:")
        print(avg[KEYS + ["n_runs"]].to_string(index=False))

    errs = error_table(avg)
    times = time_tables(avg)

    if args.output:
        errs.to_csv(f"{args.output}_errores.csv", index=False)
        times.to_csv(f"{args.output}_tiempos.csv", index=False)
        print(f"\nGuardado: {args.output}_errores.csv y "
              f"{args.output}_tiempos.csv")


if __name__ == "__main__":
    main()