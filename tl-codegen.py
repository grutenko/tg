from lark import Lark, Transformer, v_args
from dataclasses import dataclass
from typing import List

tl_grammar = r"""
start: (int_line | long_line | double_line | string_line | int128_line | int256_line | vector_line | functions_line | line)*


generic_params: "{" generic_param "}"
generic_param: NAME ":" "Type"

functions_line: "---functions---"
vector_line: "vector" HASH? generic_params HASH_SYMBOL list_type "=" return_type ";"
int_line: "int" "?" "=" "Int" ";"
long_line: "long" "?" "=" "Long" ";"
double_line: "double" "?" "=" "Double" ";"
string_line: "string" "?" "=" "String" ";"
int128_line: "int128" INT "*" list_type "=" "Int128" ";"
int256_line: "int256" INT "*" list_type "=" "Int256" ";"
line: NAME HASH? args? "=" return_type ";"

HASH: HASH_SYMBOL HEX
HASH_SYMBOL: "#"
DOT: "."
QMARK: "?"
BARE: "%"

list_type: "[" type "]"

args: arg+
arg: NAME ":" type

type: (type_component DOT INT [QMARK])? type_component

type_component: BARE? (HASH_SYMBOL | NAME ("<" type_component ">")?)

return_type: type NAME?

NAME: /[a-zA-Z_][a-zA-Z0-9_]*/
HEX: /[0-9a-fA-F]+/
INT: /\d+/

%ignore " "
%ignore "\t"
%ignore /\r?\n/

"""

@dataclass
class TypeImpl:
    name: str
    generic: List['TypeImpl']

@dataclass
class Arg:
    name: str
    optional: bool
    flag_arg: int = None
    flag_arg_offset: int = None


@dataclass
class Constructor:
    crc32: int
    name: str
    args: List[Arg]
    ret: TypeImpl

@v_args(inline=True)
class TLTransformer(Transformer):
    def start(self, *lines):
        return list(lines)

    def functions_line(self, *args):
        return None

    def int_line(self, *args):
        return None

    def long_line(self, *args):
        return None

    def double_line(self, *args):
        return None

    def string_line(self, *args):
        return None

    def int128_line(self, *args):
        return None

    def int256_line(self, *args):
        return None

    def vector_line(self, *args):
        # Просто игнорируем vector_line
        return None

    def line(self, name, hash_, args, return_type = None):
        print(name, hash_, args)
        return {
            "name": str(name),
            "id": str(hash_[1:]) if hash_ else None,
            "args": args or [],
            "return_type": str(return_type)
        }

    def HASH(self, token):
        return token

    def args(self, *args):
        return list(args)

    def arg(self, name, type_):
        return {"name": str(name), "type": type_}

    def flag_type(self, flags_num, type_):
        return {"flags_bit": int(flags_num), "type": str(type_)}

    def flag_flag_only(self, flags_num):
        return {"flags_bit": int(flags_num), "type": None}

    def return_type(self, *args):
        return str(args[0])

    def NAME(self, token):
        return str(token)

    def TYPE(self, token):
        return str(token)

    def INT(self, token):
        return int(token)

    def HEX(self, token):
        return str(token)

parser = Lark(tl_grammar, parser='lalr', transformer=TLTransformer())

# Пример
with open("mtproto.tl", "r") as f:
    tl_text = f.read()

parsed = parser.parse(tl_text)
from pprint import pprint
pprint(parsed)