/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VMU_FILESYSTEM_UTILIZATION_BAR_HPP
#define GYRO_VMU_FILESYSTEM_UTILIZATION_BAR_HPP


#include <QWidget>
#include <evmu-core/gyro_vmu_flash.h>

#define VMU_FILESYSTEM_UTILIZATION_BAR_BLOCK_FREE_COLOR 	Qt::darkGreen
#define VMU_FILESYSTEM_UTILIZATION_BAR_BLOCK_USED_COLOR		Qt::green
#define VMU_FILESYSTEM_UTILIZATION_BAR_BLOCK_LOCKED_COLOR	QColor(180, 60, 230)


#define VMU_FILESYSTEM_UTILIZATION_BAR_HIPSTER_MODE_ENABLED

struct VMUDevice;

class VmuFileSystemUtilizationBar: public QWidget {
public:
    enum  BLOCK_STATUS {
        BLOCK_STATUS_FREE,
        BLOCK_STATUS_USED,
        BLOCK_STATUS_DAMAGED,
        BLOCK_STATUS_LOCKED,
        BLOCK_STATUS_COUNT
    };
private:
    QColor              _blockColors[BLOCK_STATUS_COUNT];
    VMUDevice*          _dev 								= nullptr;
    bool                _showLockedBlocks					= true;
    size_t              _blockCounts[BLOCK_STATUS_COUNT]	= { 0 };
    size_t              _blocksTotal						= 0;
    VMUFlashMemUsage    _internalMemUsage;

    virtual void        _beginQueryBlockCount(void);
    virtual size_t      _queryBlockCount(BLOCK_STATUS status) const;

public:

                    VmuFileSystemUtilizationBar(QWidget* parent=nullptr);
    virtual         ~VmuFileSystemUtilizationBar(void) = default;

    void            setDevice(VMUDevice* dev);
    void 			setBlockColor(BLOCK_STATUS status, QColor color);
    QColor			getBlockColor(BLOCK_STATUS status) const;
    size_t			getBlockCount(BLOCK_STATUS status) const;
    size_t			getBlocksTotal(void) const;
    void			setShowLockedBlocks(bool show=true);

    bool			updateBlockUtilization(void);

    virtual void 	paintEvent(QPaintEvent* event) override;
    virtual void    mousePressEvent(QMouseEvent *event) override;


};




inline void VmuFileSystemUtilizationBar::setDevice(VMUDevice* dev) {
    _dev = dev;
}

inline size_t VmuFileSystemUtilizationBar::getBlocksTotal(void) const {
    return _blocksTotal;
}

inline size_t VmuFileSystemUtilizationBar::getBlockCount(BLOCK_STATUS status) const {
    Q_ASSERT(status < BLOCK_STATUS_COUNT);
    return _blockCounts[status];
}

inline QColor VmuFileSystemUtilizationBar::getBlockColor(BLOCK_STATUS status) const {
    Q_ASSERT(status < BLOCK_STATUS_COUNT);
    return _blockColors[status];
}

inline void	VmuFileSystemUtilizationBar::setBlockColor(BLOCK_STATUS status, QColor color) {
    Q_ASSERT(status < BLOCK_STATUS_COUNT);
    _blockColors[status] = color;
}

inline void VmuFileSystemUtilizationBar::setShowLockedBlocks(bool show) {
    _showLockedBlocks = show;
}





#endif // GYRO_VMU_FILESYSTEM_UTILIZATION_BAR_HPP
