#!/usr/bin/python
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

# ----------------------------------------------------------------------
# Fichier : tfcv.py
# Type    : Python
# Contient: A tool for PoS-tagger cross-validation
# Usage   : ./tfcv.py [-c] [-n] corpus
# ----------------------------------------------------------------------

from optparse import OptionParser
import sys
from os import path,system, listdir, chdir, getcwd, remove, mkdir, environ
from os.path import getsize, exists
from shutil import copy, rmtree
from re import search

# Variables definition
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

# Functions definitions

def TenPcSample(path,sep):
    """
    Split a tagged corpus in "contiguous" 10% portions de 10% (no splitting in
    the middle of a sentence, based on the number of words and a sentence 
    separator (PONCTU_FORTE by default)).
    Organises samples in numbered folders inside results.<lang>.<tagger>
    Instead of trying to have samples of about 10%, ensures that the number i 
    sample stops at about 10% i *. This avoids having a small last sample.
    """
    with open(path,'r') as c:
        lines = 0
        lines = sum(1 for line in c)
        partition_size = (lines/numfold)
    print "*** Sample of "+path+" "+str(100/numfold)+"% ("+str(partition_size)+"/"+str(lines)+") ongoing ..."
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
    Build the complement of the 10% partitions produced by TenPcSample(path). 
    Organizes samples in numbered folders in results.<lang>.<tagger>
    """
    print "*** Sample "+str(100-100/numfold)+"% ongoing ..."
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
    Product raw equivalent () text ready to be analyzed) of all 10% samples 
    produced by TenPcSample(path).
    Organizes samples in numbered folders in results.<lang>.<tagger>
    """
    print "*** Producing raw equivalent of test partitions ..."
    for i in range(1,numfold+1):
        system("%(path)s/reBuildRawCorpus.sh %(lang)s %(results)s/%(i)d/10pc.tfcv > %(results)s/%(i)d/10pc.brut" 
            % {"path" : PELF_BIN_PATH, "lang" : lang, "results": results, "i": i})

def Disamb_matrices(scripts_path):
    """
    Computes the new disambiguation matrices for each 90% sample of the gold corpus.
    Organizes results in numbered folders in results.<lang>.<tagger>
    """
    print "*** Computing matrices..."
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
    # TODO : merge with Disamb_matrices ?
    """
    Tag the test  corpus (10%) with the POS-tagger trained with 
    the matrices computed from the test complement (90%).
    """
    for i in range(1,numfold+1):
        print "    ==== ANALYSING SAMPLE %d"%i
        chdir(results + "/" + str(i))
        copy("matrices/bigramMatrix-%s.dat"%lang, matrix_path)
        copy("matrices/trigramMatrix-%s.dat"%lang, matrix_path)
        copy("matrices/unigramMatrix-%s.dat"%lang, matrix_path)
        copy("matrices/priorUnigramMatrix-%s.dat"%lang, matrix_path)
        print "in " + getcwd()
        ret = system("analyzeText -l %s 10pc.brut -o text:.out "%lang)
        chdir("../..")
        #if ret is not 0:
          #raise Exception('analyzeText failure')

def TrainSVMT(conf, svmli, svmle):
    """
    Produces models for each sample.
    """
    for i in range(1,numfold+1):
        wd=getcwd()+"/"+results+"/" + str(i)
        str_wd    = wd.replace("/", "\/")
        str_svmli = svmli.replace("/", "\/")
        print "\n---  Treat sample n° "+str(i)+"  --- "
        system("sed -e 's/%SAMPLE-PATH%/"+str_wd+"/g' -e 's/%SVM-DIR%/"+str_svmli+"/g' "+conf+" > "+wd+"/config.svmt")
        print "\t**Learning model..."
        chdir(results + "/" + str(i))
        svmlestring = "%s %s/config.svmt"%(svmle,wd) 
        print svmlestring
        ret = system(svmlestring)
        if ret is not 0: raise Exception('svmtrain', 'svmle did not return 0')
        chdir("../..")


def AnalyzeTextAllSVMT(init_conf, conf_path):
    """
    Tag the test corpus (10%) with the models obtained after applying SVMTlearn 
    to the complementary partition (90%).
    """
    try:
      system("ln -sf %s $LIMA_RESOURCES/Disambiguation/SVMToolModel-EVAL" % (getcwd()))
    except OSError:
      pass


    try: 
      for i in range(1,numfold+1):
        wd  = getcwd() + "/" + results + "/" + str(i)
        print "    ==== SVMTool analysis for sample %i"%i
        system("sed -i 's,"+init_conf+",Disambiguation/SVMToolModel-EVAL/"+ results + "/" + str(i)+"/lima,g' "+conf_path)
        #print "sed -i 's,"+init_conf+",Disambiguation/SVMToolModel-EVAL/"+ results + "/" + str(i)+"/lima,g' "+conf_path
        print wd
        chdir(wd)
        system("analyzeText -l %s 10pc.brut"%lang)
        print "analyzeText -l %s 10pc.brut"%lang
        system("sed -i 's,Disambiguation/SVMToolModel-EVAL/" + results + "/" + str(i)+"/lima,"+init_conf+",g' "+conf_path)
        #print "sed -i 's,Disambiguation/SVMToolModel-EVAL/" + results + "/" + str(i)+"/lima,"+init_conf+",g' "+conf_path
        chdir("../..")
    except:
      system("rm -rf $LIMA_RESOURCES/Disambiguation/SVMToolModel-EVAL/" + results)
      raise Exception("Erreur d'évaluation")


def FormaterPourAlignement(sep):
    """
    Put the two portions of annotated corpus (gold and test) into a format 
    directly understandable by the aligner.
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
    sys.exit(" ******* TextDumper seems to not being activated! Stop... *******")
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
        print "TRAINING !"
        try: rmtree(results)
        except: pass
        print """ \n
        ==================================================
        ====         PoS-tagger Evaluation         ====
        ==================================================
        
Data produced are available in results.%s.%s      
        """%(lang,tagger)
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
print " ******* NUMFOLD: %s *******  \n" % numfold
main(args[0], args[1], args[2], args[3], options.sep, options.lang, options.clean, options.forceTrain)
