C++ Solutions to 2023 Advent of Code
====================================

Project Info
------------

Build system: [Bazel](https://bazel.build)

I included my [`.bazelrc`](.bazelrc) for MacOS 14, C++ 17. You might have to change that for your system.

Look at the [BUILD](BUILD) file for different targets, _("day1", "day2", etc)_.

Puzzles
=======

Puzzle 1: Trebuchet
-------------------

Read a file, and for each line, find the first digit from the left and from the right, either by itself or
spelled out _("one", "two", "three")_, put the two digits together _("1abc2"" gives you "12")_, then add
up all the lines for a final answer.

___Note:___ I went a little overboard trying to be optimal on this one :)

- **Full desccription:** [here](DAY_1.md)

- **Code:** [here](day1.cc)

- **Input:** [here](input/day1/input.txt)

- **Output:** [here](output/day1.txt)

- **Run Code:**

```
bazel run //:day1 -- --input_file=input/day1/input.txt
```

Puzzle 2: Cube Conundrum
------------------------

Read a file that represents someone reaching into a bag of dice and making several rolls per short
game. Figure out some info about possible counts of different colors in that bag based on the rolls
seen that game.

- **Full desccription:** [here](DAY_2.md)

- **Code:** [here](day2.cc)

- **Input:** [here](input/day2/input.txt)

- **Output:** [here](output/day2.txt)

- **Run Code:**

```
bazel run //:day2 -- --input_file=input/day2/input.txt
```
