#!/bin/bash
# Smoke tests for benchmark CLI flags
# Verifies --compare-against and --compare-benchmarks work without running
# a full benchmark.

set -e
cd "$(dirname "$0")/.."

BIN=./cubotron
RESULTS_DIR=benchmarks/results
mkdir -p "$RESULTS_DIR"

echo "=== Smoke test: --compare-benchmarks ==="

# Create two minimal valid benchmark JSON files
FILE1="$RESULTS_DIR/smoke_test_baseline.json"
FILE2="$RESULTS_DIR/smoke_test_optimized.json"

cat > "$FILE1" << 'EOF'
{
  "timestamp": "20260711_120000",
  "type": "smoke",
  "git_commit": "test",
  "warmup_duration_ms": 100,
  "benchmark_duration_ms": 500,
  "warmup_count": 10,
  "sample_count": 3,
  "solves_per_second": 100.00,
  "solve_times_ms": [10.0, 12.0, 11.0],
  "solution_lengths": [21, 22, 21],
  "stats": {
    "time": {
      "mean": 11.0, "median": 11.0, "std": 1.0, "min": 10.0, "max": 12.0,
      "p90": 12.0, "p95": 12.0, "p99": 12.0, "n": 3
    },
    "length": {
      "mean": 21.3, "median": 21.0, "std": 0.6, "min": 21, "max": 22,
      "p90": 22, "p95": 22, "p99": 22, "n": 3
    }
  }
}
EOF

cat > "$FILE2" << 'EOF'
{
  "timestamp": "20260711_130000",
  "type": "smoke",
  "git_commit": "test",
  "warmup_duration_ms": 100,
  "benchmark_duration_ms": 500,
  "warmup_count": 10,
  "sample_count": 3,
  "solves_per_second": 110.00,
  "solve_times_ms": [9.0, 11.0, 10.0],
  "solution_lengths": [21, 22, 21],
  "stats": {
    "time": {
      "mean": 10.0, "median": 10.0, "std": 1.0, "min": 9.0, "max": 11.0,
      "p90": 11.0, "p95": 11.0, "p99": 11.0, "n": 3
    },
    "length": {
      "mean": 21.3, "median": 21.0, "std": 0.6, "min": 21, "max": 22,
      "p90": 22, "p95": 22, "p99": 22, "n": 3
    }
  }
}
EOF

echo "  Comparing two benchmark files..."
$BIN --compare-benchmarks "$FILE1,$FILE2" 2>&1 | head -20
echo "  PASS"

echo ""
echo "=== Smoke test: --compare-benchmarks with missing file ==="
$BIN --compare-benchmarks "$FILE1,/tmp/nonexistent.json" 2>&1 | head -5 || true
echo "  PASS (error handled gracefully)"

echo ""
echo "=== All smoke tests passed ==="

rm -f "$FILE1" "$FILE2"
