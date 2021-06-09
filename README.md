# scdev Kernel Module

A **S**imple **C**har **DEV**ice kernel module.
It has been tested on Ubuntu.

I used it in a project to simulate communication between two user space binaries that use kernek kfifo:
```
      MASTER (/proc/scdev) |       KERNEL        | SLAVE (/dev/scdev)
           USERSPACE       |     KERNELSPACE     |     USERSPACE
      ---------------------+---------------------+-------------------
      write /proc/scdev    | -> [ cmds fifo ]    |
                           |    [ cmds fifo ] -> | Read the cmd from /dev/scdev
                           |                     |    -> Execute the cmd
                           |    [ data fifo ] <- | Write the result into /dev/scdev
     Read from /proc/scdev | <- [ data fifo ]    |
```
