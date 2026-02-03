//
// ECB - tests for yj_schema module
//
// Copyright (C) 2025, Felix Maier <felix.maier@psi.ch>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "gtest/gtest.h"
#include "nlohmann/json.hpp"

#include "yj_schema.h"

using nlohmann::json;
using namespace ecb;

class YjSchemaFixture : public testing::Test
{
protected:

    YjSchemaFixture()
    {
        j1 = json();
    }

    std::stringstream schema;
    json j1;
};

TEST_F(YjSchemaFixture, normalize_string_integer)
{
    schema.str(R"(
        {
          "testSchema": {
            "schema": {
              "axis.type": {"normalize": "(string=integer) Real=1 dEbug=0 2=2"}
            }
          }
        })"
    );

    auto dut1 = YjSchema(schema, "");
    //
    // value is string
    j1.clear();
    j1["/axis/type"_json_pointer] = "rEaL";
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/axis/type"_json_pointer] == 1) << "axis: " << "real";

    j1.clear();
    j1["/axis/type"_json_pointer] = "DeBug";
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/axis/type"_json_pointer] == 0) << "axis: " << "debug";

    j1.clear();
    j1["/axis/type"_json_pointer] = "2";
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/axis/type"_json_pointer] == 2) << "axis: " << "2";

    // value is int
    j1.clear();
    j1["/axis/type"_json_pointer] = 0;
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/axis/type"_json_pointer] == 0);
}

TEST_F(YjSchemaFixture, normalize_string_remove_whitespaces_integer)
{
    schema.str(R"(
        {
          "testSchema": {
            "schema": {
              "axis.type": {"normalize": "(string_remove_whitespaces=integer) Real=1 dEbug=0 2=2"}
            }
          }
        })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/axis/type"_json_pointer] = " rE aL ";
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/axis/type"_json_pointer] == 1) << "axis: " << "real";
}

TEST_F(YjSchemaFixture, normalize_string_boolean)
{
    schema.str(R"(
        {
          "testSchema": {
            "schema": {
              "a.b": {"normalize": ["(string=boolean) yes=true no=false", "(integer=boolean) 1=true 0=false"]}
            }
          }
        })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = "yEs";
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == true) << "a.b: true";

    j1.clear();
    j1["/a/b"_json_pointer] = "nO";
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == false) << "a.b: false";

    j1.clear();
    j1["/a/b"_json_pointer] = 1;
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == true) << "a.b: true";

    j1.clear();
    j1["/a/b"_json_pointer] = 0;
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == false) << "a.b: false";

    j1.clear();
    j1["/a/b"_json_pointer] = true;
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == true) << "axis: true";

    j1.clear();
    j1["/a/b"_json_pointer] = false;
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == false) << "axis: false";
}

TEST_F(YjSchemaFixture, normalize_string_string)
{
    schema.str(R"(
        {
          "testSchema": {
            "schema": {
              "a.b": {"normalize": "(string=string) csP=CSP Csv=cSv"}
            }
          }
        })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = "CSp";
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == "CSP") << "a.b: true";

    j1.clear();
    j1["/a/b"_json_pointer] = "csv";
    dut1.normalize(j1);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == "cSv") << "a.b: false";
}

TEST_F(YjSchemaFixture, check_datatypes_mixed)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "integer string"},
            "c.d": {"type": "boolean"},
            "e.f": {"type": "float"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    // string
    j1.clear();
    j1["/a/b"_json_pointer] = "virtual";
    EXPECT_NO_THROW(dut1.check_datatypes(j1));

    // integer
    j1.clear();
    j1["/a/b"_json_pointer] = 2;
    EXPECT_NO_THROW(dut1.check_datatypes(j1));

    // invalid datatype
    j1.clear();
    j1["/a/b"_json_pointer] = true;
    EXPECT_ANY_THROW(dut1.check_datatypes(j1));
}

TEST_F(YjSchemaFixture, add_default_from_key)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema testSchema"
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": false}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b": {"type": "integer", "default": 42},
            "a.c": {"type": "integer"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "abc");
    EXPECT_TRUE(j1.contains("/a/b"_json_pointer) == false);
    EXPECT_TRUE(j1.contains("/a/c"_json_pointer) == false);

    // add default value
    EXPECT_NO_THROW(dut1.add_default_value_from_key(j1, "a.b"));
    EXPECT_TRUE(j1.contains("/a/b"_json_pointer) == true);
    EXPECT_TRUE(j1.contains("/a/c"_json_pointer) == false);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == 42);

    // check that checks are not added when no default is set
    EXPECT_NO_THROW(dut1.add_default_value_from_key(j1, "a.c"));
    EXPECT_TRUE(j1.contains("/a/b"_json_pointer) == true);
    EXPECT_TRUE(j1.contains("/a/c"_json_pointer) == false);
    EXPECT_TRUE(j1["/a/b"_json_pointer] == 42);
}

