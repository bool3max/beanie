project(
    'beanie',
    'c',
    license: 'MIT',
    license_files: 'LICENSE',
    default_options: [
        'warning_level=2'
    ]
)

incdir = include_directories('src/include/')

lib_main = library(
    'beanie',
    'src/beanie.c',
    include_directories: incdir,
    install: true
)

install_headers('src/include/beanie.h')