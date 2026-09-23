import os
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns


def plot_bsr_metrics(csv_filename="bsr_benchmark_results.csv", output_dir="bsr_plots"):
    if not os.path.exists(csv_filename):
        print(f"Error: File {csv_filename} does not exist.")
        return

    os.makedirs(output_dir, exist_ok=True)

    df = pd.read_csv(csv_filename)
    sns.set_theme(style="whitegrid")
    palette = ["#3f51b5","#009688","#4caf50","#fdcd3d","#ff2f94"]  

    metrics = [
        (
            "Time_ms",
            "Execution Time (ms)",
            "Time (ms)",
            "bsr_time.png",
            ),
        (
            "GFLOPS",
            "Computational Throughput (GFLOPS)",
            "GFLOPS",
            "bsr_gflops.png",
            ),
        (
            "Bandwidth_GBs",
            "Effective Memory Bandwidth (GB/s)",
            "Bandwidth (GB/s)",
            "bsr_bandwidth.png",
            ),
        (
            "Allocation_Ratio",
            "Allocation Ratio",
            "Allocation Ratio",
            "bsr_allocation_ratio.png",
            ),
        (
            "Conversion_Time_ms",
            "Conversion Time (ms)",
            "Conversion Time (ms)",
            "bsr_conversion_time.png",
            ),
        (
            "Memory_MB",
            "Memory Consumption (MB)",
            "Allocated Memory (MB)",
            "bsr_memory.png",
            ),
        ]

    for col_name, chart_title, y_label, file_name in metrics:
        fig, ax = plt.subplots(figsize=(9, 5.5))

        chart = sns.barplot(
            data=df,
            x="Matrix",
            y=col_name,
            hue="Format",
            palette=palette,
            ax=ax,
            edgecolor="black",
            linewidth=0.6,
        )

        plt.title(f"BSR Performance: {chart_title}",fontsize=13,fontweight="bold",pad=12)
        plt.xlabel("Test Matrix", fontsize=11, fontweight="bold", labelpad=8)
        plt.ylabel(y_label, fontsize=11, fontweight="bold", labelpad=8)
        plt.xticks(fontsize=10)
        plt.yticks(fontsize=10)

        max_val = df[col_name].max()
        plt.ylim(0, max_val * 1.18)

        for bar in chart.patches:
            bar_height = bar.get_height()
            if not pd.isna(bar_height) and bar_height > 0:
                if bar_height < 1.0:
                    label_text = f"{bar_height:.3f}"
                elif bar_height < 100:
                    label_text = f"{bar_height:.2f}"
                else:
                    label_text = f"{bar_height:.1f}"

                ax.annotate(
                    label_text,
                    (bar.get_x() + bar.get_width() / 2.0, bar_height),
                    ha="center",
                    va="bottom",
                    fontsize=8.5,
                    fontweight="bold",
                    xytext=(0, 3),
                    textcoords="offset points",
                )

        plt.legend(title="Block Size",title_fontsize="10",fontsize="9",loc="upper left")
        plt.tight_layout()

        output_path = os.path.join(output_dir, file_name)
        plt.savefig(output_path, dpi=300)
        plt.close()
        print(f"Plot saved successfully: {output_path}")


if __name__ == "__main__":
    plot_bsr_metrics()