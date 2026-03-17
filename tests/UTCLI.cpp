#include "CPGen/CLI/CLI.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(UTCLI, PathValidationTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-p"); // No arg for path
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--path"); // No arg for path
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.push_back((char *)"-/path"); // Ill formatted path
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"/home"); // Valid path
  EXPECT_NO_THROW(cli.parse(2, arr.data()));

  arr.pop_back();
  arr.push_back((char *)"/non_existing_weird/path123");
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);
}

TEST(UTCLI, NameValidationTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-n"); // No arg for name
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--name"); // No arg for name
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.push_back((char *)"-"); // Invalid arg for name
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"TestValid_Name");
  EXPECT_NO_THROW(cli.parse(2, arr.data()));

  arr.pop_back();
  arr.push_back((char *)"InvalidName-");
  EXPECT_THROW(cli.parse(2, arr.data()), std::runtime_error);
}

TEST(UTCLI, TemplateValidationTests) {
  std::vector<char *> arr;
  CLI cli;

  arr.push_back((char *)"-t");
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.pop_back();
  arr.push_back((char *)"--template");
  EXPECT_THROW(cli.parse(1, arr.data()), std::runtime_error);

  arr.push_back((char *)"TemplateName");
  EXPECT_NO_THROW(cli.parse(2, arr.data()));
}
