https://stackoverflow.com/questions/69630870/ubuntu-wsl-is-listed-in-clion-toolchains-but-says-not-found

Upgrade your CLion IDE if possible.

On my CLion v2019.2.3, the IDE wasn't even able to detect that I had already installed 'wsl2 Ubuntu v22.04'. This was resolved by upgrading to CLion IDE v2023.1.3.
In your 'wsl2 Ubuntu shell terminal', update the package index files on your system.

sudo apt update
Install the 'build-essential' package in your 'wsl2 Ubuntu shell terminal. These will contain the GNU/g++ compiler collection and a couple more libraries and tools needed for compiling a program.

sudo apt install build-essential
Install the 'cmake' package in your 'wsl2 Ubuntu shell terminal.

sudo apt install cmake
Install the 'gdb' debugger for C (and C++) in your 'wsl2 Ubuntu shell terminal.

sudo apt install gdb
Restart your IDE and reconfigure your 'Tool chain' to use WSL. All entries will now be auto-detected.

- This needs to use wsl v 2
  Check with wsl -l -v. To change versions, use the command: wsl --set-version <distro name> 2 replacing with the name of the Linux distribution that you want to update. For example, wsl --set-version Ubuntu-20.04 2.
