# SMBingo

## Very early alpha version

### Disclaimer

This release was rushed out for a chapter meeting occurring two one day after this push. A less scuffed version will eventually come out (I hope).
This is hacked together *af* and I do not want any comments about the program's "hackiness" directed towards me.

### Dependencies

The program depends on:

- [gcc c compiler](https://gcc.gnu.org/)
- [Gtk4](https://www.gtk.org/docs/installations/)
- [wget](https://www.gnu.org/software/wget/)
- [make](https://www.gnu.org/software/make/)

### Building

```sh
make
gcc source/gtkclient.c -o debug/bin/gtkclient $(pkg-config --cflags --libs gtk4) -g
```

### Running

```sh
debug/bin/server | debug/bin/gtkclient 
```
