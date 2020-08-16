import yahtzee


def test_Roll():
    roll = yahtzee.Roll()


def test_Roll_item_access():
    roll = yahtzee.Roll()
    for i in range(0, 5):
        assert roll[i] >= 1 and roll[i] <= 6

