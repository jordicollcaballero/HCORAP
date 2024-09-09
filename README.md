#Instructions to generate MaxSAT instances

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
