#!/usr/bin/env python3

import glob
import subprocess

filenames = []

for filename in glob.glob('./**', recursive=True):
    if (filename.endswith('.cpp') or filename.endswith('.h')) and not(filename.startswith("./cmake")):
        filenames.append(filename)

subprocess.run(['clang-format', '-style=file', '-i', *filenames])