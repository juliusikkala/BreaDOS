project(
  'BreaDOS',
  'c',
  default_options: ['buildtype=release']
)

run_command('cross/setup.sh')

if meson.is_cross_build() == false
  error('The kernel must be built with cross build enabled. Use --cross-file cross_file.txt. Did you run setup.sh?')
endif

#FIXME: Workaround, when meson 0.43.0 lands find_program should be able to use
#cross_file.txt to find the right program.
as = find_program('cross/bin/i686-elf-as')

custom_target('bootstrap',
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
