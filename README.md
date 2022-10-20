# Dr0pIt

Dr0pIt is a stealthy dropper for Linux. It aims to be able to execute any payload without any writes to disk. It can either inject a shellcode a running process or execute a binary file stored in memory.

## Installation

```
git clone git@github.com:alagroy-42/Dr0pIt.git
make
```

## Usage

`./Dr0pIt [-h] [-p proto] [-f file] host port`
- `-h`: Displays help
- `-p`: Specify the protocol to retreive the file. The supported protocols for now are HTTP and raw TCP. Default protocol is TCP.
- `-f`: Specify the resource to fetch on the host if the protocol is HTTP
- `host`: A net node to connect to, it can be an IP or a domain.
- `port`: The port to connect to, it can be a port number or a service name

## Todo

- Protocols:
    - FTP ?
    - HTTPS ?
- Execution:
    - Fileless execution
    - Process injection:
        - ptrace
        - shared library
- Argument support for payload
- Persistence
