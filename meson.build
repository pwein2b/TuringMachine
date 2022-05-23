project('TuringMachine', 'cpp', default_options: ['cpp_std=c++17'])

tm_sources = [
  'Tape.cpp',
  'TuringMachine.cpp',
]

main_sources = [
  'main.cpp',
]

tm_lib = library('TuringMachine', tm_sources)
executable('TuringMachine', main_sources, link_with: tm_lib)

subdir('demo')
