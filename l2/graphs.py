import subprocess
import matplotlib.pyplot as plt

# subprocess.run(['make'], universal_newlines=True)

thread_count = []

time_taken = []
acceleration = []
efficiency = []

unnoise_factor = 20

for i in [1, 2, 4, 6, 8, 10, 12, 14, 16, 24, 30, 32, 40, 48, 56, 64, 76, 88, 100, 110, 128]:
    print(f'{i} threads...')
    thread_count.append(i);

    current_taken = []

    for j in range(unnoise_factor):
        output = subprocess.check_output(['./main.out', str(i)], universal_newlines=True)
        current_taken.append(float(output.strip()))

    current_taken.sort();

    taken = ((current_taken[unnoise_factor // 2] + current_taken[unnoise_factor // 2 + 1]) / 2) * 1000

    time_taken.append(taken)

    current_acceleration = time_taken[0] / taken

    acceleration.append(current_acceleration)
    efficiency.append(current_acceleration / i)


plt.plot(thread_count, time_taken, marker='o', linestyle='-')
plt.title('Time taken')

# plt.yscale('log')

plt.xlabel('Threads')
plt.ylabel('Time taken, milliseconds')

plt.savefig('plot_time_taken.png')


plt.figure()
plt.plot(thread_count, acceleration, marker='o', linestyle='-')
plt.title('Acceleration')

# plt.xscale('log')

plt.xlabel('Threads')
plt.ylabel('Acceleration')

plt.savefig('plot_acceleration.png')


plt.figure()
plt.plot(thread_count, efficiency, marker='o', linestyle='-')
plt.title('Efficiency')

# plt.xscale('log')

plt.xlabel('Threads')
plt.ylabel('Efficiency')

plt.savefig('plot_efficiency.png')
