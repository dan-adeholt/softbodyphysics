#!/usr/bin/env python3

import os
import re

# Directory where your .test.h files reside
dirs = ["containers", "physics"]

# Regex to match lines like: UNIT_TEST(TestName)
pattern = re.compile(r'UNIT_TEST\(([A-Za-z0-9_]+)\)')

add_test_lines = []

for test_dir in dirs:
    for root, dirs, files in os.walk(test_dir):
        for f in files:
            if f.endswith(".test.h"):
                full_path = os.path.join(root, f)
                with open(full_path, 'r') as file:
                    contents = file.read()
                    matches = pattern.findall(contents)
                    for test_name in matches:
                        # Generate an add_test line for each test found
                        add_test_lines.append(
                            f'add_test(NAME Unit_{test_name} COMMAND SoftBodyPhysicsTests --test {test_name})'
                        )

# Print all add_test lines. This will be captured by CMake.
print("\n".join(add_test_lines))
