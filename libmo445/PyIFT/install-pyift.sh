#!/usr/bin/env bash

python parser/parser.py && python setup.py develop && mv _pyift.*.so pyift/
