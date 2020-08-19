import pytest
import yatzy



def test_Roll_item_access():
    roll = yatzy.Roll(1,2,3,4,5)
    assert roll[0] == 1
    assert roll[1] == 2
    assert roll[2] == 3
    assert roll[3] == 4
    assert roll[4] == 5


def test_Roll_repr():
    roll = yatzy.Roll(1,2,3,4,5)
    assert repr(roll) == '[1, 2, 3, 4, 5]'


def test_Roll_str():
    roll = yatzy.Roll(5,4,3,2,1)
    assert str(roll) == '[5, 4, 3, 2, 1]'


def test_Roll_random_creation():
    roll = yatzy.Roll.roll()
    for i in range(0, 5):
        assert roll[i] >= 1 and roll[i] <= 6


def test_Roll_hold():
    roll1 = yatzy.Roll.roll()
    roll2 = roll1.hold(2, 4)
    assert roll1[2] == roll2[2]
    assert roll1[4] == roll2[4]
    for i in range(0, 5):
        # indexes not held are randomly assigned by `hold`
        # just confirm all die are still in range
        assert roll2[i] >= 1 and roll2[i] <= 6


def test_Roll_rolls_left():
    roll = yatzy.Roll.roll()
    roll = roll.hold()
    roll = roll.hold()
    with pytest.raises(RuntimeError):
        roll = roll.hold()


def test_Roll_item_access_out_of_bounds():
    roll = yatzy.Roll.roll()
    with pytest.raises(IndexError):
        assert roll[5] == None


def test_Roll_iter():
    for die in iter(yatzy.Roll.roll()):
        assert die >= 1 and die <= 6


def test_Scorecard_score_as_ones():
    scorecard = yatzy.Scorecard()
    result = scorecard.score_as_ones(yatzy.Roll(1,2,3,4,5))
    assert result == 1


def test_Scorecard_score_as_ones_reapply():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_ones(yatzy.Roll(1,2,3,4,5)) == 1
    with pytest.raises(RuntimeError):
        assert scorecard.score_as_ones(yatzy.Roll(1,1,3,4,5)) == 2


def test_Scorecard_score_as_twos():
    scorecard = yatzy.Scorecard()
    result = scorecard.score_as_twos(yatzy.Roll(1,2,3,4,5))
    assert result == 2


def test_Scorecard_score_as_threes():
    scorecard = yatzy.Scorecard()
    result = scorecard.score_as_threes(yatzy.Roll(1,2,3,4,5))
    assert result == 3


def test_Scorecard_score_as_fours():
    scorecard = yatzy.Scorecard()
    result = scorecard.score_as_fours(yatzy.Roll(1,2,3,4,5))
    assert result == 4


def test_Scorecard_score_as_fives():
    scorecard = yatzy.Scorecard()
    result = scorecard.score_as_fives(yatzy.Roll(1,2,3,4,5))
    assert result == 5


def test_Scorecard_score_as_sixes():
    scorecard = yatzy.Scorecard()
    result = scorecard.score_as_sixes(yatzy.Roll(6,6,6,6,6))
    assert result == 30


def _Scorecard_earned_bonus():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_ones(yatzy.Roll(1,1,1,3,3)) == 1 * 3
    assert scorecard.score_as_twos(yatzy.Roll(1,1,2,2,2)) == 2 * 3
    assert scorecard.score_as_threes(yatzy.Roll(1,3,3,3,6)) == 3 * 3
    assert scorecard.score_as_fours(yatzy.Roll(4,5,4,5,4)) == 4 * 3
    assert scorecard.score_as_fives(yatzy.Roll(5,5,4,5,6)) == 5 * 3
    assert scorecard.score_as_sixes(yatzy.Roll(6,2,6,6,3)) == 6 * 3
    return scorecard


def test_Scorecard_top_total_with_bonus():
    scorecard = _Scorecard_earned_bonus()
    assert scorecard.top_total() == 63 + 35


def test_Scorecard_top_subtotal_earned_bonus():
    scorecard = _Scorecard_earned_bonus()
    assert scorecard.top_subtotal() == 63


def test_Scorecard_total():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_ones(yatzy.Roll(1,2,3,2,1)) == 2
    assert scorecard.score_as_twos(yatzy.Roll(1,2,3,2,1)) == 4
    assert scorecard.score_as_threes(yatzy.Roll(3,2,1,2,3)) == 6
    assert scorecard.score_as_fours(yatzy.Roll(4,5,4,5,6)) == 8
    assert scorecard.score_as_fives(yatzy.Roll(4,5,4,5,6)) == 10
    assert scorecard.score_as_sixes(yatzy.Roll(3,2,1,2,3)) == 0
    assert scorecard.total() == 30


def test_Scorecard_total_with_bonus():
    scorecard = _Scorecard_earned_bonus()
    assert scorecard.total() == 63 + 35


def test_Scorecard_three_of_a_kind_exact():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_three_of_a_kind(yatzy.Roll(6,6,6,2,5)) == 25


def test_Scorecard_three_of_a_kind_at_least():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_three_of_a_kind(yatzy.Roll(6,6,6,6,6)) == 30


def test_Scorecard_three_of_a_kind_0():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_three_of_a_kind(yatzy.Roll(6,6,1,2,3)) == 0


def test_Scorecard_four_of_a_kind_exact():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_four_of_a_kind(yatzy.Roll(5,5,5,5,1)) == 21


def test_Scorecard_four_of_a_kind_at_least():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_four_of_a_kind(yatzy.Roll(5,5,5,5,5)) == 25


def test_Scorecard_four_of_a_kind_0():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_four_of_a_kind(yatzy.Roll(1,6,2,5,3)) == 0


def test_Scorecard_full_house():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_full_house(yatzy.Roll(5,5,1,1,5)) == 25


def test_Scorecard_full_house_0():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_full_house(yatzy.Roll(1,6,2,5,3)) == 0


def test_Scorecard_small_straight():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_small_straight(yatzy.Roll(2,4,3,6,1)) == 30


def test_Scorecard_small_straight_qualifies_as_large():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_small_straight(yatzy.Roll(1,2,3,4,5)) == 30


def test_Scorecard_large_straight():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_large_straight(yatzy.Roll(2,4,3,5,6)) == 40


def test_Scorecard_chance1():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_chance(yatzy.Roll(2,2,2,2,2)) == 10


def test_Scorecard_chance2():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_chance(yatzy.Roll(1,2,3,4,5)) == 15


def test_Scorecard_yatzy():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_yatzy(yatzy.Roll(5,5,5,5,5)) == 50
    assert scorecard.score_as_yatzy(yatzy.Roll(3,3,3,3,3)) == 100
    assert scorecard.score_as_yatzy(yatzy.Roll(4,4,4,4,4)) == 100


def test_Scorecard_yatzy_0():
    scorecard = yatzy.Scorecard()
    assert scorecard.score_as_yatzy(yatzy.Roll(1,2,3,4,5)) == 0
    with pytest.raises(RuntimeError):
        assert scorecard.score_as_yatzy(yatzy.Roll(1,2,3,4,5)) == 0

