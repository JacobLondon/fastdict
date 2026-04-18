import sys
from collections import defaultdict
import shlex

def arg_get(dash, args, default=None):
    for i, val in enumerate(args):
        if dash == val:
            if i+1 < len(args):
                return args[i+1]
            else:
                return default
    return default

def arg_must_get(dash, args):
    sentinel = object()
    a = arg_get(dash, args, sentinel)
    if a is sentinel:
        raise LookupError(f"Cannot find argument {dash}")
    return a

def arg_check(dash, args):
    if isinstance(dash, tuple) or isinstance(dash, list):
        for d in dash:
            if d in args:
                return True
        return False
    else:
        return dash in args

def dict_conversion(filename, args):
    kname = arg_must_get("-kname", args)
    ktype = arg_must_get("-ktype", args)
    vname = arg_must_get("-vname", args)
    vtype = arg_must_get("-vtype", args)
    equalkeys = arg_must_get("-kequals", args)

    keyisptr = arg_check("-kptr", args)
    keyissmall = arg_check("-ksmall", args)

    freekey = arg_get("-kfree", args, "NULL")
    zerokey = arg_get("-kzero", args, "NULL")
    freevalue = arg_get("-vfree", args, "NULL")
    zeroval = arg_get("-vzero", args, "NULL")

    malloc = arg_get("-malloc", args, "malloc")
    realloc = arg_get("-realloc", args, "realloc")
    free = arg_get("-free", args, "free")

    refkey = arg_get("-kref", args, "NULL")

    SENTINEL = object()
    _h_include_filenames = arg_get("-hincludes", args, SENTINEL)
    if _h_include_filenames is not SENTINEL:
        h_include_filenames = _h_include_filenames.split(",")
        h_includes = list(map(lambda filename: f"#include \"{filename}\"", h_include_filenames))
    else:
        h_includes = []

    _c_include_filenames = arg_get("-cincludes", args, SENTINEL)
    if _c_include_filenames is not SENTINEL:
        c_include_filenames = _c_include_filenames.split(",")
        c_includes = list(map(lambda filename: f"#include \"{filename}\"", c_include_filenames))
    else:
        c_includes = []

    if arg_check("-hashtrivial", args):
        hash_trivial = True
        hash_use_default = True
        hash_non_default = ""
    elif arg_check("-hashcustom", args):
        hash_trivial = False
        hash_use_default = False
        hash_non_default = arg_must_get("-hashcustom", args)
    else:
        hash_trivial = False
        hash_use_default = True
        hash_non_default = ""

    block = {
        "$TT": kname.upper(),
        "$Tt": kname.title(),
        "$tt": kname.lower(),
        "$K": ktype,
        "$VV": vname.upper(),
        "$Vv": vname.title(),
        "$vv": vname.lower(),
        "$U": vtype,
        "$REFKEY": refkey,
        "$FREEKEY": freekey,
        "$FREEVALUE": freevalue,
        "$SIZEOFKEY": f"sizeof({ktype})",
        "$ZEROKEY": zerokey,
        "$ZEROVALUE": zeroval,
        "$EQUAL_KEYS": equalkeys,
        "$OPT_KEY_IS_TRIVIALLY_HASHABLE": "true" if hash_trivial else "false",
        "$OPT_KEY_IS_PTR": "true" if keyisptr else "false",
        "$OPT_KEY_IS_SMALLER_THAN_SIZE_T": "true" if keyissmall else "false",
        "$OPT_HASH_USE_DEFAULT": "true" if hash_use_default else "false",
        "$OPT_HASH_NON_DEFAULT_FUNC": hash_non_default,
        "$MALLOC": malloc,
        "$REALLOC": realloc,
        "$FREE": free,
        "$H_INCLUDES": h_includes,
        "$C_INCLUDES": c_includes,

        "$__OUT_NAME__": filename,
    }
    return block

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

def enforce_schema(block):
    assert isinstance(block, dict)
    for filename, conversion_list in block.items():
        assert isinstance(filename, str)
        assert isinstance(conversion_list, list)
        for conversion in conversion_list:
            enforce_schema_single(conversion)

def enforce_schema_single(conversion):
    assert isinstance(conversion, dict)
    for unconv, conv in conversion.items():
        assert isinstance(unconv, str)

def convert_single(template, conversion):
    assert isinstance(template, str)
    enforce_schema_single(conversion)

    ptext = template
    for unconv, conv in conversion.items():
        ptext = text_replace(ptext, unconv, conv)
    return ptext

def run_args(outfilename, args, cout, hout):

    template_filename = arg_get("--template", args, None)

    with open(f"{template_filename}.c", "r") as fp:
        ctemplate = fp.read()

    with open(f"{template_filename}.h", "r") as fp:
        htemplate = fp.read()

    conversion = dict_conversion(outfilename, args)

    converted = convert_single(ctemplate, conversion)
    cout.append(converted)

    converted = convert_single(htemplate, conversion)
    hout.append(converted)

def usage():
    pass

def main():
    if arg_check(("-h", "--help"), sys.argv):
        usage()

    output = arg_must_get("--output", sys.argv)
    assert isinstance(output, str)

    ctxt = []
    htxt = []
    definition = arg_must_get("-f", sys.argv)
    with open(definition, "r") as fp:
        for line in fp.readlines():
            args = shlex.split(line)
            run_args(output, args, ctxt, htxt)

    with open(f"{output}.c", "w") as fp:
        for txt in ctxt:
            fp.write(txt)
            fp.write("\n")

    with open(f"{output}.h", "w") as fp:
        for txt in htxt:
            fp.write(txt)
            fp.write("\n")

    return 0

if __name__ == '__main__':
    exit(main())
