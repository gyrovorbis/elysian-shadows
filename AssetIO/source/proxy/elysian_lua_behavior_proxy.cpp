#include <proxy/elysian_entity_proxy.hpp>

/*!
    \qmltype Behavior
    \inherits Component
    \since 1.6.0
    \inmodule Engine
    \brief Extendable component for adding custom triggers and logic outside of the engine.

    \table
        \row
            \li Inherits:
            \li \l{Component}, \l{EventTarget}, \l{EventObserver}
    \endtable.

    A Behavior is a custom Component that can be attached to an Entity, allowing a developer to
    add their own data and logic, extending the engine's functionality. Behaviors have been
    modeled similarly to their counterparts in the Unity3D engine, providing a shallow learning
    curve and allowing for easy migration from existing Unity Behaviors. A LuaBehavior is analogous
    to a MonoBehavior, within the Elysian Shadows Engine.

    Behaviors can be subclassed and implemented as standard C++ objects, to extend the engine
    in a statically compiled language or it can be implemented as a \l{LuaBehavior}
    which delegates control to a Lua script. C++ Behaviors offer speed and efficiency and
    can be used when optimizing, and \l{LuaBehavior}s offer rapid development and versatility.



    \sa LuaBehavior, LuaScript
*/

/*!
\qmlproperty enumeration Behavior::TRIGGER_TYPE

Represents the various different types of triggers that can be used in a Behavior.

A TRIGGER_TYPE can be one of the following:
\list
\li Behavior.ON_SCRIPT_CREATE
\li Behavior.ON_SCRIPT_DESTROY
\li Behavior.ON_CREATE
\li Behavior.ON_DESTROY
\li Behavior.ON_LOAD
\li Behavior.ON_UNLOAD
\li Behavior.ON_ENTER
\li Behavior.ON_EXIT
\li Behavior.ON_UPDATE
\li Behavior.ON_RENDER
\li Behavior.ON_COLLIDE
\li Behavior.ON_COLLISION_ENTER
\li Behavior.ON_COLLISION_EXIT
\li Behavior.ON_EXAMINE
\li Behavior.ON_DIALOG
\li Behavior.ON_COMBAT_ENTER
\li Behavior.ON_COMBAT_EXIT
\li Behavior.ON_COMBAT_DEATH
\li Behavior.ON_SHATTER
\li Behavior.ON_COMBAT_RECEIVE_STATUS_MODIFIER
\li Behavior.ON_COMBAT_TARGET_COLLISION_ENTER
\li Behavior.ON_BUTTON_PRESS
\li Behavior.ON_BUTTON_RELEASE
\li Behavior.ON_TK_SELECT
\li Behavior.ON_TK_UNSELECT
\endlist
*/
/*!
 * \qmlmethod void Behavior::addTrigger(TRIGGER_TYPE type)
 *
 * Adds the trigger of \a{type} to the list of active triggers by the Behavior.
 */
/*!
 * \qmlmethod void Behavior::removeTrigger(TRIGGER_TYPE type)
 *
 * Removes the trigger of \a{type} from the list of active triggers by the Behavior.
 */
/*!
 * \qmlmethod string Behavior::getBehavName()
 *
 * Returns the name of the Behavior for a C++ Behavior or the asset-relative file path of
 * the Behavior script for a LuaBehavior.
 */
EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Behavior,
                                   addTrigger,
                                   removeTrigger)
EXPORT_OOLUA_FUNCTIONS_1_CONST(Behavior,
                                getBehavName)


