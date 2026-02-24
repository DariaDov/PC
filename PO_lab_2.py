import subprocess
import matplotlib.pyplot as plt

for n in [10000, 100000, 1000000, 10000000, 100000000]:
    threadpoints = []
    timepoints = []

    for threads_num in [1, 3, 6, 12, 24]:
        result = subprocess.run([f"/home/daria/ПО/PO_lab_2/build/PO_lab_2 {n} {threads_num}"], shell=True, check=False, capture_output=True, text=True)
        print(result)
        
        threadpoints.append(threads_num)
        timepoints.append(int(result.stdout.split('\n')[0]))

    plt.plot(threadpoints, timepoints,  label=f'{n}')

plt.legend() 
# plt.xscale('log')
plt.show()