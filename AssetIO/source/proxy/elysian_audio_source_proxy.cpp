#include <proxy/elysian_entity_proxy.hpp>

EXPORT_OOLUA_FUNCTIONS_0_CONST(GYWav)
EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(GYWav)




OOLUA_EXPORT_FUNCTIONS(AudioSource,
                                    setWav,
                                    playWav,
                                    play,
                                    stop,
                                    setGain,
                                    setPitch,
                                    setLooping)
OOLUA_EXPORT_FUNCTIONS_CONST(AudioSource,
                            getLooping,
                            getPitch,
                            getGain)
