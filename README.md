# Yatzy

Python framework for playing Yatzy (Yahtzee).

## Types


### Roll


#### __init__(die1, die2, die3, die4, die5)

Initialize `Roll` with five specific die values. Must include all five dice. Arguments must be `int` values 1-6.


#### classmethod: roll()

Create a `Roll` object with random die values.

#### hold(die...)

Re-roll with indicated die held from previous roll. Returns new `Roll`. Does not mutate previous `Roll` object.


### Scorecard

Object for scoring rolls.


## TODO

* Add `Scorecard` yatzy getter (separate getter for yatzy bonus?)
* Implement `Scorecard` getter tests.
* Retrieve available `Scorecard` scoring slots.
* Fill out `Scorcard` API documentation.
* Create objects for game state?

