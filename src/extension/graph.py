import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.patches import Patch

df = pd.read_csv("out.csv")  # Read from output csv file
df.columns = df.columns.str.strip()  # Remove white space
df["algorithm"] = df["algorithm"].str.strip()  # Remove white space from algorithm

# Only use valid rows
df = df[df["difficulty"].isin([1, 2, 3, 4, 5])]

# Types of algorithm
algorithms = [
    "0 - without algorithms",
    "1 - without algorithms",
    "0 - with algorithms",
    "1 - with algorithms",
]

# Sort based on difficulty
difficulties = sorted(df["difficulty"].unique())

data = []
positions = []
colors = []

cluster_gap = 5


# Grouping the boxplot data into dataframes
for i, difficulty in enumerate(difficulties):
    base = i * cluster_gap

    for j, algorithm in enumerate(algorithms):
        values = df[(df["difficulty"] == difficulty) & (df["algorithm"] == algorithm)][
            "timeElapsed"
        ]

        if len(values) > 0:
            data.append(values)
            positions.append(base + j)
            colors.append(f"C{j}")

fig, ax = plt.subplots(figsize=(14, 7))


# Create clustered boxplot
bp = ax.boxplot(
    data,
    positions=positions,
    widths=0.6,
    patch_artist=True,
    showfliers=True,
    whis=10.0,  # Outliers at (10 * IQR) + UQ
    medianprops={"color": "black"},
)

# Set box colours
for box, color in zip(bp["boxes"], colors):
    box.set_facecolor(color)

# Sets x axis ticks
ax.set_xticks([i * cluster_gap + 1.5 for i in range(len(difficulties))])
ax.set_xticklabels(difficulties)

ax.set_yscale("log")  # Logarithmic scale so data is visible

# Set labels
ax.set_xlabel("Difficulty")
ax.set_ylabel("Time elapsed (microseconds) - log scale")
ax.set_title("Sudoku solver time by difficulty")

# Boxplot descriptions
ax.legend(
    handles=[
        Patch(
            facecolor="C0",
            label="Backtracking",
        ),
        Patch(
            facecolor="C1",
            label="Backtracking and MRV heuristic",
        ),
        Patch(
            facecolor="C2",
            label="Backtracking with naked singles, hidden singles, naked subsets",
        ),
        Patch(
            facecolor="C3",
            label="Backtracking and MRV heuristic with naked singles, hidden singles, naked subsets",
        ),
    ]
)

plt.tight_layout()
plt.savefig("output.png")
