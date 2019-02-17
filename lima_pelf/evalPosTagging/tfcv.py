#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

# ----------------------------------------------------------------------
# File    : tfcv.py
# Type    : Python
# Contains: Ten Fold Cross Validation, a tool for PoS-tagger validation
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
import math


# Variables definition
PELF_BIN_PATH = path.join(environ.get('LIMA_DIST', '/usr/local'),
                          'share/apps/lima/scripts')

# svn blame material:
# let's use global variables for those infos because:
#  - we don't want to have to send them down to every function
#  - it's really something global that is never going to change

lang = 'none'
results = 'results.none.none'


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


def TenPcSample(path, sep, strip_size):
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
        if lines > strip_size:
            lines = strip_size
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
            if cnt > strip_size:
                break
            s.write(line)
            if(cnt > partition_size*num[0] and line.find(sep) != -1):
                s.close()
                num = num[1:]
                if num:
                    s = open(results + "/%d/10pc.tfcv" % num[0], 'w',
                             encoding='utf-8', newline='\n')
        s.close()


def SVMFormat():
    for i in range(1, numfold+1):
        os.system("sed -i -e's/ /_/g' -e's/\t/ /g' {}/{}/90pc.tfcv ".format(
            results, i))
        os.system("sed -e's/ /_/g' -e's/\t/ /g' {}/{}/10pc.tfcv > 10pc.svmt".format(
            results, i))
        #os.system('bash -c "python3 {0}/{1}/lima_linguisticdata/scripts/convert-ud-to-success-categ-retag.py {0}/{1}/90pc.tfcv | sed -e\'s/ /_/g\' -e\'s/\t/ /g\' > {0}/{1}/90pc.tfcv.svmt"'.format(results, i))


