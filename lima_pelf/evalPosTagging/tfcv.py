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


def SVMFormat(files, strip_size):
    print('SVMFormat({}, {})'.format(files, strip_size))
    for file in files.items():
        if os.system('bash -c "set -o nounset -o errexit -o pipefail ; python3 {3}/lima_linguisticdata/scripts/convert-ud-to-success-categ-retag.py --features=none {2} | sed -e\'s/ /_/g\' -e\'s/\t/ /g\' > {0}/{1}.svmt"'.format(
          results, file[0], file[1], os.environ['LIMA_SOURCES'], strip_size)) > 0:
            raise RuntimeError('system call returned non zero value')
    if strip_size != math.inf:
        print('bash -c "set -o nounset -o errexit -o pipefail ; head -n {1} {0}/train.svmt > {0}/train.svmt.s ; mv {0}/train.svmt.s {0}/train.svmt"'.format(results, strip_size))
        if os.system('bash -c "set -o nounset -o errexit -o pipefail ; head -n {1} {0}/train.svmt > {0}/train.svmt.s ; mv {0}/train.svmt.s {0}/train.svmt"'.format(results, strip_size)) > 0:
            raise RuntimeError('system call returned non zero value')


def Tagged2raw(testFile):
    """
    Product raw equivalent () text ready to be analyzed) of all 10% samples
    produced by TenPcSample(path).
    Organizes samples in numbered folders in results.<lang>.<tagger>
    """

    print("*** Producing raw equivalent of test file {0}/test.svmt".format(results))
    with open('{0}/test.svmt.brut'.format(results), 'w',
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
    wd = results + ""
    print('\n***  Build dictionary ***')
    print('running {}/build-dico.sh {}'.format(PELF_BIN_PATH, language))
    subprocess.run(
        ['{}/build-dico.sh'.format(PELF_BIN_PATH), language],
        cwd=wd,
        check=True)


def trainSVMT(conf, svmli, svmle):
    """
    Produces models for each sample.
    """
    wd = '{}/{}'.format(os.getcwd(), results)
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
        wd = '{}/{}'.format(os.getcwd(), results)
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

    with pushd('{}'.format(results)):
        print('    ==== formatForAlignement {}: {}'.format(sep,os.getcwd()))
        if os.system("gawk -F' ' '{print $2\"\t\"$4}' test.svmt.brut.out | sed -e 's/\t.*#/\t/g' -e 's/ $//g' -e 's/\t$/\tNO_TAG/g' -e 's/^ //g' -e 's/ \t/\t/g'| tr \" \" \"_\" > test.tfcv") > 0:
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
    with pushd('{}'.format(results)):
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
            elif line.strip() == '<item value="SvmToolPosTagger"/>' or line.strip() == '<item value="PerlSvmToolPosTagger"/>' :
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
        os.mkdir(results + "")
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
