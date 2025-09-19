# ecb - ecmc configuration builder

## Features

+ Use YAML to configure [ECMC](https://github.com/epics-modules/ecmc) with
  [ECMCCFG](https://github.com/paulscherrerinstitute/ecmccfg)
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
### dependencies
- `make`, to build
- git, for version numbering
- (google test, for unit tests)
- (astyle, for code formatting `make format`)

to build ECB, just type:

    make clean
    make 

### tested compilers
- g++ 8.5.0: does not work because of broken `std::filesystem` implementation 
- g++ 11.5.0: works
- clang++ 18.1.8: works
- clang++ 19.1.7: works

## testing

### unit tests

    # make unit tests (test_ecb)
    make test
    ./test_ecb

    # ecb debug version, disables compiler optimization
    make debug

### compare with python jinja2
The script `scripts/compare_ecb_jinja` runs ECB and Jinja2 on the same configuration
and template directory and compares the output of both.

    cd scripts
    ./compare_ecb_jinja yaml/pax1.yaml axis
    ./compare_ecb_jinja yaml/vax1.yaml axis
    ./compare_ecb_jinja yaml/openloop.yaml enc
    ./compare_ecb_jinja yaml/plc1.yaml plc

please adapt paths in `compare_ecb_jinja` to fit your setup.
