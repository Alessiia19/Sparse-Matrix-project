import os
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def plot_single_matrix_results(csv_filepath="benchmark_results.csv", output_dir="plots"):
    if not os.path.exists(csv_filepath):
        print(f"Error: File '{csv_filepath}' not found.")
        return

    os.makedirs(output_dir, exist_ok=True)
    df = pd.read_csv(csv_filepath)

    matrix_name = df["Matrix"].iloc[0]

    col_format = "Format"
    col_memory = "Memory_MB"
    col_ratio = "Allocation_Ratio"
    col_bandwidth = "Bandwidth_GBs"
    col_gflops = "GFLOPS"
    col_time = "Time_ms"
    col_conv_time = "Conversion_Time_ms"

    sns.set_theme(style="whitegrid")
    palette = sns.color_palette("Set2", len(df))

    
    # Allocated Memory Chart (MB)

    plt.figure(figsize=(7, 4.5))
    bars = plt.bar(
        df[col_format],
        df[col_memory],
        color=palette,
        width=0.55,
        align='center'
    )

    plt.title(f"Memory Consumption - Matrix: {matrix_name}", fontsize=12, fontweight='bold', pad=12)
    plt.xlabel("Matrix Format", fontsize=10, fontweight='bold', labelpad=8)
    plt.ylabel("Allocated Memory (MB)", fontsize=10, fontweight='bold', labelpad=8)
    plt.xticks(fontsize=9.5)
    plt.yticks(fontsize=9)

    y_max = df[col_memory].max()
    plt.ylim(0, y_max * 1.15)

    for bar in bars:
        height = bar.get_height()
        if not pd.isna(height) and height > 0:
            label = f"{height:.3f}" if height < 1.0 else f"{height:.2f}"
            plt.annotate(
                label,
                (bar.get_x() + bar.get_width() / 2., height),
                ha='center', va='bottom',
                fontsize=8.5, fontweight='bold',
                xytext=(0, 3),
                textcoords='offset points'
            )

    plt.tight_layout()
    chart_path = os.path.join(output_dir, f"memory_{matrix_name}.png")
    plt.savefig(chart_path, dpi=300)
    plt.close()
    print(f"[OK] Memory chart saved: {chart_path}")

    
    # Allocation Ratio Chart

    fig_tbl, ax_tbl = plt.subplots(figsize=(6, 3.5))
    ax_tbl.axis('off')

    table_data = df[[col_format, col_ratio]].copy()
    table_data[col_ratio] = table_data[col_ratio].map('{:.3f}'.format)

    table = ax_tbl.table(
        cellText=table_data.values,
        colLabels=["Format", "Allocation Ratio"],
        loc='center',
        cellLoc='center'
    )
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1.2, 1.6)

    for (r, c), cell in table.get_celld().items():
        if r == 0:
            cell.set_facecolor('#2c3e50')
            cell.get_text().set_color('white')
            cell.get_text().set_weight('bold')
        elif c == 0:
            cell.set_facecolor('#ecf0f1')
            cell.get_text().set_weight('bold')

    plt.title(f"Allocation Ratio - Matrix: {matrix_name}", fontsize=13, fontweight='bold', pad=10)
    table_path = os.path.join(output_dir, f"allocation_ratio_{matrix_name}.png")
    plt.savefig(table_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"[OK] Allocation Ratio table saved: {table_path}")

    
    # Bandwidth Chart (GB/s)
    
    plt.figure(figsize=(7, 4.5))

    bars_bw = plt.bar(
        df[col_format],
        df[col_bandwidth],
        color=palette,
        width=0.55,
        align='center'
    )

    plt.title(f"Effective Memory Bandwidth - Matrix: {matrix_name}", fontsize=12, fontweight='bold', pad=12)
    plt.xlabel("Matrix Format", fontsize=10, fontweight='bold', labelpad=8)
    plt.ylabel("Bandwidth (GB/s)", fontsize=10, fontweight='bold', labelpad=8)
    plt.xticks(fontsize=9.5)
    plt.yticks(fontsize=9)

    bw_max = df[col_bandwidth].max()
    plt.ylim(0, bw_max * 1.15)

    for bar in bars_bw:
        height = bar.get_height()
        if not pd.isna(height) and height > 0:
            plt.annotate(
                f"{height:.2f}",
                (bar.get_x() + bar.get_width() / 2., height),
                ha='center', va='bottom',
                fontsize=8.5, fontweight='bold',
                xytext=(0, 3),
                textcoords='offset points'
            )

    plt.tight_layout()
    bw_chart_path = os.path.join(output_dir, f"bandwidth_{matrix_name}.png")
    plt.savefig(bw_chart_path, dpi=300)
    plt.close()
    print(f"[OK] Bandwidth chart saved: {bw_chart_path}")

    
    # GFLOPS Chart
    
    plt.figure(figsize=(7, 4.5))

    bars_gflops = plt.bar(
        df[col_format],
        df[col_gflops],
        color=palette,
        width=0.55,
        align='center'
    )

    plt.title(f"Computational Throughput - Matrix: {matrix_name}", fontsize=12, fontweight='bold', pad=12)
    plt.xlabel("Matrix Format", fontsize=10, fontweight='bold', labelpad=8)
    plt.ylabel("Performance (GFLOPS)", fontsize=10, fontweight='bold', labelpad=8)
    plt.xticks(fontsize=9.5)
    plt.yticks(fontsize=9)

    gflops_max = df[col_gflops].max()
    plt.ylim(0, gflops_max * 1.15)

    for bar in bars_gflops:
        height = bar.get_height()
        if not pd.isna(height) and height > 0:
            label = f"{height:.3f}" if height < 1.0 else f"{height:.2f}"
            plt.annotate(
                label,
                (bar.get_x() + bar.get_width() / 2., height),
                ha='center', va='bottom',
                fontsize=8.5, fontweight='bold',
                xytext=(0, 3),
                textcoords='offset points'
            )

    plt.tight_layout()
    gflops_chart_path = os.path.join(output_dir, f"gflops_{matrix_name}.png")
    plt.savefig(gflops_chart_path, dpi=300)
    plt.close()
    print(f"[OK] GFLOPS chart saved: {gflops_chart_path}")

    
    # Average Time Chart (ms)
    
    fig_tbl_time, ax_tbl_time = plt.subplots(figsize=(6, 3.5))
    ax_tbl_time.axis('off')

    table_time_data = df[[col_format, col_time]].copy()
    table_time_data[col_time] = table_time_data[col_time].map('{:.4f}'.format)

    table_time = ax_tbl_time.table(
        cellText=table_time_data.values,
        colLabels=["Format", "Average Time (ms)"],
        loc='center',
        cellLoc='center'
    )
    table_time.auto_set_font_size(False)
    table_time.set_fontsize(10)
    table_time.scale(1.2, 1.6)

    for (r, c), cell in table_time.get_celld().items():
        if r == 0:
            cell.set_facecolor('#2c3e50')
            cell.get_text().set_color('white')
            cell.get_text().set_weight('bold')
        elif c == 0:
            cell.set_facecolor('#ecf0f1')
            cell.get_text().set_weight('bold')

    plt.title(f"Execution Time - Matrix: {matrix_name}", fontsize=13, fontweight='bold', pad=10)
    table_time_path = os.path.join(output_dir, f"time_{matrix_name}.png")
    plt.savefig(table_time_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"[OK] Execution Time table saved: {table_time_path}")

    # Conversion Time Table (ms)
    fig_tbl_conv, ax_tbl_conv = plt.subplots(figsize=(6, 3.5))
    ax_tbl_conv.axis("off")

    table_conv_data = df[[col_format, col_conv_time]].copy()
    table_conv_data[col_conv_time] = table_conv_data[col_conv_time].map(
        lambda x: f"{x:.2f}" if x >= 100.0 else f"{x:.3f}"
    )

    table_conv = ax_tbl_conv.table(
        cellText=table_conv_data.values,
        colLabels=["Format", "Conversion Time (ms)"],
        loc="center",
        cellLoc="center",
    )
    table_conv.auto_set_font_size(False)
    table_conv.set_fontsize(10)
    table_conv.scale(1.2, 1.6)

    for (r, c), cell in table_conv.get_celld().items():
        if r == 0:
            cell.set_facecolor("#2c3e50")
            cell.get_text().set_color("white")
            cell.get_text().set_weight("bold")
        elif c == 0:
            cell.set_facecolor("#ecf0f1")
            cell.get_text().set_weight("bold")

    plt.title(
        f"Conversion Time - Matrix: {matrix_name}",
        fontsize=13,
        fontweight="bold",
        pad=10,
    )
    conv_table_path = os.path.join(
        output_dir, f"conversion_time_{matrix_name}.png"
    )
    plt.savefig(conv_table_path, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"[OK] Conversion Time table saved: {conv_table_path}")


if __name__ == "__main__":
    plot_single_matrix_results(csv_filepath="benchmark_results.csv")