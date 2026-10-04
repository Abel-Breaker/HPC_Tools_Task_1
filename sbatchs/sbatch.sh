#!/bin/bash
#SBATCH --job-name=bench_task_1
#SBATCH --output=sbatchs/outs/%x-%j.out
#SBATCH --error=sbatchs/errors/%x-%j.err
#SBATCH -p ilk
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -c 1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:40:00
#SBATCH --mem=3G
#SBATCH --hint=nomultithread

module load cesga/2020 intel/2021.3.0

COMPILERS=("gcc" "icc" "icx")
FLAGS=("-O0" "-O2 -march=native" "-O3 -march=native" "-Ofast -march=native")
PARAMETERS=("20000 50" "50000 300" "2000 2000")

for COMPILER in "${COMPILERS[@]}"; do
    for FLAG in "${FLAGS[@]}"; do

        make rebuild \
                    CC="$COMPILER" \
                    MODE=release \
                    EXTRA_CFLAGS="$FLAG" \
                    TARGET="sbatchs/executables/program_$SLURM_JOB_ID"

        for PARAMETER in "${PARAMETERS[@]}"; do
            for REPETITION in {1..5}; do
                echo "Compiler: $COMPILER Flags: \"$FLAG\" Parameters: $PARAMETER"
                srun --cpu-bind=cores ./sbatchs/executables/program_$SLURM_JOB_ID $PARAMETER
            done
        done
    done
done

rm sbatchs/executables/program_$SLURM_JOB_ID