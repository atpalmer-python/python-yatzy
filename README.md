# Yatzy

Python framework for playing Yatzy (Yahtzee).

## Types


### Roll


#### __init__(die1, die2, die3, die4, die5)

Initialize `Roll` with five specific die values. Must include all five dice. Arguments must be `int` values 1-6.


#### classmethod: roll()

Create a `Roll` object with random die values.


### Scorecard

Object for scoring rolls.


## TODO

* Add `str`/`repr` methods.
* Add `Scorecard` methods for accessing individual score slots.
* Retrieve available scoring slots.
* Fill out `Scorcard` API documentation.