/*!
    \qmltype LuaBehavior
    \inherits Behavior
    \since 1.6.0
    \inmodule Engine
    \brief Custom Behavior Component for extending the engine through a Lua script.

    LuaBehavior is a \l{Behavior} implementation that allows a LuaScript to implement the \l{Behavior}'s
    trigger methods, providing a means of extending the Engine with the Lua scripting language.
    The script file and environment table of the script, which are shared among instances, are
    stored within LuaBehaviorScript, while the actual instance data is a LuaBehavior object. You
    can compare a LuaBehavior to the \l{Collidable} \l{Component}, as they are both instanced objects,
    created separately for each Entity. Both also contain a reference to an object containing attribute
    data which can be shared among similar instances. Just as similar \l{Collidable} instances can share the
    same \l{CollidableAttrib} instance, multiple LuaBehavior instances created from the same script share
    the same \l{LuaBehaviorScript} object.

    \section2 Managing LuaBehavior Instances
    \section3 Attaching a LuaBehavior to an Entity
    Assuming a LuaBehaviorScript has already been created properly (see the \l{Scripting a LuaBehavior}
    section for more information on script setup and code requirements), it can be instantiated and attached to an
    active Entity as a LuaBehavior. To do this, simply call:
    \code
    entity:addLuaBehavior("/assets/full/path/to/script/behav.lua", paramTable)
    \endcode
    The paramTable is an optional table that can be passed to \l{Entity::addLuaBehavior} containing
    arbitrary arguments that the Engine will then pass as the second argument to the onCreate() trigger. This is
    analagous to passing data to the constructor of an object. If the object doesn't require additional data,
    or if the LuaBehaviorScript does not use the onCreate() trigger, the paramTable argument may be omitted.

    \section3 Removing a LuaBehavior from an Entity
    An existing LuaBehavior can be removed an Entity by simply calling:
    \code
    entity:removeLuaBehavior("/assets/full/path/to/script/behav.lua")
    \endcode
    If the associated LuaBehaviorScript defines an onDestroy() trigger, it will be called just before the
    script is removed and its associated data is deleted.
    \sa {Entity::removeLuaBehavior}

    \section3 Retrieving a LuaBehavior from an Entity
    A reference to an existing LuaBehavior can be fetched from the Entity it's attached to by index
    or by its script path:
    \code
    local behav1 = entity.behaviors[1]
    local behav2 = entity:getLuaBehavior("/assets/some_behav.lua")
    \endcode

    \section2 Scripting a LuaBehavior
    \section3 Triggers
    A \l{Behavior} contains a set of methods which are used as callbacks by the Engine, allowing it
    to transfer the control of execution to a Behavior when certain events occur. Each trigger is an
    optionally overridable method which can be reimplemented within a subclass for custom logic.
    All triggers in the base Behavior class can be used within a LuaBehavior and are accessible
    from Lua.

    \e{NOTE: For \l{LuaBehavior}s, each instance-based trigger has an extra argument called the
    "self" table, which is a standard Lua Table used to hold any instance-specific data. For more
    information on, refer to \l{LuaBehavior Instance Variables and Self Table}}

    \section4 Core Engine Triggers
    The following triggers are built into the Engine, supported on every platform, and are available everywhere.

    \table
        \row
            \li {1, 1} \l{Behavior::TRIGGER_TYPE}{TRIGGER_TYPE}
            \li {1, 1} \b Signature
        \row
            \li {1, 1} \b{ON_SCRIPT_CREATE}
            \li {1, 1} \c{onScriptCreate()}
        \row
            \li {2, 1} Called only the first time the script is loaded (rather than per-instance as with
                \c onCreate()). This essentially works like a static constructor and is the same thing
                as placing logic globally outside of a trigger for immediate execution, although
                this is neater and allows for better memory control. Use this to initialize anything
                shared by every LuaBehavior instantiated from this script, rather than duplicating it for
                every instance by implementing it in \c onCreate().
        \row
            \li {1, 1} \b{ON_SCRIPT_DESTROY}
            \li {1, 1} \c{onScriptDestroy()}
        \row
            \li {2, 1} Opposite of \c onScriptCreate(), acts as a static destructor called only after all
                LuaBehavior instances referencing the script have been destroyed (onDestroy is called
                for each of these instances, if implemented), and is only called once. Use this (rather than \c onDestroy())
                to clean up data shared by every instance of the script that has been created or allocated by \c onScriptScreate().
        \row
           \li {1, 1} \b{ON_CREATE}
           \li {1, 1} \c{onCreate(LuaTable self, LuaTable params)}

        \row
           \li  {2, 1} Serves as the constructor for a LuaBehavior and is called immediately after its
                creation. \c params is an optional table that can be passed through \l{Entity::addLuaBehavior}
                for providing custom input arguments during instantiation. Any instance-specific variables should be
                initialized in the \c self table, and a well-formed script should not REQUIRE a \c params
                table (and should use default values instead if one is not supplied or if not all parameters
                are present within the table supplied).
        \row
           \li {1, 1} \b ON_DESTROY
           \li {1, 1} \c { onDestroy(LuaTable self) }
       \row
            \li {2, 1} Serves as the destructor for a LuaBehavior. Clean up any assets or data that won't be
                automatically garbage collected, usually residing within the \c self table and associated with
                a particular instance.

       \row
            \li {1, 1} \b ON_LOAD
            \li {1, 1} \c { onLoad(LuaTable self) }
       \row
            \li {2, 1} Called when the \l{Area} associated with the LuaBehavior has \e{finished} loading. If the
                script needs to access any other Entities or Behaviors that are stored within the \l{Area},
                it should be done here, rather than \c onCreate(), since you cannot ensure that the other objects
                have been constructed, initialized, and loaded yet using the \c onCreate() trigger!
       \row
            \li {1, 1} \b ON_UNLOAD
            \li {1, 1} \c { onUnload(LuaTable self) }
       \row
            \li {2, 1} Opposite of \c onLoad(), called immediately \e{before} an \l{Area} is about to be deleted from memory,
                so all other Entities and Behaviors within the \l{Area} should still be available for access
                during this context (as opposed to the \c onDestroy() trigger, when other objects within the \l{Area} may have
                already been deleted).
       \row
            \li {1, 1} \b ON_ENTER
            \li {1, 1} \c { onEnter(LuaTable self) }
       \row
            \li {2, 1} Called immediately \e{after} the player enters the same \l{Area} as the parent \l{Entity} that the LuaBehavior
                has been attached to. Should be used rather than \c onCreate() or \c onLoad() when you need to ensure that the player
                and party members are present within an \a{Area}.
       \row
            \li {1, 1} \b ON_EXIT
            \li {1, 1} \c { onExit(LuaTable self)}
       \row
            \li {2, 1} Opposite of \c onEnter(). Called just \e{before} the player leaves the \l{Area} with the associated
                LuaBehavior. Should be used rather than \c onDestroy() or \c onUnload() when you need to access the player
                player or any other party members for deinitialization logic.

       \row
            \li {1, 1} \b ON_UPDATE
            \li {1, 1} \c {onUpdate(LuaTable self)}
       \row
            \li {2, 1} Most versatile trigger, can implement any arbitrary logic and is called by the Engine once per frame.
                Note that because of this invocation frequency, this is also the slowest trigger, and the amount of
                logic and processing done here should be kept to a minimum, instead prefering to delegate it to other,
                more specific triggers or using the \l{EventSystem} when there is no trigger satisfying your requirements.
      \row
            \li {1, 1} \b ON_COLLIDE
            \li {1, 1} \c {onCollide(LuaTable self, CollisionContact contact, Collidable otherCollidable)}
      \row
            \li {2, 1} Called \e{every} frame a collision is detected between the script's parent \l{Entity} and another Entity.
                The associated CollisionContact and the \l Collidable \l Component of the other Entity are both passed as arguments. If you
                are not requring frame-base logic to be executed during collisions, you should favor the \c onCollisionEnter() and
                \c onCollisionExit() triggers, which reduce overhead by not repeatedly firing every frame the contact is active.
                \e{NOTE: The parent \l Entity must contain a \l Collidable and the bitwise AND operation of its \l{CollisionFlags::type}
                must produce a nonzero value with the colliding Entity's \l{CollisionFlags::typeAgainst}, or the \l CollisionSystem
                will ignore collisions between the two. Refer to \l{CollidableAttrib} for more information.}
      \row
            \li {1, 1} \b ON_COLLISION_ENTER
            \li {1, 1} \c {onCollide(LuaTable self, CollisionContact contact, Collidable otherCollidable)}
      \row
            \li {2, 1} Called \e{once} as soon as another \l Entity makes contact with the script's parent Entity and will not
                be called again (for the same Entity) until it breaks contact then collides again.
                \e{NOTE: The parent Entity must contain a \l Collidable and the bitwise AND operation of its \l{CollisionFlags::type}
                must produce a nonzero value with the colliding Entity's \l{CollisionFlags::typeAgainst}, or the \l CollisionSystem
                will ignore collisions between the two. Refer to \l{CollidableAttrib} for more information.}
      \row
            \li {1, 1} \b ON_COLLISION_EXIT
            \li {1, 1} \c {onCollide(LuaTable self, Collidable otherCollidable)}
      \row
            \li {2, 1} Called \e{once} as soon as an \l Entity that was previously colliding with the script's parent Entity
                moves out of contact with it, and will not be called again (for the same Entity), until it collides
                then moves out of contact again.
                \e{NOTE: The parent \l Entity must contain a \l Collidable and the bitwise AND operation of its \l{CollisionFlags::type}
                must produce a nonzero value with the colliding Entity's \l{CollisionFlags::typeAgainst}, or the \l CollisionSystem
                will ignore collisions between the two. Refer to \l{CollidableAttrib} for more information.}
      \row
            \li {1, 1} \b ON_SHATTER
            \li {1, 1} \c {onShatter(LuaTable self)}
      \row
            \li {2, 1} Called when the \l Entity containing the associated LuaBehavior shatters, which happens when its
                \l{RigidBody::durability} property decreases to below 0.
                \e {NOTE: The parent \l Entity of the associated script requires a RigidBody \l Component and a nonnegative
                   \l{RigidBodyAttrib::baseDurability} property for this trigger to be called correctly. Refer to \l RigidBody
                   for more information.}

      \row
            \li {1, 1} \b ON_EXAMINE
            \li {1, 1} \c {onExamine(LuaTable self)}
      \row
            \li {2, 1} Called when the player examines the \l Entity containing the associated LuaBehavior.
                \e {The parent \l Entity of the associated LuaBehavior must have a properly configured \l Examinable \l Component
                    attached for this trigger to be called properly.}

      \row
            \li {1, 1} \b ON_DIALOG
            \li {1, 1} \c {onDialog(LuaTable self, unsigned screen, int option)}
      \row
            \li {2, 1} Called when the player engages in a \l{Dialog} with the parent \l Entity containing the associated
                LuaBehavior, and is only called for screens whose \l{DialogScreen::trigger} property is set to true.
                The first argument is the screen index of the triggering \l{DialogScreen}. The second argument is the index of the
                \l{DialogOption} chosen by the player if the triggering \l{DialogScreen} is a \l{DialogPrompt} or -1 if the triggering
                screen is \l{DialogText}.

       \row
            \li {1, 1} \b ON_COMBAT_ENTER
            \li {1, 1} \c { onCombatEnter(LuaTable self) }
       \row
            \li {2, 1} Called when the parent \l Entity containing the associated LuaBehavior enters into combat,
                whether automatically through CombatSystem aggro detection or whether manually added to combat from a script.
                \e {NOTE: The parent \l Entity must contain a properly configured \l Combat \l Component for this trigger to be
                    called correctly.}
       \row
            \li {1, 1} \b ON_COMBAT_EXIT
            \li {1, 1} \c { onCombatExit(LuaTable self) }
       \row
            \li {2, 1} Called when the parent \l Entity containing the associated LuaBehavior leaves combat, whether it be
                removed automatically by the \l{CombatSystem} when the player gets too far away for aggro or whether it's removed
                manually by a script.
                 \e {NOTE: The parent \l Entity must contain a properly configured \l Combat \l Component for this trigger to be
                    called correctly.}
       \row
            \li {1, 1} \b ON_COMBAT_DEATH
            \li {1, 1} \c { onCombatDeath(LuaTable self) }
       \row
            \li {2, 1} Called when the parent \l Entity containing the associated LuaBehavior has 0 or less HP.
                \e {NOTE: The parent \l Entity must contain a properly configured \l Combat \l Component for this trigger to be
                    called correctly.}

       \row
            \li {1, 1} \b ON_COMBAT_RECEIVE_STATUS_MODIFIER
            \li {1, 1} \c { onCombatReceiveStatusModifier(LuaTable self, CombatStatus statusModifier) }
       \row
            \li {2, 1} Called whenever something attempts to modify the stats of the parent \l Entity
                (enemy damage, using a potion, manually within script, MP restored from save point, etc). The CombatStatus
                reference contains the raw, requested modification to the affected Entity's stats, and any armor, weapons, spells,
                buffs, resistances, special effects, or counters that reduce, mitigate, increase, or otherwise alter this
                damage are to be handled here, by adjusting the respective values within the CombatStatus reference, which
                the Engine will use to apply the resulting status change.
                \e {NOTE: The parent \l Entity must contain a properly configured \l Combat \l Component for this trigger to be
                    called correctly.}

       \row
            \li {1, 1} \b ON_COMBAT_TARGET_COLLISION_ENTER
            \li {1, 1} \c { onCombatTargetCollisionEnter(LuaTable self, CollisionContact contact, Collidable otherCollidable, CombatStatus statusModifier)}
       \row
            \li {2, 1} Called if the parent \l Entity (whose \l{Combat::dmgType} must either be \c Combat::DMG_SOURCE or \c Combat::DMG_SOURCE_TARGET collides
            with another combatant Entity whose \l{Combat::dmgType} is either \c Combat::DMG_TARGET or \c Combat::DMG_SOURCE_TARGET. The intention is
            for this trigger to be used to apply collision-based damage from an object such as a sword or a projectile to the
            Entity it hits. The \c{statusModifier} argument is is provided to allow the collision to produce damage or any other
            type of status effect and is sent to the receiving Entity's \c onCombatReceiveStatusModifier() for additoinal modification before the
            \c statusModifier is ultimately applied.
        \e{NOTE: The parent \l Entity requires both a properly configured \l Collidable and \l Combat \l Component. For this to work as intended,
            the bitwise AND operation of the parent Entity's \l{CombatAttrib::type}
            with the colliding Entity's \l{CombatAttrib::typeAgainst} must produce a nonzero number, indicating that the two
            can damage each other, and likewise, the parent Entity's \l{CollisionFlags::type} flags bitwise AND-ed with the
            colliding Entity's \l{CollisionFlags::typeAgainst} must produce a nonzero number, indicating that the \l{CollisionSystem}
            should be checking for collisions between the two Entities.}


    \endtable

    \section4 General/Extendible Triggers
    A LuaBehavior inherits the following triggers from \e{EventTarget} and \e{EventObserver}, allowing
    Lua scripts to process an unlimited number of different \l{Events} which may or may not be
    implemented within the Engine itself. Every built-in trigger has a corresponding \l Event, which
    can be used to bypass the trigger mechanism. This can be useful when proxying trigger
    events or managing them through some external object. In addition, custom \l{LuaEvent}s allow a
    script to send, receive, and observe any number of user-defined Events which the Engine is not
    even aware of. In this manner, these triggers may be seen as the building blocks for implementing
    custom triggers. For a more detailed description of how these methods and the rest of the
    \l{EventSystem} interoperate, refer to the \l{EventSystem}{EventSystem Documentation}.


    \table
        \row
            \li {1, 1} \l{Behavior::TRIGGER_TYPE}{TRIGGER_TYPE}
            \li {1, 1} \b Signature
        \row
            \li {1, 1} \b{ON_EVENT_RECEIVE}
            \li {1, 1} \c{onEventReceive(LuaTable self, Event event)}
        \row
            \li {2, 1} Called when an \l{Event} is directly sent to the LuaBehavior, whether it's from
                        the Engine, an external script source, or the \l{LuaBehavior} itself.
        \row
            \li {1, 1} \b{ON_EVENT_OBSERVE}
            \li {1, 1} \c{onEventObserve(LuaTable self, Event event, EventTarget target)}
        \row
            \li {2, 1} Called when a particular \l{Event} type being observed by the LuaBehavior is
                        sent to an observed \c target or is broadcasted to all observers.
                        \e{NOTE: For this trigger to ever be called, you have to register the LuaBehavior as an
                        EventObserver for the desired \l Event through \l{EventSystem::addEventObserver}, which is usually
                        done on the onCreate() trigger, passing self.selfBehav as the EventObserver. Refer to \l{EventSystem}
                        for more information.}
    \endtable

    \section4 Toolkit-Specific Editor and GUI Triggers
    The following triggers are only available when the script is run within ESTk and may be used to
    implement custom UI logic for the Behavior or to extend the Toolkit with custom editors. Because they are only
    used from the Toolkit, no gameplay-specific code should depend on these triggers, as they will
    never be called in standalone Engine builds on any platforms.

    \table
        \row
            \li {1, 1} \l{Behavior::TRIGGER_TYPE}{TRIGGER_TYPE}
            \li {1, 1} \b Signature
        \row
            \li {1, 1} \b{ON_TK_SELECT}
            \li {1, 1} \c{onTkSelect(LuaTable self)}
        \row
            \li {2, 1} Called when the parent \l{Entity} becomes selected within ESTk.
                        \e{NOTE: ESTk supports multiselection with Entities, so this trigger is can even
                            be called if the parent Entity is added to an existing selection, and does not
                            imply the Entity is the only item selected.}
        \row
            \li {1, 1} \b{ON_TK_UNSELECT}
            \li {1, 1} \c{onTkUnselect(LuaTable self)}
        \row
            \li {2, 1} Called when the parent \l{Entity} is no longer selected within ESTk.
                        \e{NOTE: ESTk supports multiselection with Entities, so this trigger is called
                        only when the specified Entity is no longer a part of the selection. In the
                        case of multiselect, there may still be other selected Entities remaining, and
                        this trigger does not necessarily imply that the entire selection has been cleared.}
    \endtable


    \section3 Script Attributes
    Every LuaBehavior script must supply a global function named \c setScriptAttrib() at the bare minimum.
    This is a special function used by LuaBehaviors to inform the Engine about the script's
    properties, such as which triggers it supports or whether it \l{Inheriting from another LuaBehavior}{inherits from another LuaBehavior}.
    The Engine passes a LuaBehaviorScriptAttrib reference to the function, which is then configured
    based on the script's attributes and is later used by the Engine.

    \section3 Adding a Trigger
    To add a trigger to the script, the proper \l{Behavior::TRIGGER_TYPE} must be added to the
    LuaBehaviorScriptAttrib argument, passed by the Engine, telling it that we are implementing the associated function
    within the LuaBehavior. For example, to use the onUpdate() trigger within a Lua script, we must also add
    the associated trigger to the list of supported triggers:
    \code
    function setScriptAttrib(attrib)
        --tell the Engine we're implementing the onUpdate() trigger!
        attrib:addSupportedTrigger(Behavior.ON_UPDATE)
    end

    --[[ Triggers are implemented as global functions with a Behavior script.
        Engine will call this trigger once every frame now that it has been
        properly registered in setScriptAttrib(). ]]--
    function onUpdate(self)
        print("Hello, world!")
    end
    \endcode
    \e{NOTE: If you add a supported trigger in \c setScriptAttrib(), you MUST also define its corresponding
    function, or the script will error out after \c setScriptAttrib() returns and control is transferred to the
    Engine, as it will attempt to begin building an index of each trigger function. You will also get an
    error when it comes time to invoke the missing trigger as well. }

    \section3 Toggling Triggers Per Instance
    When you add a trigger via \c setScriptAttrib(), you are essentially telling the Engine which triggers
    the LuaBehaviorScript defines, but a particular LuaBehavior instance may not actually use all of the
    triggers defined within a script at any given point in time, or it may only use certain ones under
    certain scenarios.

    By default every trigger provided by a LuaBehaviorScript is active within a LuaBehavior, but you can
    toggle triggers on and off at run-time by using the \l{Behavior::addTrigger} and \l{Behavior::removeTrigger}
    functions. This is the \e{preferred} way for LuaBehaviors to operate, as only keeping the relevant triggers
    active and removing the other ones reduces Engine overhead caused by wasted calls into Lua triggers
    that will ultimately be ignored.

    \section3 Script Lifetime and Implicit Data Sharing
    The Engine is smart enough to only parse and load a new LuaBehaviorScript once, sharing internal data
    among any subsequent LuaBehavior instances referencing the same script, as long as at least one instance
    is still in use. When every LuaBehavior instance referencing the LuaBehaviorScript is freed, the
    LuaBehaviorScript will finally be freed as well, with any subsequent LuaBehavior instantiation using the script requiring
    the Engine to reprocess the script and reload it into memory again (resulting in triggers like \c onScriptCreate() and
    \c onScriptDestroy() to be called once again).

    \section3 LuaBehavior Script Environment and Variable Scope
    The Engine optimizes LuaBehavior overhead by sharing the same LuaScript environment table (\c _ENV)
    among all instances of the same LuaScript. Because of this, anything declared within the global
    scope is analogous to a static class variable, shared among all instances of the same LuaBehaviorScript
    as though it were global from their perspective. Despite this fact, it's still not actually a true global variable,
    and is only a global within the script's environment, so there are some subtle differences to be aware of. For example,
    a LuaBehavior with the following code:
    \code
    myVal = 0

    function onCreate()
        myVal = myVal + 1
    end
    \endcode
    Will create a static variable, \c myVal, which is incremented every time the \c onCreate() trigger is called
    to create a new LuaBehavior. Since \c myVal has been declared globally, it's stored within the shared \c _ENV
    table, meaning all instances of the LuaBehavior share the same instance of \c myVal, rather than each getting
    its own copy, resulting in myVal incrementing every time a LuaBehavior instance is created from the script.

    Since LuaBehaviorScript is a subclass of LuaScript, it uses the same sandboxing mechanism. Please refer to
    the \l{Sandboxing and Environment Table} and \l {Global Environment and Variable Shadowing} LuaScript documentation
    subsections for more details on how this is implemented, the \c _ENV and \c _G tables, and their implications.

    \section3 LuaBehavior Instance Variables and Self Table
    Since all global variables are shared among instances of the same LuaBehavior, any instance-specific data
    must be stored within a special \c self table, which is created by the Engine and is passed back as the first
    argument to all instance-level trigger methods. By default, the Engine adds two variables to the self table:
          s\c selfBehav and \c selfEntity, which are references
    to the current LuaBehavior object and the \l Entity it is attached to respectively.

    The following code demonstrates variable scope and \c self table usage.
    \code
    [..]
    staticCounter = 1   --Counter value shared by all scripts

    --self table created by engine and passed to each trigger
    function onCreate(self)
        print(self.selfEntity.name)     --prints the name of the Entity we've attached the Behavior to
        staticCounter = staticCounter+1 --Increment shared counter when a new instance is created
        self.instanceCounter = 0        --Separate counter created for every instance of a script
        print(staticCounter)            --prints instance number of script
        print(self.instanceCounter)     --always prints 0
    end

    --self table passed back to onUpdate for access to instance data
    function onUpdate(self)
        self.instanceCounter = self.instanceCounter + 1 --increment each instance's private counter
    end
    \endcode
    In the previous example, the global "environment" table (\c _ENV) is used to store the staticCounter, which
    is only ever created once and is shared by every instance of a LuaBehavior, while the \c self table
    is used to create separate counters which are unique to each instance.

    The first parameter of the \c onCreate() and \c onUpdate() triggers is the \c self table, which is
    a separate table created for every LuaBehavior, used to maintain any
    instance-unique data. This \c self table is then passed back to the script as the first argument
    to most Behavior triggers, allowing each trigger to operate on its own unique instance data.

    \section3 Inheriting from another LuaBehavior
    A LuaBehavior can be designated as "inheriting" from another LuaBehavior, as a child class in a traditional
    object-oriented heirarchy, so that existing trigger implementations and instance data from a parent can be
    accessed from within the child LuaBehavior. While only single inheritance is supported, there is no limit on
    how large the inheritance heirarchy can be. To inform the Engine of a parent/child relationship between two
    LuaBehaviors, the child LuaBehavior must provide a path to its parent script for the Engine in its
    \c ssetScriptAttrib() method:
    \code
    function setScriptAttrib(attrib)
        --tell the Engine we're implementing the onUpdate trigger!
        attrib:addSupportedTrigger(Behavior.ON_UPDATE)
        --tell the Engine we intend to inherit from another behavior.
        attrib:inheritFromBehavior("/assets/path/to/parent/script.lua")
    end
    \endcode

    Inheriting from another LuaBehavior in such a manner has two major implications:

        \b{1. The Child LuaScript inherits from its parent's LuaScript.}
        This means that the child LuaBehavior will have access to all global variables and functions declared within
        the parent. The parent may implement and rely on trigger methods that are not used within the child, and this
        not a problem, as the Engine will find the corresponding trigger function definitions in the parent table.
        Reimplementing such a trigger within the child will \e{override} the parent's implementation, and the Engine will now
        invoke the function implemented within the child LuaBehavior for the trigger. A child's implementation of a trigger can still
        manually call into the parent's implementation by directly calling the parent's implementation through the _ENV
        table's \c __index metamethod (which is set to the parent's \c _ENV table). The only exceptions to the overriding rule
        are for the \c onCreate() and \c onDestroy() triggers, which will be called at every level in the heirarchy (just like
        constructors and destructors for objects in C++, Java, C#, and other OO languages).

        \b{2. The Child self table inherits from its parent's self table.}
        The \c self table of the child LuaBehavior has its \c __index metatable set to the \c self table of the parent LuaBehavior,
        meaning the child has access to all of its instance data and all of the instance data of its parent(s). Modifying
        a value of the parent's self table through the child's table will override the data but will not actually modify it.


   The following example of a LuaBehavior heirarchy demonstrates these concepts:
   \e{parent_behav.lua}
    \code
    parentGlobalString  = "Hello World!"
    parentInstances     = 0

    function setScriptAttrib(attrib)
        attrib:addSupportedTrigger(Behavior.ON_UPDATE)
        attrib:addSupportedTrigger(Behavior.ON_CREATE)
    end

    --First function called when a new instance of child is created
    function onCreate(self, params)
        parentInstances = parentInstances + 1
        self.parentInstance = parentInstance
        print("Creating parent #"..parentInstances)
    end

    function onUpdate(self)
        print("Hello from parent!")
    end

    function onDestroy(self)
        print("Destroying parent #"..self.parentInstance)
    end
    \endcode

   \e{child_behav.lua}
    \code
    childInstances = 0

    function setScriptAttrib(attrib)
        attrib:addSupportedTrigger(Behavior.ON_UPDATE)
        attrib:addSupportedTrigger(Behavior.ON_CREATE)
        --tell the Engine we intend to inherit from another behavior.
        attrib:inheritFromBehavior("parent_behav.lua")
    end

    --[[
        Called after the parent's onCreate() when a new child is created,
        with the self table automatically using the parent's self table as
        its __index metatable.
    ]]--
    function onCreate(self, params)
        childInstances = childInstances + 1
        self.childInstance = childInstances
        print("Creating child #"..childInstances)

        --Accesses "parentGlobalString" without a problem, since it is located
        --in the parent's _ENV table which the current _ENV table inherits from.
        print(parentGlobalString)
    end

    --OVERRIDES the parent trigger, meaning only the child implementation will be
    --invoked for the trigger by the Engine.
    function onUpdate(self)
        getmetatable(_ENV).__index.onUpdate(self) --manually call into the parent's trigger too!
        print("Hello from Child!")
    end

    function onDestroy(self)
        print("Destroying child #"..self.childInstance)
    end
    \endcode
    Assuming two LuaBehaviors were instantiated from the child script, the following output would
    be generated:
    \code
    --during creation
    Creating parent #1. --first parent's onCreate is called with a new self table
    Creating child  #1. --child's onCreate is called with a new self table that inherits from parent's
    Creating parent #2  --same for second instance
    Creating child  #2
    --after a single frame
    Hello from parent. --Child's onUpdate is called, which first manually calls into parent's implementation
    Hello from child.  --Child's onUpdate
    Hello from parent. --Same for second instance
    Hello from child.
    --during deletion
    Destroying child  #1. --Destructors are called in reverse order from constructors, so
    Destroying parent #1. --child destructor is called first, then parent's.
    Destroying child  #2. --Same for second instance.
    Destroying parent #2.
    \endcode
    \e{NOTE: The onUpdate() trigger defined within the child can access both self.childInstance (regular
    instance data) and self.parentInstance (since it's inheriting the parent's self table), while the
    parent's onUpdate() implementation can only access self.parentInstance and not self.childInstance.}

    \section3 Best Practices When Scripting Lua Behaviors
    \section4 Using the params Table for User-Configurable Input Data
    The ideal LuaBehavior is a generic, user-configurable script which can be attached to any Entity
    then customized for individual Entities through the SelectionView in ESTk. The mechanism allowing
    for this Toolkit-level specialization of a generic Behavior is the \c params table passed to the
    \c onCreate() trigger. Use this table to provide any kind of user-configurable variable that may
    be different for certain instances, such as a list of waypoints to a waypoint Behavior or a sound
    effect to some sort of spell Behavior. This will not only allow level designers and non-scripters
    to customize behaviors through the GUI, but it also maximizes code reusage and minimizes resource
    requirements on the Engine by reducing the amount of redundant scripts having to be maintained
    within memory. It is a good practice to create a \c defaultParams table as well, which can be
    referenced when the user does not customize a particular input value or does not provide a \c
    params table, so that the LuaBehavior is still well-behaved without external inputs and is ready-to-use
    immediately upon being attached to an Entity.

    \section4 Dynamically Disable Unecessary Triggers
    Not every LuaBehavior requires every trigger it implements at any given point in time. For example,
    if your LuaBehavior only cares about collision checks when in a certain state, be sure to disable
    triggers such as \c onCollide(), \c onCollisionEnter(), and \c onCollisionExit() until you enter that
    state. This will boost performance by reducing overhead from unecessary Lua trigger calls that will
    ultimately be ignored.

    \section4 Share Common Data and Assets
    Since script-global variables are actually static to the environment and are shared among LuaBehavior
    instances of the script, avoid unecessarily duplicating static data for every instance. Instead, use
    the global storage and \c onScriptCreate() and \c onScriptDestroy() triggers to manage the lifetime
    of this data, allowing a single instance of the data to be accessed by each LuaBehavior instance.

    \section4 Use the Least Frequently Invoked Trigger for the Job
    There are numerous scenarios where logic can be implemented by "polling" the Engine or by using
    a more specific trigger. Examples of this are implementing a check to see if the parent Entity
    has died or has collided with an object within the \c onUpdate() function or simply letting
    the Engine do the work and using the \c onCombatDeath() or \c onCollisionEnter() triggers.
    The latter, event-driven method is going to be substantially more efficient, as the polling logic
    is handled by the Engine, in statically compiled C/++ code rather than interpreted Lua code. If
    there are multiple ways to solve a problem, make every attempt possible to use the method that offloads
    the majority of the work to the Engine, and only use the "polling" method of checking for an
    event within the \c onUpdate() trigger sparingly.

    \section4 General Lua Optimizations
    Any general Lua optimization is always a good idea when working with LuaBehaviors, espeically when
    implementing triggers which are called frequently (such as \c onupdate). Refer to the
    \l{Performance Optimizations and Recommended Practices} subsection of the LuaScript documentation for
    more information.

    \sa Behavior, LuaScript, LuaBehaviorScript
*/

