project(
  'vokiso',
  'cpp',
  version : '1.1.0',
  default_options : ['warning_level=3', 'cpp_std=c++17']
)

cxx = meson.get_compiler('cpp')
inc = [include_directories('source'), include_directories('lib')]

common_lib = shared_library(
  meson.project_name() + '-common',
  [
    'source/main.cpp',
    'source/utils.cpp',
    'source/AudioManager.cpp',
    'source/WordEncoder.cpp',
    'source/PortMapping.cpp',
    'source/HostConnection.cpp',
    'source/NetworkManager.cpp'
  ],
  dependencies : [cxx.find_library('openal'), dependency('miniupnpc'), dependency('threads')],
  include_directories : inc,
  install : true
)
link_args = '-lpthread'

executable(
  meson.project_name(),
  [
    'source/main.cpp'
  ],
  link_with : common_lib,
  include_directories : inc,
  install : true,
  link_args : link_args
)

executable(
  meson.project_name() + '-audio-test-a',
  [
    'source/mainAudioTestA.cpp'
  ],
  link_with : common_lib,
  include_directories : inc,
  link_args : link_args
)
executable(
  meson.project_name() + '-audio-test-b',
  [
    'source/mainAudioTestB.cpp'
  ],
  link_with : common_lib,
  include_directories : inc,
  link_args : link_args
)

unit_tests = executable(
  meson.project_name() + '-test',
  [
    'test/libs/catch.cpp',
    'test/testWordEncoder.cpp'
  ],
  link_with : common_lib,
  include_directories : [inc, include_directories('test')],
  link_args : link_args
)

test('unit tests', unit_tests)
