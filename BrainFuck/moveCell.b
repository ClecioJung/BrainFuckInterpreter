Code:   Pseudo code:
,       Get one byte from the input and stores it at cell 0 
>>      Move the pointer to cell2
[-]     Set cell2 to 0 
<<      Move the pointer back to cell0
[       While cell0 is not 0
    -   Subtract 1 from cell0
    >>  Move the pointer to cell2
    +   Add 1 to cell2
    <<  Move the pointer back to cell0
]       End while
@       Print all used cells in the debug mode