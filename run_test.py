import subprocess
import glob


def run_test(test_path:str, test_pattern:str):
    all_item = glob.glob(test_path + test_pattern + "*")

    print("start",test_pattern,"test")
    
    fail_test_sum = 0

    for item in all_item:
        if item.endswith(".dump", 0) == False:
            result = subprocess.run(["./main.exe", item], stderr=subprocess.PIPE)
            #the test fails
            if result.returncode != 0:
                print(result.stderr.decode('utf-8'))
                print("return",result.returncode, "from",item)
                fail_test_sum += 1

    if fail_test_sum == 0:
        print("all pass")
    else:
        print(fail_test_sum, "tests fails")
    return


test_directory_path = "../../riscv-tests/isa/"
test_target_pattern_list = ["rv64ui-p-", "rv64um-p-", "rv64uc-p-"]


for pattern in test_target_pattern_list:
    run_test(test_directory_path, pattern)


