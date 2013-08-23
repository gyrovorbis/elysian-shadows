#ifndef ELYSIAN_ASSET_HPP
#define ELYSIAN_ASSET_HPP

#include "elysian_level_manager.hpp"
#include <stdio.h>
#include <stdarg.h>

#define ELYSIAN_ASSETIO_BINARY_READ
#define ELYSIAN_ASSETIO_BINARY_WRITE 

#define ELYSIAN_DEBUG_BUFFER_SIZE		500
#define ELYSIAN_DEBUG_PREFIX_STACK      "\t"
#define ELYSIAN_DEBUG_PREFIX_CRITICAL   "! - "
#define ELYSIAN_DEBUG_PREFIX_FATAL      "!!! - "

#define ELYSIAN_CMD_LINE_ARG_FILE		"gamma_args.txt"

//COMMAND-LINE ARGUMENT LIST!
#define ELYSIAN_CMDLINE_ARG_X			"-x"
#define ELYSIAN_CMDLINE_ARG_Y			"-y"
#define ELYSIAN_CMDLINE_ARG_LEVEL		"-l"
#define ELYSIAN_CMDLINE_ARG_AREA		"-a"

/* 
 DEBUG LOGGING CONVENTION
 * invokee prints its own name, pushes, prints errors/events, pops
 * invoker does NOT print anything about invokee
 * ensure that functions with early-outs pop their frames, or you've reested the stack
 * DON'T use tabs, newlines, or special characters and shit. That's the point of
   implementation-specific formatting.

 EXAMPLE CODE:
bool LoadShit() {
	bool success = true;

	Log(VERBOSE, "LoadShit()"); //Function name/action comes before push
	Push();

	//Only report errors happening within THIS function, not children.
	if(SOMETHING_REESTED) {
		Log(CRITICAL, "Something has reested!");
		Pop();			//EVERY EARLY RETURN MUST POP, otherwise you reest the stack!
		return false;	//reested so badly, we must exit early
	}
	success &= LoadOtherShit(); //Children functions push their own frames and announce their own errors
	success &= LoadOtherShit();

	Pop();
	return success;
}
 
 EXAMPLE STACK:
 + Level::Load
	+ Level::LoadTileset
		+ FAILURE!
	+ Level::LoadObjectset
	+ Level::LoadAreas
		+ Area::Load
			+ Area::LoadMap
				+ FAILURE!
			+ Area::LoadEntities
				+ Entity::Load
					+ Component::Load
					+ Component::Load
				+ Entity::Load
					+ FAILURE!

 */

namespace elysian {

	/*
		Base class for all "Assets" within the AssetIO. The DebugStack is to be overriden and handed off
		to this class, so that all debugging is handled internally. If none is specified, we default
		to C stdio printf().
	*/

    //Assets really SHOULDN'T be LevelManagers...
    class Asset: public LevelManager { //tolua_export
    public:
		enum LOG_LEVEL { LOG_VERBOSE, LOG_CRITICAL, LOG_FATAL };

		class DebugStack { 
		public:
			virtual void push(void) = 0;
			virtual void pop(void) = 0;
			virtual void printf(const LOG_LEVEL type, const char* const str) = 0;
		};

        static void setDebugStack(DebugStack* const ptr) { _dbgPtr = ptr; }

		static void dbgPush(void) { if(_dbgPtr) _dbgPtr->push(); }
		static void dbgPop(void) { if(_dbgPtr) _dbgPtr->pop(); }

        static void dbgprintf(LOG_LEVEL type, const char *string, ...) {
			va_list argptr; 
			static char buffer[ELYSIAN_DEBUG_BUFFER_SIZE];

			va_start(argptr,string); 
			vsprintf(buffer,string,argptr); 
			va_end(argptr); 

			if(_dbgPtr) _dbgPtr->printf(type, buffer);
			else printf("%s", buffer); 
		}

    private:
        static DebugStack* _dbgPtr;

    };//tolua_export

}

#endif
