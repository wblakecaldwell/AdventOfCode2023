C++ Solutions to 2023 Advent of Code
====================================

Build system: [Bazel](https://bazel.build)

How to run solutions
--------------------

I included my `.bazelrc` for MacOS 14, C++ 17. You might have to change that for your system.

From the main directory:

```
# Run day 1 solution:
bazel run //:day1 -- --input_file=input/day1/input.txt
```