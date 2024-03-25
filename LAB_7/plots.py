import matplotlib.pyplot as plt
import numpy as np

# Function to calculate the fraction of valid virtual addresses
def calculate_valid_fraction(seed, bounds_values, address_space_size, physical_memory_size, num_addresses):
    valid_fractions = []
    for bounds in bounds_values:
        valid_count = 0
        for _ in range(num_addresses):
            # Generate random virtual addresses
            virtual_address = int(address_space_size * np.random.random())
            # Check if virtual address is within bounds
            if virtual_address < bounds:
                valid_count += 1
        valid_fraction = valid_count / num_addresses
        valid_fractions.append(valid_fraction)
    return valid_fractions

# Set up parameters
num_addresses = 1000
address_space_size = 1024  # 1k address space size
physical_memory_size = 16384  # 16k physical memory size
bounds_values = range(0, address_space_size + 1)  # Range of bounds register values
random_seeds = [1, 2, 3]  # Random seeds for different runs

# Plotting
plt.figure(figsize=(10, 6))
for seed in random_seeds:
    valid_fractions = calculate_valid_fraction(seed, bounds_values, address_space_size, physical_memory_size, num_addresses)
    plt.plot(bounds_values, valid_fractions, label=f"Seed {seed}")

# Add labels and title
plt.xlabel('Bounds Register Value')
plt.ylabel('Fraction of Valid Virtual Addresses')
plt.title('Fraction of Valid Virtual Addresses vs. Bounds Register Value')
plt.legend()
plt.grid(True)
plt.show()
