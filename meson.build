project(
    'beanie',
    'c',
    license: 'MIT',
    license_files: 'LICENSE',
    default_options: [
        'warning_level=2'
    ]
)

cc = meson.get_compiler('c')

incdir = include_directories('src/include/')
lib_main = library(
    'beanie',
    'src/beanie.c',
    dependencies: cc.find_library(
        'bhashmap',
        required: true,
        has_headers: ['bhashmap.h']
    ),
    include_directories: incdir,
    install: true
)

install_headers('src/include/beanie.h')