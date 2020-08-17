import pytest
import yahtzee


def test_Roll_item_access():
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

