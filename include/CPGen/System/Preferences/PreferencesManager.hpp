#pragma once

/**
 * @file PreferencesManager.hpp
 * @brief Manages persistent user preferences loaded from a JSON configuration
 * file.
 * @author Darleanow
 */

#include <filesystem>
#include <optional>

/**
 * @struct PreferencesConfig
 * @brief Holds all user-configurable application preferences.
 *
 * Deserialised from the JSON configuration file found at
 * @c ~/.config/CPGen/config.json (or a custom path supplied to
 * @ref PreferencesManager).
 */
struct PreferencesConfig {
  bool support_modern_font =
      false; ///< Whether the terminal font supports Nerd Font / Unicode glyphs.
};

/**
 * @class PreferencesManager
 * @brief Loads and exposes user preferences from a JSON file.
 *
 * Accepts an optional custom configuration path.  When no path is provided the
 * default @c ~/.config/CPGen/config.json is used.  The configuration is lazily
 * available via @ref getConfig; call @c readConfig() internally to populate it.
 *
 * ### Configuration file format
 * @code{.json}
 * {
 *   "SupportModernFont": true
 * }
 * @endcode
 */
class PreferencesManager {
public:
  /**
   * @brief Construct a @c PreferencesManager, optionally overriding the config
   * path.
   * @param config_path Custom path to the JSON preferences file.  When empty
   *        (@c std::nullopt) the default path @c ~/.config/CPGen/config.json is
   * used.
   */
  explicit PreferencesManager(std::optional<std::filesystem::path> config_path);

  /**
   * @brief Return the currently loaded preferences.
   * @return A const reference to the internal @ref PreferencesConfig.
   *         Returns default-initialised values if the config has not been read
   * yet.
   */
  [[nodiscard]] const PreferencesConfig &getConfig() const;

private:
  std::filesystem::path m_config_path{
      "~/.config/CPGen/config.json"}; ///< Resolved path to the JSON config
                                      ///< file.
  PreferencesConfig
      m_preferences; ///< In-memory representation of the loaded preferences.

  /**
   * @brief Read and deserialise the JSON configuration file into @ref
   * m_preferences.
   * @throws std::runtime_error If the file cannot be opened or the JSON is
   * invalid.
   */
  void readConfig();
};
