import subprocess

for n in [10, 100, 1000, 10000]:
    for threads_num in [1, 3, 6, 12, 24, 48, 96, 192]:
        result = subprocess.run(["./output/main", f"{n} {threads_num}"], check=False, capture_output=True, text=True)