#ifndef ELYSIAN_DEPTH_BUFF_ENTRY_HPP
#define ELYSIAN_DEPTH_BUFF_ENTRY_HPP

#ifdef ELYSIAN_ENGINE
#   include <level/elysian_depth_buff_entry_base.hpp>
namespace elysian {
    typedef MapDepthBuffEntryImplBase       MapDepthBuffEntryImpl;
    typedef EntityDepthBuffEntryImplBase    EntityDepthBuffEntryImpl;
}
#else
#   include <ESToolkit/AssetIO/tk_depth_buff_entry.h>
namespace elysian {
    typedef TkMapDepthBuffEntryImpl       MapDepthBuffEntryImpl;
    typedef TkEntityDepthBuffEntryImpl    EntityDepthBuffEntryImpl;
}
typedef TkMapDepthBuffEntryImpl         MapDepthBuffEntryImpl;
typedef TkEntityDepthBuffEntryImpl      EntityDepthBuffEntryImpl;

#endif



#endif // ELYSIAN_DEPTH_BUFF_ENTRY_HPP

