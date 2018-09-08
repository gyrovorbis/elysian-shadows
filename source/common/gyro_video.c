#ifndef GYRO_NO_SPINE
#   include <spine/Attachment.h>
#endif
#include <libGyro/gyro_video_api.h>

#ifndef GYRO_NO_SPINE
static void (*_attachmentCbs[SP_ATTACHMENT_COUNT])(struct spAttachment*) = { NULL };
#endif

void gyVidSkeletonAttachmentCallback(const int type, void (*callback)(struct spAttachment *)) {
#ifndef GYRO_NO_SPINE
    _attachmentCbs[type] = callback;
#endif
}

void gyVidSkeletonAttachment(struct spAttachment*const attachment) {
#ifndef GYRO_NO_SPINE
    (*_attachmentCbs[attachment->type])(attachment);
#endif
}