// ----------------------------------------------------------------------------
TEST_F(YjSchemaFixture, add_schema_defaults)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema testSchema",
              "optional": "testSchema2"
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b": {"type": "integer string", "default": -1},
            "a.d": {"type": "boolean", "default": true},
            "a.f": {"type": "float", "default": -23.12},
            "a.h": {"type": "string", "default": "abc42"},
            "a.j": {"type": "integer", "default": 1}
          }
        },

        "testSchema2": {
          "identifier": "a.def",
          "schema": {
            "a.def.a": {"type": "integer string", "default": 34},
            "a.def.b": {"type": "boolean"}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    EXPECT_NO_THROW(dut1.add_schema_default_values(j1));

    EXPECT_TRUE(j1["/a/b"_json_pointer] == -1) << "default integer";
    EXPECT_TRUE(j1["/a/d"_json_pointer] == true) << "default boolean";
    EXPECT_DOUBLE_EQ(j1["/a/f"_json_pointer], -23.12) << "default float";
    EXPECT_TRUE(j1["/a/h"_json_pointer] == "abc42") << "default string";
    EXPECT_TRUE(j1["/a/j"_json_pointer] == 1) << "default integer";
    EXPECT_FALSE(j1.contains("/b/a"_json_pointer) == true);
    EXPECT_TRUE(j1.contains("/a/def/a"_json_pointer) == false);

    j1["/a/def/b"_json_pointer] = true;
    EXPECT_NO_THROW(dut1.add_schema_default_values(j1));
    EXPECT_TRUE(j1.contains("/a/def/a"_json_pointer) == true);
    EXPECT_TRUE(j1["/a/def/a"_json_pointer] == 34) << "default for optional key";
}

// ----------------------------------------------------------------------------
TEST_F(YjSchemaFixture, check_datatypes_boolean)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "boolean"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = true;
    EXPECT_NO_THROW(dut1.check_datatypes(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "test";
    EXPECT_ANY_THROW(dut1.check_datatypes(j1));
}

TEST_F(YjSchemaFixture, check_datatypes_python_boolean)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "boolean"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = "True";
    EXPECT_NO_THROW(dut1.check_datatypes(j1));
    EXPECT_TRUE(j1["/a/b"_json_pointer] == true);

    j1.clear();
    j1["/a/b"_json_pointer] = "False";
    EXPECT_NO_THROW(dut1.check_datatypes(j1));
    EXPECT_TRUE(j1["/a/b"_json_pointer] == false);
}

TEST_F(YjSchemaFixture, check_datatypes_integer)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "integer"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = 2;
    EXPECT_NO_THROW(dut1.check_datatypes(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = -1;
    EXPECT_NO_THROW(dut1.check_datatypes(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "test";
    EXPECT_ANY_THROW(dut1.check_datatypes(j1));
}

TEST_F(YjSchemaFixture, check_datatypes_float)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "float"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = 42.42;
    EXPECT_NO_THROW(dut1.check_datatypes(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "test";
    EXPECT_ANY_THROW(dut1.check_datatypes(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 42;
    EXPECT_NO_THROW(dut1.check_datatypes(j1));
}

TEST_F(YjSchemaFixture, check_datatypes_string)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "string"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = "fjefe";
    EXPECT_NO_THROW(dut1.check_datatypes(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = true;
    EXPECT_ANY_THROW(dut1.check_datatypes(j1));
}

TEST_F(YjSchemaFixture, check_datatypes_empty)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": ""}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = true;
    EXPECT_ANY_THROW(dut1.check_datatypes(j1));
}

TEST_F(YjSchemaFixture, check_datatypes_list)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "list"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    // invalid list
    j1.clear();
    j1["/a/b"_json_pointer] = true;
    EXPECT_ANY_THROW(dut1.check_datatypes(j1)) << "case: invalid list";

    // valid list
    j1.clear();
    j1["/a/b"_json_pointer] = {"a", "b"};
    EXPECT_NO_THROW(dut1.check_datatypes(j1)) << "case: valid list";
}

TEST_F(YjSchemaFixture, check_datatypes_unknown)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "fjfjf"}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = true;
    EXPECT_ANY_THROW(dut1.check_datatypes(j1));
}

TEST_F(YjSchemaFixture, check_required_key)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "testSchema axisSchema",
              "optional": ""
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "test",
          "schema": {
            "test.a.b": {"required": true},
            "test.c.d": {"required": false}
          }
        }
      })"
    );

    // test.a.b is missing
    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");
    EXPECT_THROW(dut1.check_schema("abc", j1), std::runtime_error);

    j1["/test/a/b"_json_pointer] = "test value";
    EXPECT_NO_THROW(dut1.check_schema("abc", j1));

    // optional key
    j1["/test/c/d"_json_pointer] = "test value";
    EXPECT_NO_THROW(dut1.check_schema("abc", j1));
}

