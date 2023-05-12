
## TODO in this mofo

- [ ] last things fuckin' first: we have to figure out how to get the macOS linker to do the same shit.
- [ ] the logging is fucked.
- [ ] needs errors/warnings/debug

- [ ] ld itself and its args need to be configurable
  from the cmdline or its worthless

- [ ] the fuck with std:: everywhere

- [ ] better instructions when printing usage

- [ ] rename it.

## Might as well take notes while I’m doing this

- The linker  on Linux generates an “ELF” (Executable and Linkable Format) object file as output.

- On macOS, the linker generates an object file format called “Mach-O” (Mach Object), which happens to be incompatible with ELF.

    These file format differences would explain the errors we see about ‘unsupported file format’, even if they are both compiled for the same architecture.

- I made some discoveries:

First of all, the reason that the GNU `ld` is willing to look the other way and link in a file it doesn’t know anything about is because **it was designed to**. If you don’t believe me, go read the first page of `man ld`.

As a result of this liberal and flexible philosophy, when taking in commands to link object files together,  it has an option `"binary"` as an architecture that is supported.

macOS doesn’t play it this cool, *but* if you say just the right things, `ld` will in fact convert an ELF object file into a Mach-O one:

```zsh
```
