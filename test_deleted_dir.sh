#!/bin/bash
echo "Testing minishell behavior with deleted directories..."

# Create test directory
mkdir -p test_deleted_dir/subdir
cd test_deleted_dir/subdir

# Test with our minishell
echo "=== Testing with minishell ==="
{
    echo "mkdir testingdir"
    echo "cd testingdir"
    echo "mkdir testingdirdir"  
    echo "cd testingdirdir"
    echo "pwd"
    echo "rm -rf ../../testingdir/"
    echo "pwd"
    echo "cd ../"
    echo "pwd"
    echo "exit"
} | ./minishell

# Clean up
cd ../../
rm -rf test_deleted_dir

echo "Test completed!"