/*!
\qmlproperty string LuaBehavior::path

The asset-relative path to the Lua script file.
\sa getBehavName(), setBehavName()
*/
/*!
\qmlproperty Lua_table LuaBehavior::paramTable

Lua table passed to the onCreate() trigger providing the script with instance-specific initialization arguments.
\sa getParamTable(), setParamTable()
*/
/*!
\qmlproperty int LuaBehavior::triggers

Bitmask of Lua triggers currently active for the Behavior instance. If this mask contains triggers that are not
even implemented by the Lua script, behavior is undefined (ie: shit is probably going to crash).
\sa getTriggers(), setTriggers()
*/
/*!
\qmlproperty Lua_table LuaBehavior::selfTable

Read-only property containing a reference to a Lua_table used for storing instanced variables for LuaBehaviors.
This table is created by the engine and cannot be reassigned, but the table itself is fully modifiable by the
Lua script.

By default, the table has two fields, provided by the engine:
\list
\li \b{selfEntity} - reference to the Entity which the LuaBehavior is attached to
\li \b{selfBehav} - referece to the LuaBehavior object containing the Lua script
\endlist

\sa getSelfTable()
*/
/*!
\qmlproperty LuaBehaviorScript* LuaBehavior::script

Read-only property containing a reference to the LuaBehaviorScript object describing the the script
loaded from \l{path}.

\sa getScript()
*/
/*!
 * \qmlmethod string LuaBehavior::getBehavName()
 *
 * Returns the value of the \l{path} property.
 * \sa path, setBehavName()
 */
