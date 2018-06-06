#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

# ----------------------------------------------------------------------
# File    : tfcv.py
# Type    : Python
# Contains: A tool for PoS-tagger cross-validation
# Usage   : ./tfcv.py [-c] [-n] corpus
# ----------------------------------------------------------------------

from optparse import OptionParser
import sys
from os import path, environ
from os.path import getsize, exists, join
from shutil import copy, copytree, rmtree
from re import search
import subprocess
import os
from os.path import realpath
import multiprocessing

# Variables definition
SCRIPTS_PATH = "@SCRIPTS_PATH@"
MATRIX_PATH = path.join(environ.get('LIMA_RESOURCES', '/usr/local'),
                        'Disambiguation')
print("MATRIX_PATH={}".format(MATRIX_PATH))
PELF_BIN_PATH = path.join(environ.get('LIMA_DIST', '/usr/local'),
                          'share/apps/lima/scripts')
MAX_PROCESSES = multiprocessing.cpu_count()

# svn blame material:
# let's use global variables for those infos because:
#  - we don't want to have to send them down to every function
#  - it's really something global that is never going to change

lang = 'none'
results = 'results.none.none'
numfold = 10


class PushdContext:
    cwd = None
    original_dir = None

    def __init__(self, dirname):
        self.cwd = realpath(dirname)

    def __enter__(self):
        self.original_dir = os.getcwd()
        os.chdir(self.cwd)
        return self

    def __exit__(self, type, value, tb):
        os.chdir(self.original_dir)


def pushd(dirname):
    return PushdContext(dirname)


def TenPcSample(path, sep):
    """
    Split a tagged corpus in "contiguous" 10% portions de 10% (no splitting in
    the middle of a sentence, based on the number of words and a sentence
    separator (PONCTU_FORTE by default)).
    Organizes samples in numbered folders inside results.<lang>.<tagger>
    Instead of trying to have samples of about 10%, ensures that the number i
    sample stops at about 10% i *. This avoids having a small last sample.
    """
    with open(path, 'r') as c:
        lines = 0
        lines = sum(1 for line in c)
        partition_size = (lines/numfold)
    print('*** Sample of {} {}% ({}/{}) sep:"{}" ongoing ...'.format(
        path, str(100/numfold), str(partition_size), str(lines), sep))
    num = range(1, numfold+1)
    with open(path, 'r') as corpus:
        cnt = 1
        s = open(results + "/%d/10pc.tfcv" % num[0], 'w',
                 encoding='utf-8', newline='\n')
        for line in corpus:
            cnt += 1
            s.write(line)
            if(cnt > partition_size*num[0] and line.find(sep) != -1):
                s.close()
                num = num[1:]
                if num:
                    s = open(results + "/%d/10pc.tfcv" % num[0], 'w',
                             encoding='utf-8', newline='\n')


def SVMFormat():
    for i in range(1, numfold+1):
        os.system("sed -i -e's/ /_/g' -e's/\t/ /g' {}/{}/90pc.tfcv ".format(
            results, i))
        os.system("sed -e's/ /_/g' -e's/\t/ /g' {}/{}/10pc.tfcv > 10pc.svmt".format(
            results, i))


def NinetyPcSample():
    """
    Build the complement of the 10% partitions produced by TenPcSample(path).
    Organizes samples in numbered folders in results.<lang>.<tagger>
    """
    global lang
    print('*** Sample {}% ongoing ...'.format(str(100-100/numfold)))
    for i in range(1, numfold+1):
        for j in range(1, numfold+1):
            if j != i:
                if path.isfile(results + "/%d/10pc.tfcv" % j):
                    os.system("cat %(results)s/%(j)d/10pc.tfcv >> %(results)s/%(i)d/90pc.tfcv "
                              % {"results": results, "j": j, "i": i})
                else:
                    sys.stderr.write("Error: no file %s/%d/10pc.tfcv\n"
                                     % (results, j))
                    exit(1)


def Tagged2raw():
    """
    Product raw equivalent () text ready to be analyzed) of all 10% samples
    produced by TenPcSample(path).
    Organizes samples in numbered folders in results.<lang>.<tagger>
    """

    processes = set()
    max_processes = MAX_PROCESSES
    print("*** Producing raw equivalent of test partitions ...")
    for i in range(1, numfold+1):
        with open('{}/{}/10pc.brut'.format(results, i), 'w',
                  encoding='utf-8', newline='\n') as outfile:
            processes.add(subprocess.Popen(
                ['{}/reBuildRawCorpus.sh'.format(PELF_BIN_PATH), lang,
                 '{}/{}/10pc.tfcv'.format(results, i)],
                stdout=outfile))
        if len(processes) >= max_processes:
            os.wait()
            for p in processes:
                if p.poll() is not None and p.returncode is not 0:
                    raise Exception('reBuildRawCorpus',
                                    'reBuildRawCorpus did not return 0')
            processes.difference_update([
                p for p in processes if p.poll() is not None])

    while processes:
        os.wait()
        for p in processes:
            if p.poll() is not None and p.returncode is not 0:
                raise Exception('reBuildRawCorpus',
                                'reBuildRawCorpus did not return 0')
        processes.difference_update([
            p for p in processes if p.poll() is not None])


