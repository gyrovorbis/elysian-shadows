#include <proxy/elysian_entity_proxy.hpp>


/*!
    \qmltype AnimationTrack
    \since 1.6.0
    \inmodule ESToolkit
    \brief A Skeletal animation created within Spine to be played on a single track.

    AnimationTrack represents a single animation created within Spine. It represents
    a Spine animation to be played on a single track within a multitrack Animation.

    \e {NOTE: The \l{name} property should exactly match the name of the animation
    within Spine!}

    \sa Animation, SkeletonAttrib, Skeleton
*/
/*!
\qmlproperty string AnimationTrack::name

String property containing the name of the animation within Spine. If the name
does not match an animation with in the Spine JSON file, behavior is undefined.
\sa getName(), setName()
*/
/*!
\qmlproperty float AnimationTrack::timeScale

Property holds the time scale multiplier to be applied to the timeline for this
track. The default value is 1.0, which doesn't change the animation speed. A
value of 0.5 will half the animation speed, and a value of 2.0 will double the
animation speed.
\sa getTimeScale(), setTimeScale()
*/
/*!
\qmlproperty bool AnimationTrack::looping

Property is set to true if the animation should continue looping indefinitely or
false if it should only be played once. By default, it is set to true.
\sa getLooping(), setLooping()
*/
/*!
 * \qmlmethod string AnimationTrack::getName()
 *
 * Returns the value of the \l{name} property.
 * \sa name, setName()
 */
/*!
 * \qmlmethod void AnimationTrack::setName(string n)
 *
 * Sets the value of the \l{name} property to \a{n}.
 * \sa name, getName()
 */
/*!
 * \qmlmethod float AnimationTrack::getTimeScale()
 *
 * Returns the value of the \l{timeScale} property.
 * \sa timeScale, setTimeScale()
 */
/*!
 * \qmlmethod void AnimationTrack::setTimeScale(float factor)
 *
 * Sets the value of the \l{timeScale} property to \a{factor}.
 * \sa timeScale, getTimeScale()
 */
/*!
 * \qmlmethod bool AnimationTrack::getLooping()
 *
 * Returns the value of the \l{looping} property.
 * \sa looping, setLooping()
 */
/*!
 * \qmlmethod void AnimationTrack::setLooping(bool val)
 *
 * Sets the value of the \l{looping} property to \a{val}.
 * \sa looping, getLooping()
 */

OOLUA_EXPORT_FUNCTIONS(AnimationTrack,
                       setName,
                       setLooping,
                       setTimeScale)
OOLUA_EXPORT_FUNCTIONS_CONST(AnimationTrack,
                             getName,
                             getLooping,
                             getTimeScale)


/*!
    \qmltype Animation
    \since 1.6.0
    \inmodule ESToolkit
    \brief Multitrack animation formed as an aggregate of \l{AnimationTrack}s.

    Animation contains a group of \l{AnimationTrack}s, each of which corresponds to an
    animation created within Spine. All \l{AnimationTrack}s are played simultaneously on
    their own tracks, creating a single multitrack animation as an aggregate of animations
    created in Spine. Rather than managing a bunch of disjoint Spine animations, the engine
    and scripts generally only need to concern themselves with a group.

    Being able to play multiple independent skeletal animations simulataneously offers
    much more flexibility over a single track. For example, one track may represent a
    character's arms and one track my represent his legs. Now allowing the character to
    hold an item only requires a new arm animation, so the legs can be reused to
    immediately allow the character to hold an item while they walk, run, dash, and jump.
    With a single track, every combination of actions would have to be a separate,
    hand-created action.

    With multitrack animations, the engine is also able to smoothly transition between
    Animations using the same AnimationTracks on at least one track. For example, if
    the player is walking while he picks up an item, his legs will not reset when his
    arms pick up the object, because even though the arms are playing a different
    animation on one of the tracks, the legs stayed the same on the other track.

    \sa AnimationTrack, SkeletonAttrib, Skeleton
*/
/*!
\qmlproperty string Animation::name

String name of the aggregate animation. This name is how the Animation is referred to.
Certain names are built into the engine and are managed by default by the \l{Character}
\l{Component}. For the Lua script containing these built-in name definitions, see
\l{file:/assets/scripts/component/character.lua}
\sa getName(), setName()
*/
/*!
\qmlproperty array Animation::tracks

Array of type \l{AnimationTrack}. The index of each element in the array correponds
to the track number on which the \l{AnimationTrack} is played.
\sa getTrack(), setTrack(), getTrackCount()
*/
/*!
 * \qmlmethod string Animation::getName()
 *
 * Returns the value of the \l{name} property.
 * \sa name, setName()
 */
