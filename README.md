<img src="http://cdn.kylecombes.com/projects/bocs/bocs-logo-tagline.svg" align="center" width="60%">

BOCS is an extensible platform for creating puzzles. A cohesive software layer connects various Arduinos, each running
multiple BOCS inputs and/or outputs (e.g. a numeric keypad, a piano keyboard, an [Adafruit Trellis](https://learn.adafruit.com/adafruit-trellis-diy-open-source-led-keypad/overview)
with light-up buttons).

## Write our own puzzles

This interface then allows developers to write their own games for the BOCS, without even knowing
how the internals of the BOCS work. You can even submit puzzle statistics (e.g. time to completion, incorect attempts)
to the leaderboard server to be displayed on our website! Check out [our wiki](https://github.com/kylecombes/bocs/wiki)
to see how you can write a puzzle for the BOCS.

## Add your own I/O

New I/O devices can also be easily added to the BOCS. Just program an Arduino to follow
[some basic protocol](https://github.com/kylecombes/bocs/wiki/Adding-an-IO-Arduino), plug it into the computer (e.g. a
Raspberry Pi) running the core software, and away you go!

An overview of the software system can be seen below:
![BOCS software overview](http://cdn.kylecombes.com/projects/bocs/Software-overview.svg)

The BOCS was created as part of the fall 2017 iteration of [Principles of Engineering](http://poe.olin.edu/) at [Olin
College of Engineering](http://olin.edu/). For more details on the project, including our implementation of the
electrical system, check out [our project website](http://poe.olin.edu/2017/Poezzle/).