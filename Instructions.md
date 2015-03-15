### Introduction ###

Dukto is a simple utility to help people transfer files from one computer to another over a local area network. Very often it happens that you want to transfer a file from a PC to the one next to it, and you wonder "They are connected, they can talk, but how can I transfer this damn file???". Dukto is the answer.

### What is it? ###

Dukto is a standalone software, it doesn't need installation, it doesn't creates any configuration file or registry key on your PC, it doesn't require any software (apart from the operating system) to run, it doesn't require configuration or account registration. And it runs on Windows, Mac OS X and Linux.

### How it works? ###

Dukto usage is very simple, just start it on the two PC connected over the LAN (or directly connected). On the buddy list will be showed the other peer. Now simply drag and drop your files and folders over the dukto window and they'll be transferred to the other end. That's all.

### Where does dukto saves my received files? ###

Received files are saved in the dukto current directory, usually it's the same of the folder where there's the dukto executable file. You can change the output folder pressing the button on the right of the output path showed on the dukto window. If you want to make dukto start with your preferred output folder, just write it as the first command line option (like _dukto.exe c:\Dukto\_).

### What if there's a router on the LAN? ###

If your LAN is routed dukto discovery protocol can't discover all the other clients on the LAN. So, if you want to send files to a client that's behind a router, just switch tab to "IP connection", insert the other peer's IP and drag the files in the window.
NOTE: this works only if there is a router that acts just as router, not as NAT or Firewall.

### What about the secutity of the application and protocol? ###
There isn't any security feature implemented by the application or by the communication protocol as Dukto has been developed for LAN use. Use it only in trusted networks. Anyway, currently there are no known issues about the protocol.

### What about this software and the authors? ###
Dukto has been written by me (Emanuele) in C++ using the LGPL QT libraries. Special thanks goes to Davide for compiling and testing on Mac OS X and for making the icons for all the supported OS. Also thanks to Vittorio for compiling and testing it under Linux.