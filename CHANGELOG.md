v1.6.0
------

+ the normalize function now accepts a list of strings for normalization, e.g.
  `["(A=B) a1=b1 a2=b2", "(C=D) c1=d1 c2=d2"]`

+ the following values are now accepted as booleans:
  - true/false (case insensitive)
  - yes/no (case insensitive)
  - 1/0

v1.5.0
------

+ support True/False as boolean values

v1.4.0
------

+ uses inja release v3.5.0 (modifications are no longer needed).

+ fix `nlohmann/json` include paths, enabling use of vanilla inja version

+ `stdc++fs` issue resolved ; compilation now works with GCC 8.3.0 and 8.5.0

+ cross-compilation support for Debian 10 and 12


v1.3.0
------

+ rename binary to "ecb"


v1.2.0
------

+ speedup compilation and restructure Makefiles

+ fix version number


v1.1.0
------

+ better regex for `|default(X)`, `|default(X)|float`, and `default(X)|int`,
  especially when they appear on the same line.

+ jinja2 float casts are no longer simply removed. Casting a number to a float 
  now works as intended.

+ remove the newline at the end of the rendered template.


v1.0.0 
------

initial version
