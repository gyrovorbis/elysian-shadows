#include <spine/Attachment.h>
#include <gyro_video_api.h>

static void (*_attachmentCbs[SP_ATTACHMENT_COUNT])(struct spAttachment*) = { NULL };

void gyVidSkeletonAttachmentCallback(const int type, void (*callback)(spAttachment *)) {
    _attachmentCbs[type] = callback;
}

void gyVidSkeletonAttachment(spAttachment*const attachment) {
    (*_attachmentCbs[attachment->type])(attachment);
}
