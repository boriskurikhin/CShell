
Boris Skurikhin
Assignment 1

How to run?
    -> to run the shell simply type the make command into the directory where 
       the C files are located.
    -> then run ./ish.o which will launch the shell
    -> type exit to exit the shell

Custom function
    -> my custom function is called adder, it adds two (positive/negative numbers together)
    -> the hard thing about this function is that the numbers be can be as large as
        100,000 DIGITS -- yes it would not fit on your calculator
    -> I AM NOT USING ANY LIBRARIES TO ADD THE NUMBERS TOGETHER, EVERYTHING WAS DONE IN C USING STRINGS
       and a little bit of math theory
    
    Here's an example, you can go check it online:

    [boris@Hackintosh-2]$ adder 98723446829472349832473249732473827432782349732473284732847 -4234234324523423523423423234234
    98723446829472349832473249728239593108258926209049861498613

Assumptions:
        -> I used my own parser, wrote it to parse char-by-char
        -> Assumed file names follow < and >