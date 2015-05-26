# Debugging the Module

## Ensure that you have the debug version of Node installed.

If you've on a Mac, and you used Homebrew to install Node, then you can run something like the following.
```
brew reinstall node --with-debug --with-openssl --with-icu4c
```


## Using LLDB

LLDB is basically GDB for the LLVM toolchain. It's also what Apple wants you to use as the newer versions of Mac OSX (Yosemite and above) don't have GDB out of the box.

TODO: Write up how to use LLDB
      http://lldb.llvm.org/tutorial.html
      http://lldb.llvm.org/scripting.html
      http://lldb.llvm.org/lldb-gdb.html
TODO: ext/lldb-v8

### 1. Build the module

```
npm run build
```

### 2.  Run LLDB

```
lldb -- node src/test.js
```

### 3. Enable Guard Malloc on MacOS (optional)

```
env DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib
```

Info on Guard Malloc is on [developer.apple.com](https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/libgmalloc.3.html#//apple_ref/doc/man/3/libgmalloc).


## Using GDB


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

### 4. Run it

Using `r` or `run`

Once the program crashs you can use the `where` command to orientate yourself.

## Demangling Segfault Stacktraces

The `c++filt` is what you want here. E.g.

```
node src/test.js  2>&1 | c++filt -n
```

