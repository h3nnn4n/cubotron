# Cubotron Benchmark Analysis

This directory contains tools for performing robust statistical analysis of different Cubotron build variants.

## Overview

The benchmark analysis script (`benchmark_analysis.py`) performs comprehensive performance testing and statistical analysis of the four build variants:

- `all` - Standard build
- `speed` - Speed-optimized build  
- `ultra` - Ultra speed-optimized build
- `size` - Size-optimized build

## What It Does

1. **Builds all variants** - Cleans and builds each variant using `make clean && make <variant>`
2. **Runs 11 benchmarks per variant** - Uses `./cubotron_<variant> --benchmarks` to get solves per second
3. **Discards first run** - Treats the first run as warmup and excludes it from analysis
4. **Performs robust statistical analysis**:
   - Descriptive statistics (mean, median, std dev, quartiles)
   - Kruskal-Wallis test (non-parametric ANOVA)
   - Pairwise Mann-Whitney U tests
   - Performance ranking

## Quick Start

### Option 1: Automated Setup and Run
```bash
./run_benchmark_analysis.sh
```

### Option 2: Manual Setup
```bash
# Install dependencies
pip3 install -r requirements.txt

# Run analysis
python3 benchmark_analysis.py
```

## Requirements

- Python 3.7+
- pip3
- Make (for building variants)
- GCC/Clang (for compilation)

### Python Dependencies
- numpy >= 1.21.0
- pandas >= 1.3.0  
- matplotlib >= 3.5.0
- seaborn >= 0.11.0
- scipy >= 1.7.0

## Output

The script generates:

1. **Console output** - Real-time progress and summary statistics
2. **Visualizations** - Saved to `benchmark_results/benchmark_analysis.png`:
   - Box plots showing performance distribution
   - Violin plots showing performance density
   - Strip plots showing individual runs
   - Bar chart comparing mean performance
3. **CSV data** - Detailed statistics saved to `benchmark_results/performance_summary.csv`

## Statistical Methods

### Descriptive Statistics
- **Mean & Median** - Central tendency measures
- **Standard Deviation** - Variability measure
- **Quartiles & IQR** - Robust spread measures
- **Min/Max** - Range bounds

### Inferential Statistics
- **Kruskal-Wallis Test** - Non-parametric ANOVA to test if any variant differs significantly
- **Mann-Whitney U Tests** - Pairwise comparisons between all variants
- **Significance Level** - α = 0.05

### Why These Methods?
- **Non-parametric** - Don't assume normal distribution of performance data
- **Robust** - Handle outliers and non-normal data well
- **Conservative** - Reduce false positives in performance comparisons

## Example Output

```
BENCHMARK ANALYSIS RESULTS
================================================================================

PERFORMANCE SUMMARY (excluding first run):
------------------------------------------------------------
Variant    Mean       Median     Std Dev    Min        Max        Runs  
------------------------------------------------------------
all        0.0250     0.0248     0.0008     0.0234     0.0267     10    
speed      0.0289     0.0287     0.0012     0.0265     0.0312     10    
ultra      0.0301     0.0298     0.0015     0.0278     0.0334     10    
size       0.0234     0.0231     0.0009     0.0218     0.0251     10    

KRUSKAL-WALLIS TEST (Non-parametric ANOVA):
--------------------------------------------------
H-statistic: 28.4567
P-value: 0.000031
Significant difference: YES (α=0.05)

PAIRWISE MANN-WHITNEY U TESTS:
--------------------------------------------------
Comparison           P-value      Significant 
--------------------------------------------------
all_vs_speed         0.000123     YES         
all_vs_ultra         0.000045     YES         
all_vs_size          0.000234     YES         
speed_vs_ultra       0.123456     NO          
speed_vs_size        0.000012     YES         
ultra_vs_size        0.000008     YES         

PERFORMANCE RANKING (by median):
----------------------------------------
1. ultra      - 0.0298 solves/sec
2. speed      - 0.0287 solves/sec  
3. all        - 0.0248 solves/sec
4. size       - 0.0231 solves/sec
```

## Troubleshooting

### Build Failures
- Ensure all dependencies are installed
- Check that `make` is available
- Verify compiler is installed (gcc/clang)

### Python Import Errors
```bash
pip3 install --upgrade pip
pip3 install -r requirements.txt
```

### Permission Errors
```bash
chmod +x run_benchmark_analysis.sh
```

### Insufficient Data
- The script requires at least 5 successful runs per variant
- If builds fail, check compilation errors
- If benchmarks fail, verify the `--benchmarks` flag works

## Customization

### Modify Test Parameters
Edit `benchmark_analysis.py`:
- `runs_per_variant` - Number of runs per variant (default: 11)
- `variants` - List of variants to test
- `timeout` - Command timeout in seconds

### Add New Variants
Add new variants to the `variants` list in the `main()` function.

### Change Statistical Tests
Modify the `statistical_tests()` function to add different tests or change significance levels.
