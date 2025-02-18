import os
import argparse
import importlib.util
import json

## entry: name: [TP,TN,FP,FN,ERR,case_id,full_case_name]
# True Positive, True Negative, False Positive, False negative, ERR=error in parsing the output or runnung case, case_id for later analysis refers to the dir_name
TP = 0
TN = 1
FP = 2
FN = 3
TW = 4
FW = 5
ERR = 6
case_id = 7
full_case_name = 8

# read env vars
BENCH_BASE_DIR = os.environ["MPI_CORRECTNESS_BM_DIR"];
INPUT_DIR = os.environ["MPI_CORRECTNESS_BM_EXPERIMENT_DIR"];


def add_cases(score, case):
    score[0] += case[0]
    score[1] += case[1]
    score[2] += case[2]
    score[3] += case[3]
    score[4] += case[4]
    score[5] += case[5]
    score[6] += case[6]
    assert score[case_id] == case[case_id]
    assert score[full_case_name] == case[full_case_name]

    return score


def parse_command_line_args():
    parser = argparse.ArgumentParser(
        description=(
            "Script that evaluates the report from a tool"
        )
    )

    # parser.add_argument('IN_DIR')
    parser.add_argument('TOOL', choices=['MUST', 'ITAC', 'MPI-Checker', 'PARCOACH', 'TODO_More_Tools'])
    # parser.add_argument('--BENCH_BASE_DIR', default=".")
    parser.add_argument('--outfile', default="[BENCH_BASE_DIR]/output/[TOOL].json")

    args = parser.parse_args()
    return args


def main():
    # if (not BENCH_BASE_DIR):
    #    print("Error: provide BENCH_BASE_DIR environment variable")
    args = parse_command_line_args()

    combined_data = {}

    # advanced data evaluate if tool found correct error
    for test_dir in os.scandir(INPUT_DIR + "/" + args.TOOL):
        # only read the directories
        if not test_dir.is_dir():
            continue
            # exclude mini apps
        if test_dir.name == "kripke" or test_dir.name == "amg2013" or test_dir.name == "lulesh":
            continue

        case_id = test_dir.name
        # read the data from dir
        data = {}
        with open(test_dir.path + "/results.json", 'r') as f:
            data = json.load(f)

        if not combined_data:
            # copy first dataset
            combined_data = data.copy()
        else:
            for key, val in combined_data.items():
                combined_data[key] = add_cases(val, data[key])

    outfile = args.outfile.replace("[TOOL]", args.TOOL).replace("[BENCH_BASE_DIR]", BENCH_BASE_DIR)
    with open(outfile, 'w') as file:
        json.dump(combined_data, file)


if __name__ == '__main__':
    main()
