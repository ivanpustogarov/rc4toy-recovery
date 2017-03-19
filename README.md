# rc4toy-recovery
Secret state recovery for rc4-reduced (default is 4 bits)
using backtracking.

## Generate keystream
```
$ ./rc4test aabbccddee 30
Keystream: 070d010f0d0e01000b090c0c0e00010b0807020e0b0a0200090a080c0507
Secret state at the last step:
Permuation:
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
  4   8  14  11  10   3   6   9   0   1   7   2  15  13   5  12
```

## Recover secret state (permutation during the last step)
```
$ ./state-recovery 070d010f0d0e01000b090c0c0e00010b0807020e0b0a0200090a080c0507
Starting state recovery of RC4-16...
 ** Success (t=29) Press any key ** 
Permuation:
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
  4   8  14  11  10   3   6   9   0   1   7   2  15  13   5  12 
Reverse permuation:
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15 
  8   9  11   5   0  14   6  10   1   7   4   3  15  13   2  12 
t=29; i=14; j=14
```

