#!/usr/bin/env python

from setuptools import setup, find_packages, Extension

timers_module = Extension('_libtask9_timers',
#                          sources = ['libtask9/timers_ext/posix_rt.c'],
#                          libraries = ['pthread', 'rt'])
                          sources = ['libtask9/timers_ext/posix.c'],
                          libraries = ['pthread'])

setup(
    name = 'libtask9',
    version = '0.2.1',
    description = 'Python port of the Plan 9 from User Space (aka plan9port) version of libthread',
    author = 'Yuval Pavel Zholkover',
    author_email = 'paulzhol@gmail.com',
    url = 'https://bitbucket.org/paulzhol/libtask9',
    classifiers = [
        'Development Status :: 3 - Alpha',
        'Topic :: Software Development :: Libraries :: Python Modules',
        'Programming Language :: Python :: 2.7',
        'License :: OSI Approved :: MIT License',
    ],
    packages = find_packages(),
    ext_modules = [timers_module],
    install_requires = [
        'greenlet >= 0.4.0',
    ]
)