def Disamb_matrices(scripts_path):
    """
    Computes the new disambiguation matrices for each 90% sample of the gold
    corpus.
    Organizes results in numbered folders in results.<lang>.<tagger>
    """

    print("*** Computing matrices...")
    for i in range(1, numfold+1):
        with pushd('{}/{}'.format(results, str(i))):
            try:
                os.mkdir("matrices")
            except OSError:
                pass
            os.system("gawk -F'\t' '{ print $2 }' 90pc.tfcv >  succession_categs_retag.txt")
            os.system(scripts_path+"/disamb_matrices_extract.pl succession_categs_retag.txt")
            os.system("sort succession_categs_retag.txt|uniq -c|awk -F' ' '{print $2\"\t\"$1}' > matrices/unigramMatrix-%s.dat"
                      % lang)
            os.system(scripts_path+"/disamb_matrices_normalize.pl trigramsend.txt matrices/trigramMatrix-%s.dat"
                      % lang)
            os.system("mv bigramsend.txt matrices/bigramMatrix-%s.dat" % lang)
            os.system("gawk -F'\t' '{ print $1\"\t\"$2 }' 90pc.tfcv > priorcorpus.txt")
            os.system(scripts_path+"/disamb_matrices_prior.pl priorcorpus.txt matrices/priorUnigramMatrix-%s.dat U,ET,PREF,NPP,PONCT,CC,CS"
                      % lang)


def BuildDictionary(language):
    """
    Build a dictionary merging the original language dictionary and entries
    deducible from the PoS tagging learning corpus for the current sample.
    Add the path of the generated resource to LIMA resources search path
    """
    global numfold
    processes = set()
    max_processes = MAX_PROCESSES
    for sample in range(1, numfold+1):
        wd = results + "/" + str(sample)
        print('\n***  Build dictionary for sample n° {} ***'.format(sample))
        # with pushd('{}/{}'.format(results, sample)):
        print('running {}/build-dico.sh {}'.format(PELF_BIN_PATH, language))
        processes.add(
            subprocess.Popen(
                ['{}/build-dico.sh'.format(PELF_BIN_PATH), language],
                cwd=wd))
        if len(processes) >= max_processes:
            os.wait()
            for p in processes:
                if p.poll() is not None and p.returncode is not 0:
                    raise Exception('build-dico.sh',
                                    'build-dico.sh did not return 0')
            processes.difference_update([
                p for p in processes if p.poll() is not None])

    while processes:
        os.wait()
        for p in processes:
            if p.poll() is not None and p.returncode is not 0:
                raise Exception('build-dico.sh',
                                'build-dico.sh did not return 0')
        processes.difference_update([
            p for p in processes if p.poll() is not None])


def AnalyzeTextAll(matrix_path):
    # TODO : merge with Disamb_matrices ?
    """
    Tag the test  corpus (10%) with the POS-tagger trained with
    the matrices computed from the test complement (90%).
    """
    processes = set()
    max_processes = MAX_PROCESSES
    for i in range(1, numfold+1):
        print('    ==== ANALYSING SAMPLE {}'.format(i))
        wd = results + "/" + str(i)
        my_env = os.environ.copy()
        my_env["LIMA_RESOURCES"] = '{}:{}:{}'.format(
            '{}/lima_linguisticdata/dist/share/apps/lima/resources'.format(wd),
            '{}/matrices'.format(wd),
            my_env["LIMA_RESOURCES"])
        processes.add(
            subprocess.Popen(
                ['analyzeText', '-l', lang, '10pc.brut', '-o', 'text:.out'],
                cwd=wd,
                env=my_env))
        if len(processes) >= max_processes:
            os.wait()
            for p in processes:
                if p.poll() is not None and p.returncode is not 0:
                    raise Exception('analyzeText',
                                    'analyzeText did not return 0')
            processes.difference_update([
                p for p in processes if p.poll() is not None])

    while processes:
        os.wait()
        for p in processes:
            if p.poll() is not None and p.returncode is not 0:
                raise Exception('analyzeText', 'analyzeText did not return 0')
        processes.difference_update([
            p for p in processes if p.poll() is not None])


