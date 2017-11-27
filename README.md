# Valid-Password-Counter

Given two integer arguments n,m, such that n < m, then the program will generate a pool of numerical passwords of length n to m.<br/><br/>
<i>
Ex: Let n = 3, m = 5<br/>
Password Pool: 000, 001, 002 ... 0000, 0001 ... 00000, 00001 ... 99998, 99999 <br/>
</i>

<br/>
Provided some conditions such as, <i>"let no consecutive digits be the same"</i> or <i>"let the sum of the digits not be a divisor of 4"</i> the program will compute in parallel using a message passing approach the amount of valid combinations within our pool.
