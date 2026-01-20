# polysocket
Cross platform API for socket programming🔌 in C.

# Introduction
polysocket aims to provide a simple and easy to use abstract layer independent of whichever platform you are developing on. The applications of this API are to aid the purpose of eduction as an introduction to socket programming.

# What is socket programming?
Socket programming is a method of Inter-Process Communication(IPC). This can be used to enable communication between processes running on a local machine or over the network. Sockets are essential in the implementation of most modern network protocols like HTTP,FTP,etc.
<div align=center>
  
![Sequence diagram of Sockets server-client](https://miro.medium.com/1*dw4cFoQ2OL2SjybxzU1DHA.jpeg)

[Image source](https://miro.medium.com/1*dw4cFoQ2OL2SjybxzU1DHA.jpeg)

</div>

# Prerequisties
This project uses `gnu make` for the build system and `gcc` as its C compiler. Please ensure you have these installed on your system otherwise refer to [pre-compiled]() for options to install pre-compiled static libraries.

# Development roadmap

- [ ] Core API - socket(), bind(), listen(), accept(), close()
- [ ] Error code handling - polysocket_errToStr()
- [ ] Asynchronous I/O over sockets - select(), poll()

# installation
_**TBD**_

# Useful links
[windows socket winsock2](https://learn.microsoft.com/en-us/windows/win32/WinSock/windows-sockets-start-page-2)

[Berkeley Sockets](https://en.wikipedia.org/wiki/Berkeley_sockets)
