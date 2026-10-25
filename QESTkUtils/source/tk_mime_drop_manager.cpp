/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QESTkUtils/tk_mime_drop_manager.h>
#include <QESTkUtils/tk_mime_data.h>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QFileInfo>

MimeDropManager::Handler MimeDropManager::createLocalFileHandler(const QVector<LocalFileSubHandler>& subHandlers) {
    return Handler({
        MimeDropManager::TYPE_URLS,
        [=](const QMimeData* data, Qt::DropAction action, QDropEvent* event, int row, int col, const QModelIndex& index) {
            bool retVal = true;

            for(auto&& url : data->urls()) {
                if(url.isLocalFile()) {
                    QString path = url.toLocalFile();
                    QFileInfo info(path);

                    for(auto&& fileHandler : subHandlers) {
                        if(fileHandler.extensions.contains(info.suffix(), Qt::CaseInsensitive)) {
                            if(fileHandler.dropActions & action) {
                                retVal &= fileHandler.dropCallback(path, action, event, row, col, index);
                            }
                        }
                    }
                }
            }
            return retVal;
        },
       [=](const QMimeData* data, Qt::DropAction action, QDragEnterEvent* event, int row, int col, const QModelIndex& index) {
           for(auto&& url : data->urls()) {
               if(url.isLocalFile()) {
                   QString path = url.toLocalFile();
                   QFileInfo info(path);

                   for(auto&& fileHandler : subHandlers) {
                       if(fileHandler.extensions.contains(info.suffix(), Qt::CaseInsensitive)) {
                            bool canDrop = true;
                            if(fileHandler.canDropCallback) canDrop = fileHandler.canDropCallback(path, action, event, row, col, index);
                            else return true;
                            if(canDrop) return true;
                       }
                   }
               }
           }
           return false;
       }
    });
}

MimeDropManager::MimeDropManager(QVector<Handler>&& handlers): _handlers(handlers) {}

MimeDropManager::MimeDropManager(QVector<Handler>& handlers): _handlers(handlers) {}

void MimeDropManager::addHandler(int type, DropCbType dropCallback, CanDropCbType canDropCallback, Qt::DropActions actions) {
    _handlers.push_back(Handler(
                            type,
                            std::move(dropCallback),
                            std::move(canDropCallback),
                            actions
                        ));
}

void MimeDropManager::removeHandler(int type) {
    for(auto&& it = _handlers.begin(); it != _handlers.end(); ++it) {
        if(it->mimeType == type) {
            _handlers.erase(it);
            return;
        }
    }
}

bool MimeDropManager::mimeDataHasType(int type, const QMimeData *data) {
    const TkMimeData* tkData = qobject_cast<const TkMimeData*>(data);
    return ((type == TYPE_TEXT && data->hasText()) ||
            (type == TYPE_URLS && data->hasUrls()) ||
            (type == TYPE_HTML && data->hasHtml()) ||
            (type == TYPE_IMAGE && data->hasImage()) ||
            (type == TYPE_COLOR && data->hasColor()) ||
            (tkData && type == tkData->getType()));
}

/* If for some crazy fucking reason, we need to support arrays of arrays of MimeData or
 * multiple layers of selections, this algorithm needs to become recursive or it'll shit itself.
 */
template<typename F>
bool MimeDropManager::_iterateHandlersForMimeData(const QMimeData* data, Qt::DropAction action, F&& f) {
    const auto& handleSingleMimeData = [&](const QMimeData* singleData) {
        for(auto&& handler: _handlers) {
            if((handler.dropActions & action) && mimeDataHasType(handler.mimeType, singleData)) {
                if(f(handler, singleData))
                    return true;
            }
        }
        return false;
    };

    const TkMimeData* tkData = qobject_cast<const TkMimeData*>(data);

    if(_supportMimeArray && tkData && tkData->isArray()) {
        auto& array = tkData->getVariant();
        for(int i = 1; i <= array.getLength(); ++i) {
            if(!handleSingleMimeData(static_cast<const QMimeData*>(array[i]->getValue<void*>()))) {
                return false;
            }
        }
        return true;
    } else {
        return handleSingleMimeData(data);
    }
}

bool MimeDropManager::canDrop(const QMimeData *data, Qt::DropAction action, QDragEnterEvent *event, int row, int col, const QModelIndex& parent) {
    return _iterateHandlersForMimeData(data, action, [=](const Handler& handler, const QMimeData* singleData) {
       return handler.canDropCallback? handler.canDropCallback(singleData, action, event, row, col, parent) : true;
    });
}


bool MimeDropManager::canDrop(const QMimeData *data, Qt::DropAction action, QDragEnterEvent *event) {
    return canDrop(data, action, event, -1, -1, QModelIndex());
}

bool MimeDropManager::canDrop(QDragEnterEvent* event) {
    return canDrop(event->mimeData(), event->dropAction(), event);
}

bool MimeDropManager::drop(const QMimeData *data, Qt::DropAction action, QDropEvent *event, int row, int col, const QModelIndex& parent) {
    return _iterateHandlersForMimeData(data, action, [=](const Handler& handler, const QMimeData* singleData) {
       return handler.dropCallback(singleData, action, event, row, col, parent);
    });
}


bool MimeDropManager::drop(const QMimeData *data, Qt::DropAction action, QDropEvent *event) {
    return drop(data, action, event, -1, -1, QModelIndex());
}

bool MimeDropManager::drop(QDropEvent* event) {
    return drop(event->mimeData(), event->dropAction(), event);
}
