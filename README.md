# MyFinger 📖

**MyFinger** is a custom implementation of the Linux `finger` command, developed as a homework assignment for the Operating Systems course at Sapienza University of Rome. This tool provides detailed information about system users, inspired by the original [Linux `finger` command] (https://linux.die.net/man/1/finger).

## Program Description 🏗️

The **MyFinger** command-line tool displays information about system users. Depending on the options provided, it can show basic or detailed information, similar to the functionality of the standard `finger` command in Linux.

## Options 🌟

- **`-s`**: Displays the user's login name, real name, terminal name, and write status. If write permission is denied, an asterisk (`*`) is displayed after the terminal name. Also shows idle time, login time, office location, and office phone number. Unknown devices or missing idle and login times are displayed as asterisks (`*`).

- **`-l`**: Produces a multi-line format displaying all the information from the `-s` option, plus the user's home directory, home phone number, login shell, mail status, and the contents of the files `.plan`, `.project`, `.pgpkey`, and `.forward` from the user's home directory. If write permission is denied, `(messages off)` is appended to the terminal name. Each user is displayed once, but terminal information is repeated for multiple logins.

- **`-p`**: Prevents the `-l` option from displaying the contents of the `.plan`, `.project`, `.pgpkey`, and `.forward` files.

- **`-m`**: Disables matching of real names and only matches against login names. By default, `MyFinger` matches user input with both login names and real names, case insensitively. The `-m` option restricts this to login names only.

## Default Options ⚙️

If no options are specified:
- **With operands (usernames)**: Defaults to the `-l` style output.
- **Without operands**: Defaults to the `-s` style output, listing all users currently logged into the system.

Note: Some fields may be missing in either format if the information is not available.

## How to Run 🎮

To run **MyFinger**, use the following command:

```bash
./myFinger [-lsmp] [user...]
```
- **[-lsmp]:** These represent the options described above. You can combine them as needed.
- **[user...]:** Specify one or more usernames to query. If no usernames are provided, the command will display information for all currently logged-in users.

### Examples
Display brief information for all logged-in users:
```bash
./myFinger -s
```

Display detailed information for a specific user:
```bash
./myFinger -l username
```

Display detailed information without reading .plan, .project, .pgpkey, and .forward:
```bash
./myFinger -lp username
```

Search only by login names:
```bash
./myFinger -m username
```

## Conclusion 🔚
MyFinger is a powerful and flexible tool designed to replicate and extend the functionality of the standard Linux finger command. Whether you need a quick overview of system users or detailed information, MyFinger provides the options to suit your needs.
