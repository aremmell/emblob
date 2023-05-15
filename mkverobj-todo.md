# TODO

- [x] last things fuckin' first: we have to figure out how to get the macOS linker to do the same shit – fuck ld, it will never work the same across platforms and architectures.
- [x] can’t use <filesystem>; a lot of platforms don’t have it even with std=c++20
- [ ] get incfile stuff portable (win32 should be fun)
  - [x] on macOS, version_data can’t have an underscore prefix. on linux, it has to. in the incfile it has one. (added preprocessor hacks to version.h for now. couldn’t figure out a better way)
- [x] version files should go into `build/obj`
    - [x] check if adding `build/obj` to mkverobj cmdline will handle that

- [ ] change `VERFILE` in `Makefile` once we’ve settled on a name; `version` is too common and may cause problems.
- [x] the logging is fucked.
    - [ ] set level from command line (default INFO i guess)
    - [x] needs errors/warnings/debug

- [x] the fuck with std:: everywhere
- [ ] better instructions when printing usage
- [ ] rename it. mvs?

