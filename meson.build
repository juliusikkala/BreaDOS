project(
  'BreaDOS',
  'c',
  default_options: [
    'buildtype=release',
    'c_std=gnu99'
  ]
)

run_command('cross/setup.sh')

if meson.is_cross_build() == false
  error('The kernel must be built with cross build enabled. Use --cross-file cross_file.txt. Did you run setup.sh?')
endif

#FIXME: Workaround, when meson 0.43.0 lands find_program should be able to use
#cross_file.txt to find the right program.
as = find_program('cross/bin/i686-elf-as')

bootstrap = custom_target('bootstrap',
  output: 'bootstrap.o',
  input: 'src/bootstrap.s',
  command: [
    as,
    '@INPUT@',
    '-o',
    '@OUTPUT@'
  ],
  install: true,
  install_dir: '',
)

src_inc = include_directories('src')

cc = meson.get_compiler('c')
gcc_dep = cc.find_library('gcc')

linkerfile = '@0@/@1@'.format(meson.current_source_dir(), files('src/linker.ld')[0])

executable('breados.bin',
    [
      'src/entry.c',
      'src/memory.c',
      'src/text/device.c',
      'src/text/vga/driver.c',
      bootstrap
    ],
    include_directories: [src_inc],
    dependencies: [gcc_dep],
    link_depends: ['src/linker.ld'],
    c_args: ['-ffreestanding', '-Wall', '-Wextra'],
    link_args: [
      '-T', linkerfile,
      '-ffreestanding',
      '-nostdlib'
    ],
    install: true
)
