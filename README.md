# Sequential_Pattern_Mining
Implement a Apriori-like Sequential Pattern Mining Algorithm

## User input
* min_supp: minimum support in percentage.
> Example User input: 
> Enter min_supp(%): 20

## The input DB format
* The first number of each sequence is the sequence ID
* The pair of number such as 11    166 is the transaction time and the item ID. Transaction time just shows the order of transactions.
> Example database:
> 1    11    166    11    4103    11    8715    24    4103    25    8375
> The sequence can be transfer to the following format 
> (166 4103 87015) 4103 8375
## Output Sequential Pattern
* The numbers are the items. 
* The space is to distinguish the different time pattern in a sequence.
* SUP is the support number of this pattern.
> Example output:
> (1566 3488 ) 3488  SUP: 103