/*!
 * \qmlmethod void Animation::setName(string n)
 *
 * Sets the value of the \l{name} property to \a{n}.
 * \sa name, getName()
 */
/*!
 * \qmlmethod AnimationTrack* Animation::getTrack(int index)
 *
 * Returns a reference to the \l{AnimationTrack} located at the given \a{index}
 * within the \l{tracks} array or nil if there isn't one.
 * \sa tracks, setTrack(), getTrackCount()
 */
/*!
 * \qmlmethod void Animation::setTrack(int index, AnimationTrack* track)
 *
 * Sets the value at the location given by \a{index} within the \l{tracks} array to
 * \a{track}. If \a{index} is equal to the size of \l{tracks}, \a{track} is inserted
 * as the last element in the array. Ownership of \a{track} is transferred from Lua
 * to the engine.
 * \sa tracks, getTrack(), getTrackCount()
 */
/*!
 * \qmlmethod int Animation::getTrackCount()
 *
 * Returns the size of the \l{tracks} array.
 * \sa tracks, getTrack(), setTrack()
 */

OOLUA_EXPORT_FUNCTIONS(Animation,
                       setName,
                       setTrack)
OOLUA_EXPORT_FUNCTIONS_CONST(Animation,
                             getName,
                             getTrack,
                             getTrackCount)


/*!
    \qmltype SkeletonAttrib
    \since 1.6.0
    \inmodule ESToolkit
    \brief Attributes defining atlas and json files and animations for the \l{Skeleton}s.

    SkeletonAttrib is a collection of data such as paths to Spine texture atlas and JSON files,
    and aggregate animations required by the \l{Skeleton} \l{Component}. SkeletonAttrib loads
    a Spine skeleton and animation through its \l{jsonPath} property and the corresponding
    sprite sheet and texture atlas file through its \l{atlasPath} property. All non-instance
    skeleton and animation data are stored here, rather than in \l{Skeleton} to reduce
    overhead and encourage the reuse of shared data.

    Because parsing through the spine JSON file and creating the skeleton and animation data
    is such a time consuming and expensive process that uses a shitlaod of RAM, it is
    \b{fucking imperative} that multiple instances of the same Spine skeleton always share
    the same SkeletonAttrib object.

    \sa Animation, Skeleton, AnimationTrack
*/
/*!
\qmlproperty string SkeletonAttrib::atlasPath
The asset-relative path to the Spine *.atlas file. This file tells the Spine runtime how to
unpack each animation frame and image from a large, combined sprite sheet texture. The texture
itself must also be in the same location with the same name, only using the *.png extension.

\sa getAtlasPath(), setAtlasPath()
*/
/*!
\qmlproperty string SkeletonAttrib::jsonPath
The asset-relative path to the Spine *.json file. This file contains all of the information
for the skeleton, including its bones, sockets, poses, skins, and animations.

\sa getJsonPath(), setJsonPath()
*/
/*!
\qmlproperty array SkeletonAttrib::animations
Array of type \l{Animation}, containing a list of every high-level aggregate animation
to be used by the \l{Skeleton}.

\sa getAnimation(), setAnimation(), getAnimationCount()
*/
/*!
 * \qmlmethod string SkeletonAttrib::getAtlasPath()
 *
 * Returns the value of the \l{atlasPath} property.
 * \sa atlasPath, setAtlasPath()
 */
/*!
 * \qmlmethod void SkeletonAttrib::setAtlasPath(string path)
 *
 * Sets the value of the \l{atlasPath} property to \a{path}.
 * \sa atlasPath, getAtlasPath()
 */
/*!
 * \qmlmethod string SkeletonAttrib::getJsonPath()
 *
 * Returns the value of the \l{jsonPath} property.
 * \sa jsonPath, setJsonPath()
 */
/*!
 * \qmlmethod void SkeletonAttrib::setJsonPath(string path)
 *
 * Sets the value of the \l{jsonPath} property to \a{path}.
 * \sa jsonPath, getJsonPath()
 */
/*!
 * \qmlmethod Animation* SkeletonAttrib::getAnimation(int index)
 *
 * Returns a reference to the \l{Animation} object within the \l{animations}
 * array at the given \a{index} or nil if there isn't one.
 * \sa animations, setAnimation(), getAnimationCount()
 */
