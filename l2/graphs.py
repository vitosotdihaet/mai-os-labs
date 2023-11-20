import subprocess
import matplotlib.pyplot as plt

subprocess.run(['make'], universal_newlines=True)

thread_count = []
time_taken = []

acceleration = []
efficiency = []

for i in [1, 2, 3, 4, 5, 10, 50, 100, 250, 500, 1000]:
    output = subprocess.check_output(['./main.out', str(i)], universal_newlines=True)

    thread_count.append(i);

    taken = float(output.strip()) / 1000
    time_taken.append(taken)

    current_acceleration = time_taken[0] / taken
    acceleration.append(current_acceleration)
    efficiency.append(current_acceleration / i)


plt.plot(thread_count, time_taken, marker='o', linestyle='-')
plt.title('Time taken')

plt.xscale('log')
plt.yscale('log')

plt.xlabel('Threads')
plt.ylabel('Time taken, milliseconds')

plt.savefig('plot_time_taken.png')


plt.figure()
plt.plot(thread_count, acceleration, marker='o', linestyle='-')
plt.title('Acceleration')

plt.xscale('log')
plt.yscale('log')

plt.xlabel('Threads')
plt.ylabel('Acceleration')

plt.savefig('plot_acceleration.png')


plt.figure()
plt.plot(thread_count, efficiency, marker='o', linestyle='-')
plt.title('Efficiency')

plt.xscale('log')

plt.xlabel('Threads')
plt.ylabel('Efficiency')

plt.savefig('plot_efficiency.png')

# plt.show()
