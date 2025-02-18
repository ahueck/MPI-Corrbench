# MPI-CorrBench 1.2.3 [![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause) [![visualize](https://github.com/tudasc/MPI-Corrbench/actions/workflows/visualize.yml/badge.svg?branch=main)](https://github.com/tudasc/mpi-corrbench-dashboard)

MPI-CorrBench \[[CORRBE21](#ref-Corrbe21)\] enables a structured comparison of the different available tools for MPI correctness checking  w.r.t. various types of errors.

## [Visualization Dashboard](https://github.com/tudasc/mpi-corrbench-dashboard)
MPI-CorrBench is automatically executed by our CI pipeline.
You can find the automatically generated visualizations in our [Visualization Dashboard](https://github.com/tudasc/mpi-corrbench-dashboard).

## Usage
Provide the Following environment Variables:
* `MPI_CORRECTNESS_BM_DIR` full path to the local directory of this repo
* `MPI_CORRECTNESS_BM_EXPERIMENT_DIR` full path to the directory where the tools should be executed

### Running the benchmarks
* Adapt the SLURM configuration in `scripts/SLURM_header.in`
* for each tool a script file `scripts/<TOOL>/execute_tool.sh` exists. You may need to adopt it to use the tool on your system
* run `scripts/run_measurement.sh 1` to generate the jobscript and submit them via sbatch (the argument given is the number of repetition for each test case)
* after all jobs are finished use `scripts/gather_results.sh` to gather all the results, which will _override_ previous data in `output` (such as our results)

### Visualize the results
The visualizations presented in \[[CORRBE21](#ref-Corrbe21)\] can be generated by the usage of the provided scripts. The scripts don't take any arguments and read the data in `output` to generate the visualization.

#### References
<table style="border:0px">
<tr>
    <td valign="top"><a name="ref-Corrbe21"></a>[CORRBE21]</td>
    <td>Lehr, Jan-Patrick and Jammer, Tim and Bischof, Christian:
      MPI-CorrBench: Towards an MPI Correctness Benchmark Suite 2021. https://dl.acm.org/doi/abs/10.1145/3431379.3460652</td>
</tr>
