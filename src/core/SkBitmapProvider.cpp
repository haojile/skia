/*
 * Copyright 2015 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkBitmapProvider.h"
#include "SkImage_Base.h"
#include "SkImageCacherator.h"
#include "SkPixelRef.h"

int SkBitmapProvider::width() const {
    return fImage->width();
}

int SkBitmapProvider::height() const {
    return fImage->height();
}

uint32_t SkBitmapProvider::getID() const {
    return fImage->uniqueID();
}

SkImageInfo SkBitmapProvider::info() const {
    return as_IB(fImage)->onImageInfo();
}

bool SkBitmapProvider::isVolatile() const {
    // add flag to images?
    const SkBitmap* bm = as_IB(fImage)->onPeekBitmap();
    return bm ? bm->isVolatile() : false;
}

SkBitmapCacheDesc SkBitmapProvider::makeCacheDesc(int w, int h) const {
    return SkBitmapCacheDesc::Make(fImage, w, h);
}

SkBitmapCacheDesc SkBitmapProvider::makeCacheDesc() const {
    return SkBitmapCacheDesc::Make(fImage);
}

void SkBitmapProvider::notifyAddedToCache() const {
    as_IB(fImage)->notifyAddedToCache();
}

bool SkBitmapProvider::asBitmap(SkBitmap* bm) const {
    return as_IB(fImage)->getROPixels(bm, fColorMode, SkImage::kAllow_CachingHint);
}

bool SkBitmapProvider::accessScaledImage(const SkRect& srcRect,
                                         const SkMatrix& invMatrix,
                                         SkFilterQuality fq,
                                         SkBitmap* scaledBitmap,
                                         SkRect* adjustedSrcRect,
                                         SkFilterQuality* adjustedFilterQuality) const {
    if (!fImage) {
        return false;
    }

    SkImageCacherator* cacherator = as_IB(fImage)->peekCacherator();
    if (!cacherator) {
        return false;
    }

    // TODO: stash the matrix someplace to avoid invert()?
    SkMatrix m;
    if (!invMatrix.invert(&m)) {
        return false;
    }

    SkImageGenerator::ScaledImageRec rec;
    if (!cacherator->directAccessScaledImage(srcRect, m, fq, &rec) ||
        !rec.fImage->asLegacyBitmap(scaledBitmap, SkImage::kRO_LegacyBitmapMode)) {
        return false;
    }

    *adjustedSrcRect       = rec.fSrcRect;
    *adjustedFilterQuality = rec.fQuality;

    return true;
}
