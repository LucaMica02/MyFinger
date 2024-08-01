# MyFinger
Implementation of the linux finger command for a homework for the operating systems course at Sapienza University of Rome

# Program Description
Displays information about the system users

# Options
[-s] displays the user's login name, real name, terminal name and write status (as a ''*'' after the terminal name if write permission is denied), idle time, login time, office location and office phone number.
Unknown devices as well as non existent idle and login times are displayed as single asterisks.

[-l] Produces a multi-line format displaying all of the information described for the -s option as well as the user's home directory, home phone number, login shell, mail status, and the contents of the files ".plan", ".project", ".pgpkey" and ".forward" from the user's home directory.
If write permission is denied to the device, the phrase ''(messages off)'' is appended to the line containing the device name. One entry per user is displayed with the -l option; if a user is logged on multiple times, terminal information is repeated once per login.

[-p] Prevents the -l option of finger from displaying the contents of the ".plan", ".project", ".pgpkey" and ".forward" files.

[-m] Prevent matching of user names. User is usually a login name; however, matching will also be done on the users' real names, unless the -m option is supplied. All name matching performed by finger is case insensitive.

# Default Options
If no options are specified, finger defaults to the -l style output if operands are provided, otherwise to the -s style. Note that some fields may be missing, in either format, if information is not available for them.

If no arguments are specified, finger will print an entry for each user currently logged into the system.

# How To Run
./myFinger [-lsmp] [user..]

* Inspired by https://linux.die.net/man/1/finger
