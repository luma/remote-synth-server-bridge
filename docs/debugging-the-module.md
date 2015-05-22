# Debugging the Module

### 0. Ensure that you have the debug version of Node installed.

```
brew reinstall node --with-debug --with-openssl --with-icu4c
```

### 1. Build the module

```
npm run build
```

### 2.  Run gdb

```
gdb --args node src/test.js
```

### 3. Set breakpoints where ever

``
b Peer.cpp:43
```

Choose `Y` if it asks `Make breakpoint pending on future shared library load? (y or n)`

### 4. Run it: `r` or `run`

