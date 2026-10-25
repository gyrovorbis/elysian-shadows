/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ElysianVMU/jet_set_radio_graffiti_tool.hpp>
#include <ElysianVMU/gyro_vmu_qt_util.hpp>
#include <QImage>
#include <QImageWriter>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QFileInfo>
#include <QBuffer>
#include "ui_jet_set_radio_graffiti_tool.h"
#include <evmu-core/gyro_vmu_flash.h>
#include <evmu-core/gyro_vmu_vms.h>
#include <evmu-core/gyro_vmu_vmi.h>
#include <QDesktopWidget>

using namespace QESTkUtils;

size_t JetSetRadioGraffitiTool::_baseSize = 0;
const unsigned char JetSetRadioGraffitiTool::_vmsHeader[0x28C] = { 68,75,32,80,105,99,116,117,114,101,32,32,32,32,32,32,68,114,101,97,109,75,101,121,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,224,34,233,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,190,251,206,252,222,253,238,254,255,255,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,38,220,203,167,102,84,67,34,34,34,34,34,34,34,34,34,39,235,205,223,237,203,169,118,85,67,34,34,34,34,34,34,57,230,85,103,154,188,222,253,204,185,118,101,84,50,34,34,57,214,34,34,52,69,102,121,187,205,238,220,186,147,34,34,74,214,34,34,34,34,34,35,69,86,119,171,205,197,34,34,74,213,34,34,34,34,34,34,34,34,34,52,90,214,34,34,75,197,34,34,34,34,34,34,34,50,34,34,75,197,34,34,91,197,34,34,34,34,34,34,38,116,34,34,75,197,34,34,92,181,34,34,34,34,34,34,107,198,34,34,91,197,34,34,92,180,34,34,34,34,34,37,173,234,66,34,92,181,34,34,92,180,34,34,34,34,34,90,223,252,114,34,92,180,34,34,93,164,34,34,34,34,35,156,222,237,164,34,93,164,34,34,109,163,34,34,34,34,34,71,154,171,182,34,109,163,34,34,109,147,36,85,68,67,34,34,51,68,67,34,109,147,34,34,110,146,39,204,187,169,98,34,34,34,34,34,110,146,34,34,110,114,55,223,254,236,147,34,34,34,34,34,110,114,34,34,127,114,55,223,255,253,147,34,34,34,34,34,127,114,34,34,126,98,55,223,255,253,147,34,34,34,34,34,126,98,34,34,158,98,55,206,239,253,147,34,34,34,34,35,158,98,34,35,157,98,37,154,187,204,115,34,34,35,100,35,173,98,34,35,173,98,34,52,68,85,66,34,34,89,219,84,173,98,34,36,173,82,34,34,34,34,34,34,71,190,238,181,188,82,34,36,188,84,67,50,34,34,35,71,173,238,239,233,188,82,34,36,189,203,186,169,119,119,121,190,238,239,254,236,188,82,34,34,122,188,222,238,255,255,254,238,255,255,255,238,203,82,34,34,36,85,103,154,188,221,238,238,255,255,255,254,219,66,34,34,34,34,34,52,69,102,122,188,222,255,238,238,218,66,34,34,34,34,34,34,34,34,35,69,102,121,188,205,217,50,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,0,0,194,68,25,37,239,163,83,73,90,69};

JetSetRadioGraffitiTool::JetSetRadioGraffitiTool(QWidget* parent):
    QDialog(parent, Qt::Window | Qt::WindowTitleHint | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint),
    _srcPixmapDefault(JET_SET_RADIO_GRAFFITI_TOOL_SRC_PIXMAP_DEFAULT),
    _dstPixmapDefault(JET_SET_RADIO_GRAFFITI_TOOL_DST_PIXMAP_DEFAULT),
    _ui(new Ui::JetSetRadioGraffitiTool),
    _srcScene(new QGraphicsScene),
    _dstScene(new QGraphicsScene),
    _srcItem(new QGraphicsPixmapItem(_srcPixmapDefault)),
    _dstItem(new QGraphicsPixmapItem(_dstPixmapDefault))
{
    _ui->setupUi(this);

    _ui->sourceGraphicsView->setScene(_srcScene);
    _ui->destGraphicsView->setScene(_dstScene);

    _srcScene->addItem(_srcItem);
    _dstScene->addItem(_dstItem);

    Q_ASSERT(!_srcPixmapDefault.isNull());
    Q_ASSERT(!_dstPixmapDefault.isNull());

    _baseSize = gyVmuVmsFileInfoHeaderSize((const VMSFileInfo*)_vmsHeader) + 0x30;
    _setupDefaultUI();

    move(qApp->desktop()->availableGeometry(this).center()-geometry().center());
}