TEST_F(YjSchemaFixture, check_schema_required_schema_missing)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "testSchema axisSchema",
              "optional": ""
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "test",
          "schema": {
            "test.abc": {"required": true},
            "test.a":   {"required": true},
            "test.c.d": {"required": false}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");
    EXPECT_ANY_THROW(dut1.check_schema("abc", j1));

    j1["/test/abc"_json_pointer] = "fjfj";
    EXPECT_ANY_THROW(dut1.check_schema("abc", j1));

    j1["/test/a"_json_pointer] = "fjfj";
    EXPECT_NO_THROW(dut1.check_schema("abc", j1));
}

TEST_F(YjSchemaFixture, check_schema_optional_schema_missing)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema",
              "optional": "testSchema"
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "test",
          "schema": {
            "test.abc": {"required": true},
            "test.c.d": {"required": false}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");
    EXPECT_NO_THROW(dut1.check_schema("abc", j1));

    // optional schema is defined but not complete
    j1["/test/c/d"_json_pointer] = "fjfj";
    EXPECT_ANY_THROW(dut1.check_schema("abc", j1));

    j1["/test/abc"_json_pointer] = "fjfj";
    EXPECT_NO_THROW(dut1.check_schema("abc", j1));
}

TEST_F(YjSchemaFixture, check_dependencies)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema testSchema",
              "optional": ""
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b.c": {"dependencies": "a.b.d a.b.e"},
            "a.b.d": {"required": false},
            "a.b.e": {"required": false},
            "a.b.h": {"required": false},
            "a.b.z": {"required": false, "dependencies": "a.b.h"}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    j1["/a/b/c"_json_pointer] = 42;
    EXPECT_ANY_THROW(dut1.check_schema("abc", j1));

    j1["/a/b/d"_json_pointer] = 42.42;
    EXPECT_ANY_THROW(dut1.check_schema("abc", j1));

    j1["/a/b/e"_json_pointer] = 42.42;
    EXPECT_NO_THROW(dut1.check_schema("abc", j1));
}

TEST_F(YjSchemaFixture, check_undefined_dependencie)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema testSchema",
              "optional": ""
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b.a": {"required": false},
            "a.b.c": {"dependencies": "a.b.d"},
            "a.b.d": {"required": false}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    j1["/a/b/a"_json_pointer] = 12.12;
    EXPECT_NO_THROW(dut1.check_schema("abc", j1));
}

TEST_F(YjSchemaFixture, check_dependencies_incomplete_key)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema testSchema",
              "optional": ""
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b": {"dependencies": "a.b.c"},
            "a.b.a": {"required": false},
            "a.b.c": {"required": false}
          }
        }
      })"
    );


    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    // a.b.a also defines a.b and this depends on a.b.c which
    // is not defined -> fail
    j1["/a/b/a"_json_pointer] = 42;
    EXPECT_ANY_THROW(dut1.check_schema("abc", j1));

    j1["/a/b/c"_json_pointer] = 42.42;
    EXPECT_NO_THROW(dut1.check_schema("abc", j1));
}

TEST_F(YjSchemaFixture, check_schema_unknown_keys)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema testSchema",
              "optional": ""
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b": {"required": false},
            "a.b.a": {"required": false},
            "a.b.c": {"required": false},
            "a.b.e": {"required": false}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    // known key
    j1["/a/b/a"_json_pointer] = 42;
    EXPECT_NO_THROW(dut1.check_for_valid_keys(j1));

    // sequence
    j1["/a/b/e"_json_pointer] = {"1", "2", "3"};
    EXPECT_NO_THROW(dut1.check_for_valid_keys(j1));

    // unknown key
    j1["/a/b/d"_json_pointer] = 42.42;
    EXPECT_ANY_THROW(dut1.check_for_valid_keys(j1));
}

TEST_F(YjSchemaFixture, check_schema_unknown_required_schema)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema testSchema2",
              "optional": ""
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b": {"required": false},
            "a.b.a": {"required": false},
            "a.b.c": {"required": false}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    EXPECT_THROW(dut1.check_schema("abc", j1), std::runtime_error);
}

