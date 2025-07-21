#include "gtest/gtest.h"

#include "ecb_arg_handler.h"

using namespace ecb;

class ArgHandlerFixture : public testing::Test
{
protected:

    ArgHandlerFixture()
    {
        dut1 = ArgHandler();
    }

    ArgHandler dut1;
};

TEST_F(ArgHandlerFixture, readKey)
{
    dut1.set_argument("--yaml", "filea.yaml");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--action", "readkey");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--key", "abc");
    EXPECT_TRUE(dut1.get_yj_yaml_filename() == "filea.yaml");
    EXPECT_TRUE(dut1.get_output_filename() == "");
    EXPECT_TRUE(dut1.get_yj_key_value() == "abc");
    EXPECT_TRUE(dut1.get_mode() == mode::YJ_READ_KEY_TO_STDOUT);

    dut1.set_argument("--output", "fileb.txt");
    EXPECT_TRUE(dut1.get_yj_yaml_filename() == "filea.yaml");
    EXPECT_TRUE(dut1.get_output_filename() == "fileb.txt");
    EXPECT_TRUE(dut1.get_yj_key_value() == "abc");
    EXPECT_TRUE(dut1.get_mode() == mode::YJ_READ_KEY_TO_FILE);
}

TEST_F(ArgHandlerFixture, updateKey_to_file)
{
    dut1.set_argument("--yaml", "filea.yaml");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--action", "updatekey");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--key", "abc");
    EXPECT_TRUE(dut1.get_yj_yaml_filename() == "filea.yaml");
    EXPECT_TRUE(dut1.get_output_filename() == "");
    EXPECT_TRUE(dut1.get_yj_key_value() == "abc");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--output", "fileb.txt");
    EXPECT_TRUE(dut1.get_yj_yaml_filename() == "filea.yaml");
    EXPECT_TRUE(dut1.get_output_filename() == "fileb.txt");
    EXPECT_TRUE(dut1.get_yj_key_value() == "abc");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--value", "helloWorld");
    EXPECT_TRUE(dut1.get_yj_yaml_filename() == "filea.yaml");
    EXPECT_TRUE(dut1.get_output_filename() == "fileb.txt");
    EXPECT_TRUE(dut1.get_yj_key_value() == "abc");
    EXPECT_TRUE(dut1.get_yj_value() == "helloWorld");

    EXPECT_TRUE(dut1.get_mode() == mode::YJ_UPDATE_KEY);
}

TEST_F(ArgHandlerFixture, updateKey_to_stdout)
{
    dut1.set_argument("--yaml", "filea.yaml");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--action", "updatekey");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--key", "abc");
    EXPECT_TRUE(dut1.get_yj_yaml_filename() == "filea.yaml");
    EXPECT_TRUE(dut1.get_output_filename() == "");
    EXPECT_TRUE(dut1.get_yj_key_value() == "abc");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);

    dut1.set_argument("--value", "helloWorld");
    EXPECT_TRUE(dut1.get_yj_yaml_filename() == "filea.yaml");
    EXPECT_TRUE(dut1.get_output_filename() == "");
    EXPECT_TRUE(dut1.get_yj_key_value() == "abc");
    EXPECT_TRUE(dut1.get_yj_value() == "helloWorld");

    EXPECT_TRUE(dut1.get_mode() == mode::YJ_UPDATE_KEY_TO_STDOUT);
}

TEST_F(ArgHandlerFixture, set_argument_true)
{
    EXPECT_TRUE(dut1.set_argument("--yaml", "filea.yaml"));
    EXPECT_TRUE(dut1.set_argument("--help", "filea.yaml"));
    EXPECT_TRUE(dut1.set_argument("--output", "filea.yaml"));
    EXPECT_TRUE(dut1.set_argument("--action", "build"));
    EXPECT_TRUE(dut1.set_argument("--action", "readkey"));
    EXPECT_TRUE(dut1.set_argument("--action", "updatekey"));
    EXPECT_TRUE(dut1.set_argument("--key", "updatkey"));
    EXPECT_TRUE(dut1.set_argument("--template", "updatekey"));
    EXPECT_TRUE(dut1.set_argument("--templatedir", "updatekey"));
}

TEST_F(ArgHandlerFixture, set_argument_false)
{
    EXPECT_FALSE(dut1.set_argument("--Help", "filea.yaml"));
    EXPECT_FALSE(dut1.set_argument("--action", "c"));
}

TEST_F(ArgHandlerFixture, overwriteArgument)
{
    // set default
    dut1.set_argument("--yaml", "filea.yaml");
    dut1.set_argument("--key", "abc");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);
    dut1.set_argument("--action", "readkey");
    EXPECT_TRUE(dut1.get_mode() == mode::YJ_READ_KEY_TO_STDOUT);

    // allow arguments to be overwritten
    dut1.set_argument("--action", "updatekey");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);
    dut1.set_argument("--output", "outfile.txt");
    EXPECT_TRUE(dut1.get_mode() == mode::INVALID);
    dut1.set_argument("--value", "hello");
    EXPECT_TRUE(dut1.get_mode() == mode::YJ_UPDATE_KEY);
}
