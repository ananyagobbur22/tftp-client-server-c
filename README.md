# TFTP Client-Server using UDP (C)

## 📌 Overview
This project implements a simplified TFTP (Trivial File Transfer Protocol) using UDP sockets. Since UDP is unreliable, a stop-and-wait protocol is implemented to ensure reliable file transfer.

## 🚀 Features
- UDP-based client-server communication
- File transfer (GET and PUT)
- Stop-and-wait protocol for reliability
- Packet structures (RRQ, WRQ, DATA, ACK, ERROR)
- Error handling for missing files and invalid requests

## 🛠️ Technologies Used
- C Programming
- UDP Socket Programming
- Linux System Calls
- Networking Concepts

## ⚙️ How It Works
- Client sends request (RRQ/WRQ)
- Server sends DATA packets
- Client sends ACK after each packet
- Retransmission occurs if ACK not received

## 🧪 Usage

### Compile
