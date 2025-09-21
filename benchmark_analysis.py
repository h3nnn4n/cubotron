#!/usr/bin/env python3
"""
Benchmark Analysis Script for Cubotron Build Variants

This script:
1. Cleans and builds all 4 variants (all, speed, ultra, size)
2. Runs 11 benchmarks for each variant (discarding the first)
3. Performs robust statistical analysis to determine significance
4. Outputs detailed results and visualizations
"""

import subprocess
import sys
import time
import statistics
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
from scipy.stats import mannwhitneyu, kruskal, wilcoxon
import warnings
warnings.filterwarnings('ignore')

def run_command(cmd, timeout=300):
    """Run a command and return its output, handling errors gracefully."""
    try:
        result = subprocess.run(
            cmd, 
            shell=True, 
            capture_output=True, 
            text=True, 
            timeout=timeout,
            cwd='.'
        )
        if result.returncode != 0:
            print(f"Error running command: {cmd}")
            print(f"STDERR: {result.stderr}")
            return None
        return result.stdout.strip()
    except subprocess.TimeoutExpired:
        print(f"Command timed out: {cmd}")
        return None
    except Exception as e:
        print(f"Exception running command: {cmd}, Error: {e}")
        return None

def build_variant(variant):
    """Build a specific variant and return success status."""
    print(f"Building {variant}...")
    
    # Clean first
    clean_result = run_command("make clean")
    if clean_result is None:
        return False
    
    # Build the variant
    build_result = run_command(f"make {variant}")
    if build_result is None:
        return False
    
    # Check if binary exists
    binary_name = "cubotron" if variant == "all" else f"cubotron_{variant}"
    check_result = run_command(f"test -f {binary_name} && echo 'exists'")
    return check_result == "exists"

def run_benchmark(variant, run_number):
    """Run a single benchmark and return the solves per second."""
    binary_name = "cubotron" if variant == "all" else f"cubotron_{variant}"
    
    # Run the benchmark
    result = run_command(f"./{binary_name} --benchmarks")
    if result is None:
        return None
    
    try:
        # Parse the output to get the number
        solves_per_second = float(result)
        print(f"  Run {run_number}: {solves_per_second:.4f} solves/sec")
        return solves_per_second
    except ValueError:
        print(f"  Run {run_number}: Failed to parse output: {result}")
        return None

def collect_benchmark_data(variants, runs_per_variant=11):
    """Collect benchmark data for all variants."""
    data = {}
    
    for variant in variants:
        print(f"\n{'='*50}")
        print(f"Processing variant: {variant}")
        print(f"{'='*50}")
        
        # Build the variant
        if not build_variant(variant):
            print(f"Failed to build {variant}, skipping...")
            continue
        
        # Run benchmarks
        variant_data = []
        for run in range(1, runs_per_variant + 1):
            print(f"Running benchmark {run}/{runs_per_variant} for {variant}...")
            result = run_benchmark(variant, run)
            if result is not None:
                variant_data.append(result)
            else:
                print(f"  Benchmark {run} failed, retrying...")
                # Retry once
                time.sleep(1)
                result = run_benchmark(variant, run)
                if result is not None:
                    variant_data.append(result)
                else:
                    print(f"  Benchmark {run} failed after retry, skipping...")
        
        if len(variant_data) >= 5:  # Need at least 5 successful runs
            data[variant] = variant_data
            print(f"Collected {len(variant_data)} successful runs for {variant}")
        else:
            print(f"Insufficient data for {variant} ({len(variant_data)} runs), skipping...")
    
    return data

def robust_statistics(data):
    """Calculate robust statistics for the data."""
    stats_dict = {}
    
    for variant, values in data.items():
        if len(values) < 2:
            continue
            
        # Remove first run (warmup)
        if len(values) > 1:
            values = values[1:]
        
        stats_dict[variant] = {
            'mean': statistics.mean(values),
            'median': statistics.median(values),
            'std': statistics.stdev(values) if len(values) > 1 else 0,
            'min': min(values),
            'max': max(values),
            'q1': np.percentile(values, 25),
            'q3': np.percentile(values, 75),
            'iqr': np.percentile(values, 75) - np.percentile(values, 25),
            'count': len(values),
            'values': values
        }
    
    return stats_dict

