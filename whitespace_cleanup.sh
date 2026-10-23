#!/bin/bash

# Iterate through all .cpp and .h files in current directory and subdirectories
find . -type f \( -name "*.cpp" -o -name "*.h" \) | while read -r file; do
  # Use sed to process each file
  sed -i.bak -E 's/^[[:space:]]+$//' "$file"
  echo "Processed: $file"
done

echo "Whitespace cleanup complete!"
