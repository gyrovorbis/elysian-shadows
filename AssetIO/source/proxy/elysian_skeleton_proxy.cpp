#include <proxy/elysian_skeleton_proxy.hpp>


OOLUA_EXPORT_FUNCTIONS(SkeletonAttrib,
                        loadAssets)
OOLUA_EXPORT_FUNCTIONS_CONST(SkeletonAttrib)



OOLUA_EXPORT_FUNCTIONS_CONST(Skeleton,
                             getAttrib,
                             getAnimTimeScale,
                             setAnimByName)
OOLUA_EXPORT_FUNCTIONS(Skeleton,
                       setAttrib,
                       setAnimTimeScale)
