#pragma once

/**
 * @file ComponentGroup.hpp
 * @brief A titled container that manages a vertical list of child components.
 * @author Darleanow
 */

#include "Components/Component.hpp"
#include <memory>
#include <string>
#include <vector>

/**
 * @class ComponentGroup
 * @brief A container widget that groups and lays out child @ref Component objects.
 *
 * Renders itself as a fixed-width bordered box with a title (and optional icon)
 * in the top border.  Child components are stacked vertically inside the box.
 * The group tracks an internal focus index and routes @ref Defs::Special::Up /
 * @ref Defs::Special::Down to cycle focus between children; all other keys are
 * forwarded to the currently focused child.
 */
class ComponentGroup : public Component {
public:
  /**
   * @brief Construct a ComponentGroup with a title and an optional icon.
   * @param title Displayed in the top border of the group box.
   * @param icon  Optional Nerd Font / Unicode glyph prepended to the title.
   *              Defaults to an empty string (no icon).
   */
  explicit ComponentGroup(std::string title, std::string icon = "");

  /**
   * @brief Append a child component to the bottom of the group.
   *
   * Ownership is transferred to the group.  The first child added automatically
   * receives focus when the group itself gains focus.
   *
   * @param child The component to add.
   */
  void addChild(std::unique_ptr<Component> child);

  /**
   * @brief Render the group and all its children as a multi-line ANSI string.
   * @return Box border + one rendered child per line, all padded to the fixed width.
   */
  [[nodiscard]] std::string render() const override;

  /**
   * @brief Handle a key event, routing it to the focused child or managing focus.
   *
   * - @ref Defs::Special::Up / @ref Defs::Special::Down → move internal focus.
   * - Any other key → forwarded to the focused child's @c handleInput.
   *
   * @param key The key to process.
   * @return @c true if the key was consumed (either by focus movement or by a child).
   */
  bool handleInput(Defs::Key key) override;

  /**
   * @brief Propagate focus state changes to the currently focused child.
   *
   * When the group gains focus, the child at @ref m_focused_index is also focused.
   * When focus is removed from the group, all children are unfocused.
   *
   * @param focused @c true to grant focus, @c false to remove it.
   */
  void setFocused(bool focused) override;

private:
  /**
   * @brief Move internal focus by @p dir steps (-1 = up, +1 = down).
   *
   * Clamps to the valid child range.  Updates @c setFocused on the old and new
   * focused children accordingly.
   *
   * @param dir Direction and magnitude of the focus move.
   * @return @c true if the focus actually moved (i.e. was not already at a boundary).
   */
  bool moveFocus(int dir);

  std::string m_title;                               ///< Title text shown in the group's top border.
  std::string m_icon;                                ///< Optional icon glyph prepended to the title.
  std::vector<std::unique_ptr<Component>> m_children; ///< Owned child components, rendered top to bottom.
  size_t m_focused_index = 0;                        ///< Index of the child that currently has focus.
};
