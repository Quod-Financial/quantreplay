#include <gtest/gtest.h>

#include "cfg/api/cfg.hpp"

int main(int argc, char** argv) {
  simulator::cfg::init();
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}