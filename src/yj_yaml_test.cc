//
// ECB - tests for yj_yaml module
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
#include "yj_yaml.h"
#include <string>

using nlohmann::json;
using namespace ecb;

class YjYamlFixture : public testing::Test
{
protected:

    YjYamlFixture()
    {
        dut1 = YjYaml();
        j1 = json();
    }

    YjYaml dut1;
    json j1;
};


TEST_F(YjYamlFixture, int)
{
    const char* testYaml =
        "key: \n"
        "  key1: 100\n"
        "  key2: -100\n"
        "  key3: 0";

    std::stringstream data;
    data << testYaml;

    dut1.read_yaml(data, j1);
    EXPECT_EQ(j1["/key/key1"_json_pointer], 100);
    EXPECT_EQ(j1["/key/key2"_json_pointer], -100);
    EXPECT_EQ(j1["/key/key3"_json_pointer], 0);
}

TEST_F(YjYamlFixture, double)
{
    const char* testYaml =
        "key:\n"
        "  key1: 12.34\n"
        "  key2: 0.1234\n"
        "  key3: 0.0\n"
        "  key4: 1.345e5\n"
        "  key5: -0.00023459403784";

    std::stringstream data;
    data << testYaml;

    dut1.read_yaml(data, j1);
    EXPECT_DOUBLE_EQ(j1["/key/key1"_json_pointer], 12.34);
    EXPECT_DOUBLE_EQ(j1["/key/key2"_json_pointer], 0.1234);
    EXPECT_DOUBLE_EQ(j1["/key/key3"_json_pointer], 0.0);
    EXPECT_DOUBLE_EQ(j1["/key/key4"_json_pointer], 1.345e5);
    EXPECT_DOUBLE_EQ(j1["/key/key5"_json_pointer], -0.00023459403784);
}


TEST_F(YjYamlFixture, bool)
{
    const char* testYaml =
        "key:\n"
        "  key1: true\n"
        "  key2: false\n";

    std::stringstream data;
    data << testYaml;

    dut1.read_yaml(data, j1);
    EXPECT_EQ(j1["/key/key1"_json_pointer], true);
    EXPECT_EQ(j1["/key/key2"_json_pointer], false);
}

TEST_F(YjYamlFixture, string)
{
    const char* testYaml =
        "key:\n"
        "  key1: ec0.s$(DRV_SID).velocitySetpoint01\n"
        "  key2: test-ABC\n"
        "  key3: 'ec0.s$(DRV_SID).ONE.0'\n";

    std::stringstream data;
    data << testYaml;

    dut1.read_yaml(data, j1);
    EXPECT_TRUE(j1["/key/key1"_json_pointer] == "ec0.s$(DRV_SID).velocitySetpoint01");
    EXPECT_TRUE(j1["/key/key2"_json_pointer] == "test-ABC");
    EXPECT_TRUE(j1["/key/key3"_json_pointer] == "ec0.s$(DRV_SID).ONE.0");
}

TEST_F(YjYamlFixture, intList)
{
    const char* testYaml =
        "key:\n"
        "  key1:\n"
        "    - 23\n"
        "    - 42\n"
        "    - 0";

    std::stringstream data;
    data << testYaml;

    dut1.read_yaml(data, j1);
    auto expect = std::vector<int> {23, 42, 0};
    EXPECT_EQ(j1["/key/key1"_json_pointer], expect);
}

TEST_F(YjYamlFixture, stringList)
{
    const char* testYaml =
        "key:\n"
        "  key1:\n"
        "    - dead\n"
        "    - beef\n"
        "    - yes";

    std::stringstream data;
    data << testYaml;

    const auto expect = std::vector<std::string> {"dead", "beef", "yes"};

    EXPECT_NO_THROW(dut1.read_yaml(data, j1));
    std::vector<std::string> result = j1["/key/key1"_json_pointer];

    EXPECT_EQ(expect.size(), result.size()) << "size mismatch";

    for (size_t i = 0 ; i < expect.size() ; ++i)
        EXPECT_EQ(expect[i], result[i]) << "mismatch at index " << i;
}


TEST_F(YjYamlFixture, replaceExistingYamlVariables)
{
    const char* testYaml =
        "testVariables:\n"
        "   test1: z08\n"
        "   test2: er.tz\n"
        "key:\n"
        "  key1: test{{testVariables.test1}}\n"
        "  key2: test_{{testVariables.test2}}_test\n"
        "  key3: test_{{testVariables.test1}}_and.{{testVariables.test2}}012";

    std::stringstream data;
    data << testYaml;

    dut1.read_yaml(data, j1);
    EXPECT_TRUE(j1["/key/key1"_json_pointer] == "testz08") << "is: " << j1["/key/key1"_json_pointer];
    EXPECT_TRUE(j1["/key/key2"_json_pointer] == "test_er.tz_test") << "is: " <<
        j1["/key/key2"_json_pointer];
    EXPECT_TRUE(j1["/key/key3"_json_pointer] == "test_z08_and.er.tz012") << "is: " <<
        j1["/key/key3"_json_pointer];
}

TEST_F(YjYamlFixture, replaceNoneExistingYamlVariables)
{
    const char* testYaml =
        "key:\n"
        "  key1: test{{testVariables.test1}}\n";

    std::stringstream data;
    data << testYaml;

    EXPECT_THROW(dut1.read_yaml(data, j1), std::runtime_error);
}

