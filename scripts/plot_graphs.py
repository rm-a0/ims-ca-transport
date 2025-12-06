#!/usr/bin/env python3
"""
Traffic Simulation Data Analysis and Visualization
Authors: Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
"""
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import matplotlib as mpl
from pathlib import Path
import sys

# Hypothesis thresholds
H1_THROUGHPUT_REQUIRED_INCREASE = 20.0   # percent
H2_QUEUE_REQUIRED_REDUCTION = 20.0       # percent
H3_SPEED_REQUIRED_INCREASE = 0.0         # percent

# Default file names
SUMMARY_FILE = "summary_statistics.csv"
TIMESTEP_FILE = "timestep_metrics.csv"
SPATIAL_FILE = "spatial_heatmap.csv"
TRAJECTORY_FILE = "vehicle_trajectories.csv"

# Color palette
CMAP = mpl.colormaps["viridis"]
COLOR_BASE = CMAP(0.2)
COLOR_MOD  = CMAP(0.8)

# Plot styling
plt.style.use('seaborn-v0_8-darkgrid')
sns.set_palette("husl")


# PLOTTING FUNCTIONS
def plot_throughput_comparison(baseline_dir, modified_dir, output_dir):
    baseline_summary = pd.read_csv(f"{baseline_dir}/{SUMMARY_FILE}")
    modified_summary = pd.read_csv(f"{modified_dir}/{SUMMARY_FILE}")

    baseline_throughput = baseline_summary.loc[
        baseline_summary['metric'] == 'throughputPerMinute', 'value'
    ].values[0]
    modified_throughput = modified_summary.loc[
        modified_summary['metric'] == 'throughputPerMinute', 'value'
    ].values[0]

    fig, ax = plt.subplots(figsize=(10, 6))
    scenarios = ['Baseline\n(1 lane)', 'Modified\n(2 lanes)']
    throughputs = [baseline_throughput, modified_throughput]
    colors = [COLOR_BASE, COLOR_MOD]

    bars = ax.bar(scenarios, throughputs, color=colors, edgecolor='black', linewidth=1.5)

    for bar in bars:
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width() / 2, height,
                f'{height:.2f}', ha='center', va='bottom', fontsize=12)

    improvement = ((modified_throughput - baseline_throughput) / baseline_throughput) * 100
    ax.text(0.5, max(throughputs) * 0.9, f'+{improvement:.1f}% improvement',
            ha='center', fontsize=14,
            bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

    ax.set_ylabel('Throughput (vehicles/minute)')
    ax.set_title('Intersection Throughput Comparison')

    plt.tight_layout()
    plt.savefig(f"{output_dir}/throughput_comparison.png", dpi=300)
    plt.close()


def plot_queue_comparison(baseline_dir, modified_dir, output_dir):
    baseline_summary = pd.read_csv(f"{baseline_dir}/{SUMMARY_FILE}")
    modified_summary = pd.read_csv(f"{modified_dir}/{SUMMARY_FILE}")

    baseline_queue = baseline_summary.loc[
        baseline_summary['metric'] == 'maxQueueLength', 'value'
    ].values[0]
    modified_queue = modified_summary.loc[
        modified_summary['metric'] == 'maxQueueLength', 'value'
    ].values[0]

    fig, ax = plt.subplots(figsize=(10, 6))
    scenarios = ['Baseline\n(1 lane)', 'Modified\n(2 lanes)']
    queues = [baseline_queue, modified_queue]
    colors = [COLOR_BASE, COLOR_MOD]

    bars = ax.bar(scenarios, queues, color=colors, edgecolor='black', linewidth=1.5)

    for bar in bars:
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width() / 2, height,
                f'{int(height)}', ha='center', va='bottom', fontsize=12)

    reduction = ((baseline_queue - modified_queue) / baseline_queue) * 100
    ax.text(0.5, max(queues) * 0.9, f'-{reduction:.1f}% reduction',
            ha='center', fontsize=14,
            bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.5))

    ax.set_ylabel('Maximum Queue Length (vehicles)')
    ax.set_title('Maximum Queue Length on Eastbound Approach')

    plt.tight_layout()
    plt.savefig(f"{output_dir}/queue_comparison.png", dpi=300)
    plt.close()


