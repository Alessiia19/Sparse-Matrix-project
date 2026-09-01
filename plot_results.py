import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

def plot_benchmarks(csv_filepath="benchmark_results.csv", output_dir="plots"):
    if not os.path.exists(csv_filepath):
        print(f"Errore: File '{csv_filepath}' non trovato. Esegui prima il programma C++.")
        return

    os.makedirs(output_dir, exist_ok=True)
    
    # Carica i dati
    df = pd.read_csv(csv_filepath)
    
    # Imposta lo stile grafico
    sns.set_theme(style="whitegrid", palette="muted")
    
    metrics = [
        ("Memory_MB", "Memoria Occupata (MB)", "MB", "chart_memory.png"),
        ("Time_ms", "Tempo Esecuzione SpMV (ms)", "ms", "chart_time.png"),
        ("GFLOPS", "Performance SpMV (GFLOPS)", "GFLOPS", "chart_gflops.png"),
        ("Bandwidth_GBs", "Banda Passante Effettiva (GB/s)", "GB/s", "chart_bandwidth.png"),
        ("Density", "Fattore di Padding / Density", "Rapporto (Stronzi/Utili)", "chart_density.png")
    ]
    
    matrices = df["Matrix"].unique()
    
    for metric_col, title, ylabel, filename in metrics:
        if metric_col not in df.columns:
            continue
            
        plt.figure(figsize=(10, 6))
        ax = sns.barplot(data=df, x="Matrix", y=metric_col, hue="Format")
        
        plt.title(title, fontsize=14, fontweight='bold', pad=15)
        plt.xlabel("Matrice d'Ingresso", fontsize=12, fontweight='bold')
        plt.ylabel(ylabel, fontsize=12, fontweight='bold')
        plt.legend(title="Formato Matrice", bbox_to_anchor=(1.05, 1), loc='upper left')
        
        # Aggiungi etichette valori sulle barre
        for p in ax.patches:
            height = p.get_height()
            if not pd.isna(height) and height > 0:
                ax.annotate(f'{height:.2f}',
                            (p.get_x() + p.get_width() / 2., height),
                            ha='center', va='bottom',
                            fontsize=8, color='black',
                            xytext=(0, 3),
                            textcoords='offset points')
                            
        plt.tight_layout()
        output_path = os.path.join(output_dir, filename)
        plt.savefig(output_path, dpi=300)
        plt.close()
        print(f"Grafico salvato: {output_path}")

if __name__ == "__main__":
    plot_benchmarks()