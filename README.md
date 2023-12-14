C++ Solutions to 2023 Advent of Code
====================================

Project Info
------------

Build system: [Bazel](https://bazel.build)

I included my [`.bazelrc`](.bazelrc) for MacOS 14, C++ 17. You might have to change that for your system.

Look at the [`BUILD`](BUILD) file for different targets, _("day1", "day2", etc)_.

I put a bunch of effort into Day 1 to try to be somewhat optimal. By Day 3, I'm now just trying to solve
and move on :)

Puzzles
=======

Puzzle 1: Trebuchet
-------------------

Read a file, and for each line, find the first digit from the left and from the right, either by itself or
spelled out _("one", "two", "three")_, put the two digits together _("1abc2"" gives you "12")_, then add
up all the lines for a final answer.

_Note: I went a little overboard trying to be optimal on this one :)_

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

Puzzle 3: Gear Ratios
---------------------

Read a file that has a block of text that includes periods, digits, and special characters. Sum
the numbers that are neighboring a special character (same line, or prev/next, including diagonal).
In addition, if a special character is an asterisk, and neighbors exactly two numbers, multiply
those numbers together and keep a separate running sum of these "gear ratios".

- **Full desccription:** [here](DAY_3.md)

- **Code:** [here](day3.cc)

- **Input:** [here](input/day3/input.txt)

- **Output:** [here](output/day3.txt)

- **Run Code:**

```
bazel run //:day3 -- --input_file=input/day3/input.txt
```

Day 4: Scratchcards
-------------------

Read a file that represents scratch-off cards, one card per line. Each line has a card number, followed
by winning numbers, a `|`, and then the numbers in play. Part 1 and 2 have different methods for
calculating the number of points you win.

- **Full desccription:** [here](DAY_4.md)

- **Code:** [here](day4.cc)

- **Input:** [here](input/day4/input.txt)

- **Output:** [here](output/day4.txt)

- **Run Code:**

```
bazel run //:day4 -- --input_file=input/day4/input.txt
```
