ecb - ecmc configuration builder
--------------------------------

usage
-----

usage: ecb [OPTION] ...

    --action               readkey, updatekey or build (default)
    --help                 this help text
    --key                  key to read/write in yaml configuration
    --output               save output to file, if this option is not specified, output is stdout. 
    --schema               filename of given schema file
    --schemafile           filename of schema file
    --template             filename of template 
    --templatedir          path to template files, needed for includes
    --value                value to write to key in yaml configuration
    --version              show version
    --yaml                 filename of yaml configuration

    usage: build a configuration
      ecb [--action build] --yaml <infile> --template <templatefile> --templatedir <templates> [--output <outfile>]
    
    example:
      ecb --yaml TRY.yaml --template axis_main.jinja2 --templatedir /scripts/templates
    
    usage: read a value from a yaml key
      ecb --action readkey --yaml <infile> --key <key> [--output <outfile>]
    
    usage: write value of key in yaml
      ecb --action updatekey --yaml <infile> --key <key> --value <value> [--output <outfile>]


schema file
-----------
In the schema file all allowed keys are defined, which can be used in a yaml
configuration. Keys can be grouped into what are called schemas. Schemas are named
and can be flagged as required or optional.  Grandschemas define which schemas
are required or optional. A grandschema must not include all schemas defined
in the schema file. The grandschemas are defined in the `grandSchema` section. 
All other sections define schemas. Schemas include key/value pairs.

## grandschema
The explanation is based on the following example:

    {
      "grandSchema": {
        "axis": {
          "axis.type=0": {
            "required": "axisSchema",
            "optional": ""
          },
          "axis.type=1": {
            "required": "axisSchema driveSchema",
            "optional": "metaSchema homingSchema"
          }
        }
      }
    }

The grandschema is named `axis` and is subdivided by the two conditions
`axis.type=0` and `axis.type=1`. ECB checks which condition is true and uses
the corresponding `required/optional` fields.  Let's assume `axis.type=1`, then
the grandschema `axis` defines, that the schemas `axisSchema` and `driveSchema`
are required. In this case the yaml configuration must contain at least one key
that begins with the prefix defined in in the `identifier`field of `axisSchema`
and `driveSchema` (see section schema below). Often schemas contain keys 
that are `required` in this case, at least these keys must be defined of the 
required schemas.

As the name suggests, `optional` defines schemas that are optional for the
grandschema. If at least one key of a optional schema is defined, the whole
schema is applied. If this optional schema also includes `required` keys, these
keys must be defined then. 

## keys
the yaml configuration can have nested key/value pairs, for example:

    drive:
      numerator: 360
      denominator: 4096
      brake:
        enable: true

each key level is separated by a `.` in the schema. For example, the brake
enable key is `drive.brake.enable`


## schema
a schema names a group of key/value pairs that belong together. An example
schema looks like this:

        {
          "testSchema": {
            "identifier": "test.test2",
            "schema": {
              "test.test2.type": {
                "default": 1,
                "type": "string integer",
                "required": false,
                "dependencies": "test4.status",
                "normalize": "(string=string) csv=CSV csp=CSP",
              },
              "test.test2.id": {
                "type": "integer",
              }
            }
          }
        }


- `testSchema`: name of the schema, used in grandschema
- `identifier`: defines the common part of the keys belonging to this schema.
  ECB uses `identifier` to find if a schema is defined or not.
- `schema`: in this section the key/pair values are listed that belong to
  `testSchema`
- `test.test2.type`: full name of key
- `default`: if the schema is in use, but `test.test2.type` is not defined,
  then this default value is applied. 
- `type`: defines the expected datatype of `test.test2.type`. Allowed datatypes
  are integer, float, string, boolean or list. More than one datatype can be
  defined, ECB checks the datatype from left to right and uses the first that
  applies. If none of the defined datatypes matches ECB exits with an error.
  Using `string integer` allows to use epics macros in the yaml configuration.
  But please note that type safety is not given in this case, because the macro
  expansion is performed later. If the macro expands to something different
  than an integer in this case, it could lead to an invalid configuration or
  more worse unexpected behavior. So use this possibility carefully. 
- `required`: if true, then `test.test2.type` must be defined as soon as
  `testSchema` is used. If `required` is false, the key is optional.
- `dependencies`: defines dependencies to other key/value pairs. In the example
  `test.test2.type` also needs `test4.status`. If `test4.status` is not
  defined, ecb stops.
- `normalize`: `(D1=D2) A=B D=F...` if value has datatype D1 and matches A then
  value is changed to B, if value matches D then value is changed to F and so
  on. D2 defines the datatype of B and F. The normalization pairs are separated
  by white spaces. If D1 is a string, then matching is case insensitive. If D2
  is a string then the casing is not neglected. Possible normalizations are:
    - `(string=string) foo=Foo bar=Bar`: string to string normalization. 
    - `(string=integer) A=1, B=2)`: string to integer normalization
    - `(string_remove_whitespaces=integer) endeffector=2`: if the value in the
      yaml configuration contains whitespaces, these are removed before
      comparison.  For example `end effector` is changed to `endeffector` and
      matches then the first normalization. Therefore `end effector` is
      normalized to 2.
    - `(string=boolean) yes=true no=false` string to bool normalization


templates
---------

ECB only supports a subset of the Jinja2 syntax. This is because Inja, the
rendering engine that ECB uses under the hood, does not support all of Jinja's
features. Some features are added by ECB, so that ECMCCFG templates can be
rendered. In general all syntax that is supported by Inja can be used.
Additionally the following section describes differences or highlights some
pitfalls.

## comments
take care if the line that is commented out include jinja statemets. If jinja
statements are included use `{# #}` to comment out. Especially `#` and `#-` are
no comments in the Jinja context:

    {# this is a comment and will be ignored #}

    #- will be interpreted: {% foo.bar is defined %}
    #- will not be interpreted: {# foo.bar is defined %}


## pipes
ECB supports basic pipes, but it's important to note that some pipes may not be
fully implemented. Nevertheless, the provided implementation is sufficient for
using ECMCCFG templates.


    # `X|int` is replaced in generell with `X`. But if `X` is of type boolean,
    then # `true` is replaced with 1 and `false` is replaced with `0`. Example:

        {{ foo.bar|int }}


    # `X|default(Y), use Y as a default value if key X is not defined.  # Note:
    whenever possible try to use the `default` in the schema file # instead of
    `|default`. Example:

        {{ foo.bar|default(3) }} 


    # `foo.bar|float` is replaced with `foo.bar`. The implementation performs
    no conversion from float to integer. In the long term this pipe should be
    removed.  Defining datatypes in the schema is enough. Example:

        {{ foo.bar|float }}


## `is` keyword
ECB adds basic support for the `is` keyword, which is not supported by Inja:

    # `A is defined`, returns true if key A is defined, otherwise false. Example:

        {% if foo.bar is defined %}
          key foo.bar is defined
        {% endif %}
  
        # `is not defined`
        {% if (foo.bar is not defined) %}
          key foo.bar is not defined
        {% endif %}

    # `A is string`, returns true if the value of key A is a string, otherwise
    false. Example:

        {% if foo.bar is string %}
          value of key foo.bar is a string
        {% endif %}
