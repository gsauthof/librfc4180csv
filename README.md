C++ library for parsing CSV ([RFC 4180][1]) files

2014-05-01, Georg Sauthoff <mail@georg.so>

## Features

- uses [Ragel][2] to implement the CSV grammar from [RFC 4180][1] in a robust way
- expects UTF8, i.e. grammar is extended with UTF8 rules from [RFC 3629][3], thus illegal UTF8 byte-sequences are detected
- an optional byte order mark (BOM) is supported
- callback interface
- pull style interface
- iterator interface

## Examples

The unittests demonstrate how to use the API.

## Compile

    $ mkdir build
    $ cd build
    $ cmake ../CMakeLists.txt ..
    $ make test ut rfc4180csv

## Licence

GPLv3+


[1]: http://tools.ietf.org/html/rfc4180
[2]: http://www.complang.org/ragel/