void JetSetRadioGraffitiTool::_setupDefaultUI(void) {
    _srcItem->setPixmap(_srcPixmapDefault);
    _dstItem->setPixmap(_dstPixmapDefault);
    _ui->sourceNameLabel->setText("Drag n Drop Image");
    _ui->exportButton->setEnabled(false);
    _ui->destSavePreviewButton->setEnabled(false);
    _ui->qualitySlider->setEnabled(false);
    _ui->widthSlider->setEnabled(false);
    _ui->heightSlider->setEnabled(false);

}

void JetSetRadioGraffitiTool::on_sourceBrowseButton_clicked(void)
{
    QString path = QFileDialog::getOpenFileName(nullptr, "Specify Source Image",
                                             QString(),
                                             "Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)");

    if(!path.isEmpty()) {
        Debug::log(Debug::LOG_VERBOSE, "[JetSetRadioGraffitiTool]: Loading Source Image: [%s]", Q_CSTR(path));
        openSourceImage(path);
    }
}

bool JetSetRadioGraffitiTool::openSourceImage(QString path) {
    bool success = true;
    QPixmap pixmap(path);

    if(pixmap.isNull()) {
        Debug::log(Debug::LOG_ERROR, "[JetSetRadioGraffitiTool]: Could not open Source Image: [%s]", Q_CSTR(path));
        success = false;
    } else {
        _srcItem->setPixmap(pixmap);
        _updateDstImageFromSrcImage(pixmap);
    }

    _setSrcImageProperties(path, pixmap);
    return success;
}

void JetSetRadioGraffitiTool::_setSrcImageProperties(QString path, const QPixmap& pixmap) {
    if(!path.isEmpty() && !pixmap.isNull()) {
        _ui->sourceDimLabel->setText(QString("<%1, %2>").arg(pixmap.width()).arg(pixmap.height()));
        _ui->sourceBytesLabel->setText(QString("%1").arg(QFileInfo(path).size()));
        _ui->sourceFormatLabel->setText(path.split(".").last());
        _ui->sourceNameLabel->setText(path.split("/").last());
    } else {
        _ui->sourceDimLabel->setText("<0, 0>");
        _ui->sourceBytesLabel->setText("0");
        _ui->sourceFormatLabel->setText("?");
        _ui->sourceNameLabel->setText("Drag n Drop Image");
    }
}

void JetSetRadioGraffitiTool::_updateDstImage(void) {
    _updateDstImageFromSrcImage(_srcItem->pixmap());
}

QSizeF JetSetRadioGraffitiTool::getScaleFactor(void) const {
    return QSizeF( (float)_ui->widthSlider->value()/(float)_ui->widthSlider->maximum(),
                   (float)_ui->heightSlider->value()/(float)_ui->heightSlider->maximum());
}

void JetSetRadioGraffitiTool::_updateDstImageFromSrcImage(const QPixmap& srcImage) {
    _updateDstImage(srcImage,
                    _ui->qualitySpinbox->value(),
                    getScaleFactor());
}

bool JetSetRadioGraffitiTool::_writeJpegToBuffer(const QImage& image, QBuffer& buffer, int qualityLevel) {
    QImageWriter writer;

    writer.setDevice(&buffer);
    writer.setFormat("JPEG");
    writer.setQuality(qualityLevel);
    //writer.setCompression(compressionLevel);
    writer.setProgressiveScanWrite(false);
    writer.setOptimizedWrite(true);

    return writer.write(image);

}