def statistical_tests(data):
    """Perform statistical tests to determine significance."""
    variants = list(data.keys())
    if len(variants) < 2:
        return {}
    
    results = {}
    
    # Prepare data (excluding first run)
    test_data = {}
    for variant, values in data.items():
        if len(values) > 1:
            test_data[variant] = values[1:]
    
    # Kruskal-Wallis test (non-parametric ANOVA)
    if len(test_data) >= 2:
        groups = list(test_data.values())
        try:
            h_stat, p_value = kruskal(*groups)
            results['kruskal_wallis'] = {
                'h_statistic': h_stat,
                'p_value': p_value,
                'significant': p_value < 0.05
            }
        except Exception as e:
            print(f"Kruskal-Wallis test failed: {e}")
    
    # Pairwise Mann-Whitney U tests
    pairwise_results = {}
    for i, var1 in enumerate(variants):
        for var2 in variants[i+1:]:
            if var1 in test_data and var2 in test_data:
                try:
                    stat, p_value = mannwhitneyu(
                        test_data[var1], 
                        test_data[var2], 
                        alternative='two-sided'
                    )
                    pairwise_results[f"{var1}_vs_{var2}"] = {
                        'statistic': stat,
                        'p_value': p_value,
                        'significant': p_value < 0.05
                    }
                except Exception as e:
                    print(f"Mann-Whitney test failed for {var1} vs {var2}: {e}")
    
    results['pairwise'] = pairwise_results
    return results

def create_visualizations(data, output_dir='benchmark_results'):
    """Create visualizations of the benchmark data."""
    import os
    os.makedirs(output_dir, exist_ok=True)
    
    # Prepare data for plotting
    plot_data = []
    for variant, values in data.items():
        if len(values) > 1:
            # Exclude first run
            values = values[1:]
            for value in values:
                plot_data.append({'Variant': variant, 'Solves_per_Second': value})
    
    df = pd.DataFrame(plot_data)
    
    # Set up the plotting style
    plt.style.use('default')
    sns.set_palette("husl")
    
    # Create figure with subplots
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    fig.suptitle('Cubotron Build Variant Performance Analysis', fontsize=16, fontweight='bold')
    
    # Box plot
    sns.boxplot(data=df, x='Variant', y='Solves_per_Second', ax=axes[0,0])
    axes[0,0].set_title('Performance Distribution by Variant')
    axes[0,0].set_ylabel('Solves per Second')
    axes[0,0].tick_params(axis='x', rotation=45)
    
    # Violin plot
    sns.violinplot(data=df, x='Variant', y='Solves_per_Second', ax=axes[0,1])
    axes[0,1].set_title('Performance Density by Variant')
    axes[0,1].set_ylabel('Solves per Second')
    axes[0,1].tick_params(axis='x', rotation=45)
    
    # Strip plot with jitter
    sns.stripplot(data=df, x='Variant', y='Solves_per_Second', ax=axes[1,0], jitter=True, alpha=0.7)
    axes[1,0].set_title('Individual Run Performance')
    axes[1,0].set_ylabel('Solves per Second')
    axes[1,0].tick_params(axis='x', rotation=45)
    
    # Mean comparison bar plot
    means = df.groupby('Variant')['Solves_per_Second'].mean().sort_values(ascending=False)
    means.plot(kind='bar', ax=axes[1,1], color='skyblue', edgecolor='black')
    axes[1,1].set_title('Mean Performance Comparison')
    axes[1,1].set_ylabel('Solves per Second')
    axes[1,1].tick_params(axis='x', rotation=45)
    
    plt.tight_layout()
    plt.savefig(f'{output_dir}/benchmark_analysis.png', dpi=300, bbox_inches='tight')
    plt.show()
    
    # Create a detailed comparison table
    stats_summary = robust_statistics(data)
    comparison_df = pd.DataFrame({
        variant: {
            'Mean': f"{stats['mean']:.4f}",
            'Median': f"{stats['median']:.4f}",
            'Std Dev': f"{stats['std']:.4f}",
            'Min': f"{stats['min']:.4f}",
            'Max': f"{stats['max']:.4f}",
            'IQR': f"{stats['iqr']:.4f}",
            'Runs': stats['count']
        }
        for variant, stats in stats_summary.items()
    }).T
    
    comparison_df.to_csv(f'{output_dir}/performance_summary.csv')
    print(f"\nDetailed results saved to {output_dir}/")

