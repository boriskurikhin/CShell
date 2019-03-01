# How to run the shell

The shell includes all functionality of your regular Bash. You may:
+ redirect output to a file using '>'
+ redirect input from a file using '<'
+ run a process in the background using '&'
+ change directories
+ run commands in kernel mode using 'sudo'
+ use my custom function call 'adder'

1. Simply type the make command into the directory where the C files are located.
2. Then run ./ish.o which will launch the shell
3. Type exit to exit the shell

# Custom function
- My custom function is called adder, it adds two (positive/negative numbers together)
- The hard thing about this function is that the numbers be can be as large as 100,000 DIGITS -- yes it would not fit on your calculator
    
# Here's an example, you can go check it online:

    [boris@Hackintosh-2]$ adder 98723446829472349832473249732473827432782349732473284732847 -4234234324523423523423423234234
    98723446829472349832473249728239593108258926209049861498613