def NinetyPcSample():
    """
    Build the complement of the 10% partitions produced by TenPcSample(path) by
    just concatenating, for each 10% sample, the 9 other 10% samples.
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

    print("*** Producing raw equivalent of test file {0}/1/test.svmt".format(results))
    with open('{0}/1/test.svmt.brut'.format(results), 'w',
              encoding='utf-8', newline='\n') as outfile:
        print('{}/conllu_to_text.pl {}'.format(PELF_BIN_PATH, testFile))
        subprocess.run(
            ['perl', '{}/conllu_to_text.pl'.format(PELF_BIN_PATH), testFile],
            stdout=outfile,
            check=True)


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
    wd = '{}/{}/1'.format(os.getcwd(), results)
    str_wd = wd.replace("/", "\/")
    str_svmli = svmli.replace("/", "\/")
    print("\n---  Train {} {} {} --- ".format(conf, svmli,svmle))
    if 'PERL5LIB' in os.environ:
        os.environ['PERL5LIB'] = '{}/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/lib:{}'.format(
            os.environ['LIMA_SOURCES'], os.environ['PERL5LIB'])
    else:
        os.environ['PERL5LIB'] = '{}/lima_pelf/evalPosTagging/SVM/SVMTool-1.3.1/lib'.format(
            os.environ['LIMA_SOURCES'])

    if os.system("sed -e 's/%SAMPLE-PATH%/{}/g' -e 's/%SVM-DIR%/{}/g' {} > {}/config.svmt".format(
        str_wd, str_svmli, conf, wd)) > 0:
        raise RuntimeError('system call returned non zero value')
    svmlestring = "{}/config.svmt".format(wd)
    print("\t**Learning model... {} {}".format(svmle, svmlestring))
    subprocess.run([svmle, svmlestring], cwd=wd, check=True)


def analyzeTextAllSVMT(init_conf, conf_path):
    """
    Tag the test corpus (10%) with the models obtained after applying SVMTlearn
    to the complementary partition (90%).
    """

    try:
        print("    ==== SVMTool analysis: {}, {}".format(
            init_conf, conf_path))
        wd = '{}/{}/1'.format(os.getcwd(), results)
        local_conf_dir = '{}/conf'.format(wd)
        local_conf_path = '{}/lima-lp-{}.xml'.format(local_conf_dir, lang)
        os.makedirs(local_conf_dir, exist_ok=True)
        copy(conf_path, local_conf_path)
        if os.system("sed -i 's,"+init_conf+",lima,g' "+local_conf_path) > 0:
            raise RuntimeError('system call returned non zero value')

        print("subprocess for analyzeText -l {} test.svmt.brut from {}".format(
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
        subprocess.run(['analyzeText', '-d', 'text', '-o',
                                        'text:.out', '-l', lang,
                                        'test.svmt.brut'],
                       cwd=wd,
                       env=my_env,
                       check=True)
    except:
        print("Erreur d'évaluation")
        raise


def formatForAlignement(sep):
    """
    Put the two portions of annotated corpus (gold and test) into a format
    directly understandable by the aligner.
    """

    with pushd('{}/1'.format(results)):
        print('    ==== formatForAlignement {}: {}'.format(sep,os.getcwd()))
        if os.system("gawk -F' ' '{print $3\"\t\"$5}' test.svmt.brut.out | sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tNO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > test.tfcv") > 0:
            raise RuntimeError('system call returned non zero value')
        if os.system("gawk -F' ' '{print $1\"\t\"$2}' test.svmt | sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tNO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > gold.tfcv") > 0:
            raise RuntimeError('system call returned non zero value')
        #if os.system('bash -c "set -o nounset -o errexit -o pipefail ; python3 {}/lima_linguisticdata/scripts/convert-ud-to-success-categ-retag.py --features=none test.svmt | sed -e\'s/ /_/g\' > gold.tfcv"'.format(os.environ['LIMA_SOURCES'])) > 0:
            #raise RuntimeError('system call returned non zero value')


    with pushd('{}'.format(results)):
        print('    ==== formatForAlignement {}: {}'.format(sep,os.getcwd()))
        if os.system("gawk -F' ' '{print $2\"\t\"$4}' test.svmt.brut.out | sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tNO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > test.tfcv") > 0:
            raise RuntimeError('system call returned non zero value')
        if os.system("gawk -F' ' '{print $1\"\t\"$2}' test.svmt | sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tNO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > gold.tfcv") > 0:
            raise RuntimeError('system call returned non zero value')
        #if os.system('bash -c "set -o nounset -o errexit -o pipefail ; python3 {}/lima_linguisticdata/scripts/convert-ud-to-success-categ-retag.py --features=none test.svmt | sed -e\'s/ /_/g\' > gold.tfcv"'.format(os.environ['LIMA_SOURCES'])) > 0:
            #raise RuntimeError('system call returned non zero value')

def align():
    with pushd('{}/1'.format(results)):
        print("\n\n ALIGNEMENT" + " - " + os.getcwd())
        if os.system("%(path)s/aligner.pl gold.tfcv test.tfcv > aligned 2> aligned.log"
                  % {"path": PELF_BIN_PATH}) > 0:
            raise RuntimeError('system call returned non zero value')


def checkConfig(conf):
    foundDumper = False
    method = 'none'

    with open(conf) as f:
        for i in range(80):
            line = f.readline()
            if line.strip() == '<item value="conllDumper"/>':
                foundDumper = True
            elif line.strip() == '<item value="SvmToolPosTagger"/>':
                method = 'svmtool'

    if not foundDumper:
        sys.exit(" ******* ConllDumper seems to not being activated! Stop... *******")
    elif method == 'none':
        raise Exception('No method found, was expecting Viterbi of SvmTool')
    else:
        return method


def makeTree():
    try:
        os.mkdir(results)
        os.mkdir(results + "/1")
    except OSError:
        # ignored
        pass


def trained(lang, tagger):
    return exists('training-sets/training.%s.%s' % (lang, tagger))


def main(corpus, conf, svmli, svmle, sep, lang_, clean, forceTrain, strip_size):
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

    train = '{}train.conllu'.format(corpus)
    dev = '{}dev.conllu'.format(corpus)
    test = '{}test.conllu'.format(corpus)
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
        print(" ******* CORPUS USED: "+train+" *******  \n")
        print(" ******* SEPARATOR: "+sep+" *******  \n")
        makeTree()
        #TenPcSample(corpus, sep, strip_size)
        #NinetyPcSample()
        SVMFormat({'train':train,'dev':dev,'test':test}, strip_size)
        Tagged2raw(test)
        BuildDictionary(lang)
        trainSVMT(conf, svmli, svmle)
        # copy training data in another folder for later use
        try:
            copytree(results, "training-sets/training.%s.%s" % (lang, tagger))
        except:
            pass

    print("EVALUATION !")
    os.makedirs(results, exist_ok=True)
    analyzeTextAllSVMT(initial_config, conf_path)

    formatForAlignement(sep)
    align()


parser = OptionParser()
parser.add_option("-c", "--clean", dest="clean", action="store_true",
                  default=False, help="Clean up results tree")
parser.add_option("-f", "--fragment-size", dest="fragment", action="store",
                  default='infinity', help="Size of the learning corpus fragment to use (defaults to infinity, thus whole corpus)")
parser.add_option("-l", "--lang", dest="lang", action="store",
                  help="Analysis language")
parser.add_option("-s", "--sep", dest="sep", action="store",
                  default='PUNCT', help="Sentences separator")
parser.add_option("-t", "--forceTrain", dest="forceTrain", action="store_true",
                  default=False, help="Force to learn")

(options, args) = parser.parse_args()
print(args)

strip_size = 0
if options.fragment == 'infinity':
    strip_size = math.inf
else:
    strip_size = int(options.fragment)

main(args[0], args[1], args[2], args[3],
     options.sep,
     options.lang,
     options.clean,
     options.forceTrain,
     strip_size)
