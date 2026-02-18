#!/bin/bash

MY_SHELL="./mysh"
TEMP_DIR="temp_folder_$(date +$s)"                          # create a unique folder name
TEMP_OUT="actual_output.txt"

verify_output_contains() {
    local expected_value=$1
    grep -q "$expected_value" "$TEMP_OUT"
}

verify_file_content() {
    local file_name=$1
    local expected_value=$2
    [ -f "$file_name" ] && grep -q "$expected_value" "$file_name"
}

# syntax: run_test <test name> <command args>
run_test() {
    local test_name="$1"
    local command_args="$2"
    local expected_type="$3"
    local expected_val="$4"

    echo -n "Running $test_name:  "

    # run command in mysh and capture output
    echo -e "$command_args" | $MY_SHELL > $TEMP_OUT 2>&1
    local exit_status=$?

    # check if mysh exited with an error
    if [ $exit_status -ne 0 ]; then
        echo "FAILED (Shell crashed with code $exit_status)"
        return 1
    fi

    # verification logic
    case $expected_type in
        "contains")
            verify_output_contains "$expected_val" && echo "PASSED" || echo "FAILED (Output missing: $expected_val)"
            ;;
        "file_match")
            local file_name="$5"
            verify_file_content "$file_name" "$expected_val" && echo "PASSED" || echo "FAILED (File error)"
            ;;
        "silent")
            # Just checks if it ran without error
            echo "PASSED"
            ;;
    esac
}

# Clean up before starting
rm -f out.txt "$TEMP_OUT"
mkdir -p "$TEMP_DIR"

# test logic
run_test "External ls" "ls" "contains" "mysh"
run_test "PWD Test" "pwd" "contains" "$(pwd)"

run_test "CD Test" "cd $TEMP_DIR\npwd" "contains" "$TEMP_DIR"
run_test "CD Home" "cd\npwd" "contains" "$HOME"

run_test "Redirection Output" "echo modularity > out.txt" "file_match" "modularity" "out.txt"
run_test "Append Test" "echo line1 > app.txt\necho line2 >> app.txt" "file_match" "line2" "app.txt"

run_test "Fake Command" "nonexistent_cmd" "contains" "not found"

rmdir "$TEMP_DIR"
rm -f out.txt app.txt "$TEMP_OUT"