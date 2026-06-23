# README

Test de compilation et application de règles. Lancer le test avec la commande
```./run-test.sh```

Ce test
- compile le fichier de règles `rules/test.rules`, qui trouve des entités de type **A**, et met le fichier compilé dans le répertoire `resources/`. Les fichiers de configuration de LIMA qui sont dans le répertoire `conf/` contiennent les définitions nécessaires pour que ce nouveau type d'entité soit reconnu et que le traitement associé (l'application des règles) soit réalisé.
- applique l'analyse, incluant les règles compilées, au fichier `test.txt`. Avec les règles de base, la sortie de cette analyse permet d'identifier les mots "*petit test*" comme une entité de type **A**:
```
# sent_id = 1
# text = on fait un petit test ici.
1       on      on      CLS     _       GENDER=MASC|NUMBER=SING|PERSON=3        0       _       _       Pos=1|Len=2
2       fait    fait    NC      _       GENDER=MASC|NUMBER=SING 0       _       _       Pos=4|Len=4
3       un      un      DET     _       GENDER=MASC|NUMBER=SING 0       _       _       Pos=9|Len=2
4       petit   petit   ADJ     _       GENDER=MASC|NUMBER=SING _       _       _       NE=B-TEST.A|NE-value=petit test|Pos=12|Len=5
5       test    test    NC      _       GENDER=MASC|NUMBER=SING _       _       _       NE=I-TEST.A|NE-value=petit test|Pos=18|Len=4
6       ici     ici     ADV     _       _       0       _       _       Pos=23|Len=3|SpaceAfter=No
7       .       .       PONCTU_FORTE    _       _       0       _       _       Pos=26|Len=1
```