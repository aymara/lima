import argparse
import re
import sys
from collections import defaultdict

from tqdm import tqdm


def add_reflexive_verb_rule(abs_type, trigger, exp, trigger_is_verb,
                            rules_file):
    match = re.match(r"(s'|se)(.*) (\[V\]\&?[^ ]*)(.*)", exp)
    if match:
        pron, pre, verb, post = match.groups()
        pre = pre.strip()
        post = post.strip()

        pre = re.sub(r'\(pp\)', '', pre)
        post = re.sub(r'\(D\)', '[($ADV?)]', post)

        with open(rules_file, 'a') as frules:
            if trigger_is_verb:
                trigger = re.sub(r'\[V\]', '', trigger) + '$V'
                frules.write(f"{trigger}:$PRON-CLR")
                if pre:
                    frules.write(
                        f"(({pre} [$PRON?] [@AuxiliaireEtre ($ADV){{0-2}}])"
                        f"|([$PRON?] [(@AuxiliaireEtre ($ADV){{0-2}})?] "
                        f"{pre}))")
                else:
                    frules.write('([$PRON?] [@AuxiliaireEtre ($ADV){0-2})?])')
                frules.write(f":{post}:ABS_IDIOM$V---p---:\n")
                frules.write(
                    '+AgreementConstraint(trigger.1,left.1,"NUMBER")\n')
                frules.write(
                    '+AgreementConstraint(trigger.1,left.1,"PERSON")\n')
            else:
                verb = re.sub(r'\[V\]', '', verb) + '$V'
                quotemeta_trigger = re.escape(trigger)
                left, right = '', ''

                if re.search(f"(.*)(^| ){quotemeta_trigger}( |$)(.*)", pre):
                    left, right = re.split(
                        f"(.*)(^| ){quotemeta_trigger}( |$)(.*)", pre)[0::3]
                elif re.search(f"(.*)(^| ){quotemeta_trigger}( |$)(.*)", post):
                    left, right = re.split(
                        f"(.*)(^| ){quotemeta_trigger}( |$)(.*)", post)[0::3]

                if left or right:
                    frules.write(f"{trigger}:$PRON-CLR {left if left else ''}:"
                                 f"{right if right else ''} [$PRON?] "
                                 f"[(@AuxiliaireEtre ($ADV){{0-2}})] {verb}:"
                                 f"{abs_type.upper()}_IDIOM$V---p---:\n")
                    frules.write(f'+AgreementConstraint('
                                 f'left.1,right.{4 if left else 3},'
                                 f'"NUMBER")\n')
                    frules.write(f'+AgreementConstraint('
                                 f'left.1,right.{4 if left else 3},'
                                 f'"PERSON")\n')
                else:
                    sys.stderr.write(f"Warning: cannot find trigger "
                                     f"[{trigger}] in expression \"{exp}\"\n")
    else:
        sys.stderr.write(f"Warning: failed to parse complex reflexive verb "
                         f"rule in expression \"{exp}\"\n")


def main():
    parser = argparse.ArgumentParser(
        description='Extract a list of reflexive verbs from the list of '
                    'idiomatic expressions and store them in a file as a '
                    'class used for automaton rules.')
    parser.add_argument('idiom_file', type=argparse.FileType('r'),
                        help='Input file containing idiomatic expressions')
    parser.add_argument('-list', required=True, help='Output file for lists')
    parser.add_argument('-rules', required=True, help='Output file for rules')

    args = parser.parse_args()

    reflexive_verbs = defaultdict(list)
    idiom_lines = args.idiom_file.readlines()

    with tqdm(total=len(idiom_lines),
              desc="Processing idiomatic expressions") as pbar:
        for line in idiom_lines:
            pbar.update(1)
            if ';verbe pronominal;' in line:
                match = re.match(
                    r'^[^;]*;[^;]*;([^;]*);([^;]*);([^;]*);', line)
                if match:
                    abs_type, trigger, exp = match.groups()
                    abs_type = 'C' if abs_type == '' else abs_type
                    if '[V]' in trigger:
                        if exp in [f"s' (pp) {trigger}", f"se (pp) {trigger}"]:
                            trigger = re.sub(r'\[V\]\&?', '', trigger)
                            reflexive_verbs[abs_type].append(trigger)
                        else:
                            add_reflexive_verb_rule(abs_type, trigger, exp,
                                                    True, args.rules)
                    else:
                        if re.search(r'\[V\]\&?([^ ]*) ', exp):
                            add_reflexive_verb_rule(abs_type, trigger, exp,
                                                    False, args.rules)
                        else:
                            sys.stderr.write(
                                f"Warning: did not know how to deal with "
                                f"pronominal expression {exp}\n")
                            sys.stdout.write(line)
                else:
                    sys.stdout.write(line)
            else:
                sys.stdout.write(line)

    with open(args.list, 'w') as flist:
        if 'C' in reflexive_verbs:
            rv = ',\n'.join(f'{verb}$V' for verb in reflexive_verbs['C'])
            flist.write(f"@ReflexiveVerbs=({rv}$V)\n\n")
        else:
            sys.stderr.write(
                "Warning: no contextual rule for pronominal verb\n")

        if 'A' in reflexive_verbs:
            rv = ',\n'.join(f'{verb}$V' for verb in reflexive_verbs['A'])
            flist.write(f"@AbsReflexiveVerbs=({rv}$V)\n\n")
        else:
            sys.stderr.write("Warning: no absolute rule for pronominal verb\n")


if __name__ == "__main__":
    main()