def plot_velocity_timeseries(baseline_dir, modified_dir, output_dir):
    baseline_ts = pd.read_csv(f"{baseline_dir}/{TIMESTEP_FILE}")
    modified_ts = pd.read_csv(f"{modified_dir}/{TIMESTEP_FILE}")

    fig, ax = plt.subplots(figsize=(14, 6))

    ax.plot(baseline_ts['step'], baseline_ts['avgVelocity'],
            label='Baseline (1 lane)', color=COLOR_BASE, linewidth=1.5)
    ax.plot(modified_ts['step'], modified_ts['avgVelocity'],
            label='Modified (2 lanes)', color=COLOR_MOD, linewidth=1.5)

    ax.set_xlabel('Time (seconds)')
    ax.set_ylabel('Average Velocity (cells/step)')
    ax.set_title('Average Vehicle Velocity Over Time')
    ax.legend()

    plt.tight_layout()
    plt.savefig(f"{output_dir}/velocity_timeseries.png", dpi=300)
    plt.close()


def plot_direction_speeds(baseline_dir, modified_dir, output_dir):
    baseline_ts = pd.read_csv(f"{baseline_dir}/{TIMESTEP_FILE}")
    modified_ts = pd.read_csv(f"{modified_dir}/{TIMESTEP_FILE}")

    directions = ['North', 'South', 'East', 'West']
    baseline_speeds = [
        baseline_ts['avgVelNorth'].mean(),
        baseline_ts['avgVelSouth'].mean(),
        baseline_ts['avgVelEast'].mean(),
        baseline_ts['avgVelWest'].mean()
    ]
    modified_speeds = [
        modified_ts['avgVelNorth'].mean(),
        modified_ts['avgVelSouth'].mean(),
        modified_ts['avgVelEast'].mean(),
        modified_ts['avgVelWest'].mean()
    ]

    x = np.arange(len(directions))
    width = 0.35

    fig, ax = plt.subplots(figsize=(12, 6))
    bars1 = ax.bar(x - width / 2, baseline_speeds, width,
                   label='Baseline (1 lane)', color=COLOR_BASE, edgecolor='black')
    bars2 = ax.bar(x + width / 2, modified_speeds, width,
                   label='Modified (2 lanes)', color=COLOR_MOD, edgecolor='black')

    ax.set_ylabel('Average Speed (cells/step)')
    ax.set_title('Average Speed by Direction')
    ax.set_xticks(x)
    ax.set_xticklabels(directions)
    ax.legend()

    for bars in (bars1, bars2):
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width() / 2, height,
                    f'{height:.2f}', ha='center', va='bottom', fontsize=9)

    plt.tight_layout()
    plt.savefig(f"{output_dir}/speed_per_direction.png", dpi=300)
    plt.close()


def plot_spatial_heatmap(data_dir, output_dir, scenario_name):
    spatial = pd.read_csv(f"{data_dir}/{SPATIAL_FILE}")
    pivot = spatial.pivot(index='y', columns='x', values='avgVelocity')

    fig, ax = plt.subplots(figsize=(12, 10))
    sns.heatmap(
        pivot, cmap=CMAP, center=1.5, vmin=0, vmax=3,
        cbar_kws={'label': 'Average Velocity (cells/step)'}, ax=ax, square=True
    )

    ax.set_title(f'Spatial Heatmap - {scenario_name}')
    ax.set_xlabel('X Position')
    ax.set_ylabel('Y Position')

    plt.tight_layout()
    plt.savefig(f"{output_dir}/heatmap_{scenario_name.lower().replace(' ', '_')}.png", dpi=300)
    plt.close()


