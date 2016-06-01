#ifndef TK_MIME_DROP_MANAGER_H
#define TK_MIME_DROP_MANAGER_H

#include <QVector>
#include <functional>
#include <initializer_list>

class QMimeData;
class QDropEvent;
class QDragEnterEvent;

class MimeDropManager {
public:
    using DropCbType        = std::function<bool(const QMimeData*, Qt::DropAction, QDropEvent*)>;
    using CanDropCbType     = std::function<bool(const QMimeData*, Qt::DropAction, QDragEnterEvent*)>;

    enum BUILTIN_MIME_TYPES {
        TYPE_TEXT   = -1,
        TYPE_HTML   = -2,
        TYPE_COLOR  = -3,
        TYPE_IMAGE  = -4,
        TYPE_URLS   = -5
        //TYPE_CSV    = -6
    };

    struct Handler {
                        Handler(void) = default;
                        Handler(int type, DropCbType dropCb, CanDropCbType canDropCb=CanDropCbType(), Qt::DropActions actions=Qt::ActionMask());

        int             mimeType;
        DropCbType      dropCallback;
        CanDropCbType   canDropCallback = CanDropCbType();
        Qt::DropActions dropActions     = Qt::ActionMask;
    };

                        MimeDropManager(void) = default;
                        MimeDropManager(QVector<Handler>&& handlers);

    bool                isMimeArraySupported(void) const;
    void                setMimeArraySupported(bool v);
    void                addHandler(int type, DropCbType dropCallback, CanDropCbType canDropCallback, Qt::DropActions actions);
    void                removeHandler(int type);
    bool                canDrop(const QMimeData* data, Qt::DropAction action, QDragEnterEvent* event=nullptr);
    bool                canDrop(QDragEnterEvent* event);
    bool                drop(const QMimeData* data, Qt::DropAction action, QDropEvent* event=nullptr);
    bool                drop(QDropEvent* event);

    static bool         mimeDataHasType(int type, const QMimeData* data);

private:

    template<typename F>
    bool                _iterateHandlersForMimeData(const QMimeData* data, Qt::DropAction action, F&& f);

    QVector<Handler>    _handlers;
    bool                _supportMimeArray = true;

};






//========= INLINEZ =========
inline bool MimeDropManager::isMimeArraySupported(void) const { return _supportMimeArray; }
inline void MimeDropManager::setMimeArraySupported(bool v) { _supportMimeArray = v; }
inline MimeDropManager::Handler::Handler(int type, DropCbType dropCb, CanDropCbType canDropCb, Qt::DropActions actions):
    timeType(type), dropCallback(dropCb), canDropCallback(canDropCb), dropActions(actions)
{}


#endif // TK_MIME_DROP_MANAGER_H

