//
// ECB - tests for yj_render module
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
#include "json.hpp"
#include "yj_render.h"

using nlohmann::json;
using namespace ecb;

class YjRenderFixture : public testing::Test
{
protected:

    YjRenderFixture()
    {
        dut1 = YjRender();
        j1 = json();
    }

    YjRender dut1;
    json j1;

    std::stringstream input;
    std::string expect;
    std::string result;
};

TEST_F(YjRenderFixture, replace_isDefined_oneKeyExists)
{
    j1["/key1/a"_json_pointer] = true;
    input.str("{% if key1.a is defined %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isDefined_oneKeyDoNotExist)
{
    input.str("{% if (key1.a is defined) %}\nFALSE{% else %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isDefined_testIncompleteKey)
{
    j1["/key1/a/b"_json_pointer] = false;

    input.str("{%- if (key1.a is defined) %}\nOK\n{% else %}\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isDefined_twoKeys_bothTrue)
{
    j1["/key1/a"_json_pointer] = false;
    j1["/key2/a"_json_pointer] = false;

    input.str("{%- if (key1.a is defined) and (key2.a is defined) %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isDefined_twoKeys_oneFalse)
{
    j1["/key1/a"_json_pointer] = false;

    input.str("{%- if (key1.a is defined) and (key2.a is defined) %}\nFALSE\n{% else %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isDefined_wrongCasing)
{
    j1["/key1/a"_json_pointer] = false;

    input.str("{%- if key1.a Is Defined %}\nFALSE\n{% endif %}");
    expect = "";

    EXPECT_ANY_THROW(dut1.render(input, "", j1));
}

TEST_F(YjRenderFixture, replace_isNotDefined_oneKeyExists)
{
    j1["/key1/a"_json_pointer] = "def";

    input.str("{%- if (key1.a is not defined) %}\nFALSE\n{% else %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isNotDefined_oneKeyDoNotExist)
{
    input.str("{%- if key1.a is not defined %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isNotDefined_testIncompleteKey)
{
    j1["/key2/a/b"_json_pointer] = false;

    input.str("{%- if (key2.a is not defined) %}\nFALSE\n{% else %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isNotDefined_testTwoKeys)
{
    j1["/key2/a"_json_pointer] = 0;

    input.str("{%- if (key1.a is not defined) and (key2.a is not defined) %}\nFALSE{% else %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isNotDefined_wrongCase)
{
    j1["/key2/a"_json_pointer] = 0;

    input.str("{%- if key2.a Is Not Defined %}\nFALSE\n{% endif %}");

    EXPECT_ANY_THROW(dut1.render(input, "", j1));
}

TEST_F(YjRenderFixture, replace_isString_one)
{
    j1["/key1/a"_json_pointer] = "string1";

    input.str("{%- if (key1.a is string) %}\nOK\n{% endif %}");

    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isString_two)
{
    j1["/key1/a"_json_pointer] = 4242;
    j1["/key2/a"_json_pointer] = "42242424";

    input.str("{% if (key1.a is string) and (key2.a is string) %}\nFALSE\n{% else %}\nOK\n{% endif %}");

    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_isString_wrongCase)
{
    j1["/key1/a"_json_pointer] = 4242;
    j1["/key1/a"_json_pointer] = 42242424;

    input.str("{%- if key1.a Is String %}\nFALSE\n{% endif %}");

    EXPECT_ANY_THROW(dut1.render(input, "", j1));
}

TEST_F(YjRenderFixture, replace_loopIndex0_one)
{
    j1["/drive/error"_json_pointer] = {"ONE", "TWO"};


    const char* inputData =
        "{% for bit in drive.error %}\n"
        "ecmcConfigOrDie \"Cfg.LinkEcEntryToObject(0,ax0.drv.alarm{{loop.index0}}\")\n"
        "{% endfor %}";

    input.str(inputData);

    const char* expectData =
        "ecmcConfigOrDie \"Cfg.LinkEcEntryToObject(0,ax0.drv.alarm0\")"
        "\necmcConfigOrDie \"Cfg.LinkEcEntryToObject(0,ax0.drv.alarm1\")\n";

    expect = std::string(expectData);
    result = dut1.render(input, "", j1);

    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_loopIndex0_wrongCase)
{
    const char* inputData =
        "{% for bit in drive.error %}\n"
        "ecmcConfigOrDie \"Cfg.LinkEcEntryToObject(0,ax0.drv.alarm{{loop.Index0}}\")\n"
        "{% endfor %}";

    input.str(inputData);

    EXPECT_ANY_THROW(dut1.render(input, "", j1));
}

TEST_F(YjRenderFixture, replace_intCast_oneInteger)
{
    j1["/key1/a"_json_pointer] = 2;

    input.str("{%- if key1.a|int == 2 %}\nOK\n{% else %}\nFALSE\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_intCast_twoCasts)
{
    j1["/key1/a"_json_pointer] = 2;
    j1["/key2/a"_json_pointer] = 5;

    input.str("{%- if key1.a|int and key2.a|int %}\nOK\n{% endif %}");
    expect = "OK\n";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_intCast_true)
{
    j1["/axis/enable"_json_pointer] = true;

    input.str("ecmcConfigOrDie(XYZ, {{axis.enable|int}}");;
    expect = "ecmcConfigOrDie(XYZ, 1";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_intCast_false)
{
    j1["/axis/enable"_json_pointer] = false;

    input.str("ecmcConfigOrDie(XYZ, {{axis.enable|int}}");;
    expect = "ecmcConfigOrDie(XYZ, 0";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_intCast_float)
{
    j1["/axis/enable"_json_pointer] = 42.42;

    input.str("ecmcConfigOrDie(XYZ, {{axis.enable|int}}");;
    expect = "ecmcConfigOrDie(XYZ, 42.42";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_intCast_missingKey)
{
    input.str("ecmcConfigOrDie(XYZ, {{axis.enable|int}}");;
    EXPECT_ANY_THROW(dut1.render(input, "", j1));
}

TEST_F(YjRenderFixture, replace_intCast_wrongCase)
{
    j1["/key1/a"_json_pointer] = 2;
    input.str("{%- if key1.a|Int == 2 %}\nOK\n{% else %}\nFALSE\n{% endif %}");
    EXPECT_ANY_THROW(dut1.render(input, "", j1));
}

TEST_F(YjRenderFixture, replace_default_one_defined)
{
    j1["/keya/1"_json_pointer] = 2;

    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|default(1)}}");
    expect = "ecmcConfigOrDie(XYDWD, 2";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_default_one_notDefined)
{
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|default(1)}}");
    expect = "ecmcConfigOrDie(XYDWD, 1";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_default_two_oneNotDefined)
{
    j1["/keya/1"_json_pointer] = 2;
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|default(3)}}, {{keyb.2|default(\"test\")}})");
    expect = "ecmcConfigOrDie(XYDWD, 2, test)";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, replace_default_wrongCase)
{
    j1["/keya/1"_json_pointer] = 2;
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|Default(3)}}, {{keyb.2|default(\"test\")}}");
    EXPECT_ANY_THROW(dut1.render(input, "", j1));
}

TEST_F(YjRenderFixture, remove_floatCast_one)
{

    j1["/keya/1"_json_pointer] = 2;
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|float}})");
    expect = "ecmcConfigOrDie(XYDWD, 2)";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, remove_floatCast_two)
{
    j1["/keya/1"_json_pointer] = 2;
    j1["/keya/2"_json_pointer] = 8;

    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|float}}, {{keya.2|float}})");
    expect = "ecmcConfigOrDie(XYDWD, 2, 8)";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, remove_floatCast_wrongCase)
{
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|Float}}");
    EXPECT_ANY_THROW(dut1.render(input, "", j1));
}

TEST_F(YjRenderFixture, readTemplate)
{
    j1["/key1/a"_json_pointer] = 2;
    j1["/key2/a"_json_pointer] = 2;

    std::string output;
    output = dut1.render("scripts/templates/file1.inja", "scripts/templates", j1);

    std::string expect = "f1:A\nf2:A\nf3:A\nf2:B\nf1:B";
    EXPECT_EQ(output.compare(expect), 0);
}

TEST_F(YjRenderFixture, include_ok)
{
    input.str(" {% include \'scripts/templates/file3.inja\' %}");
    expect = "f3:A";

    result = dut1.render(input, ".", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, default_int_key_defined)
{
    j1["/keya/1"_json_pointer] = 2;
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|default(1)|int}})");
    expect = "ecmcConfigOrDie(XYDWD, 2)";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, default_int_key_not_defined)
{
    j1["/keya/1"_json_pointer] = 2;
    input.str("ecmcConfigOrDie(XYDWD, {{keya.2|default(1)|int}})");
    expect = "ecmcConfigOrDie(XYDWD, 1)";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, default_int_bool_true)
{
    j1["/keya/1"_json_pointer] = true;
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|default(0)|int}})");
    expect = "ecmcConfigOrDie(XYDWD, 1)";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, default_int_bool_false)
{
    j1["/keya/1"_json_pointer] = false;
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|default(0)|int}})");
    expect = "ecmcConfigOrDie(XYDWD, 0)";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, default_int_bool_undefined)
{
    input.str("ecmcConfigOrDie(XYDWD, {{keya.1|default(1)|int}})");
    expect = "ecmcConfigOrDie(XYDWD, 1)";

    result = dut1.render(input, "", j1);
    EXPECT_EQ(result.compare(expect), 0) << "result is: " << result;
}

TEST_F(YjRenderFixture, cyclicIncludes)
{
    EXPECT_THROW(dut1.render("scripts/templates/fileA.inja", "scripts/templates", j1),
        std::runtime_error);
}