TEST_F(YjSchemaFixture, check_schema_unknown_optional_schema)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema",
              "optional": "testSchema2"
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b": {"required": false},
            "a.b.a": {"required": false},
            "a.b.c": {"required": false}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    EXPECT_THROW(dut1.check_schema("abc", j1), std::runtime_error);
}

TEST_F(YjSchemaFixture, schema_remove_undefined_keys)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema",
              "optional": "testSchema"
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "testSchema": {
          "identifier": "a",
          "schema": {
            "a.b": {"required": false},
            "a.b.a": {"required": false},
            "a.b.c": {"required": false}
          }
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    j1["/a/b"_json_pointer] = 42;
    j1["/b/c/d"_json_pointer] = 56;
    auto dut1 = YjSchema(schema, "abc");

    EXPECT_NO_THROW(dut1.check_schema("abc", j1));
    EXPECT_NO_THROW(dut1.remove_undefined_keys(j1));
    EXPECT_TRUE(j1.contains("/a/b"_json_pointer));
    EXPECT_FALSE(j1.contains("/b/c/d"_json_pointer));
}

TEST_F(YjSchemaFixture, check_schema_ignore_keys)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema",
              "optional": "varvarSchema"
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "varvarSchema": {
          "identifier": "var2",
          "allowAnySubkey": true
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    j1["/var2/a"_json_pointer] = 42;
    EXPECT_NO_THROW(dut1.check_for_valid_keys(j1));

    j1["/var2/c/ba"_json_pointer] = 42898;
    EXPECT_NO_THROW(dut1.check_for_valid_keys(j1));

    j1["/axis/fe"_json_pointer] = 498;
    EXPECT_ANY_THROW(dut1.check_for_valid_keys(j1));
}

TEST_F(YjSchemaFixture, check_schema_ignore_keys_disabled)
{
    schema.str(R"(
      {
        "grandSchema": {
          "abc": {
            "axis.abc=0": {
              "required": "",
              "optional": ""
            },
            "axis.abc=2": {
              "required": "axisSchema",
              "optional": "varvarSchema"
            }
          }
        },

        "axisSchema": {
          "identifier": "axis",
          "schema": {
            "axis.abc": {"required": true}
          }
        },

        "varvarSchema": {
          "identifier": "var2",
          "allowAnySubkey": false
        }
      })"
    );

    j1["/axis/abc"_json_pointer] = 2;
    auto dut1 = YjSchema(schema, "abc");

    j1["/var2/a"_json_pointer] = 42;
    EXPECT_ANY_THROW(dut1.check_for_valid_keys(j1));
}

TEST_F(YjSchemaFixture, check_min_range_integer)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "integer", "min": 1}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");


    j1.clear();
    j1["/a/b"_json_pointer] = 2;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 1;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 0;
    EXPECT_ANY_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = -1;
    EXPECT_ANY_THROW(dut1.check_min_max_ranges(j1));
}

TEST_F(YjSchemaFixture, check_min_range_float)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "float", "min": 1}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");


    j1.clear();
    j1["/a/b"_json_pointer] = 2;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 1;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 0;
    EXPECT_ANY_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = -1;
    EXPECT_ANY_THROW(dut1.check_min_max_ranges(j1));
}

TEST_F(YjSchemaFixture, check_min_range_string)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "float", "min": 1}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");


    j1.clear();
    j1["/a/b"_json_pointer] = "2";
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "1";
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "0";
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "-1";
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));
}

TEST_F(YjSchemaFixture, check_max_range_integer)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "integer", "max": 10}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");


    j1.clear();
    j1["/a/b"_json_pointer] = 2;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 10;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 11;
    EXPECT_ANY_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = -1;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));
}

TEST_F(YjSchemaFixture, check_max_range_float)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "float", "max": 10}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");


    j1.clear();
    j1["/a/b"_json_pointer] = 2.0;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 10.0;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = 11.0;
    EXPECT_ANY_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = -1.0;
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));
}

TEST_F(YjSchemaFixture, check_max_range_string)
{
    schema.str(R"(
      {
        "testSchema": {
          "schema": {
            "a.b": {"type": "float", "max": 10}
          }
        }
      })"
    );

    auto dut1 = YjSchema(schema, "");

    j1.clear();
    j1["/a/b"_json_pointer] = "2";
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "10";
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "11";
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));

    j1.clear();
    j1["/a/b"_json_pointer] = "-1";
    EXPECT_NO_THROW(dut1.check_min_max_ranges(j1));
}
