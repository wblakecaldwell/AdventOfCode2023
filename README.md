C++ Solutions to 2023 Advent of Code
====================================

Build system: [Bazel](https://bazel.build)

How to run solutions
--------------------

I included my [`.bazelrc`](.bazelrc) for MacOS 14, C++ 17. You might have to change that for your system.

Look at the [BUILD](BUILD) file for different targets, _("day1", etc)_.

From the main directory:

**Run day 1 solution ("day1" build target):**
```
bazel run //:day1 -- --input_file=input/day1/input.txt
```

Puzzles
=======

Puzzle 1: Trebuchet
-------------------

Read a file, and for each line, find the first digit from the left and from the right, either by itself or
spelled out _("one", "two", "three")_, put the two digits together _("1abc2"" gives you "12")_, then add
up all the lines for a final answer.

- **Full desccription:** [here](DAY_1.md)

- **Code:** [here](day1.cc)

- **Run Code:**

```
bazel run //:day1 -- --input_file=input/day1/input.txt
```
