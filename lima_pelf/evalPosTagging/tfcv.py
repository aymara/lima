#!/usr/bin/python
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

# ----------------------------------------------------------------------
# Fichier : tfcv.py
# Type    : Python
# Contient: Un petit outil pour mener une validation croisee sur le POS-tagger
# Usage   : ./tfcv.py [-c] [-n] corpus
# ----------------------------------------------------------------------

from optparse import OptionParser
import sys
from os import path,system, listdir, chdir, getcwd, remove, mkdir, environ
from os.path import getsize, exists
from shutil import copy, rmtree
from re import search

# Definitions variables
#SVMT         = "/home/anoka/soft/SVMTool-1.3.1/bin/"
#CORPUS_PATH  = "../../orig_corpus_tabbed"
#ELSEVAL_PATH = "/home/anoka/Logiciels/else-0.33/code/bin"
SCRIPTS_PATH = "@SCRIPTS_PATH@"
MATRIX_PATH  = environ.get("LIMA_RESOURCES")+"/Disambiguation/"
PELF_BIN_PATH = environ.get("LIMA_DIST")+"/share/apps/lima/scripts/"

# svn blame material:
# let's use global variables for those infos because:
#  - we don't want to have to send them down to every function
#  - it's really something global that is never going to change

lang = 'none'
results = 'results.none.none'
numfold=10
# Definitions Fonctions

def TenPcSample(path,sep):
    """
    Partitonne un corpus etiquette en portions de 10% « contigues » (pas de découpage
    au milieu d'une phrase, basé sur le nombre de mots et un separateur de phrases
    (PONCTU_FORTE par defaut)).
    Organise les echantillons dans des dossiers numerotes de /results
    Au lieu d'essayer d'avoir des portions d'environ 10%, on s'assure que la portion
    numéro i s'arrête environ à i*10%. Ceci évite d'avoir une portion finale toute petite.
    """
    with open(path,'r') as c:
        lines = 0
        for line in c.xreadlines(  ): lines += 1
        partition_size = (lines/numfold)
    print "*** Partition "+str(100/numfold)+"% ("+str(partition_size)+"/"+str(lines)+") en cours ..."
    num = range(1,numfold+1)
    with open(path,'r') as corpus:
        cnt = 1
        s = open(results + "/%d/10pc.tfcv"%num[0], 'w') 
        for line in corpus:
            cnt += 1
            s.write(line)
            if(cnt > partition_size*num[0] and line.find(sep) != -1):
                s.close()
                num = num[1:]
                if num:
                  s = open(results + "/%d/10pc.tfcv" % num[0], 'w')

def SVMFormat():
    for i in range(1,numfold+1):
        system("sed -i -e's/ /_/g' -e's/\t/ /g' %s/%d/90pc.tfcv "%(results,i))

def NinetyPcSample():
    """
    Construit le complement des partitions de 10% produites par TenPcSample(path).
    Organise les echantillons dans des dossiers numerotes de /results
    """
    print "*** Partition "+str(100-100/numfold)+"% en cours ..."
    for i in range(1,numfold+1):
        for j in range(1,numfold+1):
            if j!=i:
                if path.isfile(results + "/%d/10pc.tfcv" % j):
                  system("cat %(results)s/%(j)d/10pc.tfcv >> %(results)s/%(i)d/90pc.tfcv " % {"results" : results, "j": j, "i": i})
                else:
                    sys.stderr.write("Error: no file %s/%d/10pc.tfcv\n"%(results,j));
                    exit(1);

    
def Tagged2raw():
    """
    Produit l'equivalent brut de toutes les partitions de 10% produites par TenPcSample(path)
    Organise les echantillons dans des dossiers numerotes de /results
    """
    print "*** Production de l'equivalent brut des partitions de test..."
    for i in range(1,numfold+1):
        system("%(path)s/reBuildRawCorpus.sh %(lang)s %(results)s/%(i)d/10pc.tfcv > %(results)s/%(i)d/10pc.brut" 
            % {"path" : PELF_BIN_PATH, "lang" : lang, "results": results, "i": i})