def plot_waiting_time_distribution(baseline_dir, modified_dir, output_dir):
    baseline_veh = pd.read_csv(f"{baseline_dir}/{TRAJECTORY_FILE}")
    modified_veh = pd.read_csv(f"{modified_dir}/{TRAJECTORY_FILE}")

    baseline_veh = baseline_veh[baseline_veh['exitStep'] > 0]
    modified_veh = modified_veh[modified_veh['exitStep'] > 0]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

    ax1.hist(baseline_veh['stepsAtZero'], bins=30, alpha=0.6,
             label='Baseline', color=COLOR_BASE, edgecolor='black')
    ax1.hist(modified_veh['stepsAtZero'], bins=30, alpha=0.6,
             label='Modified', color=COLOR_MOD, edgecolor='black')
    ax1.set_xlabel('Waiting Time (seconds)')
    ax1.set_ylabel('Number of Vehicles')
    ax1.set_title('Waiting Time Distribution')
    ax1.legend()

    ax2.hist(baseline_veh['totalSteps'], bins=30, alpha=0.6,
             label='Baseline', color=COLOR_BASE, edgecolor='black')
    ax2.hist(modified_veh['totalSteps'], bins=30, alpha=0.6,
             label='Modified', color=COLOR_MOD, edgecolor='black')
    ax2.set_xlabel('Time in System (seconds)')
    ax2.set_ylabel('Number of Vehicles')
    ax2.set_title('Travel Time Distribution')
    ax2.legend()

    plt.tight_layout()
    plt.savefig(f"{output_dir}/waiting_time_distribution.png", dpi=300)
    plt.close()

