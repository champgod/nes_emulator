
import re
from itertools import zip_longest

def parse_state(line, reference=False):
    if reference:
        pc = line[:4]
    else:
        pc = re.search(r"PC:([0-9A-F]{4})", line).group(1)

    registers = dict(re.findall(
        r"(A|X|Y|P|SP):([0-9A-F]{2})", line
    ))

    return pc, registers


official = set()
unofficial = set()
passed = 0

with open("nestest.log") as expected_file, open("output.log") as actual_file:
    for instruction, (expected, actual) in enumerate(
        zip_longest(expected_file, actual_file), start=1
    ):
        if expected is None or actual is None:
            print(f"Log length mismatch at instruction {instruction}")
            break

        expected_state = parse_state(expected, reference=True)
        actual_state = parse_state(actual)

        if expected_state != actual_state:
            print(f"FAILED at instruction {instruction}")
            print(f"Expected: {expected_state}")
            print(f"Actual:   {actual_state}")
            print(f"Previous instruction: {previous_line.strip()}")
            break

        # The previous instruction produced this matching state.
        if passed > 0:
            opcode = previous_line[6:8]

            if previous_line[15:20].strip().startswith("*"):
                unofficial.add(opcode)
            else:
                official.add(opcode)

        passed += 1
        previous_line = expected

    else:
        print("All compared states matched!")


print("\n========== CPU TEST RESULTS ==========")
print(f"Matching CPU states: {passed}")
print(f"Official opcodes exercised: {len(official)} / 151")
print(f"Undocumented opcodes exercised: {len(unofficial)}")
print(f"Total unique opcodes exercised: {len(official | unofficial)}")
print("======================================")
