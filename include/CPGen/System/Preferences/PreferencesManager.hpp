#pragma once
#include <filesystem>
#include <optional>

struct PreferencesConfig {
  bool support_modern_font = false;
};

class PreferencesManager {
public:
  PreferencesManager(std::optional<std::filesystem::path>);

  const PreferencesConfig &getConfig() const;

private:
  std::filesystem::path m_config_path{"~/.config/CPGen/config.json"};

  PreferencesConfig m_preferences;

  void readConfig();
};