/*!
 * \qmlmethod void SkeletonAttrib::setAnimation(int index, Animation* animation)
 *
 * Sets the value of the \l{Animation} at the given \a{index} within the \l{animations}
 * array to \a{animation}. If \a{index} is equal to the size of the \l{animations} array,
 * \a{animation} is inserted into the array at the end.
 * \sa animations, getAnimation(), getAnimationCount()
 */
/*!
 * \qmlmethod int SkeletonAttrib::getAnimationCount()
 *
 * Returns the size of the \l{animations} array.
 * \sa animations, getAnimation(), setAnimation()
 */
/*!
 * \qmlmethod void SkeletonAttrib::loadAssets(string atlas, string json)
 *
 * Sets the value of the \l{atlasPath} property to \a{atlas} and sets the value of the
 * \l{jsonPath} property to \a{json}, loading both.
 * \sa atlasPath, jsonPath, setAtlasPath(), setJsonPath()
 */

OOLUA_EXPORT_FUNCTIONS(SkeletonAttrib,
                        loadAssets,
                        setJsonPath,
                        setAtlasPath,
                        setAnimation)
OOLUA_EXPORT_FUNCTIONS_CONST(SkeletonAttrib,
                             getJsonPath,
                             getAtlasPath,
                             getAnimation,
                             getAnimationCount)

