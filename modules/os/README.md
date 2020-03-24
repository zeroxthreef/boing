# OS
This module lets you call a bunch of system specific functions.

## Dependencies
Linux and Unixes
---
\* libdl

Windows
---
\* (TODO FINISH THESE)
\* winsock2

## Documentation
Note: this entire project is not at version 1.0.0 yet. Things will change with this module.

Non-Function Identifiers
---
| Name | Value | Description |
| --- | --- | --- |
| `OS_AF_INET` | AF_INET | system dependent |
| `OS_AF_INET6` | AF_INET6 | system dependent |
| `OS_AF_UNIX` | AF_UNIX | system dependent |
| `OS_SOCK_STREAM` | SOCK_STREAM | system dependent |
| `OS_SOCK_DGRAM` | SOCK_DGRAM | system dependent |
| `OS_SOCK_RAW` | SOCK_RAW | system dependent |
| `OS_IPPROTO_UDP` | IPPROTO_UDP | system dependent |
| `OS_IPPROTO_TCP` | IPPROTO_TCP | system dependent |
| `OS_INADDR_ANY` | INADDR_ANY | system dependent |
| `OS_SHUT_WR` | SHUT_WR | system dependent |
| `OS_SHUT_RD` | SHUT_RD | system dependent |
| `OS_SHUT_RDWR` | SHUT_RDWR | system dependent |
| `OS_NAME` | "windows" or "android" or "linux" or "unix" or "unknown" | the name of the platform boing was compiled for |

Function Identifiers
---
In most cases, a numeric 0 means success.

| Name | Value | Attributes |
| --- | --- | --- |
| `OS_TIME` | external value (C function pointer) | returns a number, format: (void) |
| `OS_GETENV` | external value (C function pointer) | returns an array(string), format: (array(string) environment_variable) |
| `OS_SRAND` | external value (C function pointer) | returns a number, format: (number seed) |
| `OS_SYSTEM` | external value (C function pointer) | returns a number, format: (array(string) shell_string) |
| `OS_REMOVE` | external value (C function pointer) | returns a number, format: (array(string) file_path) |
| `OS_RENAME` | external value (C function pointer) | returns a number, format: (array(string) file_path) |
| `OS_EXIT` | external value (C function pointer) | returns a number, format: (number exit_code) |
| `OS_SLEEP` | external value (C function pointer) | returns a number, format: (number seconds) |
| `OS_SOCKET` | external value (C function pointer) | returns a number, format: (number domain, number type, number protocol) |
| `OS_SETSOCKOPT` | external value (C function pointer) | returns a numberic 0 (if failed) or an external value for the socket_fd, format: (external(socket) socket_fd) |
| `OS_BIND` | external value (C function pointer) | returns a number, format: (external(socket) socket_fd, number family, number in_addr, number port) |
| `OS_LISTEN` | external value (C function pointer) | returns a number, format: (external(socket) socket_fd, number max_sockets) |
| `OS_ACCEPT` | external value (C function pointer) | returns a number (0 for success, -1 for failure), format: (external(socket) socket_fd, any new_socket_to_be_set) |
| `OS_RECV` | external value (C function pointer) | returns an array, format: (external(socket) socket_fd, number max_read) ``note that if max_read is <0, it will read until EOF`` |
| `OS_SEND` | external value (C function pointer) | returns a number (the amount sent successfully), format: (external(socket) socket_fd, array data) |
| `OS_SHUTDOWN` | external value (C function pointer) | returns a number, format: (external(socket) socket_fd, number how) |
| `OS_CONNECT` | external value (C function pointer) | returns a number, format: (external(socket) socket_fd, array(string) ipv4, number port) |
| `OS_GET_IPVFOUR` | external value (C function pointer) | returns an array(string), format: (external(socket) sock_fd) |
| `OS_GET_IPVSIX` | external value (C function pointer) | returns an array(string), format: (external(socket) socket_fd) |
| `OS_FILE_APPEND` | external value (C function pointer) | returns a number (1 for success, 0 for failure), format: (array(string) file_path, array(string) data) |
| `OS_IS_FILE` | external value (C function pointer) | returns a number (1 for the path is a file, 0 for not exists/not a file), format: (array(string) file_path) |
| `OS_IS_DIR` | external value (C function pointer) | returns a number (1 for the path is a directory, 0 for not exists/not a directory), format: (array(string) file_path) |
| `OS_LIST_DIR` | external value (C function pointer) | returns an array of array(string)s or a number (0) if not a directory or failure, format: (void) |

## TODO
\* finish adding more numeric identifiers for socket options and various other socket controls
\* maybe add unix domain sockets and the equivalent for windows