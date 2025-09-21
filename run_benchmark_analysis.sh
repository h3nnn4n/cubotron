#!/bin/bash

# Cubotron Benchmark Analysis Runner
# This script installs dependencies and runs the benchmark analysis

set -e

echo "Cubotron Benchmark Analysis Setup"
echo "================================="

# Check if Python 3 is available
if ! command -v python3 &> /dev/null; then
    echo "Error: Python 3 is required but not installed."
    exit 1
fi

# Check if pip is available
if ! command -v pip3 &> /dev/null; then
    echo "Error: pip3 is required but not installed."
    exit 1
fi

echo "Installing Python dependencies..."
pip3 install -r requirements.txt

echo ""
echo "Running benchmark analysis..."
echo "This will take several minutes as it builds and tests all variants."
echo ""

python3 benchmark_analysis.py

echo ""
echo "Benchmark analysis complete!"
echo "Check the 'benchmark_results/' directory for detailed outputs and visualizations."