/*!
    \qmltype Skeleton
    \inherits Component
    \since 1.6.0
    \inmodule ESToolkit
    \brief \l{Component} attached to an \l{Entity} to provide support for Spine skeletal animations.

    The Skeleton \l{Component} allows an \l{Entity} to import skeletons and animations created by
    \l{http://www.esotericsoftware.com} {Esoteric Software's Spine editor}. The Skeleton \l{Component}
    can apply different skins to the skeleton, add and remove \l{Entity} attachments from slots,
    and apply/manage multiple animations simultaneously by using multitrack animations.

    The Skeleton \l{Component} requires the \l{attrib} property be set to a valid \l{SkeletonAttrib}
    object when attached to an active Entity to provide it with skeleton and animation data. If no
    \l{SkeletonAttrib} has been set, behavior is undefined, and the engine will most likely crash and
    shit itself.

    \section2 Skeletal Animation Overview
    Skeletal animations are an extremely powerful form of 2D animation which offers far more flexibility
    in terms of geometric complexity and animation capabilities over traditional sprite-based animation.
    Rather than having to draw every frame of animation as a separate sprite, animations are represented
    as transforms applied to bones of a Skeleton. This results in far less work for an artist, far more
    animation flexibility, and far less video RAM usage for textures in exchange for a larger RAM footprint
    for animation transform data, more processing time to apply animations via skeletal transformations,
    and a higher polygon count to render each bone as a separate polygon.

    For complex objects such as players, NPCs, and enemies, which can have a myriad of different animations
    and can carry objects, the benefits of using a \l{Skeleton} far outweigh the downsides. For very simple
    static or single animation objects such as barrels which only have a single frame or chests which can
    either be open or closed, the overhead of a \l{Skeleton} is not justifiable, and the object should be
    treated as a traditional sprite using the \l{Renderable} \l{Component} instead.

    \section2 Skins
    A skin is a texture applied to the bones of a Skeleton to give it a particular appearance. By simply
    changing skins on Entities with similar bone configurations, a single skeleton can be used to
    represent a wide array of different entities and objects. For example, a single skeleton can be used
    to model all human characters, then each specific character can be made by applying a certain skin
    to the skeleton to change its appearance. This way no work has to be duplicated for creating additional
    human skeletons, and every skeleton automatically shares the same animation data. Skins are created
    in the Spine editor and can can be managed through the \l{skin} property and the getSkin() and setSkin()
    methods.

    \section2 Attachments and Slots
    A slot or socket is a point on a bone where an object can be attached and transformed as part of
    the skeleton. This is useful for handling accessories and weapons that are visible on a character.
    For example, a human skeleton can have a slot on its back and a slot on its right hand for a
    sword. Then the sword can be attached to the back slot to sheathe it, and it can be attached to
    the right hand slot to use it during battle.

    Slots are created through the Spine editor. Within the engine, an \l{Entity} can be attached or
    removed from a slot through attachEntityToSlot() or removeEntityFromSlot(). It can also be
    retrieved with getEntityAttachedToSlot(). An \l{Entity} attachment can be any arbitrary kind of
    Entity and does not have to be a skeleton itself. For example, a character can be a full skeleton
    with skeletal aimations, while a sword can be a simple sprite Entity with a \l{Renderable}
    \l{Component}.

    \section2 Multitrack Animation
    The most powerful aspect of skeletal animations is the ability to play multiple animations
    simulataneously on different tracks. For example, animations can be split into body parts
    such as arms, torso, and legs, where each body part animates separately on a different
    track. Then complex animations can be created by simply combining different animations
    on different tracks. For example, walking and running may be a single set of animations.
    Then adding the ability to pick up and carry items just requires a single new animation
    to put the arms in a holding position. Then the existing torso and leg tracks can be
    played while walking and running, while the arm track can play the new holding animation
    to animate walking while holding an object and running while holding an object.

    \section3 Creating Multitrack Animations
    Multitrack animations are first created as regular animations within the Spine Editor. These
    regular animations are then grouped together within the engine to create a single multitrack
    animation object, containing multiple Spine animations.

    For example, lets say that we have created a bunch of animations within the Spine Editor with
    the following names:
    \list
    \li "spine_legs_walk"
    \li "spine_legs_run"
    \li "spine_arms_normal"
    \li "spine_arms_hold"
    \endlist

    Now within the engine, we can use these animations to create a series of animation groups representing
    more complex animations.
    \code
    multiTrackAnimations = {
        [1] = Animation:new {
            name = "Walk",
            tracks = {
                [1] = AnimationTrack:new {
                    name = "spine_legs_walk"
                },
                [2] = AnimationTrack:new {
                    name = "spine_arms_normal"
                }
            }
        },
        [2] = Animation:new {
            name = "Run",
            tracks = {
                [1] = AnimationTrack:new {
                    name = "spine_legs_run"
                },
                [2] = AnimationTrack:new {
                    name = "spine_arms_normal"
                }
            }
        },
        [3] = Animation:new {
            name = "HoldWalk",
            tracks = {
                [1] = AnimationTrack:new {
                    name = "spine_legs_walk"
                },
                [2] = AnimationTrack:new {
                    name = "spine_arms_hold"
                }
            }
        },
        [4] = Animation:new {
            name = "HoldRun",
            tracks = {
                [1] = AnimationTrack:new {
                    name = "spine_legs_run"
                },
                [2] = AnimationTrack:new {
                    name = "spine_arms_hold"
                }
            }
        }
    }
    \endcode
    With the preceding code, we have now taken a group of regular animations created within Spine and have
    grouped them together to form 4 complex, multitrack animations within the engine:
    \list
    \li \b{Walk} - animation for walking
    \li \b{Run} - animation for running
    \li \b{HoldWalk} - animation for walking while holding an object
    \li \b{HoldRun} - animation for running while holding an object
    \endlist

    Within the code, you will see that multitrack animations for a Skeleton are represented as an
    array of \l{Animation} objects each containing an array of \l{AnimationTrack} objects. An
    \l{Animation} object represents a group of animations. An \l{AnimationTrack} object represents
    an animation imported from Spine to be played on a single track within its parent \l{Animation}
    object. Animations are then referenced by the engine and Lua through Animation::name as a single
    unit rather than separately by their Spine names.

    \e {NOTE: AnimationTrack::name must exactly match the name given to the animation within the Spine
    editor.}

    \section3 Creating Multitrack Animations from Selection View
    For creating multitrack animations through the Selection View widget within the ESTk GUI rather than
    programmatically, please see \l{Managing Multitrack Animations with Selection View}.

    \sa SkeletonAttrib, Animation, AnimationTrack, Renderable
*/
/*!
\qmlproperty SkeletonAttrib* Skeleton::attrib

Reference to a \l{SkeletonAttrib} object used to provide the Skeleton with additional data or nil
if no object has been set.
\sa getAttrib(), setAttrib()
*/
/*!
\qmlproperty string Skeleton::skin

String name of the current skin in use by the Skeleton, as it appears in Spine. Skins allow a
skeleton to swap between different textures, changing its appearance.
\sa getSkin(), setSkin(), Skins
*/
/*!
\qmlproperty string Skeleton::curAnim

String name of the aggregate \l{Animation} currently being played by the Skeleton.
\sa getAnimName(), playAnimNameBlocking(), setAnimName(), Animation::getName()
*/
/*!
 * \qmlmethod SkeletonAttrib* Skeleton::getAttrib()
 *
 * Returns the value of the \l{attrib} property.
 * \sa attrib, setAttrib()
 */
