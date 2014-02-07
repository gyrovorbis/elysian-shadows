if(NOT GY_CONTRIB_DOWNLOAD_DIR)
	set(GY_CONTRIB_DOWNLOAD_DIR ${GY_CONTRIB_DIR}/downloads)
endif()

include(ExternalGLEW)
include(ExternalGLFW)
include(ExternalOpenAL)
include(ExternalPNG)
include(ExternalOggVorbis)
