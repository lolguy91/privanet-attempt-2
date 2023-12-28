## Functions

Function names are written in `snake_case`, in the form of `object_do()`, where `object` is commonly the same as the filename the function is written in.

- Reasonable exceptions can be made.

Example:

network.c:
```c
void network_connect(...);
```

Private functions are prepended with two underscores.

Example:
```c
void __network_private_function();
```

---

## TODO

- Structure this document better
- Completely document the code style