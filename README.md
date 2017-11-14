# BreaDOS

BreaDOS is a hobby OS with built-in bread.

## Features

- [x] Load kernel to physical 1 MiB address
- [x] Enable long mode
    - [x] Nag on 32-bit systems
- [x] Enable paging
    - [x] Map kernel to 0xFFFFFFFF80000000 in virtual address space
    - [x] Map stack to the top of virtual address space
- [x] VGA text mode driver
- [x] Rye bread
- [ ] Interrupt handling
- [ ] PS/2 driver
- [ ] Memory manager
- [ ] ATA/AHCI drivers
    - [ ] FAT16 filesystem driver
    - [ ] Own filesystem driver?
- [ ] Usermode
- [ ] Multitasking
- [ ] VGA video mode driver
- [ ] Vesa VBE 2.0/3.0 driver
- [ ] PCI driver
- [ ] USB driver
- [ ] Ethernet driver
- [ ] SMP

## Compilation

``` bash
$ ./setup.sh
$ ninja -C build
```

setup.sh downloads required tools (cross-compiler, GRUB, binutils) and compiles
them. If setup.sh fails due to flaky internet connection, you can just restart
it once you can actually download files.

## Running in QEMU

``` bash
$ qemu-system-x86_64 -cdrom build/breados.iso
```

## Authors

- [Julius Ikkala](https://github.com/juliusikkala)

## License

The version 3 of the GNU General Public License applies. Please see
[License File](COPYING) for more information.
