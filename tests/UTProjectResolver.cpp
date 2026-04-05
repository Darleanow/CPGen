#include "CPGen/Resolvers/ProjectResolver.hpp"
#include "CPGen/Core/Config.hpp"
#include <gtest/gtest.h>

TEST(UTProjectResolver, EmptyModulesProducesNoTargets) {
  ProjectResolver resolver;
  ProjectConfig config;
  config.name = "MyProject";
  config.path = "/tmp/";

  auto result = resolver.resolve(config);
  EXPECT_TRUE(result.modules.empty());
  EXPECT_TRUE(result.targets.empty());
}

TEST(UTProjectResolver, ConfigIsPreservedInResult) {
  ProjectResolver resolver;
  ProjectConfig config;
  config.name = "TestName";
  config.path = "/tmp/";
  config.standard = CppStandard::Cpp20;

  auto result = resolver.resolve(config);
  EXPECT_EQ(result.config.name, "TestName");
  EXPECT_EQ(result.config.path, "/tmp/");
  EXPECT_EQ(result.config.standard, CppStandard::Cpp20);
}

TEST(UTProjectResolver, GtestModuleAddsOneModule) {
  ProjectResolver resolver;
  ProjectConfig config;
  config.name = "MyProject";
  config.path = "/tmp/";
  config.modules = {"gtest"};

  auto result = resolver.resolve(config);
  ASSERT_EQ(result.modules.size(), 1U);
  EXPECT_EQ(result.modules[0].name, "gtest");
}

TEST(UTProjectResolver, GtestModuleAddsTestTarget) {
  ProjectResolver resolver;
  ProjectConfig config;
  config.name = "MyProject";
  config.path = "/tmp/";
  config.modules = {"gtest"};

  auto result = resolver.resolve(config);
  ASSERT_FALSE(result.targets.empty());
  EXPECT_EQ(result.targets[0].type, TargetType::Test);
  EXPECT_EQ(result.targets[0].name, "test");
}

TEST(UTProjectResolver, GtestTargetPathContainsTests) {
  ProjectResolver resolver;
  ProjectConfig config;
  config.name = "MyProject";
  config.path = "/tmp/";
  config.modules = {"gtest"};

  auto result = resolver.resolve(config);
  ASSERT_FALSE(result.targets.empty());
  EXPECT_NE(result.targets[0].path.find("tests"), std::string::npos);
}

TEST(UTProjectResolver, UnknownModuleThrows) {
  ProjectResolver resolver;
  ProjectConfig config;
  config.name = "MyProject";
  config.path = "/tmp/";
  config.modules = {"nonexistent_module_xyz"};

  EXPECT_THROW(resolver.resolve(config), std::runtime_error);
}

TEST(UTProjectResolver, DuplicateModulesDeduplicateTargets) {
  ProjectResolver resolver;
  ProjectConfig config;
  config.name = "MyProject";
  config.path = "/tmp/";
  config.modules = {"gtest", "gtest"};

  auto result = resolver.resolve(config);
  EXPECT_EQ(result.targets.size(), 1U);
}
