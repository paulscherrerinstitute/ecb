# ECB - ECMC configuration builder

*ECB* is a command-line tool designed to simplify the creation and validation of
configurations for [ECMC](https://github.com/epics-modules/ecmc) using
[ECMCCFG](https://github.com/paulscherrerinstitute/ecmccfg). It leverages YAML
for configuration, provides robust data validation features, and aims to
streamline the ECMC setup process.


## Features

+ Support for data validation:
   + Rules are defined in a JSON file
   + Type checking (integer, float, string, boolean...)
   + Manage dependencies between keys
   + Allow required/optional keys and schemas/subschemas
   + Restrict integer/float values by defining min/max values
   + Normalization of values (yes/no => true/false, "real" => 1, "virtual" => 2)
   + Typo detection by only permitting keys that match the schema definition
+ Support for YAML variables (not recommended, just for backwards compatibility)
+ Standalone command line tool with batteries included.


## Changelog

See [CHANGELOG.md](CHANGELOG.md)

## Documentation

See [doc/ecb.md](doc/ecb.md)


## building
### prerequisites

- `make`, standard build tool
- git, for version numbering
- (google test, for unit tests)
- (astyle, for code formatting `make checkstyle`)

to build *ECB*, run the following command:

```bash
make -f Makefile clean && make -f Makefile -j8

make -f Makefile clean && make -f Makefile ARCH=deb10 -j8  # for Debian 10 cross-compilation
make -f Makefile clean && make -f Makefile ARCH=deb12 -j8  # for Debian 12 cross-compilation
```

Make sure you have a cross-compiler toolchain for the target architecture
installed.


### tested compilers
- g++ 8.3.0, 8.5.0: works
- g++ 11.5.0: works
- g++ 12.2.0: works
- clang++ 18.1.8: works
- clang++ 19.1.7: works


## testing
### unit tests

*ECB* includes a suite of unit test, to run the tests:

```bash
make -f Makefile test -j8
cd ./bin
./ecb_test
```

to create a *ECB* version without any compiler optimization, run the following
command:

```bash
make -f Makefile debug
```

### compare with Python jinja2
The script `scripts/compare_ecb_jinja` runs ECB and Jinja2 on the same configuration
and template directory and compares the output of both.

    cd scripts
    ./compare_ecb_jinja yaml/pax1.yaml axis
    ./compare_ecb_jinja yaml/vax1.yaml axis
    ./compare_ecb_jinja yaml/openloop.yaml enc
    ./compare_ecb_jinja yaml/plc1.yaml plc

please adapt paths in `compare_ecb_jinja` to fit your setup.

## require - driver module
to build a driver module intended for use with `require`, simply run:

```bash
make clean && make install
```
