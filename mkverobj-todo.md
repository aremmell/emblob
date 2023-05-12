
## TODO in this mofo

- [ ] last things fuckin' first: we have to figure out how to get the macOS linker to do the same shit.
- [ ] the logging is fucked.
- [ ] needs errors/warnings/debug

- [ ] ld itself and its args need to be configurable
  from the cmdline or its worthless

- [ ] the fuck with std:: everywhere

- [ ] better instructions when printing usage

- [ ] rename it.

------

## Might as well take notes while I’m doing this

- The linker  on Linux generates an “ELF” (Executable and Linkable Format) object file as output.

- On macOS, the linker generates an object file format called “Mach-O” (Mach Object), which happens to be incompatible with ELF.

    These file format differences would explain the errors we see about ‘unsupported file format’, even if they are both compiled for the same architecture.

```zsh
```
