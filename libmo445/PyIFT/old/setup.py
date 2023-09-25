#!/usr/bin/python3
from distutils.core import setup, Extension

setup (name = 'pyift',
        version = '0.1',
        author = 'LIDS / Jordao',
        description = """Python  wrapper of libift""",
        py_modules = ['pyift'],
        packages=[''],
        package_data={'': ['_pyift.so']},
        )
