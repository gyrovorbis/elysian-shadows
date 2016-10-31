#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_dialog_proxy.hpp>

OOLUA_EXPORT_FUNCTIONS(DialogScreen,
                       addSourceEntity,
                       setTrigger,
                       setText)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogScreen,
                             getText,
                             getIndex,
                             isTrigger,
                             getEntity)

OOLUA_EXPORT_FUNCTIONS(DialogText,
                       setNextIndex)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogText,
                             getNextIndex)

OOLUA_EXPORT_FUNCTIONS(DialogOption,
                       setNextIndex,
                       setText)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogOption,
                             getNextIndex,
                             getText)

OOLUA_EXPORT_FUNCTIONS(DialogPrompt,
                       addOption,
                       setOption)
OOLUA_EXPORT_FUNCTIONS_CONST(DialogPrompt,
                             getOption,
                             getOptionCount)


OOLUA_EXPORT_FUNCTIONS(Dialog,
                       addTextScreen,
                       addPromptScreen,
                       setTriggerEntity,
                       setStartScreen,
                       setScreen)
OOLUA_EXPORT_FUNCTIONS_CONST(Dialog,
                             getScreen,
                             getScreenCount,
                             getTriggerEntity,
                             getStartScreen)

