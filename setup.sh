#!/bin/bash

git init
git branch -M main
# install pre-commit hooks
pip install pre-commit commitizen clang-format mkdocs mkdocs-material mkdoxy
apt-get update && apt-get install -y build-essential libtool autoconf unzip wget ninja-build doxygen

cz init
pre-commit install
