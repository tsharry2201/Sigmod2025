# Sigmod2025
Our source code consists of two parts, one is the cardinality estimation accuracy measurement, and the other is the AMSkt-MPC, a secure multi-party computation protocol. The accuracy measurement consists of the AMSkt Sketch algorithm and the corresponding parameter settings. The complete process of the AMSkt-MPC protocol includes the offline preparation phase, the online phase, and the corresponding parameter settings. The source code is developed based on C++ and Python.
# AMSkt

## To compile AMSkt

```bash
cd SIGMOD2025_cpp-main/
make 
```
## To run AMSkt
```bash
./xxx
```
#### parameters for our method and baseline
| Parameters     | Description                                                  |
| -------------- | ------------------------------------------------------------ |
| ```set_num```    | the number of sets                                         |
| ```aver_card```   |average cardinality of sets                                |
| $\varepsilon$ | parameter for differential privacy                            |

# AMSkt-MPC
## Installation Requirements:
AMSkt-MPC protocol requires several dependent libraries to run, which must be installed before running the program to ensure successful execution.

- OS: Ubuntu 16.04 LTS
- GCC  7
- MPIR library  3.0.0
- libsodium library  1.0.11
- NTL library  11.0.0 
- valgrind, version 3.14.0
- farmhash 1.1
- openssl 1.1.1

## To compile AMSkt-MPC
Download this repository and run the following commands in cmd.
```bash
cd SIGMOD2025_cpp-main/AMSkt-MPC/
make all
```
After compilation, we get executable scripts `DP.x`, `CP.x`,  `pairwise-offline.x` and `Server.x`, where `pairwise-offline.x` will be used for random number generation in the offline preparation stage, and ` DP.x` and `CP.x` will be used for protocol execution in the online phase.

## Offline preparation
In the offline preparation phase, generating the random numbers(**Triple, Rand, Rand2, and RandExp**) used in the online step is necessary. The following is the description of the parameters for random number generation. After compiling AMSkt-MPC, we can get the execution script`pairwise-offline.x` and use it to generate random numbers. `pairwise-offline.x` can be run with the following parameters:

./pairwise-offline.x  -o {Write generated results to files} -h {host of CP0 (default: localhost)} -x  {number of threads }  -f {the modulus used for the SPDZ framework} -N {number of CPs} -p {Current CP sequence number(From 0 to N-1)} -n  {number of Triples to be generated} -nr {number of  Rands to be generated} -nr2  {number of Rand2s to be generated} -nrx{number of  RandExp s to be generated} -E {the bit length of the input plaintext for SPDZ}

For a more detailed parameter description of `pairwise-offline.x`, you can execute `pairwise-offline.x` directly from the terminal to obtain.

The generated random number shares and MAC-KEY are stored in ~dp-dice/Player-Data

### Example for Offline preparation

When there are 2 CPs to generate 2000000 **Triples**.
```bash
# On CP 0
./pairwise-offline.x -o -f 60 -N 2 -p 0 -n 2000000
# On CP 1
./pairwise-offline.x -o -f 60 -N 2 -p 1 -n 2000000
```
When there are 2 CPs to generate 2000000 **Rand**.
```bash
# On CP 0
./pairwise-offline.x -o -f 60 -N 2 -p 0 -nr 2000000 -r
# On CP 1
./pairwise-offline.x -o -f 60 -N 2 -p 1 -nr 2000000 -r
```
When there are 2 CPs to generate 2000000 **Rand2**.
```bash
# On CP 0
./pairwise-offline.x -o -f 60 -N 2 -p 0 -nr2 20000 -r2
# On CP 1
./pairwise-offline.x -o -f 60 -N 2 -p 1 -nr2 20000 -r2
```
When there are 2 CPs to generate 2000000 **RandExp64**.
```bash
# On CP 0
./pairwise-offline.x -o -f 60 -N 2 -p 0 -nrx 20000 -rx -E 64
# On CP 1
./pairwise-offline.x -o -f 60 -N 2 -p 1 -nrx 20000 -rx -E 64
```
When there are 2 CPs to generate 2000000 **RandExp18**.
```bash
# On CP 0
./pairwise-offline.x -o -f 60 -N 2 -p 0 -nrx 20000 -rx -E 18
# On CP 1
./pairwise-offline.x -o -f 60 -N 2 -p 1 -nrx 20000 -rx -E 18
```
## Online 
### Simple native example 
To facilitate testing the execution of the AMSkt-MPC protocol in the online phase, we give a simple test example. Note that the necessary offline data needs to be generated before executing the online phase protocol.
In this example the DH holds about 10^6 unique data, which is shared secretly through 1000 FMS sketch.
#### To establish the communication network
The command is running on nameserver terminal to establish the communication network of each computing party
```bash
./Server.x 2 5000 1
```
####  Running the CPs
Open two new cmd and run the CPs
```bash
./CP.x  -lgp 60 -np 2 -p 0 -sec 40 -ndp 1 -oM 1000 -uN 1000000 -lan 1 -nt 1 
./CP.x  -lgp 60 -np 2 -p 1 -sec 40 -ndp 1 -oM 1000 -uN 1000000 -lan 1 -nt 1
```
####  Running the DHs
Run the DH in the Server cmd 
```bash
./DP.x -ndp 1 -ncp 2 -p 1 -lgp 60 -oM 1000 -uN 1000000 -nt 1
```
#### parameters for nameserver
```bash
./Server.x {number of computation parties} 5000 {number of threads}
```

#### parameters for CP
```bash
./CP.x  -lgp {length of the prime} -np {number of CPs} -p {party number} -sec {security parameter} -ndp {number of DHs} -oM {number of FMS sketch} -uN { number of unique items} -lan {whether in lan environment} -nt {number of threads} -h {ip of DH}
```

#### parameters for DH
```bash
./DP.x -ndp {number of DHs} -ncp {number of CPs} -p {party number} -lgp {length of the prime} -oM {number of FMS sketch} -uN { number of unique items} -ip {ip files}
```
