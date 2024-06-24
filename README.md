LIMA - Libre Multilingual Analyzer
==================================
![LIMA logo](https://raw.githubusercontent.com/aymara/lima/master/pics/lima-logo.png)

# TL;DR

LIMA python bindings are currently available **under Linux only** (x86_64).

Under Linux with python >= 3.7 and < 4, and **upgraded pip**:


```bash
# Upgrading pip is fundamental in order to obtain the correct LIMA version
$ pip install --upgrade pip
$ pip install aymara==0.5.0b6
$ lima_models.py -l eng
# Either simply use the lima command to produce an analysis of a file in CoNLLU format:
$ lima <path to the file to analyse>
# Or use the python API:
$ python
>>> import aymara.lima
>>> nlp = aymara.lima.Lima("ud-eng")
>>> doc = nlp('Hello, World!')
>>> print(doc[0].lemma)
hello
>>> print(repr(doc))
1       Hello   hello   INTJ    _       _               0       root    _       Pos=0|Len=5
2       ,       ,       PUNCT   _       _               1       punct   _       Pos=5|Len=1
3       World   World   PROPN   _       Number:Sing     1       vocative        _       Pos=7|Len=5
4       !       !       PUNCT   _       _               1       punct   _       Pos=12|Len=1
```

# Introducing LIMA

LIMA is a multilingual linguistic analyzer developed by the [CEA LIST](http://www-list.cea.fr/en), [LASTI laboratory](http://www.kalisteo.fr/en/index.htm) (French acronym for Text and Image Semantic Analysis Laboratory). LIMA is Free Software, available under the MIT license.

LIMA has [state of the art performance for more than 60 languages](https://github.com/aymara/lima-models/blob/master/eval.md) thanks to its recent deep learning (neural network) based modules. But it includes also a very powerful rules based mechanism called ModEx allowing to quickly extract information (entities, relations, events…) in new domains where annotated data does not exist.

For more information, installation instructions and documentation, please refer to [the LIMA Wiki](https://github.com/aymara/lima/wiki).

<!---
Drone.io Build Status: [![Drone.io Build Status](https://drone.io/github.com/aymara/lima/status.png)](https://drone.io/github.com/aymara/lima/latest)
-->
[![aymara](https://circleci.com/gh/aymara/lima.svg?style=svg)](https://app.circleci.com/pipelines/github/aymara/lima)
<!--[![Build status](https://ci.appveyor.com/api/projects/status/github/aymara/lima?branch=master&svg=true)](https://ci.appveyor.com/project/kleag/lima)-->
[![GitHub Action Workflow status](https://github.com/aymara/lima/actions/workflows/build.yml/badge.svg)](https://github.com/aymara/lima/actions)


[![LIMA Python Downloads](https://static.pepy.tech/personalized-badge/aymara?period=total&units=international_system&left_color=black&right_color=brightgreen&left_text=LIMA%20Python%20Downloads)](https://pepy.tech/project/aymara)
