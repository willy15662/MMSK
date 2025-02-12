# M/M/s/k Queue Simulation & Mathematical Analysis

## Overview
This project simulates an **M/M/s/k queue** and compares the simulation results with mathematical analysis. The simulation is implemented in **C language**, while the charts are generated using **Excel**.

## System Parameters
- **Number of servers (S):** 3
- **Queue capacity (K):** 8
- **Service rate (μ):** 2
- **Arrival rate (λ):** 1 to 10 (incremental testing)

## Key Findings
- The average error between simulation and mathematical analysis is **0.217%**.
- When **λ < S**, the error increases significantly. This is likely because new arrivals are immediately served, but arrival times are random. This variation increases system wait time, reducing the number of people served per hour (**L value**), leading to higher errors.
- When **λ > S**, the error decreases substantially, indicating that the model is accurate for higher loads.

## Simulation Methodology
The simulation program consists of two main parts: **arrival** and **departure**.

### Arrival Process
1. If the next arrival time is later than the earliest service completion time, process departures first.
2. If a server is available, the customer enters service immediately.
3. If no server is available, the customer joins the queue.
4. If the queue is full, the customer leaves the system.

### Departure Process
1. Compute the total time a customer spent in the system (departure time - arrival time).
2. Mark the server as idle.
3. If the queue has waiting customers, serve the next one and accumulate queue waiting time.
4. If no customers are waiting, reset the queue state and wait for the next arrival.

## Results & Graphs
The following images illustrate the comparison between simulation results and mathematical analysis:

![image](https://github.com/user-attachments/assets/710d58dc-6a64-4e2c-8214-3c7de30c6176)
![image](https://github.com/user-attachments/assets/d44ea8ae-2ec7-4284-a585-e25e116950e9)
![image](https://github.com/user-attachments/assets/61cc6de2-e1c8-4fa5-bf39-f77dad788fb1)
![image](https://github.com/user-attachments/assets/198fad59-dddd-4b18-89e3-bf513c222578)

## Implementation Details
- **Language:** C
- **Chart Generation:** Excel
- **Data Collection:** The simulation records system waiting times, number of people served per hour, and total system load for different values of **λ**.

## Conclusion
The **M/M/s/k queue model** is highly accurate when **λ > S** but shows higher deviations when **λ < S** due to stochastic arrival variations. This simulation provides insights into queue dynamics and can be applied to optimize real-world service systems.


