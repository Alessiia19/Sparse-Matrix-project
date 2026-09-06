import os
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

def plot_single_matrix_results(csv_filepath="benchmark_results.csv", output_dir="plots"):
    if not os.path.exists(csv_filepath):
        print(f"Errore: File '{csv_filepath}' non trovato. Esegui prima il programma C++.")
        return

    os.makedirs(output_dir, exist_ok=True)
    df = pd.read_csv(csv_filepath)

    # Identifica il nome della matrice presente nel CSV
    matrix_name = df["Matrix"].iloc[0]

    col_format = "Format"
    col_memory = "Memory_MB"
    col_ratio = "Allocation_Ratio"

    sns.set_theme(style="whitegrid")
    palette = sns.color_palette("Set2")

    # ---------------------------------------------------------
    # 1. GRAFICO MEMORIA OCCUPATA (MB) PER LA MATRICE
    # ---------------------------------------------------------
    plt.figure(figsize=(9, 5))
    ax = sns.barplot(
        data=df,
        x=col_format,
        y=col_memory,
        hue=col_format,
        palette=palette
    )
    
    # Rimuove la legenda in modo compatibile con tutte le versioni di Seaborn
    if ax.get_legend() is not None:
        ax.get_legend().remove()

    plt.title(f"Occupazione di Memoria - Matrice: {matrix_name}", fontsize=14, fontweight='bold', pad=15)
    plt.xlabel("Formato Matrice", fontsize=12, fontweight='bold')
    plt.ylabel("Memoria Occupata (MB)", fontsize=12, fontweight='bold')
    plt.xticks(rotation=20)

    # Annotazioni numeriche sopra le barre
    for p in ax.patches:
        height = p.get_height()
        if not pd.isna(height) and height > 0:
            label = f"{height:.3f}" if height < 1.0 else f"{height:.2f}"
            ax.annotate(
                label,
                (p.get_x() + p.get_width() / 2., height),
                ha='center', va='bottom',
                fontsize=9, fontweight='bold',
                xytext=(0, 4),
                textcoords='offset points'
            )

    plt.tight_layout()
    chart_path = os.path.join(output_dir, f"memory_{matrix_name}.png")
    plt.savefig(chart_path, dpi=300)
    plt.close()
    print(f"[OK] Grafico memoria salvato: {chart_path}")

    # ---------------------------------------------------------
    # 2. TABELLA GRAFICA ALLOCATION RATIO
    # ---------------------------------------------------------
    fig_tbl, ax_tbl = plt.subplots(figsize=(6, 3.5))
    ax_tbl.axis('off')

    table_data = df[[col_format, col_ratio]].copy()
    table_data[col_ratio] = table_data[col_ratio].map('{:.3f}'.format)

    table = ax_tbl.table(
        cellText=table_data.values,
        colLabels=["Formato", "Allocation Ratio"],
        loc='center',
        cellLoc='center'
    )
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1.2, 1.6)

    # Styling dell'intestazione e della prima colonna
    for (r, c), cell in table.get_celld().items():
        if r == 0:
            cell.set_facecolor('#2c3e50')
            cell.get_text().set_color('white')
            cell.get_text().set_weight('bold')
        elif c == 0:
            cell.set_facecolor('#ecf0f1')
            cell.get_text().set_weight('bold')

    plt.title(f"Allocation Ratio - Matrice: {matrix_name}", fontsize=13, fontweight='bold', pad=10)
    table_path = os.path.join(output_dir, f"table_ratio_{matrix_name}.png")
    plt.savefig(table_path, dpi=300, bbox_inches='tight')
    plt.close()
    print(f"[OK] Immagine tabella salvata: {table_path}")

    print(f"\n--- ALLOCATION RATIO per {matrix_name} ---")
    print(table_data.to_string(index=False))

if __name__ == "__main__":
    plot_single_matrix_results()