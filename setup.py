from setuptools import setup, Extension


setup(
    name='yatzy',
    ext_modules=[
        Extension('yatzy', sources=[
            'src/yatzy.c',
            'src/roll.c',
            'src/scorecard.c',
        ])
    ]
)

