# General description
This project contains the source code and the instances used in the paper **Optimizing Resource Allocation in Home Care Services
using MaxSAT**.

# Instructions to generate MaxSAT instances

The HCORAP instances from the instance folder can be encoded into MaxSAT using the `hcorap2sat` program. This program is compiled by running:

```sh
make
```
in the root directory.

Once compiled, an instance `INSTANCE.txt` can be encoded into MaxSAT by running:

```sh
./bin/release/hcorap2sat -e=1 -f=dimacs -S=0 INSTANCE.txt
```
which generates an encoding using the MaxSAT version of DIMACS standard, version post-2022 edition of the MaxSAT evaluation described [here](https://maxsat-evaluations.github.io/2022/rules.html#input).

The instance is written to standard output channel. If, for instance, saved to a file named `instance.wcnf`, it can be solved with an off-the-shelf MaxSAT solver, e.g. with WMaxCDCL, by running:

```sh
wmaxcdcl_static instance.wcnf
```