def TrainSVMT(conf, svmli, svmle):
    """
    Produces models for each sample.
    """
    processes = set()
    max_processes = MAX_PROCESSES
    for i in range(1, numfold+1):
        wd = '{}/{}/{}'.format(os.getcwd(), results, str(i))
        str_wd = wd.replace("/", "\/")
        str_svmli = svmli.replace("/", "\/")
        print("\n---  Treat sample n° "+str(i)+"  --- ")
        os.system("sed -e 's/%SAMPLE-PATH%/{}/g' -e 's/%SVM-DIR%/{}/g' {} > {}/config.svmt".format(
            str_wd, str_svmli, conf, wd))
        svmlestring = "{}/config.svmt".format(wd)
        print("\t**Learning model... {} {}".format(svmle, svmlestring))
        processes.add(subprocess.Popen([svmle, svmlestring], cwd=wd))
        if len(processes) >= max_processes:
            os.wait()
            for p in processes:
                if p.poll() is not None and p.returncode is not 0:
                    raise Exception(svmle, 'svmle did not return 0')
            processes.difference_update([
                p for p in processes if p.poll() is not None])
    while processes:
        os.wait()
        for p in processes:
            if p.poll() is not None and p.returncode is not 0:
                raise Exception(svmle, 'svmle did not return 0')
        processes.difference_update([
            p for p in processes if p.poll() is not None])


def AnalyzeTextAllSVMT(init_conf, conf_path):
    """
    Tag the test corpus (10%) with the models obtained after applying SVMTlearn
    to the complementary partition (90%).
    """

    processes = set()
    max_processes = MAX_PROCESSES
    try:
        for i in range(1, numfold+1):
            print("    ==== SVMTool analysis for sample {}: {}, {}".format(
                i, init_conf, conf_path))
            wd = '{}/{}/{}'.format(os.getcwd(), results, str(i))
            local_conf_dir = '{}/conf'.format(wd)
            local_conf_path = '{}/lima-lp-{}.xml'.format(local_conf_dir, lang)
            os.makedirs(local_conf_dir, exist_ok=True)
            copy(conf_path, local_conf_path)
            os.system("sed -i 's,"+init_conf+",lima,g' "+local_conf_path)

            print("subprocess for analyzeText -l {} 10pc.brut from {}".format(
                lang, wd))
            my_env = os.environ.copy()
            my_env['LIMA_CONF'] = '{}:{}'.format(local_conf_dir,
                                                 my_env['LIMA_CONF'])
            my_env["LIMA_RESOURCES"] = '{}:{}:{}'.format(
                '{}/lima_linguisticdata/dist/share/apps/lima/resources'.format(
                    wd),
                wd,
                my_env["LIMA_RESOURCES"])
            print("LIMA_CONF: {}".format(my_env['LIMA_CONF']))
            print("LIMA_RESOURCES: {}".format(my_env['LIMA_RESOURCES']))
            processes.add(subprocess.Popen(['analyzeText', '-d', 'text', '-o',
                                            'text:.out', '-l', lang,
                                            '10pc.brut'],
                                           cwd=wd,
                                           env=my_env))
            if len(processes) >= max_processes:
                os.wait()
                for p in processes:
                    if p.poll() is not None and p.returncode is not 0:
                        raise Exception('analyzeText',
                                        'analyzeText did not return 0')
                processes.difference_update([
                    p for p in processes if p.poll() is not None])
        while processes:
            os.wait()
            for p in processes:
                if p.poll() is not None and p.returncode is not 0:
                    raise Exception('analyzeText',
                                    'analyzeText did not return 0')
            processes.difference_update([
                p for p in processes if p.poll() is not None])

    except:
        print("Erreur d'évaluation")
        raise


def FormaterPourAlignement(sep):
    """
    Put the two portions of annotated corpus (gold and test) into a format
    directly understandable by the aligner.
    """
    for i in range(1, numfold+1):
        with pushd('{}/{}'.format(results, str(i))):
            print(os.getcwd())
            os.system("gawk -F'|' '{print $2\"\t\"$3}' 10pc.brut.out|sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tL_NO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > test.tfcv")
            os.system("sed 's/ /_/g' 10pc.tfcv > gold.tfcv")

    with pushd('{}'.format(results)):
        print('    ==== formatForAlignement {}: {}'.format(sep,os.getcwd()))
        if os.system("gawk -F' ' '{print $2\"\t\"$4}' test.svmt.brut.out | sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tNO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > test.tfcv") > 0:
            raise RuntimeError('system call returned non zero value')
        if os.system("gawk -F' ' '{print $1\"\t\"$2}' test.svmt | sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tNO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > gold.tfcv") > 0:
            raise RuntimeError('system call returned non zero value')
        #if os.system('bash -c "set -o nounset -o errexit -o pipefail ; python3 {}/lima_linguisticdata/scripts/convert-ud-to-success-categ-retag.py --features=none test.svmt | sed -e\'s/ /_/g\' > gold.tfcv"'.format(os.environ['LIMA_SOURCES'])) > 0:
            #raise RuntimeError('system call returned non zero value')

