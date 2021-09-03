import sys
import os
from numpy.core.defchararray import isdigit


def zeros(n):
    zeros = ""
    for i in range(n):
        zeros += "0"
    return zeros


def format(binary, n=15):
    num_zeros = n - len(binary)
    return zeros(num_zeros) + binary


def convert_decimal_to_binary(number):
    number = number
    binary = ''
    while number > 0:
        binary = str(number & 1) + binary
        number = number >> 1
    return format(binary)


def convert_decimal_to_binary_(number):
    quotient = number
    binary = ""
    while True:
        reminder = quotient % 2
        quotient = quotient // 2
        binary = str(reminder) + binary
        if(quotient == 1 or quotient == 0):
            break

    reminder = quotient % 2
    quotient = quotient // 2
    binary = str(reminder) + binary

    return format(binary)


def get_label(line):
    return line.strip("(").strip(")")


def process_a_instruction(line, label_symbols, user_defined_symbols):
    # a instruction (address)
    # 0 v v v v v v v v v v v v v v v

    # predifined symbols
    predifined_symbols = {
        "SP":     0,
        "LCL":    1,
        "ARG":    2,
        "THIS":   3,
        "THAT":   4,
        "R0":     0,
        "R1":     1,
        "R2":     2,
        "R3":     3,
        "R4":     4,
        "R5":     5,
        "R6":     6,
        "R7":     7,
        "R8":     8,
        "R9":     9,
        "R10":    10,
        "R11":    11,
        "R12":    12,
        "R13":    13,
        "R14":    14,
        "R15":    15,
        "SCREEN": 16384,
        "KBD":    24576}

    num = -1
    if(line.isdigit()):
        num = int(line)
    else:
        label = get_label(line)
        if line in predifined_symbols:
            num = predifined_symbols[line]
        elif label in label_symbols:
            num = label_symbols[label]
        else:
            if line not in user_defined_symbols:
                if not user_defined_symbols:
                    user_defined_symbols[line] = 16
                else:
                    user_defined_symbols[line] = \
                        16 + len(user_defined_symbols)
            num = user_defined_symbols[line]

    return convert_decimal_to_binary(num)


def process_c_instruction(line):
    # c1 c2 c3 c4 c5 c6
    c_instructions = {
        "0":   "101010",
        "1":   "111111",
        "-1":  "111010",
        "D":   "001100",
        "A":   "110000",
        "M":   "110000",
        "!D":  "001101",
        "!A":  "110001",
        "!M":  "110001",
        "-D":  "001111",
        "-A":  "110011",
        "-M":  "110011",
        "D+1": "011111",
        "A+1": "110111",
        "M+1": "110111",
        "D-1": "001110",
        "A-1": "110010",
        "M-1": "110010",
        "D+A": "000010",
        "D+M": "000010",
        "D-A": "010011",
        "D-M": "010011",
        "A-D": "000111",
        "M-D": "000111",
        "D&A": "000000",
        "D&M": "000000",
        "D|A": "010101",
        "D|M": "010101"}

    # d1 d2 d3
    destinations = {
        "":    "000",
        "M":   "001",
        "D":   "010",
        "MD":  "011",
        "A":   "100",
        "AM":  "101",
        "AD":  "110",
        "AMD": "111"
    }

    # j1 j2 j3
    jumps = {
        "":     "000",
        "JGT":  "001",
        "JEQ":  "010",
        "JGE":  "011",
        "JLT":  "100",
        "JNE":  "101",
        "JLE":  "110",
        "JMP":  "111"}

    # c instruction (control)
    # 1 1 1 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
    comp = zeros(7)
    dest = zeros(3)
    jump = zeros(3)

    if("=" in line):
        # ex. D=A
        args = line.split("=")
        a_code = "1" if "M" in args[1] else "0"
        comp = a_code + c_instructions[args[1]]
        dest = destinations[args[0]]
    elif(";" in line):
        # ex. D;JGE
        args = line.split(";")
        jump = jumps[args[1]]
        comp = "0" + c_instructions[args[0]]

    return comp + dest + jump


def remove_comments(line):
    return line.split("//")[0].strip()


def process_line(line, label_symbols, user_defined_symbols):
    op_code = "0"
    tail = ""

    line = remove_comments(line)

    first_symbol = line[0]
    if(first_symbol == "@"):
        op_code = "0"
        tail = process_a_instruction(line[1:],
                                     label_symbols,
                                     user_defined_symbols)
    else:
        op_code = "111"
        tail = process_c_instruction(line)
    return op_code + tail


def is_instruction(line):
    return line != None and \
        len(line) > 0 and \
        not(line.startswith("//") or line.isspace())


def is_label_symbol(line):
    return line.startswith("(")


def first_pass(fp_read):
    instruction_number = 0
    label_symbols = {}
    for line in fp_read:
        if is_instruction(line):
            if(is_label_symbol(line)):
                label = get_label(remove_comments(line))
                label_symbols[label] = instruction_number
            else:
                instruction_number += 1

    return label_symbols


def second_pass(fp_read, fp_write, label_symbols):
    user_defined_symbols = {}
    for line in fp_read:
        if(is_instruction(line) and
           not is_label_symbol(line)):
            processed_line = process_line(line.strip(),
                                          label_symbols,
                                          user_defined_symbols)
            fp_write.write(processed_line + "\n")  # + os.linesep)


def assemble_file(input_file_path, output_file_path):
    with open(input_file_path, "r") as fp_read:
        label_symbols = first_pass(fp_read)
        fp_read.seek(0)
        with open(output_file_path, "w") as fp_write:
            second_pass(fp_read, fp_write, label_symbols)


def assemble(input_file_path):
    if(os.path.isfile(input_file_path)):
        absolute_input_file_path = os.path.abspath(input_file_path)

        dirname = os.path.dirname(absolute_input_file_path)
        output_file_name = f"{os.path.splitext(os.path.basename(input_file_path))[0]}.hack"

        assemble_file(absolute_input_file_path,
                      os.path.join(dirname, output_file_name))


if __name__ == "__main__":
    arguments_length = len(sys.argv)
    if(arguments_length == 0 or arguments_length == 1):
        print(f"Input hack file path is required!")
    elif(arguments_length == 2):
        input_file_path = sys.argv[1]
        assemble(input_file_path)
    else:
        print(f"Only one parameter is required - an input hack file path!")