def print_results(data, stats_results, test_results):
    """Print formatted results."""
    print(f"\n{'='*80}")
    print("BENCHMARK ANALYSIS RESULTS")
    print(f"{'='*80}")
    
    # Summary statistics
    print("\nPERFORMANCE SUMMARY (excluding first run):")
    print("-" * 60)
    print(f"{'Variant':<10} {'Mean':<10} {'Median':<10} {'Std Dev':<10} {'Min':<10} {'Max':<10} {'Runs':<6}")
    print("-" * 60)
    
    for variant, stats in stats_results.items():
        print(f"{variant:<10} {stats['mean']:<10.4f} {stats['median']:<10.4f} "
              f"{stats['std']:<10.4f} {stats['min']:<10.4f} {stats['max']:<10.4f} {stats['count']:<6}")
    
    # Statistical significance tests
    if 'kruskal_wallis' in test_results:
        kw = test_results['kruskal_wallis']
        print(f"\nKRUSKAL-WALLIS TEST (Non-parametric ANOVA):")
        print("-" * 50)
        print(f"H-statistic: {kw['h_statistic']:.4f}")
        print(f"P-value: {kw['p_value']:.6f}")
        print(f"Significant difference: {'YES' if kw['significant'] else 'NO'} (α=0.05)")
    
    # Pairwise comparisons
    if 'pairwise' in test_results and test_results['pairwise']:
        print(f"\nPAIRWISE MANN-WHITNEY U TESTS:")
        print("-" * 50)
        print(f"{'Comparison':<20} {'P-value':<12} {'Significant':<12}")
        print("-" * 50)
        
        for comparison, result in test_results['pairwise'].items():
            significance = 'YES' if result['significant'] else 'NO'
            print(f"{comparison:<20} {result['p_value']:<12.6f} {significance:<12}")
    
    # Performance ranking
    if len(stats_results) > 1:
        print(f"\nPERFORMANCE RANKING (by median):")
        print("-" * 40)
        sorted_variants = sorted(stats_results.items(), key=lambda x: x[1]['median'], reverse=True)
        for i, (variant, stats) in enumerate(sorted_variants, 1):
            print(f"{i}. {variant:<10} - {stats['median']:.4f} solves/sec")

def main():
    """Main function to run the benchmark analysis."""
    print("Cubotron Build Variant Benchmark Analysis")
    print("=" * 50)
    
    # Check if required libraries are available
    try:
        import numpy as np
        import pandas as pd
        import matplotlib.pyplot as plt
        import seaborn as sns
        from scipy import stats
    except ImportError as e:
        print(f"Missing required library: {e}")
        print("Please install required packages:")
        print("pip install numpy pandas matplotlib seaborn scipy")
        sys.exit(1)
    
    # Define variants to test
    variants = ['all', 'speed', 'ultra', 'size']
    
    print(f"Testing variants: {', '.join(variants)}")
    print(f"Runs per variant: 11 (first run discarded as warmup)")
    print(f"Total expected runs: {len(variants) * 11}")
    
    # Collect benchmark data
    print(f"\nStarting data collection...")
    data = collect_benchmark_data(variants, runs_per_variant=11)
    
    if not data:
        print("No data collected. Exiting.")
        sys.exit(1)
    
    # Calculate robust statistics
    print(f"\nCalculating statistics...")
    stats_results = robust_statistics(data)
    
    # Perform statistical tests
    print(f"Performing statistical tests...")
    test_results = statistical_tests(data)
    
    # Print results
    print_results(data, stats_results, test_results)
    
    # Create visualizations
    print(f"\nCreating visualizations...")
    try:
        create_visualizations(data)
    except Exception as e:
        print(f"Visualization failed: {e}")
        print("Results saved as text only.")
    
    print(f"\nAnalysis complete!")

if __name__ == "__main__":
    main()
