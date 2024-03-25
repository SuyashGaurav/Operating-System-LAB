import subprocess
import matplotlib.pyplot as plt
import sys

# Command-line arguments
num_pages = int(sys.argv[1])
start_frames = int(sys.argv[2])
end_frames = int(sys.argv[3])
num_blocks = int(sys.argv[4])
input_file = sys.argv[5]

# Generate total frames range
total_frames = range(start_frames, end_frames + 1)

# Arrays to store page fault values for FIFO, LRU, and Random algorithms
page_faults_FIFO = []
page_faults_LRU = []
page_faults_Random = []

# Run the three C++ programs for each total number of frames
for frames in total_frames:
    # Run the FIFO.cpp program
    cmd_FIFO = ['fifo.exe', str(num_pages), str(frames), str(num_blocks), input_file]
    output_FIFO = subprocess.check_output(cmd_FIFO,shell=True)
    page_faults_FIFO.append(int(output_FIFO.decode('utf-8').strip()))

    # Run the LRU.cpp program
    cmd_LRU = ['lru.exe', str(num_pages), str(frames), str(num_blocks), input_file]
    output_LRU = subprocess.check_output(cmd_LRU, shell=True)
    page_faults_LRU.append(int(output_LRU.decode('utf-8').strip()))

    # Run the Random.cpp program
    cmd_Random = ['random.exe', str(num_pages), str(frames), str(num_blocks), input_file]
    output_Random = subprocess.check_output(cmd_Random, shell=True)
    page_faults_Random.append(int(output_Random.decode('utf-8').strip()))

# Plotting the graph
plt.plot(total_frames, page_faults_FIFO, label="FIFO")
plt.plot(total_frames, page_faults_LRU, label="LRU")
plt.plot(total_frames, page_faults_Random, label="Random")
plt.xlabel("Total Number of Frames")
plt.ylabel("Number of Page Faults")
plt.title("Page Replacement Algorithm Performance")
plt.legend()
plt.grid(True)
plt.show()
