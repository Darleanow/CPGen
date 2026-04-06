#include "CPGen/Resolvers/ModuleResolver.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(UTModuleResolver, UnknownModuleThrows) {
  ModuleResolver resolver;
  EXPECT_THROW(resolver.resolveModule("nonexistent_module_xyz"),
               std::runtime_error);
}

TEST(UTModuleResolver, GtestModuleResolvesName) {
  ModuleResolver resolver;
  auto [module, injections] = resolver.resolveModule("gtest");
  EXPECT_EQ(module.name, "gtest");
}

TEST(UTModuleResolver, GtestModuleHasFetchContent) {
  ModuleResolver resolver;
  auto [module, injections] = resolver.resolveModule("gtest");
  ASSERT_TRUE(module.fetch_content.has_value());
  if (module.fetch_content.has_value()) {
    EXPECT_EQ(module.fetch_content->module_name, "googletest");
    EXPECT_FALSE(module.fetch_content->url.empty());
  }
}

TEST(UTModuleResolver, GtestModuleProducesInjections) {
  ModuleResolver resolver;
  auto [module, injections] = resolver.resolveModule("gtest");
  EXPECT_FALSE(injections.empty());
}

TEST(UTModuleResolver, GtestInjectionTargetIsTest) {
  ModuleResolver resolver;
  auto [module, injections] = resolver.resolveModule("gtest");
  ASSERT_FALSE(injections.empty());
  EXPECT_EQ(injections[0].target_name, "test");
  EXPECT_EQ(injections[0].module_name, "gtest");
}

TEST(UTModuleResolver, GtestInjectionHasLinkLibraries) {
  ModuleResolver resolver;
  auto [module, injections] = resolver.resolveModule("gtest");
  ASSERT_FALSE(injections.empty());
  EXPECT_FALSE(injections[0].link.empty());
}
