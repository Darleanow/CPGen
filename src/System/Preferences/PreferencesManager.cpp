#include "CPGen/System/Preferences/PreferencesManager.hpp"
#include "nlohmann/json_fwd.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

void from_json(const nlohmann::json &j, PreferencesConfig &config) {
  j.at("SupportModernFont").get_to(config.support_modern_font);
}

PreferencesManager::PreferencesManager(
    std::optional<std::filesystem::path> config_path) {
  if (config_path.has_value()) {
    m_config_path = config_path.value();
  }
}

const PreferencesConfig &PreferencesManager::getConfig() const {
  return m_preferences;
}

void PreferencesManager::readConfig() {
  std::ifstream config_file(m_config_path.c_str());
  if (!config_file.is_open()) {
    throw std::runtime_error("Can't read config !");
  }

  nlohmann::json json_obj;
  config_file >> json_obj;

  m_preferences = json_obj.get<PreferencesConfig>();
}
