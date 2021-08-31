LIMA - Libre Multilingual Analyzer
==================================
![LIMA logo](https://raw.githubusercontent.com/aymara/lima/master/pics/lima-logo.png)

# TL;DR

Under GNU/Linux, with a user having the rights to run docker commands and the possibility to install Python packages using `pip`:

```bash
# check docker access
$ docker ps
# if there is no error here, then proceed with the installation using pip
$ pip install aymara
$ python
>>> import aymara.lima
>>> l = aymara.lima.Client(host='localhost', port='8080')
>>> sentences = l.analyzeText('Hello, World!', lang='eng', pipeline='deep')
>>> print(sentences[0][0].lemma)
hello
>>> print(sentences.conll())
# sent_id = 1
# text = Hello, World!
1       Hello   hello   PROPN   _       _       2       Dummy   _       Len=5|NE=Person.PERSON|Pos=1|SpaceAfter=No
2       ,       ,       COMMA   _       _       3       Dummy   _       Len=1|Pos=6
3       World!  world!  NOUN    _       _       _       _       _       Len=6|NE=Miscellaneous.MISCELLANEOUS|Pos=8
```

# Introducing LIMA

LIMA is a multilingual linguistic analyzer developed by the [CEA LIST](http://www-list.cea.fr/en), [LASTI laboratory](http://www.kalisteo.fr/en/index.htm) (French acronym for Text and Image Semantic Analysis Laboratory). LIMA is available under a dual licensing model, AGPL and commercial.

LIMA has [state of the art performance for more than 60 languages](https://github.com/aymara/lima-models/blob/master/eval.md) thanks to its recent deep learning (neural network) based modules. But it includes also a very powerful rules based mechanism called ModEx allowing to quickly extract information (entities, relations, events…) in new domains where annotated data does not exist.

For more information, installation instructions and documentation, please refer to [the LIMA Wiki](https://github.com/aymara/lima/wiki).

<!---
Drone.io Build Status: [![Drone.io Build Status](https://drone.io/github.com/aymara/lima/status.png)](https://drone.io/github.com/aymara/lima/latest)
-->

Appveyor Build Status: [![Build status](https://ci.appveyor.com/api/projects/status/tyj7jgks2cxx94w9?svg=true)](https://ci.appveyor.com/project/kleag/lima)
