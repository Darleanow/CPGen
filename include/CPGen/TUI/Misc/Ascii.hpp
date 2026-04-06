#pragma once

/**
 * @file Ascii.hpp
 * @brief Unicode / Nerd Font glyph constants used throughout the TUI.
 *
 * All constants are @c inline @c constexpr @c const @c char* so they have no
 * runtime overhead and produce no ODR violations when included in multiple
 * translation units.
 *
 * @note The icons in the "Proprietary" section require a
 *       [Nerd Font](https://www.nerdfonts.com/) to render correctly.
 *       The application checks font support at startup via @ref
 * System::isFontValid.
 *
 * @author Darleanow
 */

/**
 * @namespace Ascii
 * @brief UTF-8 encoded glyph constants for the TUI renderer.
 */
namespace Ascii {

/// @name UI elements — checkbox and radio button glyphs
/// @{
inline constexpr const char *RADIO_UNCHECKED =
    "\U000F0130"; ///< Empty radio button.
inline constexpr const char *RADIO_CHECKED =
    "\U000F0134"; ///< Filled radio button.
inline constexpr const char *CHECKBOX_UNCHECKED =
    "\U000F0131"; ///< Empty checkbox.
inline constexpr const char *CHECKBOX_CHECKED =
    "\U000F0856"; ///< Checked checkbox.
/// @}

/// @name Navigation
/// @{
inline constexpr const char *CURSOR =
    "\uf061"; ///< Arrow cursor indicating the focused item.
/// @}

/// @name Application-specific icons (Nerd Font — Proprietary)
/// @{
inline constexpr const char *CPP_ICON = "\U000F0672"; ///< C++ language icon.
inline constexpr const char *NEOVIM_ICON = "\ue6ae";  ///< Neovim editor icon.
inline constexpr const char *CMAKE_ICON =
    "\ue794";                                     ///< CMake build system icon.
inline constexpr const char *GIT_ICON = "\ue702"; ///< Git version control icon.
inline constexpr const char *GOOGLE_ICON =
    "\ue7f0"; ///< Google / GoogleTest icon.
/// @}

/// @name Generic icons (Nerd Font)
/// @{
inline constexpr const char *PACKAGES_ICON =
    "\U000F03D6"; ///< Package/dependency icon.
inline constexpr const char *ROCKET_ICON =
    "\U000F14DF"; ///< Rocket / launch icon.
inline constexpr const char *GEAR_ICON =
    "\uf013"; ///< Settings / configuration icon.
/// @}

/// @name Spinner frames — cycle through 1→6 to animate a loading indicator
/// @{
inline constexpr const char *SPINNER_1 = "\uee06"; ///< Spinner frame 1.
inline constexpr const char *SPINNER_2 = "\uee07"; ///< Spinner frame 2.
inline constexpr const char *SPINNER_3 = "\uee08"; ///< Spinner frame 3.
inline constexpr const char *SPINNER_4 = "\uee09"; ///< Spinner frame 4.
inline constexpr const char *SPINNER_5 = "\uee0a"; ///< Spinner frame 5.
inline constexpr const char *SPINNER_6 = "\uee0b"; ///< Spinner frame 6.
/// @}

/// @name Progress bar segments
/// @{
inline constexpr const char *PROGRESS_EMPTY_LEFT =
    "\uee00"; ///< Empty progress bar — left cap.
inline constexpr const char *PROGRESS_EMPTY_MID =
    "\uee01"; ///< Empty progress bar — middle fill.
inline constexpr const char *PROGRESS_EMPTY_RIGHT =
    "\uee02"; ///< Empty progress bar — right cap.
inline constexpr const char *PROGRESS_FULL_LEFT =
    "\uee03"; ///< Full progress bar — left cap.
inline constexpr const char *PROGRESS_FULL_MID =
    "\uee04"; ///< Full progress bar — middle fill.
inline constexpr const char *PROGRESS_FULL_RIGHT =
    "\uee05"; ///< Full progress bar — right cap.
/// @}

/// @name Status icons
/// @{
inline constexpr const char *SUCCESS = "\uf058"; ///< Success / OK indicator.
inline constexpr const char *WARNING = "\uea6c"; ///< Warning indicator.
inline constexpr const char *ERROR_ICON =
    "\uea87";                                 ///< Error / failure indicator.
inline constexpr const char *INFO = "\uea74"; ///< Informational indicator.
/// @}

/// @name Box-drawing characters (Unicode block U+2500)
/// @{
inline constexpr const char *BOX_CORNER_TOP_LEFT = "\u256d";     ///< ╭
inline constexpr const char *BOX_CORNER_TOP_RIGHT = "\u256e";    ///< ╮
inline constexpr const char *BOX_CORNER_BOTTOM_LEFT = "\u2570";  ///< ╰
inline constexpr const char *BOX_CORNER_BOTTOM_RIGHT = "\u256F"; ///< ╯
inline constexpr const char *BOX_VERTICAL = "\u2502";            ///< │
inline constexpr const char *BOX_HORIZONTAL = "\u2500";          ///< ─
/// @}

} // namespace Ascii
