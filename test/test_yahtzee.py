import pytest
import yahtzee



def test_Roll_item_access():
    roll = yahtzee.Roll(1,2,3,4,5)
    assert roll[0] == 1
    assert roll[1] == 2
    assert roll[2] == 3
    assert roll[3] == 4
    assert roll[4] == 5


def test_Roll_random_creation():
    roll = yahtzee.Roll.roll()
    for i in range(0, 5):
        assert roll[i] >= 1 and roll[i] <= 6


def test_Roll_item_access_out_of_bounds():
    roll = yahtzee.Roll.roll()
    with pytest.raises(IndexError):
        assert roll[5] == None


def test_Roll_iter():
    for die in iter(yahtzee.Roll.roll()):
        assert die >= 1 and die <= 6


def test_Scorecard_score_as_ones():
    scorecard = yahtzee.Scorecard()
    result = scorecard.score_as_ones(yahtzee.Roll(1,2,3,4,5))
    assert result == 1


def test_Scorecard_score_as_ones_reapply():
    scorecard = yahtzee.Scorecard()
    assert scorecard.score_as_ones(yahtzee.Roll(1,2,3,4,5)) == 1
    with pytest.raises(RuntimeError):
        assert scorecard.score_as_ones(yahtzee.Roll(1,1,3,4,5)) == 2


def test_Scorecard_score_as_twos():
    scorecard = yahtzee.Scorecard()
    result = scorecard.score_as_twos(yahtzee.Roll(1,2,3,4,5))
    assert result == 2


def test_Scorecard_score_as_threes():
    scorecard = yahtzee.Scorecard()
    result = scorecard.score_as_threes(yahtzee.Roll(1,2,3,4,5))
    assert result == 3


def test_Scorecard_score_as_fours():
    scorecard = yahtzee.Scorecard()
    result = scorecard.score_as_fours(yahtzee.Roll(1,2,3,4,5))
    assert result == 4


def test_Scorecard_score_as_fives():
    scorecard = yahtzee.Scorecard()
    result = scorecard.score_as_fives(yahtzee.Roll(1,2,3,4,5))
    assert result == 5


def test_Scorecard_score_as_sixes():
    scorecard = yahtzee.Scorecard()
    result = scorecard.score_as_sixes(yahtzee.Roll(6,6,6,6,6))
    assert result == 30


def _Scorecard_earned_bonus():
    scorecard = yahtzee.Scorecard()
    assert scorecard.score_as_ones(yahtzee.Roll(1,1,1,3,3)) == 1 * 3
    assert scorecard.score_as_twos(yahtzee.Roll(1,1,2,2,2)) == 2 * 3
    assert scorecard.score_as_threes(yahtzee.Roll(1,3,3,3,6)) == 3 * 3
    assert scorecard.score_as_fours(yahtzee.Roll(4,5,4,5,4)) == 4 * 3
    assert scorecard.score_as_fives(yahtzee.Roll(5,5,4,5,6)) == 5 * 3
    assert scorecard.score_as_sixes(yahtzee.Roll(6,2,6,6,3)) == 6 * 3
    return scorecard


def test_Scorecard_top_total_with_bonus():
    scorecard = _Scorecard_earned_bonus()
    assert scorecard.top_total() == 63 + 35


def test_Scorecard_top_subtotal_earned_bonus():
    scorecard = _Scorecard_earned_bonus()
    assert scorecard.top_subtotal() == 63


def test_Scorecard_total():
    scorecard = yahtzee.Scorecard()
    assert scorecard.score_as_ones(yahtzee.Roll(1,2,3,2,1)) == 2
    assert scorecard.score_as_twos(yahtzee.Roll(1,2,3,2,1)) == 4
    assert scorecard.score_as_threes(yahtzee.Roll(3,2,1,2,3)) == 6
    assert scorecard.score_as_fours(yahtzee.Roll(4,5,4,5,6)) == 8
    assert scorecard.score_as_fives(yahtzee.Roll(4,5,4,5,6)) == 10
    assert scorecard.score_as_sixes(yahtzee.Roll(3,2,1,2,3)) == 0
    assert scorecard.total() == 30


def test_Scorecard_total_with_bonus():
    scorecard = _Scorecard_earned_bonus()
    assert scorecard.total() == 63 + 35


def test_Scorecard_three_of_a_kind():
    scorecard = yahtzee.Scorecard()
    assert scorecard.score_as_three_of_a_kind(yahtzee.Roll(6,6,6,2,5)) == 25


def test_Scorecard_three_of_a_kind_0():
    scorecard = yahtzee.Scorecard()
    assert scorecard.score_as_three_of_a_kind(yahtzee.Roll(6,6,1,2,3)) == 0


def test_Scorecard_four_of_a_kind():
    scorecard = yahtzee.Scorecard()
    assert scorecard.score_as_four_of_a_kind(yahtzee.Roll(5,5,1,5,5)) == 21


def test_Scorecard_four_of_a_kind_0():
    scorecard = yahtzee.Scorecard()
    assert scorecard.score_as_four_of_a_kind(yahtzee.Roll(1,6,2,5,3)) == 0

