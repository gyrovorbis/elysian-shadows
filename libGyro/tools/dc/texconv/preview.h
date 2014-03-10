#ifndef PREVIEW_H
#define PREVIEW_H

#include <QVector>
#include <QtGui/QImage>

#include "color.h"
#include "vq.h"

// Generate a preview image for uncompressed images.
// In case there are mipmaps, the image will be 1.5 times as wide as the original
// and the mipmaps will be place to the right of the original image, top to bottom.
void Write16BPPPreview( const std::string& filename, const QVector<QImage>& mipmaps, const ColorConverter* converter );

// Generate a preview image for compressed images.
// In case there are mipmaps, the image will be 1.5 times as wide as the original
// and the mipmaps will be place to the right of the original image, top to bottom.
void WriteVqPreview( const std::string& filename, const QVector<QuadImage*>& mipmaps, const VQCodebook& codebook, const ColorConverter* converter );

// Generate a preview image for lossless compressed images.
// In case there are mipmaps, the image will be 1.5 times as wide as the original
// and the mipmaps will be place to the right of the original image, top to bottom.
void WriteLosslessVqPreview( const std::string& filename, const QVector<QuadImage16*>& mipmaps, const ColorConverter* converter );

#endif // PREVIEW_H