# SUMMARY REPORT
def generate_summary_report(baseline_dir, modified_dir, output_dir):
    baseline_summary = pd.read_csv(f"{baseline_dir}/{SUMMARY_FILE}")
    modified_summary = pd.read_csv(f"{modified_dir}/{SUMMARY_FILE}")

    report_path = f"{output_dir}/analysis_report.txt"

    with open(report_path, 'w') as f:
        f.write("=" * 60 + "\n")
        f.write("TRAFFIC SIMULATION ANALYSIS REPORT\n")
        f.write("=" * 60 + "\n\n")

        f.write("HYPOTHESIS TESTING RESULTS:\n")
        f.write("-" * 60 + "\n\n")

        # H1 throughput
        base_t = baseline_summary.loc[baseline_summary['metric'] == 'throughputPerMinute', 'value'].values[0]
        mod_t = modified_summary.loc[modified_summary['metric'] == 'throughputPerMinute', 'value'].values[0]
        inc_t = ((mod_t - base_t) / base_t) * 100

        f.write(f"H1: Throughput increases by at least {H1_THROUGHPUT_REQUIRED_INCREASE:.1f}%\n")
        f.write(f"   Baseline:  {base_t:.2f} vehicles/min\n")
        f.write(f"   Modified:  {mod_t:.2f} vehicles/min\n")
        f.write(f"   Increase:  {inc_t:.1f}%\n")
        f.write("   Result:    " + ("CONFIRMED" if inc_t >= H1_THROUGHPUT_REQUIRED_INCREASE else "REJECTED") + "\n\n")

        # H2 queue
        base_q = baseline_summary.loc[baseline_summary['metric'] == 'maxQueueLength', 'value'].values[0]
        mod_q = modified_summary.loc[modified_summary['metric'] == 'maxQueueLength', 'value'].values[0]
        red_q = ((base_q - mod_q) / base_q) * 100

        f.write(f"H2: Maximum queue length decreases by at least {H2_QUEUE_REQUIRED_REDUCTION:.1f}%\n")
        f.write(f"   Baseline:  {int(base_q)} vehicles\n")
        f.write(f"   Modified:  {int(mod_q)} vehicles\n")
        f.write(f"   Reduction: {red_q:.1f}%\n")
        f.write("   Result:    " + ("CONFIRMED" if red_q >= H2_QUEUE_REQUIRED_REDUCTION else "REJECTED") + "\n\n")

        # H3 speed
        base_s = baseline_summary.loc[baseline_summary['metric'] == 'avgVelocity', 'value'].values[0]
        mod_s = modified_summary.loc[modified_summary['metric'] == 'avgVelocity', 'value'].values[0]
        inc_s = ((mod_s - base_s) / base_s) * 100

        f.write("H3: Average vehicle speed increases\n")
        f.write(f"   Baseline:  {base_s:.3f} cells/step\n")
        f.write(f"   Modified:  {mod_s:.3f} cells/step\n")
        f.write(f"   Increase:  {inc_s:.1f}%\n")
        f.write("   Result:    " + ("CONFIRMED" if inc_s >= H3_SPEED_REQUIRED_INCREASE else "REJECTED") + "\n\n")

        f.write("=" * 60 + "\n")
        f.write("ADDITIONAL METRICS:\n")
        f.write("-" * 60 + "\n\n")

        base_wait = baseline_summary.loc[baseline_summary['metric'] == 'avgWaitingTime', 'value'].values[0]
        mod_wait = modified_summary.loc[modified_summary['metric'] == 'avgWaitingTime', 'value'].values[0]
        red_wait = ((base_wait - mod_wait) / base_wait) * 100

        f.write("Average Waiting Time:\n")
        f.write(f"   Baseline:  {base_wait:.1f} seconds\n")
        f.write(f"   Modified:  {mod_wait:.1f} seconds\n")
        f.write(f"   Reduction: {red_wait:.1f}%\n\n")

        base_sys = baseline_summary.loc[baseline_summary['metric'] == 'avgTimeInSystem', 'value'].values[0]
        mod_sys = modified_summary.loc[modified_summary['metric'] == 'avgTimeInSystem', 'value'].values[0]
        red_sys = ((base_sys - mod_sys) / base_sys) * 100

        f.write("Average Time in System:\n")
        f.write(f"   Baseline:  {base_sys:.1f} seconds\n")
        f.write(f"   Modified:  {mod_sys:.1f} seconds\n")
        f.write(f"   Reduction: {red_sys:.1f}%\n\n")

    print(f"Saved analysis report to {report_path}")


# MAIN
def main():
    if len(sys.argv) < 4:
        print("Usage: python plot_analysis.py <baseline_dir> <modified_dir> <output_dir>")
        sys.exit(1)

    baseline_dir = sys.argv[1]
    modified_dir = sys.argv[2]
    output_dir = sys.argv[3]

    Path(output_dir).mkdir(parents=True, exist_ok=True)

    print("\n" + "=" * 60)
    print("TRAFFIC SIMULATION DATA ANALYSIS")
    print("=" * 60 + "\n")

    print("Generating comparison plots...")
    plot_throughput_comparison(baseline_dir, modified_dir, output_dir)
    plot_queue_comparison(baseline_dir, modified_dir, output_dir)
    plot_velocity_timeseries(baseline_dir, modified_dir, output_dir)
    plot_direction_speeds(baseline_dir, modified_dir, output_dir)
    plot_waiting_time_distribution(baseline_dir, modified_dir, output_dir)

    print("\nGenerating spatial heatmaps...")
    plot_spatial_heatmap(baseline_dir, output_dir, "Baseline")
    plot_spatial_heatmap(modified_dir, output_dir, "Modified")

    print("\nGenerating summary report...")
    generate_summary_report(baseline_dir, modified_dir, output_dir)

    print("\n" + "=" * 60)
    print("ANALYSIS COMPLETE!")
    print("=" * 60)
    print(f"\nAll figures saved to: {output_dir}/")
    print(f"Summary report: {output_dir}/analysis_report.txt\n")


if __name__ == "__main__":
    main()
