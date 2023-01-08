# Load Balancer for Distributed Key-Value Store
I implemented a load balancer in the 2nd semester of university with personal variants of data structures, such as: linked lists, circular lists and hash tables. It distributes key-value pairs across multiple servers and retrieves values from the appropriate server.

# Functionality
The load balancer provides the following functionality: <br>

- Store key-value pairs <br>
- Retrieve values for a given key <br>
- Add new servers to the load balancer <br>
- Remove existing servers from the load balancer <br>

# How to Use
To use the load balancer, call the apply_requests function and pass it an input file. The input file should contain requests, one per line, in the following format: <br>

- "store" followed by a key-value pair in quotes <br>
- "retrieve" followed by a key in quotes <br>
- "add_server" followed by a server id (integer) <br>
- "remove_server" followed by a server id (integer) <br>

# Example
For example, the input file: <br>
```
store "key1" "value1" <br>
store "key2" "value2" <br>
retrieve "key1" <br>
add_server 1 <br>
store "key3" "value3" <br>
remove_server 1 <br>
retrieve "key3" <br>
```
will produce the following output: <br>

```
Stored value1 on server 0. <br>
Stored value2 on server 1. <br>
Retrieved value1 from server 0. <br>
Stored value3 on server 2. <br>
Key key3 not present. <br>
```

# Compiling and Running

To compile the program, run the following command: <br>
```make``` <br>

To run the program, use the following command: <br>
```./load-balancer input-file```

