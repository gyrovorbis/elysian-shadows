/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <AssetIO/proxy/elysian_entity_proxy.hpp>

/*!
    \qmltype Examinable
    \inherits Component
    \since 1.6.0
    \inmodule Engine
    \brief Component attached to allow players to examine an Entity.

    An Examinable component is an optional component that can be attached to an
    Entity, allowing it to be examined by players. For example, this can be added
    to some book lying around to let players read it or to an NPC to give it dialog
    when the player talks to them.

    \sa {Dialog Trees}, Dialog, ExaminableSystem
*/
/*!
 * \qmlproperty bool Examinable::enabled
 *
 * Boolean property set to true if the component is enabled (meaning the Entity can be
 * examined) or false otherwise.
 * \sa isEnabled(), setEnabled()
 */
/*!
 * \qmlproperty float Examinable::maxDist
 *
 * The maximum distance in pixel units the player can be from the Entity and still examine it.
 * \sa getMaxDist(), setMaxDist()
 */
/*!
 * \qmlproperty DIR4_TYPE Examinable::relativeDir
 *
 * The direction the player must be in relative to the Entity in order to examine it.
 * For example, a chest that opens from the front would need this value set to DIR4_DOWN.
 * \sa Direction::DIR4_TYPE, getRelativeDir(), setRelativeDir()
 */
/*!
 * \qmlproperty Dialog* Examinable::dialog
 *
 * Reference to the \l{Dialog} object containing the text and dialog tree to be displayed
 * when the Entity is examined.
 * \sa getDialog(), setDialog()
 */
/*!
 * \qmlmethod bool Examinable::isEnabled()
 *
 * Returns the value of the \l{enabled} property.
 * \sa enabled, setEnabled()
 */
/*!
 * \qmlmethod void Examinable::setEnabled(bool value)
 *
 * Sets the value of the \l{enabled} property to \a{value}.
 * \sa enabled, isEnabled()
 */
/*!
 * \qmlmethod float Examinable::getMaxDist()
 *
 * Returns the value of the \l{maxDist} property.
 * \sa maxDist, setMaxDist()
 */
/*!
 * \qmlmethod void Examinable::setMaxDist(float value)
 *
 * Sets the value of the \l{maxDist} property to \a{value}.
 * \sa maxDist, getMaxDist()
 */
/*!
 * \qmlmethod DIR4_TYPE Examinable::getRelativeDir()
 *
 * Returns the value of the \l{relativeDir} property.
 * \sa Direction::DIR4_TYPE, relativeDir, setRelativeDir()
 */
/*!
 * \qmlmethod void Examinable::setRelativeDir(DIR4_TYPE dir)
 *
 * Sets the value of the \l{relativeDir} property to \a{dir}.
 * \sa Direction::DIR4_TYPE, relativeDir, getRelativeDir()
 */
/*!
 * \qmlmethod Dialog* Examinable::getDialog()
 *
 * Returns the value of the \l{dialog} property.
 * \sa dialog, setDialog()
 */
/*!
 * \qmlmethod void Examinable::setDialog(Dialog* obj)
 *
 * Sets the value of the \l{dialog} property to \a{obj}.
 * Ownership of \a{obj} is transferred from Lua to the engine.
 * \sa dialog, getDialog()
 */

OOLUA_EXPORT_FUNCTIONS(Examinable,
                       setDialog,
                       setMaxDist,
                       setEnabled,
                       setRelativeDir,
                       setHasDescription)
OOLUA_EXPORT_FUNCTIONS_CONST(Examinable,
                             isEnabled,
                             getMaxDist,
                             getRelativeDir,
                             hasDescription,
                             getDialog)
