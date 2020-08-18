from setuptools import setup, Extension


setup(
    name='yahtzee',
    ext_modules=[
        Extension('yahtzee', sources=[
            'src/yahtzee.c',
            'src/yahtzee_roll.c',
            'src/yahtzee_scorecard.c',
        ])
    ]
)

