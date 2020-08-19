from setuptools import setup, Extension


setup(
    name='yatzy',
    version='0.2.0',
    author='Andrew Palmer',
    author_email='atp@sdf.org',
    description='Yatzy game framework',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/atpalmer-python/python-yatzy',

    ext_modules=[
        Extension('yatzy', sources=[
            'src/yatzy.c',
            'src/roll.c',
            'src/scorecard.c',
        ])
    ],

    classifiers=[
    ],

    python_requires=">=3.7",
)

