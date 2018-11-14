#include "gyro_vmu_filesystem_utilization_bar.hpp"
#include <QPainter>
#include <evmu-core/gyro_vmu_flash.h>
#include <QGraphicsDropShadowEffect>
#include <QFontDatabase>


VmuFileSystemUtilizationBar::VmuFileSystemUtilizationBar(QWidget* parent):
    QWidget(parent),
    _dev(nullptr)
{
    setBlockColor(BLOCK_STATUS_FREE, 	VMU_FILESYSTEM_UTILIZATION_BAR_BLOCK_FREE_COLOR);
    setBlockColor(BLOCK_STATUS_USED, 	VMU_FILESYSTEM_UTILIZATION_BAR_BLOCK_USED_COLOR);
    setBlockColor(BLOCK_STATUS_LOCKED,	VMU_FILESYSTEM_UTILIZATION_BAR_BLOCK_LOCKED_COLOR);
    QGraphicsDropShadowEffect* effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(5);
    effect->setOffset(QPointF(2.0f, 2.0f));
#ifdef VMU_FILESYSTEM_UTILIZATION_BAR_HIPSTER_MODE_ENABLED
    setGraphicsEffect(effect);
#endif
}

size_t VmuFileSystemUtilizationBar::_queryBlockCount(BLOCK_STATUS status) const {
    VMUFlashMemUsage usage = gyVmuFlashMemUsage(_dev);

    switch(status) {
    case BLOCK_STATUS_FREE: return usage.blocksFree;
    case BLOCK_STATUS_USED: return usage.blocksUsed;
    default:
    case BLOCK_STATUS_LOCKED: return 0;
    }
}

