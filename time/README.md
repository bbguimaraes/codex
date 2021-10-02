time
====

```
$ ./time sleep 100
100000858027
$ ./time sleep 10
10000858871
$ ./time sleep 1
1001093025
$ ./time sleep 0.5
500878121
$ ./time sleep 0.2
200878003
$ ./time sleep 0.1
100810828
```

```
$ ./table < table_test.txt
  total (s)   |         from first          |        from prev.         |
---------------------------------------------------------------------------------
100.000858027 | +0            | *1          | +0            | *1        | sleep 100
 10.000858871 | -89.999999156 | *0.100008   | -89.999999156 | *0.100008 | sleep 10
  1.001093025 | -98.999765002 | *0.0100108  | -8.999765846  | *0.100101 | sleep 1
  0.500878121 | -99.499979906 | *0.00500874 | -0.500214904  | *0.500331 | sleep 0.5
  0.200878003 | -99.799980024 | *0.00200876 | -0.300000118  | *0.401052 | sleep 0.2
  0.100810828 | -99.900047199 | *0.0010081  | -0.100067175  | *0.501851 | sleep 0.1
```