# Dr0pIt

Dr0pIt is a stealthy dropper for Linux. It aims to be able to execute any payload without any writes to disk. It can either inject a shellcode a running process or execute a binary file stored in memory.

## Installation

```
git clone git@github.com:alagroy-42/Dr0pIt.git
make
```

## Usage

`./Dr0pIt -h host -p port`

## Todo

- Protocols:
    - TCP
    - HTTP
    - FTP ?
    - HTTPS ?
- Execution:
    - Fileless execution
    - Process injection
- Persistence
