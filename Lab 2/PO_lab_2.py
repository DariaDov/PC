import subprocess
import matplotlib.pyplot as plt

# threads_num = 6
n_range = [10000, 100000, 1000000, 10000000, 100000000]
n_thread = [1, 3, 6, 12]

for threads_num in n_thread:
    if threads_num != 1:
        npoints = []
        timepoints = []

        for n in n_range:
            result = subprocess.run([f"/home/daria/ПО/PO_lab_2/build/PO_lab_2 {n} {threads_num}"], shell=True, check=False, capture_output=True, text=True)
            print(result)
                
            npoints.append(n)
            timepoints.append(int(result.stdout.split('\n')[0]))

        plt.plot(npoints, timepoints, label=f"mutex ({threads_num} threads)")

        npoints = []
        timepoints = []

        for n in n_range:
            result = subprocess.run([f"/home/daria/ПО/PO_lab_2/build/PO_lab_2_1 {n} {threads_num}"], shell=True, check=False, capture_output=True, text=True)
            print(result)
                
            npoints.append(n)
            timepoints.append(int(result.stdout.split('\n')[0]))

        plt.plot(npoints, timepoints, label=f"atomic ({threads_num} threads)")

    else:
        npoints = []
        timepoints = []

        for n in n_range:
            result = subprocess.run([f"/home/daria/ПО/PO_lab_2/build/PO_lab_2 {n} {1}"], shell=True, check=False, capture_output=True, text=True)
            print(result)
                
            npoints.append(n)
            timepoints.append(int(result.stdout.split('\n')[0]))

        plt.plot(npoints, timepoints, label="1 thread")

plt.legend() 
plt.xscale('log')
plt.show()