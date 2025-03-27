#AMSkt
Our source code implements the AMSkt Sketch algorithm along with the corresponding parameter configurations. This sketch  represents a novel, memory-efficient, and privacy-preserving framework designed for multi-set cardinality estimation. The source code is developed based on C++ and Python.

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

