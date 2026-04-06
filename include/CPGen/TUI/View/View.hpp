#pragma once

/**
 * @file View.hpp
 * @brief Top-level TUI view that owns all sections and drives the main event loop.
 * @author Darleanow
 *
 * @ref View is the root of the component tree.  It holds a flat list of
 * top-level components (called *sections* — typically @ref ComponentGroup or
 * @ref Input instances), manages focus between them with @ref Defs::Special::Tab,
 * and runs the blocking input loop.
 *
 * ### Typical setup
 * @code
 *   View view;
 *   view.addSection(std::make_unique<ComponentGroup>("Settings", Ascii::GEAR_ICON));
 *   view.addSection(std::make_unique<Input>("Project Name", callback));
 *   view.run(); // blocks until the user presses Escape
 * @endcode
 */

#include "Components/Component.hpp"
#include <memory>
#include <vector>

/**
 * @class View
 * @brief Root TUI manager: renders sections, routes input, and runs the event loop.
 *
 * The @ref run method constructs a @ref Terminal (entering raw mode), then loops
 * reading keys from stdin via @ref Utils::readKey.  Each key is first offered to
 * the currently focused section; if not consumed, @ref View handles @ref Defs::Special::Tab
 * to move focus between sections and @ref Defs::Special::Escape to exit the loop.
 *
 * After every input event the entire screen is redrawn via @ref redraw.
 */
class View {
public:
  /**
   * @brief Append a top-level section to the view.
   *
   * Ownership is transferred.  The first section added automatically receives
   * focus when @ref run starts.
   *
   * @param section A component to display as a top-level section.
   */
  void addSection(std::unique_ptr<Component> section);

  /**
   * @brief Start the blocking TUI event loop.
   *
   * 1. Enters terminal raw mode via @ref Terminal.
   * 2. Grants focus to the first section and redraws.
   * 3. Reads keys in a loop, routing them to the focused section or handling
   *    Tab / Escape at the view level.
   * 4. Exits (and restores the terminal) when @ref Defs::Special::Escape is pressed.
   *
   * @note This method blocks until the user exits the TUI.
   */
  void run();

private:
  /**
   * @brief Clear the screen and repaint all sections top to bottom.
   */
  void redraw() const;

  /**
   * @brief Move focus by @p dir steps through the section list.
   *
   * Cycles with wrap-around (last → first and first → last).  Calls
   * @c setFocused on the old and new sections to trigger their visual update.
   *
   * @param dir +1 to move to the next section, -1 to move to the previous one.
   */
  void moveFocus(int dir);

  std::vector<std::unique_ptr<Component>> m_sections;  ///< Owned top-level sections, rendered in order.
  size_t m_focused_index = 0;                          ///< Index of the section that currently has keyboard focus.
  bool   m_running       = false;                      ///< Event loop control flag; set to @c false to exit @ref run.
};
