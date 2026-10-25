/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef JET_SET_RADIO_GRAFFITI_TOOL_HPP
#define JET_SET_RADIO_GRAFFITI_TOOL_HPP

#include <QString>
#include <QDialog>
#include <QImageWriter>
#include <QBuffer>

#define JET_SET_RADIO_GRAFFITI_TOOL_SRC_PIXMAP_DEFAULT   ":/icons/images/drag_n_drop_icon.png"
#define JET_SET_RADIO_GRAFFITI_TOOL_DST_PIXMAP_DEFAULT   ":/icons/images/jet_set_radio_logo.png"

class   QSettings;
class   QGraphicsScene;
class   QGraphicsPixmapItem;

namespace Ui {
    class JetSetRadioGraffitiTool;
}

class JetSetRadioGraffitiTool: public QDialog {
    Q_OBJECT
private:
    QImageWriter            _writer;
    QPixmap                 _srcPixmapDefault;
    QPixmap                 _dstPixmapDefault;
    Ui::JetSetRadioGraffitiTool*
                            _ui         = nullptr;
    QGraphicsScene*         _srcScene   = nullptr;
    QGraphicsScene*         _dstScene   = nullptr;
    QGraphicsPixmapItem*    _srcItem    = nullptr;
    QGraphicsPixmapItem*    _dstItem    = nullptr;
    QImage                  _dstImage;
    QByteArray*             _byteArray  = nullptr;
    QBuffer*                _buffer     = nullptr;
    size_t                  _vmuSize    = 0;
    size_t                  _vmuBlocks  = 0;

    static const unsigned char
                            _vmsHeader[0x28C];

    static size_t           _baseSize;

    void                    _setupDefaultUI(void);
    void                    _setSrcImageProperties(QString path, const QPixmap& pixmap);
    void                    _updateDstImage(void);
    void                    _updateDstImage(const QPixmap& srcImage, int qualityLevel, QSizeF scale, int compressionLevel=9);
    void                    _updateDstImageFromSrcImage(const QPixmap& srcImage);
    bool                    _writeJpegToBuffer(const QImage& image, QBuffer& buffer, int qualityLevel);


public:
                            JetSetRadioGraffitiTool(QWidget* parent);
    virtual                 ~JetSetRadioGraffitiTool(void) = default;


    bool                    openSourceImage(QString path);
    QSizeF                  getScaleFactor(void) const;
    bool                    exportVmiVmsFromImage(const QImage& image, QString outPath);
private Q_SLOTS:
    void                    on_sourceBrowseButton_clicked(void);
    void                    on_closeButton_clicked(void);
    void                    on_qualitySpinbox_valueChanged(int arg1);
    void                    on_uniformScaleCheckbox_toggled(bool checked);
    void                    on_heightSlider_valueChanged(int value);
    void                    on_widthSlider_valueChanged(int value);
    void                    on_exportButton_clicked(void);
    void                    on_qualitySlider_valueChanged(int value);
    void                    on_destSavePreviewButton_clicked(void);
};





#endif // JET_SET_GRAFFITI_TOOL_HPP