/*!
 * \qmlmethod void LuaBehavior::setBehavName(string filePath)
 *
 * Sets the value of the \l{path} property to \a{filePath}.
 * \sa path, getBehavName()
 */
/*!
 * \qmlmethod Lua_table LuaBehavior::getParamTable()
 *
 * Returns the value of the \l{paramTable} property.
 * \sa paramTable, setParamTable()
 */
/*!
 * \qmlmethod void LuaBehavior::setParamTable(Lua_table table)
 *
 * Sets the value of the \l{paramTable} property to \a{table}.
 * \sa paramTable, getParamTable()
 */
/*!
 * \qmlmethod int LuaBehavior::getTriggers()
 *
 * Returns the value of the \l{triggers} property.
 * \sa triggers, setTriggers()
 */
/*!
 * \qmlmethod void LuaBehavior::setTriggers(int mask)
 *
 * Sets the value of the \l{triggers} property to \a{mask}.
 * \sa triggers, getTriggers()
 */
/*!
 * \qmlmethod Lua_table LuaBehavior::getSelfTable()
 *
 * Returns the value of the \l{selfTable} property.
 * \sa selfTable
 */
/*!
 * \qmlmethod LuaBehaviorScript* LuaBehavior::getScript()
 *
 * Returns the value of the \l{script} property.
 * \sa script
 */

OOLUA_EXPORT_FUNCTIONS(LuaBehavior,
                        getSelfTable,
                        setTriggers,
                        setBehavName,
                        setParamTable,
                        getParamTable)
OOLUA_EXPORT_FUNCTIONS_CONST(LuaBehavior,
                             getTriggers,
                             getScript)