def Aligner():
    for i in range(1, numfold+1):
        with pushd('{}/{}'.format(results, str(i))):
            print("\n\n ALIGNEMENT PARTITION "+str(i) + " - " + os.getcwd())
            os.system("%(path)s/aligner.pl gold.tfcv test.tfcv > aligned 2> aligned.log"
                      % {"path": PELF_BIN_PATH})


def checkConfig(conf):
    foundDumper = False
    method = 'none'

    with open(conf) as f:
        for i in range(80):
            line = f.readline()
            if line.strip() == '<item value="conllDumper"/>':
                foundDumper = True
            elif line.strip() == '<item value="viterbiPostagger-freq"/>':
                method = 'viterbi'
            elif line.strip() == '<item value="SvmToolPosTagger"/>':
                method = 'svmtool'
            elif line.strip() == '<item value="DynamicSvmToolPosTagger"/>':
                method = 'dynsvmtool'

    if not foundDumper:
        sys.exit(" ******* ConllDumper seems to not being activated! Stop... *******")
    elif method == 'none':
        raise Exception('No method found, was expecting Viterbi of SvmTool')
    else:
        return method


def makeTree():
    try:
        os.mkdir(results)
    except OSError:
        # ignored
        pass
    for i in range(1, numfold+1):
        try:
            os.mkdir(results + "/" + str(i))
        except OSError:
            # ignored
            pass


def trained(lang, tagger):
    return exists('training-sets/training.%s.%s' % (lang, tagger))


def main(corpus, conf, svmli, svmle, sep, lang_, clean, forceTrain):
    global lang, results
    lang = lang_
    # Configuration LIMA
    initial_config = "Disambiguation/SVMToolModel-"+lang+"/lima"
    conf_path = ""
    for apath in environ.get("LIMA_CONF").split(':'):
        conf_path = apath + "/lima-lp-"+lang+".xml"
        if path.isfile(conf_path):
            break
    tagger = checkConfig(conf_path)
    print("and the tagger is %s!" % tagger)
    # set up the global variables
    results = "results.%s.%s" % (lang, tagger)

    if clean:
        try:
            rmtree(results)
        except OSError:
            pass

    if forceTrain or not trained(lang, tagger):
        print("TRAINING !")
        try:
            rmtree(results)
        except:
            pass
        print(""" \n
        ==================================================
        ====         PoS-tagger Evaluation         ====
        ==================================================
        Data produced are available in results.%s.%s
        """ % (lang, tagger))
        print(" ******* CORPUS USED: "+corpus+" *******  \n")
        print(" ******* SEPARATOR: "+sep+" *******  \n")
        makeTree()
        TenPcSample(corpus, sep)
        NinetyPcSample()
        #BuildDictionary(lang)
        Tagged2raw()
        if (tagger == 'svmtool' or tagger == 'dynsvmtool'):
            SVMFormat()
            TrainSVMT(conf, svmli, svmle)
        elif (tagger == 'viterbi'):
            print("Disamb_matrices(SCRIPTS_PATH)")
            Disamb_matrices(SCRIPTS_PATH)
        # copy training data in another folder for later use
        try:
            copytree(results, "training-sets/training.%s.%s" % (lang, tagger))
        except:
            pass

    print("EVALUATION !")
    os.makedirs(results, exist_ok=True)
    if (tagger == 'svmtool' or tagger == 'dynsvmtool'):
        AnalyzeTextAllSVMT(initial_config, conf_path)
    elif (tagger == 'viterbi'):
        AnalyzeTextAll(MATRIX_PATH)

    FormaterPourAlignement(sep)
    Aligner()


parser = OptionParser()
parser.add_option("-c", "--clean", dest="clean", action="store_true",
                  default=False, help="Nettoie l'arborescence results\\")
parser.add_option("-t", "--forceTrain", dest="forceTrain", action="store_true",
                  default=False, help="Force l'apprentissage")
parser.add_option("-l", "--lang", dest="lang", action="store", default='eng',
                  help="langue de l'analyse")
parser.add_option("-s", "--sep", dest="sep", action="store",
                  default='PONCTU_FORTE', help="séparateur de phrases")
parser.add_option("-n", "--numfold", dest="numfold", action="store",
                  default='10', help="nombre de partitions")

(options, args) = parser.parse_args()
print(args)

numfold = int(options.numfold)
print(' ******* NUMFOLD: {} *******  \n'.format(numfold))
main(args[0], args[1], args[2], args[3],
     options.sep, options.lang,
     options.clean, options.forceTrain)