//Dyanmically regenerate the image with the given parameters
void JetSetRadioGraffitiTool::_updateDstImage(const QPixmap& srcPixmap, int qualityLevel, QSizeF size, int compressionLevel) {
    if(srcPixmap.isNull()) {

        _ui->destBytesLabel->setText("0");
        _ui->destDimLabel->setText("<0, 0>");
        _dstItem->setPixmap(QPixmap());
        _dstImage = QImage();
        _ui->exportButton->setEnabled(false);
        _ui->destSavePreviewButton->setEnabled(false);
        _ui->qualitySlider->setEnabled(false);
        _ui->widthSlider->setEnabled(false);
        _ui->heightSlider->setEnabled(false);

        //reset all the shit
    } else {
        delete _buffer;
        delete _byteArray;
        _byteArray = new QByteArray();
        _buffer = new QBuffer(_byteArray);

        bool success = _writeJpegToBuffer(srcPixmap.scaled(size.width()*srcPixmap.width(), size.height()*srcPixmap.height()).toImage(),
                           *_buffer,
                           qualityLevel);
        if(!success) {
            Debug::log(Debug::LOG_ERROR, "[JetSetRadioGraffitiTool]: Failed to write JPEG to internal buffer!");
        } else {

            _dstImage = QImage::fromData(*_byteArray);
            if(_dstImage.isNull()) {
                Debug::log(Debug::LOG_ERROR, "[JetSetRadioGraffitiTool]: Failed to generate JPG preview from source image!");
            } else {

                _dstItem->setPixmap(QPixmap::fromImage(_dstImage));

                _ui->destBytesLabel->setText(QString("%1").arg(_byteArray->size()));
                _ui->destDimLabel->setText(QString("<%1, %2>").arg(_dstImage.width()).arg(_dstImage.height()));
                _vmuSize = _baseSize + _byteArray->size();
                _vmuBlocks = _vmuSize / VMU_FLASH_BLOCK_SIZE;
                if(_vmuSize % VMU_FLASH_BLOCK_SIZE) ++_vmuBlocks;
                _ui->blocksLabel->setText(QString("%1").arg((_vmuBlocks)));
                _ui->exportButton->setEnabled(true);
                _ui->destSavePreviewButton->setEnabled(true);
                _ui->qualitySlider->setEnabled(true);
                _ui->widthSlider->setEnabled(true);
                _ui->heightSlider->setEnabled(true);
            }
        _ui->destGraphicsView->setSceneRect(_dstItem->sceneBoundingRect());
        }
    }
}

void JetSetRadioGraffitiTool::on_closeButton_clicked()
{
    close();
}

void JetSetRadioGraffitiTool::on_uniformScaleCheckbox_toggled(bool checked)
{
    if(checked) {
        _ui->heightSlider->setValue(_ui->widthSlider->value());
    }
}

void JetSetRadioGraffitiTool::on_heightSlider_valueChanged(int value)
{
    if(_ui->uniformScaleCheckbox->isChecked())
        _ui->widthSlider->setValue(value);
    _updateDstImage();
}

void JetSetRadioGraffitiTool::on_widthSlider_valueChanged(int value)
{
    if(_ui->uniformScaleCheckbox->isChecked())
        _ui->heightSlider->setValue(value);
    _updateDstImage();
}

void JetSetRadioGraffitiTool::on_exportButton_clicked()
{
    if(_dstImage.isNull()) return; //Should NEVER fucking happen, though...

    QString filter = "VMU VMS/VMI Files (*.vms)";
    QString path = QFileDialog::getSaveFileName(nullptr,
                                                "Specify Preview Image Destination",
                                                VmuQtUtils::getRomDir(),
                                                filter,
                                                &filter);

    if(!path.isEmpty()) {
        exportVmiVmsFromImage(_dstImage, path.split(".").first());
    }
}

void JetSetRadioGraffitiTool::on_qualitySpinbox_valueChanged(int arg1)
{
    _ui->qualitySlider->setValue(arg1);
    _updateDstImage();
}

void JetSetRadioGraffitiTool::on_qualitySlider_valueChanged(int value)
{
    _ui->qualitySpinbox->setValue(value);

}

void JetSetRadioGraffitiTool::on_destSavePreviewButton_clicked()
{
    if(_dstImage.isNull()) return; //shoudln't fucking happen, though.

    QString filter = "VMU Save Data (*.jpg)";
    QString path = QFileDialog::getSaveFileName(nullptr,
                                                "Specify Preview Image Destination",
                                                VmuQtUtils::getRomDir(),
                                                filter,
                                                &filter);

    if(path.isEmpty()) {
        return;
    }

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    QImageWriter writer(path);

    writer.setFormat("JPEG");
    writer.setQuality(_ui->qualitySpinbox->value());
    //writer.setCompression(compressionLevel);
    writer.setProgressiveScanWrite(false);
    writer.setOptimizedWrite(true);

    if(!writer.write(_srcItem->pixmap().scaled(_dstImage.width(), _dstImage.height()).toImage())) {
        Debug::log(Debug::LOG_ERROR, "[JetSetRadioGraffitiTool]: Failed to save JPEG Graffiti Preview: [%s]!", Q_CSTR(path));

    } else {
        Debug::log(Debug::LOG_VERBOSE, "[JetSetRadioGraffitiTool]: Successfully saved Graffiti Preview Image [%s]!", Q_CSTR(path));

    }
}

