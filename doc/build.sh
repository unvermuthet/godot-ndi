#!/usr/bin/env bash
cd "$(dirname "$0")"

python3 tools/make_rst.py -o classes ../doc_classes
sphinx-build . _build
