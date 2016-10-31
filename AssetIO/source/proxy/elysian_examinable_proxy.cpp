#include <proxy/elysian_entity_proxy.hpp>


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