def Disamb_matrices(scripts_path):
    """
    Calcule les nouvelles matrices de desambiguisation pour chaque 
    echantillon de 90% du corpus gold 
    Organise les resultats dans des dossiers numerotes de /results
    """
    print "*** Calcul des matrices..."
    for i in range(1,numfold+1):
        chdir(results + "/" + str(i))
        try:
            mkdir("matrices");
        except OSError: 
            pass 
        system("gawk -F'\t' '{ print $2 }' 90pc.tfcv >  succession_categs_retag.txt")
        system(scripts_path+"/disamb_matrices_extract.pl succession_categs_retag.txt")
        system("sort succession_categs_retag.txt|uniq -c|awk -F' ' '{print $2\"\t\"$1}' > matrices/unigramMatrix-%s.dat"%lang)
        system(scripts_path+"/disamb_matrices_normalize.pl trigramsend.txt matrices/trigramMatrix-%s.dat"%lang)
        system("mv bigramsend.txt matrices/bigramMatrix-%s.dat"%lang)
        system("gawk -F'\t' '{ print $1\"\t\"$2 }' 90pc.tfcv > priorcorpus.txt")
        system(scripts_path+"/disamb_matrices_prior.pl priorcorpus.txt matrices/priorUnigramMatrix-%s.dat U,ET,PREF,NPP,PONCT,CC,CS"%lang)
        chdir("../..")


def AnalyzeTextAll(matrix_path):
    # TO DO : fusionner avec Disamb_matrices ?
    """
    Taggue le corpus test (10%) avec le POS-tagger entraine avec
    les matrices calculees a partir du complement (90%) de test.
    """
    for i in range(1,numfold+1):
        print "    ==== ANALYSE POUR L'ECHANTILLON "+str(i)
        chdir(results + "/" + str(i))
        copy("matrices/bigramMatrix-%s.dat"%lang, matrix_path)
        copy("matrices/trigramMatrix-%s.dat"%lang, matrix_path)
        copy("matrices/unigramMatrix-%s.dat"%lang, matrix_path)
        copy("matrices/priorUnigramMatrix-%s.dat"%lang, matrix_path)
        print "in " + getcwd()
        ret = system("analyzeText -l %s 10pc.brut"%lang)
        chdir("../..")
        #if ret is not 0:
          #raise Exception('analyzeText failure')

def TrainSVMT(conf, svmli, svmle):
    """
    Produit les modèles pour chaque échantillon.
    """
    for i in range(1,numfold+1):
        wd=getcwd()+"/"+results+"/" + str(i)
        str_wd    = wd.replace("/", "\/")
        str_svmli = svmli.replace("/", "\/")
        print "\n---  Treat sample n° "+str(i)+"  --- "
        system("sed -e 's/%SAMPLE-PATH%/"+str_wd+"/g' -e 's/%SVM-DIR%/"+str_svmli+"/g' "+conf+" > "+wd+"/config.svmt")
        print "\t**Learning model..."
        chdir(results + "/" + str(i))
        ret = system(svmle+" config.svmt")
        if ret is not 0: raise Exception('svmtrain', 'svmle did not return 0')
        chdir("../..")


def AnalyzeTextAllSVMT(init_conf, conf_path):
    """
    Taggue le corpus test (10%) avec les modèles obtenus suite à l'application
    de SVMTlearn à la partition de corpus complémentaire (90%).
    """
    try:
      system("ln -sf %s $LIMA_RESOURCES/Disambiguation/SVMToolModel-EVAL" % (getcwd()))
    except OSError:
      pass


    try: 
      for i in range(1,numfold+1):
        wd  = getcwd() + "/" + results + "/" + str(i)
        print "    ==== ANALYSE SVMT POUR L'ECHANTILLON "+str(i)
        system("sed -i 's,"+init_conf+",Disambiguation/SVMToolModel-EVAL/"+ results + "/" + str(i)+"/lima,g' "+conf_path)
        print "sed -i 's,"+init_conf+",Disambiguation/SVMToolModel-EVAL/"+ results + "/" + str(i)+"/lima,g' "+conf_path
        print wd
        chdir(wd)
        system("analyzeText -l %s 10pc.brut"%lang)
        print "analyzeText -l %s 10pc.brut"%lang
        system("sed -i 's,Disambiguation/SVMToolModel-EVAL/" + results + "/" + str(i)+"/lima,"+init_conf+",g' "+conf_path)
        print "sed -i 's,Disambiguation/SVMToolModel-EVAL/" + results + "/" + str(i)+"/lima,"+init_conf+",g' "+conf_path
        chdir("../..")
    except:
      system("rm -rf $LIMA_RESOURCES/Disambiguation/SVMToolModel-EVAL/" + results)
      raise Exception("Erreur d'évaluation")


def FormaterPourAlignement(sep):
    """
    Met les deux portions de corpus annote (gold et test) a un format directement comprehensible
    par l'aligneur.
    """
    for i in range(1,numfold+1):
        chdir(results + "/" + str(i))
        print getcwd()
        system("gawk -F'|' '{print $2\"\t\"$3}' 10pc.brut.out|sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tL_NO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > test.tfcv")
        system("sed 's/ /_/g' 10pc.tfcv > gold.tfcv")
        chdir("../..")


