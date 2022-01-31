#!/usr/bin/env python

import os
import glob

TYPES = {
  "*.c": ("/*", " *", " */"),
  "*.cpp": ("/*", " *", " */"),
  "*.h": ("/*", " *", " */"),
  "CMakeLists.txt": "#",
  "*.cmake": "#",
  "*.rb": "#",
}

with open(os.path.dirname(os.path.abspath(__file__)) + '/../LICENSE', 'r', newline="\n") as f:
  LICENSE = f.read().splitlines()

def prefix_license(prefix):
  return "\n".join(map(lambda x: prefix + ' ' + x, LICENSE)) + "\n"

def make_license(comments):
  data = None
  if isinstance(comments, tuple):
    return comments[0] + "\n" + prefix_license(comments[1]) + comments[2] + "\n"
  else:
    return prefix_license(comments)

def replace_license(file, comments):
  buffer = file.read()
  if isinstance(comments, tuple):
    if buffer.startswith(comments[0]):
      buffer = buffer.split(comments[2], 1)[1]
  else:
    while buffer.startswith(comments):
      buffer = buffer.split("\n", 1)[1]
  while buffer.startswith("\n"):
    buffer = buffer[1:]
  buffer = make_license(comments) + "\n" + buffer
  file.seek(0)
  file.write(buffer)
  file.truncate()

for pattern in TYPES:
  comments = TYPES[pattern]
  filenames = glob.glob("**/" + pattern, recursive=True)
  for filename in filenames:
    with open(filename, "r+", newline="\n") as f:
      replace_license(f, comments)
