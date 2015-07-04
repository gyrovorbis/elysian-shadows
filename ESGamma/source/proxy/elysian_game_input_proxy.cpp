#include <proxy/elysian_instance_transform_proxy.hpp>
#include <proxy/elysian_game_input_proxy.hpp>


EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(GameInput,
                                    setUp,
                                    setDown)
EXPORT_OOLUA_FUNCTIONS_2_CONST(GameInput,
                                getDown,
                                getTapped)

OOLUA_EXPORT_FUNCTIONS_CONST(MouseInput,
                             getTapped,
                             getDown,
                             getScreenPos,
                             getScrollWheel)

OOLUA_EXPORT_FUNCTIONS(MouseInput)