bool JetSetRadioGraffitiTool::exportVmiVmsFromImage(const QImage& image, QString outPath) {
    bool success = true;

    Debug::log(Debug::LOG_VERBOSE, "Exporting Graffiti VMS file: [%s]", Q_CSTR(outPath));
    Debug::push();

    VMSFileInfo* vmsHeader = (VMSFileInfo*)_vmsHeader;

    QString vmsPath = outPath + QString(".vms");
    gyVmuPrintVMSFileInfo(vmsHeader);
   // Debug::log(Debug::LOG_VERBOSE, "Desc: %s", gyVmuVmsHeaderVmuDescGet(vmsHeader));
    //Debug::log(Debug::LOG_VERBOSE, "DCDesc: %s", )
    Debug::log(Debug::LOG_VERBOSE, "Creating VMS File: [%s]", Q_CSTR(vmsPath));
    Debug::push();

    QFile file(vmsPath);
    if(!file.open(QIODevice::WriteOnly)) {
        Debug::log(Debug::LOG_ERROR, "Failed to create file!");
        success = false;
    } else {
        QDataStream out(&file);
        int bytesWritten;
        if((bytesWritten = out.writeRawData((const char*)_vmsHeader, sizeof(_vmsHeader)))
                != sizeof(_vmsHeader))
        {
            Debug::log(Debug::LOG_ERROR, "Failed to write VMS header! [bytes written: %d of total: %d]", bytesWritten, sizeof(_vmsHeader));
            success = false;
        } else {
            QByteArray byteArray;
            QBuffer buffer(&byteArray);

            if(!_writeJpegToBuffer(image, buffer, _ui->qualitySpinbox->value())) {
                Debug::log(Debug::LOG_ERROR, "Failed to write JPEG to internal buffer!");
                success = false;
            } else {
                char mimeHeader[0x30] = { '\0' };

                mimeHeader[0]  = 'i';
                mimeHeader[1]  = 'm';
                mimeHeader[2]  = 'a';
                mimeHeader[3]  = 'g';
                mimeHeader[4]  = 'e';
                mimeHeader[5]  = '/';
            // image type, but as this is 4 JSR I just letting it use JPG and not gif, although it should be easy to change
                mimeHeader[6]  = 'j';
                mimeHeader[7]  = 'p';
                mimeHeader[8]  = 'e';
                mimeHeader[9]  = 'g';
                mimeHeader[10] = ' ';
                mimeHeader[11] = ';';
                mimeHeader[12] = ' ';

                strcat(mimeHeader, "image_name");
                int bytesWritten;

                if((bytesWritten = out.writeRawData(mimeHeader, sizeof(mimeHeader))) != sizeof(mimeHeader)) {
                    Debug::log(Debug::LOG_ERROR, "Failed to write entire MIME header: [bytes written %d/%d", bytesWritten, sizeof(mimeHeader));
                    success = false;
                } else {

                    if((bytesWritten = out.writeRawData(byteArray.data(), byteArray.size())) != byteArray.size()) {
                        Debug::log(Debug::LOG_ERROR, "Failed to write JPEG buffer to VMS file!");
                        success = false;
                    } else {

                    }
                }
            }

        }
    }

    Debug::pop();

    QString vmiPath = outPath + QString(".vmi");
    Debug::log(Debug::LOG_VERBOSE, "Creating VMI File: [%s]", Q_CSTR(vmiPath));
    Debug::push();

    VMIFileInfo vmi;
    gyVmuVmiGenerateFromVms(&vmi, vmsHeader, _vmuSize, VMI_FILE_MODE_GAME_DATA);
    QFile vmiFile(vmiPath);
    if(!vmiFile.open(QIODevice::WriteOnly)) {
        Debug::log(Debug::LOG_ERROR, "Failed to create file!");
        success = false;
    } else {
        QDataStream out(&vmiFile);
        int bytesWritten;
        if((bytesWritten = out.writeRawData((const char*)&vmi, sizeof(VMIFileInfo)))
                != sizeof(VMIFileInfo))
        {
            Debug::log(Debug::LOG_ERROR, "Failed to write VMI File! [bytes written: %d total: %d]", bytesWritten, sizeof(VMIFileInfo));
            success = false;
        }
    }


    Debug::pop();

    return success;
}