TEST_F(YjYamlFixture, readYamlKey_string_exist)
{
    const char* testYaml =
        "test: abc\n"
        "key:\n"
        "  key1: test23\n"
        "  key2: hello${NUMBER}test";

    std::stringstream data;
    data << testYaml;

    std::string expect = "hello${NUMBER}test";

    std::string result = dut1.read_yaml_key(data, "key.key2");
    EXPECT_EQ(expect.compare(result), 0) << "result is: " << result;
}

TEST_F(YjYamlFixture, readYamlKey_keyDoNotExist)
{
    const char* testYaml =
        "test: abc\n"
        "key:\n"
        "  key1: test45\n"
        "  key2: hello${NUMBER}test";

    std::stringstream data;
    data << testYaml;

    std::string expect = "";

    std::string result = dut1.read_yaml_key(data, "key.key3");
    EXPECT_EQ(expect.compare(result), 0) << "result is: " << result;
}

TEST_F(YjYamlFixture, readYamlKey_integer)
{
    const char* testYaml =
        "test: abc\n"
        "key:\n"
        "  key1: 42\n"
        "  key2: hello${NUMBER}test";

    std::stringstream data;
    data << testYaml;

    std::string expect = "42";

    std::string result = dut1.read_yaml_key(data, "key.key1");
    EXPECT_EQ(expect.compare(result), 0) << "result is: " << result;
}

TEST_F(YjYamlFixture, readYamlKey_float)
{
    const char* testYaml =
        "test: abc\n"
        "key:\n"
        "  key1: 42.12\n"
        "  key2: hello${NUMBER}test";

    std::stringstream data;
    data << testYaml;

    std::string expect = "42.120000";

    std::string result = dut1.read_yaml_key(data, "key.key1");
    EXPECT_EQ(expect.compare(result), 0) << "result is: " << result;
}

TEST_F(YjYamlFixture, readYamlKey_bool)
{
    const char* testYaml =
        "test: abc\n"
        "key:\n"
        "  key1: false\n"
        "  key2: hello${NUMBER}test";

    std::stringstream data;
    data << testYaml;

    std::string expect = "false";

    std::string result = dut1.read_yaml_key(data, "key.key1");
    EXPECT_EQ(expect.compare(result), 0) << "result is: " << result;
}

TEST_F(YjYamlFixture, updateKey)
{
    const char* testYaml =
        "test: abc\n"
        "key:\n"
        "  key1: 42.12\n"
        "  key2: hellotest";

    const char* expectYaml =
        "test: abc\n"
        "key:\n"
        "  key1: 42.12\n"
        "  key2: okGood\n";

    std::stringstream data;
    data << testYaml;

    auto x = dut1.update_yaml_key(data, "key.key2", "okGood");

    EXPECT_TRUE(x == expectYaml) << x;
}

TEST_F(YjYamlFixture, readPlcFile_fileMissing)
{
    const char* testYaml =
        "plc:\n"
        "  file: ../scripts/templates/xs.plc";

    std::stringstream data;
    data << testYaml;

    EXPECT_NO_THROW(dut1.read_yaml(data, j1));
}

TEST_F(YjYamlFixture, readPlcFile)
{
    const char* testYaml =
        "plc:\n"
        "  file: scripts/templates/test.plc";

    std::stringstream data;
    data << testYaml;

    const auto expect = std::vector<std::string> {"static.XY :=1;", "# comment", "ax${AX_M1}:=ax${AX_M2};"};

    EXPECT_NO_THROW(dut1.read_yaml(data, j1));
    EXPECT_NE(j1["/plc/code"_json_pointer], nullptr);
    std::vector<std::string> result = j1["/plc/code"_json_pointer];

    EXPECT_EQ(expect.size(), result.size()) << "size mismatch";

    for (size_t i = 0 ; i < expect.size() ; ++i)
        EXPECT_EQ(expect[i], result[i]) << "mismatch at index " << i;
}

TEST_F(YjYamlFixture, readPlcCode)
{
    const char* testYaml =
        "plc:\n"
        "  code:\n"
        "    - static.AB := 43|\n"
        "    - var:=static.AB / 12|";

    std::stringstream data;
    data << testYaml;

    const auto expect = std::vector<std::string> {"static.AB := 43|", "var:=static.AB / 12|"};

    EXPECT_NO_THROW(dut1.read_yaml(data, j1));
    EXPECT_NE(j1["/plc/code"_json_pointer], nullptr);
    std::vector<std::string> result = j1["/plc/code"_json_pointer];

    EXPECT_EQ(expect.size(), result.size()) << "size mismatch";

    for (size_t i = 0 ; i < expect.size() ; ++i)
        EXPECT_EQ(expect[i], result[i]) << "mismatch at index " << i;
}

TEST_F(YjYamlFixture, readPlcFileAndCode)
{
    const char* testYaml =
        "plc:\n"
        "  file: scripts/templates/test.plc\n"
        "  code:\n"
        "    - static.AB := 43|\n"
        "    - var:=static.AB / 12|";

    std::stringstream data;
    data << testYaml;

    const auto expect = std::vector<std::string> {"static.XY :=1;", "# comment", "ax${AX_M1}:=ax${AX_M2};", "static.AB := 43|", "var:=static.AB / 12|"};

    EXPECT_NO_THROW(dut1.read_yaml(data, j1));
    EXPECT_NE(j1["/plc/code"_json_pointer], nullptr);
    std::vector<std::string> result = j1["/plc/code"_json_pointer];

    EXPECT_EQ(expect.size(), result.size()) << "size mismatch";

    for (size_t i = 0 ; i < expect.size() ; ++i)
        EXPECT_EQ(expect[i], result[i]) << "mismatch at index " << i;
}
