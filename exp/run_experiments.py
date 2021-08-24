import os
import sys
import subprocess
import profile_parsing as pp
import pandas as pd

experiments = {
    'mod': ('python', 'cython', 'nuitka', 'numba'),
    'avg_word_len': ('python', 'cython', 'nuitka', 'numba'),
    'roberta': ('python', 'cython', 'nuitka'),
}

build_dir = os.environ['BUILD_DIR']

def run_experiment(name: str, runs: int):
    exp_results = []
    variants = experiments.get(exp)
    for var in variants:
        for run in range(runs):
            p = subprocess.run([build_dir + f"/exp/{exp}_{var}_exp"], stdout=subprocess.PIPE)
            times = pp.extract_times(p)
            assert(times)
            result = [name, var] + times
            exp_results.append(result)
    return exp_results

def plot_results(results):
    from matplotlib import pyplot as plt

    df = pd.DataFrame(results, columns = ['Experiment', 'Variant', 'Build', 'Execute'])
    df.to_csv("/tmp/results.csv")
    df = df.groupby(['Experiment', 'Variant'], sort=False)[['Build', 'Execute']].median()
    df = df.unstack(level=-1) # unstack the 'Context' column
    df.Build = df.Build.div(df.Execute.python, axis = 0)
    df.Execute = df.Execute.div(df.Execute.python, axis = 0)
    print(df)
    
    nuitka_color = plt.cm.get_cmap('RdPu')
    nuitka_vec_color = plt.cm.get_cmap('Purples')
    numba_color = plt.cm.get_cmap('Greys')
    cython_color = plt.cm.get_cmap('Greens')
    cython_vec_color = plt.cm.get_cmap('Blues')
    python_color = plt.cm.get_cmap('Oranges')
    python_vec_color = plt.cm.get_cmap('Reds')

    # color_build = [ python_color(0.3), python_vec_color(0.3), cython_color(0.3), cython_vec_color(0.3), nuitka_color(0.3), nuitka_vec_color(0.3), numba_color(0.3)]
    # color_execute = [python_color(0.6), python_vec_color(0.6), cython_color(0.6), cython_vec_color(0.6), nuitka_color(0.6), nuitka_vec_color(0.6), numba_color(0.6)]

    color_build = [ python_color(0.3), cython_color(0.3), nuitka_color(0.3), numba_color(0.3)]
    color_execute = [python_color(0.6), cython_color(0.6), nuitka_color(0.6), numba_color(0.6)]

    fig, ax = plt.subplots()
    (df['Execute']+df['Build']).plot(kind='bar', color=color_build, rot=0, ax=ax)
    (df['Execute']).plot(kind='bar', color=color_execute, rot=0, ax=ax)
    fig.set_figwidth(16)
    fig.set_figheight(9)
    legend_labels = [f'{val} ({context})' for val, context in df.columns]
    ax.legend(legend_labels)
    ax.axhline(y=1, xmin=0.0, xmax=1.0, color='r')
    plt.ylabel("Runtime (normalized)")
    plt.savefig("/tmp/results.png")


if (len(sys.argv) != 2):
    print(f"Please provide number of runs")
    print(f"Usage: {sys.argv[0]} <runs>")
    exit(1)

runs = int(sys.argv[1])
results = []
toolbar_width = len(experiments)

# setup toolbar
sys.stdout.write("Progress: [%s]" % (" " * toolbar_width))
sys.stdout.flush()
sys.stdout.write("\b" * (toolbar_width+1)) # return to start of line, after '['

for exp in experiments:
    exp_results = run_experiment(exp, runs)
    results += (exp_results)
    sys.stdout.write("-")
    sys.stdout.flush()

sys.stdout.write("]\n") # this ends the progress bar
plot_results(results)
