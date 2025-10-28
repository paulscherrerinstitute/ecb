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