def Aligner():
    for i in range(1,numfold+1): 
        chdir(results + "/" + str(i))
        print "\n\n ALIGNEMENT PARTITION "+str(i) + " - " + getcwd()
        system("%(path)s//aligner.pl gold.tfcv test.tfcv > aligned 2> aligned.log" % { "path" : PELF_BIN_PATH } )
        chdir("../..")

def checkConfig(conf):
  foundDumper = False
  method = 'none'

  with open(conf) as f:
    for i in xrange(70):
      line = f.readline()
      if line.strip() == '<item value="textDumper"/>': foundDumper = True
      elif line.strip() == '<item value="viterbiPostagger-freq"/>': method = 'viterbi'
      elif line.strip() == '<item value="SvmToolPosTagger"/>': method = 'svmtool'
      elif line.strip() == '<item value="DynamicSvmToolPosTagger"/>': method = 'dynsvmtool'

  if not foundDumper:
    sys.exit(" ******* TextDumper ne semble pas être activé ! Arrêt... *******")
  elif method == 'none':
    raise Exception('No method found, was expecting Viterbi of SvmTool')
  else: return method

                
def makeTree():
    try:
        mkdir(results)
    except OSError: 
        pass # ignored
    for i in range(1,numfold+1):
        try:
            mkdir(results + "/" + str(i))
        except OSError: 
            pass # ignored

def trained(lang, tagger):
  return exists('training-sets/training.%s.%s' %(lang, tagger)) 
        

def main(corpus, conf, svmli, svmle, sep, lang_, clean, forceTrain):
    global lang, results
    lang = lang_
    # Configuration LIMA
    initial_config = "Disambiguation/SVMToolModel-"+lang+"/lima"
    conf_path      = environ.get("LIMA_CONF") + "/lima-lp-"+lang+".xml"
    tagger = checkConfig(conf_path)
    print "and the tagger is %s!" % tagger
    # set up the global variables
    results = "results.%s.%s" % (lang, tagger)
    
    if clean:
        try:
            rmtree(results)
        except OSError:
            pass

    if forceTrain or not trained(lang, tagger):
        print "ENTRAINEMENT !"
        try: rmtree(results)
        except: pass
        print """ \n
        ==================================================
        ====         Evaluation du POS-tagger         ====
        ==================================================
        
Les donnees produites au cours de l'evaluation sont
conservees sous results.%s      
        """%lang
        print " ******* CORPUS USED: "+corpus+" *******  \n"
        print " ******* SEPARATOR: "+sep+" *******  \n"
        makeTree()
        TenPcSample(corpus,sep)
        NinetyPcSample()
        Tagged2raw()
        if (tagger=='svmtool' or tagger=='dynsvmtool'):
            SVMFormat()
            TrainSVMT(conf, svmli, svmle)
        elif (tagger=='viterbi'):    
          print "Disamb_matrices(SCRIPTS_PATH)"
          Disamb_matrices(SCRIPTS_PATH)
        # copy training data in another folder for later use
        system("mv " + results + " " + "training-sets/training.%s.%s" % (lang, tagger))

    print "EVALUATION !"
    system("mkdir -p %s" % results)
    system("cp -r training-sets/training.%s.%s/*" % (lang, tagger) + " " + results)
    if (tagger=='svmtool' or tagger=='dynsvmtool'):
        AnalyzeTextAllSVMT(initial_config, conf_path)
    elif (tagger=='viterbi'):    
        AnalyzeTextAll(MATRIX_PATH)
        
    FormaterPourAlignement(sep)
    Aligner()


parser  = OptionParser()
parser.add_option("-c", "--clean", dest="clean",action="store_true", default=False, help="Nettoie l'arborescence results\\")
parser.add_option("-t", "--forceTrain", dest="forceTrain", action="store_true", default=False, help="Force l'apprentissage")
parser.add_option("-l", "--lang", dest="lang",action="store", default='eng', help="langue de l'analyse")
parser.add_option("-s", "--sep", dest="sep",action="store", default='PONCTU_FORTE', help="séparateur de phrases")
parser.add_option("-n", "--numfold", dest="numfold",action="store", default='10', help="nombre de partitions")

(options, args) = parser.parse_args()
print args

numfold = int(options.numfold)

main(args[0], args[1], args[2], args[3], options.sep, options.lang, options.clean, options.forceTrain)
