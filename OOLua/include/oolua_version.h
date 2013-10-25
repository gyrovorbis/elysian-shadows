#ifndef OOLUA_VERSION_H_
#	define OOLUA_VERSION_H_

/** \file oolua_version.h */

#define OOLUA_DO_STRINGISE(a)   #a
#define OOLUA_STRINGISE(a)   OOLUA_DO_STRINGISE(a)

/** \def OOLUA_VERSION_MAJ
 \brief OOLua version string*/
#       define OOLUA_VERSION_MAJ 2
/** \def OOLUA_VERSION_MIN
 \brief OOLua version string*/
#       define OOLUA_VERSION_MIN 0
/** \def OOLUA_VERSION_PATCH
 \brief OOLua version string*/
#       define OOLUA_VERSION_PATCH 0
/** \def OOLUA_VERSION
 \showinitializer
 \brief OOLua version string*/
#       define OOLUA_VERSION \
					OOLUA_STRINGISE(OOLUA_VERSION_MAJ) "." \
					OOLUA_STRINGISE(OOLUA_VERSION_MIN) "." \
					OOLUA_STRINGISE(OOLUA_VERSION_PATCH) " Beta"


namespace OOLUA
{
	/** \var version_str
	  \showinitializer
	 \brief OOLua version string*/
	static const char version_str[] = OOLUA_VERSION;
	/** \var version_number
	 \brief OOLua version int*/
	static const int version_number = OOLUA_VERSION_MAJ*10000+OOLUA_VERSION_MIN*1000+OOLUA_VERSION_PATCH;
}

#undef OOLUA_DO_STRINGISE
#undef OOLUA_STRINGISE

#endif
