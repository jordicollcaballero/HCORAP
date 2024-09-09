#HCORAP instance generation

The HCORAP instances are randomly generated with the python script `create_instances.py`, that is run as follows:

```python
python3 create_instances.py N_USERS N_AGENTS N_SERVICES_PER_USER
```

In the paper, N_USERS takes values 30 and 40, N_AGENTS takes values 10, 15, 20 and 25, and N_SERVICES_PER_USER takes values 4 and 5.
The script `genTXT.sh` generates a random set of instances with the same parameters than in the experimental section of the paper.

These instances are specified in an ad-hoc plain-text format for the HCORAP. They can be encoded into MaxSAT formulas using the `hcorap2sat` binary file (see README in the root folder).
