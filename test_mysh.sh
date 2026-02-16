#!/bin/bash

MY_SHELL="./mysh"
TEMP_OUT="actual_output.txt"
EXPECTED_OUT="expected_output.txt"

# syntax: run_test <test name> <command args>
run_test() {
    local test_name=$1
    local command_args=$2

    echo -n "Running $test_name... "

    # run command in mysh and capture output
    echo "$command_args" | $MY_SHELL > $TEMP_OUT 2>&1

    # check if mysh exited with an error
    if [ $? -ne 0 ] && [[ "$test_name" != "Exit Test" ]]; then
        echo "FAILED (Shell crashed or returned error code)"
        return
    fi

    # verification logic
    case $test_name in
        "PWD Test")
            # check if the output contains the current directory
            if grep -q "$(pwd)" $TEMP_OUT; then
                echo "PASSED"
            else
                echo "FAILED (Incorrect path reported)"
            fi
            ;;
        "CD Test")
            # check if the output indicates directory change
            if grep -q "Changing directory..." $TEMP_OUT; then
                echo "PASSED"
            else
                echo "FAILED (Directory change not reported)"
            fi
            ;;
        "Redirection Test")
            # check if out.txt was created and contains "hello"
            if [ -f "out.txt" ] && grep -q "hello" "out.txt"; then
                echo "PASSED"
            else
                echo "FAILED (File not created or incorrect content)"
            fi
            rm -f out.txt
            ;;
        *)
            # default: command ran without crashing
            echo "COMPLETED"
            ;;
    esac
}

# test logic
rm -f out.txt actual_output.txt
run_test "External Command (ls)" "ls"
run_test "PWD Test" "pwd"
run_test "CD Test" "cd test"
run_test "Redirection Test" "echo hello > out.txt"
rm -f actual_output.txt