/*!
 * \qmlmethod void Skeleton::setAttrib(SkeletonAttrib* attr)
 *
 * Sets the value of the \l{attrib} property to \a{attr}.
 * \sa attrib, getAttrib()
 */
/*!
 * \qmlmethod string Skeleton::getSkin()
 *
 * Returns the value of the \l{skin} property.
 * \sa skin, setSkin(), Skins
 */
/*!
 * \qmlmethod void Skeleton::setSkin(string name)
 *
 * Sets the value of the \l{skin} property to \a{name}.
 * \sa skin, getSkin(), Skins
 */
/*!
 * \qmlmethod string Skeleton::getAnimName()
 *
 * Returns the value of the \l{curAnim} property.
 * \sa curAnim, playAnimNameBlocking()
 */
/*!
 * \qmlmethod void Skeleton::playAnimNameBlocking(string name)
 *
 * Plays the \l{Animation} with the given \a{name} as a blocking animation,
 * meaning other animtions cannot play until this animation is complete.
 * \sa curAnim, getAnimName(), setAnimName()
 */
/*!
 * \qmlmethod float Skeleton::getAnimTimeScale()
 *
 * Retuns a the time scale factor used to speed up and slow down animations.
 * \sa setAnimTimeScale()
 */
/*!
 * \qmlmethod void Skeleton::setAnimTimeScale(float factor)
 *
 * Sets the time scale factor used to speed up and slow down animations to \a{factor}.
 * The default value is 1.0, which doesn't modify animation time. A \a{factor} of
 * 0.5 slows the animation to half speed, and a \a{factor} of 2.0 speeds it up to twice
 * as fast.
 * \sa getAnimTimeScale()
 */
/*!
 * \qmlmethod Entity* Skeleton::getEntityAttachedToSlot(string slotName)
 *
 * Returns a reference to the \l{Entity} attached to the slot named \a{slotName}
 * or nil if no \l{Entity} is attached, or no slot named \a{slotName} could be
 * found. The \a{slotName} corresponds to the same name given within the Spine
 * editor.
 * \sa attachEntityToSlot(), removeEntityFromSlot(), {Attachments and Slots}
 */
/*!
 * \qmlmethod void Skeleton::attachEntityToSlot(string slotName, Entity* entity)
 *
 * Attaches the \l{Entity} provided by \a{entity} to the Spine slot named \a{slotName}.
 * \a{slotName} must exactly match the name given to the slot within the Spine
 * editor. Ownership of \a{entity} is transferred from Lua to the engine.
 * \sa getEntityAttachedToSlot(), removeEntityFromSlot(), {Attachments and Slots}
 */
/*!
 * \qmlmethod void Skeleton::removeEntityFromSlot(string slotName)
 *
 *  Removes the \l{Entity} currently attached to the Spine slot named
 * \a{slotName}, if there is one. \a{slotName} must exactly match the name
 * given to the slot within the Spine editor.
 * \sa getEntityAttachedToSlot(), attachEntityToSlot(), {Attachments and Slots}
 */
/*!
 * \qmlmethod void Skeleton::setAnimName(string name)
 *
 * Sets the multitrack animation from named \a{name} from the
 * SkeletonAttrib::animations table as the current animation. \e{NOTE: This
 * animation will not be blocking, meaning another animation can immediately
 * interrupt it the next frame. What you probably want instead is
 * playAnimNameBlocking().}
 * \sa getAnimName(), playAnimNameBlocking()
 */
/*!
 * \qmlmethod void Skeleton::setAnimTrackByName(int trackNumber, string spineAnimName, bool looping)
 *
 * Directly sets the Spine animation named \a{spineAnimName} to play on the track given by
 * \a{trackNumber}. If \a{looping} is set to true, the animation is played in a loop, otherwise
 * it is only played once. The animation is not played in a blocking manner and the track can
 * immediately be overridden the next frame by another animation playing on that track.
 * \e{NOTE: 99% of the time you do not want to manually manage animation tracks like this, and you
 * instead want to play multitrack \l{Animation}s which manage their own animation tracks.}
 */




OOLUA_EXPORT_FUNCTIONS_CONST(Skeleton,
                             getAttrib,
                             getAnimTimeScale,
                             getEntityAttachedToSlot,
                             getSkin,
                             getAnimName)
OOLUA_EXPORT_FUNCTIONS(Skeleton,
                       setAttrib,
                       setAnimName,
                       playAnimNameBlocking,
                       setAnimTimeScale,
                       attachEntityToSlot,
                       removeEntityFromSlot,
                       setAnimTrackByName,
                       setSkin)
