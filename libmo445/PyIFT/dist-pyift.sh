#!/usr/bin/env bash

python setup.py build_ext && python setup.py build_py && python setup.py bdist_wheel