void VmuFileSystemUtilizationBar::paintEvent(QPaintEvent* event) {
    if(!_dev) return;
    QPainter painter(this);
#ifdef VMU_FILESYSTEM_UTILIZATION_BAR_HIPSTER_MODE_ENABLED
    painter.setRenderHint(QPainter::Antialiasing);
#endif


#ifdef VMU_FILESYSTEM_UTILIZATION_BAR_HIPSTER_MODE_ENABLED
    QSize radius(6, 3);
#else
    QSize radius(0, 0);
#endif

    static int updateCount = 1;
    updateBlockUtilization();

    QPainterPath path;
    QRectF rec = rect();
    //rec.setSize(rec.size() + QSizeF(0.5f, 0.5f));
    //rec.translate(0.5f, 0.5f);
    QSizeF sizeDelta = rec.size() - radius;
    rec.setSize(sizeDelta);
    //rec.translate(0.5f*sizeDelta.width(), 0.5f*sizeDelta.height());
    rec.moveCenter(rect().center() + QPointF(0.5f, 0.5f));
    //rec.moveCenter(QPoint(-radius.width(), -radius.height()));


    auto createSubRect = [&](const QRectF& offset, float ratio) -> QRectF {
        QRectF subRect = rec;
        subRect.setRight(rect().right() - rect().width() * (1.0f - ratio));
        //subRect.setWidth(rect().width() * ratio);
        //subRect.translate(rect().width() - subRect.width(), 0);


#if 0
        subRect.translate((1.0f - ratio) * rect().width(), 0);
        subRect.setRight(subRect.right() - rect().right());
#endif
        return subRect;
    };

    QRectF usedRect;
    if(_blockCounts[BLOCK_STATUS_USED]) {
        float usedRatio = (float)_blockCounts[BLOCK_STATUS_USED] / (float)_blocksTotal;
        usedRect = createSubRect(QRect(0, 0, 0, 0), usedRatio);


        painter.save();
        QRectF usedRect2 = usedRect;
        QPainterPath path2;
        //rec.setSize(rec.size() + QSizeF(0.5f, 0.5f));
        //rec.translate(0.5f, 0.5f);
        QSizeF sizeDelta2 = usedRect2.size() - radius;
        QSizeF delta3 = usedRect2.size() - sizeDelta2;
        //sizeDelta2 -= QSizeF(9.0f, 9.0f);
        //usedRect2.setSize(sizeDelta2);
        //usedRect2.translate(0.5f*delta3.width() - 0.5f, 1.0f);


        QLinearGradient gradient(usedRect2.topLeft(), usedRect2.bottomRight());

        gradient.setColorAt(0, Qt::green);
        gradient.setColorAt(1, QColor(255.0f*usedRatio, 0, 0));

        //usedRect2.translate(0.5f*sizeDelta2.width(), 0.5f*sizeDelta2.height());
        //rec.moveCenter(rect().center() + QPointF(0.5f, 0.5f));
        //rec.moveCenter(QPoint(-radius.width(), -radius.height()));
        path2.addRoundedRect(usedRect2, radius.width(), radius.height());
        painter.setClipRect(rec);
        //QPen pen(Qt::black, 2);
        //painter.setPen(pen);
        //painter.setBrush(gradient);

        QColor color = Qt::white;
#if 0
        if(usedRatio <= 0.5f) color = Qt::white;
        else if(usedRatio <= 0.75f) color = Qt::yellow;
        else color = Qt::red;
#endif

        painter.fillPath(path2, color);
        //painter.drawPath(path);
        //painter.setBrush(QBrush());
        //painter.setClipRect(QRect());
        painter.restore();
       // painter.fillRect(usedRect, getBlockColor(BLOCK_STATUS_USED));
    }

    path.addRoundedRect(rec, radius.width(), radius.height());
    QPen pen(Qt::black, 2);
    painter.setPen(pen);
   // painter.fillPath(path, getBlockColor(BLOCK_STATUS_FREE));
    painter.drawPath(path);
    QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    f.setPointSize(10);
    setFont(f);

    QString blocksString = QString("Blocks Free: ");
    QString blocksRatio = QString("%1/%2").arg(_blockCounts[BLOCK_STATUS_FREE]).arg(getBlocksTotal());
    QString combinedString = blocksString + blocksRatio;

    if(painter.fontMetrics().width(combinedString) <= rec.width()) {
        painter.drawText(rec, Qt::AlignCenter, combinedString);
    } else if(painter.fontMetrics().width(blocksRatio) <= rec.width()) {
        painter.drawText(rec, Qt::AlignCenter, blocksRatio);
    }


    if(_showLockedBlocks && _blockCounts[BLOCK_STATUS_LOCKED]) {
        float lockedRatio = (float)_blockCounts[BLOCK_STATUS_LOCKED] / (float)_blocksTotal;
        QRectF lockedRect = createSubRect(usedRect, lockedRatio);
        painter.fillRect(lockedRect, getBlockColor(BLOCK_STATUS_LOCKED));
    }

}


bool VmuFileSystemUtilizationBar::updateBlockUtilization(void) {
    size_t 	prevTotal 	= _blocksTotal;
    bool 	changed 	= false;

    _blocksTotal = 0;

    for(unsigned b = 0; b < (_showLockedBlocks? BLOCK_STATUS_COUNT : BLOCK_STATUS_LOCKED); ++b) {
        size_t count = _queryBlockCount(static_cast<BLOCK_STATUS>(b));
        if(count != _blockCounts[b]) {
            changed = true;
            _blockCounts[b] = count;
        }
        _blocksTotal += _blockCounts[b];
    }

    if(_blocksTotal != prevTotal) changed = true;

    if(changed) {
        QString text = QString("Used: %1, Free: %2").arg(_blockCounts[BLOCK_STATUS_USED]).arg(_blockCounts[BLOCK_STATUS_FREE]);

        if(_showLockedBlocks) text += QString(", Locked: %1").arg(_blockCounts[BLOCK_STATUS_LOCKED]);
        setToolTip(text);
    }

    return changed;
}
