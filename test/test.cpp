#include "oclhelpers.hpp"
#include <CL/cl.hpp>
#include <gtest/gtest.h>

class SimpleTest : public ::testing::Test {
  void SetUp() override {
    platform = oclhelpers::get_default_platform();
    device = oclhelpers::get_default_device(platform);
  }
  void TearDown() override {}

protected:
  cl::Platform platform;
  cl::Device device;
  cl::Context ctx;
};

TEST_F(SimpleTest, Create) {
  auto kernel = oclhelpers::read_kernel_from_file("vadd.cl");
  ASSERT_NE(kernel, "");
  auto program = oclhelpers::make_program_from_file(ctx, "vadd.cl");
  EXPECT_NO_THROW(oclhelpers::build(program, device));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
