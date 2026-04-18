import glob
import json
import os
import sys
from collections import defaultdict

config_example = '''
{
    "vectordef.h": [
        {
            "$NN": "INT", "$Nn": "Int", "$nn": "int", "$T": "int"
        },
        {
            "$NN": "LONG", "$Nn": "Long", "$nn": "long", "$T": "long"
        }
    ]
}
'''

def usage():
    print(f"""\
def -- define file converion procedure

Convert matched expressions as defined by a JSON file.

Usage: {sys.argv[0]} [-h, --help] [-f, --file FILENAME] [-o, --out OUTPUT]

Optional Parameters:
-h, --help; View this help and exit

Required Parameters:
-f, --file; Specify a JSON definition file
-o, --out;  Specify a result file to paste conversions into

Given a file name, a list of conversions may be created. This
list of conversions has dict of expressions to replace with
their replaced counterparts. For example:

{config_example}

This will convert vectordef.h twice; the first where each
$NN/Nn/nn/T expressions are replaced with INT/Int/int/int
respectively. The second follows this procedure, until the
nth definition has been performed.

At the end, all converted text is pasted into the output file.
""")

def arg_check(argv, da, ddarg):
    return da in argv or ddarg in argv

def arg_get(argv, da, ddarg):
    try:
        ndx = argv.index(da)
    except:
        try:
            ndx = argv.index(ddarg)
        except:
            return None
    if ndx + 1 < len(argv):
        return argv[ndx + 1]
    return None

def enforce(item, istype):
    if type(item) is not istype:
        print("%s is not a %s" % (item, str(istype)), file=sys.stderr)
        exit(1)

def read_json(filename):
    try:
        with open(filename, "r") as fp:
            return json.load(fp)
    except Exception as e:
        print("Could not read JSON %s: %s" % (filename, e), file=sys.stderr)
        exit(1)

def read_file(filename):
    try:
        with open(filename, "r") as fp:
            return fp.read()
    except:
        print("Could not read file %s" % (filename,), file=sys.stderr)
        exit(1)

def write_file(filename, stuff):
    try:
        with open(filename, "w") as fp:
            if type(stuff) is list:
                for text in stuff:
                    fp.write(text)
            elif type(stuff) is str:
                fp.write(stuff)
            else:
                raise NotImplementedError()
    except:
        print("Could not write file %s" % (filename,), file=sys.stderr)
        exit(1)

def text_replace(body: str, old: str, new) -> str:
    assert isinstance(body, str)
    assert isinstance(old, str)
    if isinstance(new, str):
        return body.replace(old, new)

    elif isinstance(new, list) or isinstance(new, tuple):
        new_formatted = "\n".join(new)
        return body.replace(old, new_formatted)

    else:
        raise NotImplementedError()

def main():
    file = None
    out = None

    if len(sys.argv) == 1:
        print("-- no options specified", file=sys.stderr)
        usage()
        return 1
    if arg_check(sys.argv, '-h', '--help'):
        usage()
        return 0
    tmp = arg_get(sys.argv, '-f', '--file')
    if tmp is not None:
        file = tmp
    else:
        print("Missing --file", file=sys.stderr)
        return 1
    tmp = arg_get(sys.argv, '-o', "--out")
    if tmp is not None:
        out = tmp
    else:
        print("Missing --out", file=sys.stderr)
        return 1

    obj = read_json(file)
    enforce(obj, dict)

    converted_texts = defaultdict(list)
    for filename, conversion_list in obj.items():
        enforce(filename, str)
        enforce(conversion_list, list)

        paths = glob.glob(filename)

        for path in paths:
            _, EXT = os.path.splitext(path)
            text = read_file(path)
            for conversion in conversion_list:
                enforce(conversion, dict)

                conversion["$__OUT_NAME__"] = os.path.basename(out)

                ptext = text
                for unconv, conv in conversion.items():
                    enforce(unconv, str)

                    ptext = text_replace(ptext, unconv, conv)
                converted_texts[EXT].append(ptext)

    for ext, content_list in converted_texts.items():
        write_file(f"{out}{ext}", content_list)
    return 0

if __name__ == '__main__':
    exit(main())
