/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef KYRA_SCREEN_EOB_H
#define KYRA_SCREEN_EOB_H

#ifdef ENABLE_EOB

#include "kyra/screen.h"

namespace Kyra {

class EobCoreEngine;
class Screen_Eob : public Screen{
public:
	Screen_Eob(EobCoreEngine *vm, OSystem *system);
	virtual ~Screen_Eob();

	bool init();

	void setScreenDim(int dim);
	const ScreenDim *getScreenDim(int dim);
	int curDimIndex() const { return _curDimIndex; }
	void modifyScreenDim(int dim, int x, int y, int w, int h);
	int screenDimTableCount() const { return _screenDimTableCount; }

	void setClearScreenDim(int dim);
	void clearCurDim();

	void setMouseCursor(int x, int y, const byte *shape);

	void loadFileDataToPage(Common::SeekableReadStream *s, int pageNum, uint32 size);

	void printShadedText(const char *string, int x, int y, int col1, int col2);
	void loadEobCpsFileToPage(const char *file, const uint8 *ditheringData, int tempPage, int destPage, int copyToPage);
	void loadEobBitmap(const char *file, int tempPage, int destPage);

	uint8 *encodeShape(uint16 x, uint16 y, uint16 w, uint16 h, bool flag = false);
	void drawShape(uint8 pageNum, const uint8 *shapeData, int x, int y, int sd = -1, int flags = 0, ...);
	const uint8 *scaleShape(const uint8 *shapeData, int blockDistance);
	const uint8 *scaleShapeStep(const uint8 *shp);
	void replaceShapePalette(uint8 *shp, const uint8 *pal);
	void applyShapeOverlay(uint8 *shp, int ovlIndex);

	void setShapeFrame(int x1, int y1, int x2, int y2) { _dsX1 = x1; _dsY1 = y1; _dsX2 = x2; _dsY2 = y2; }
	void setShapeFadeMode (uint8 i, bool b) { if (!i || i == 1) _shapeFadeMode[i] = b; }

	void fadeTextColor(Palette *pal, int color1, int fadeTextColor);
	bool delayedFadePalStep(Palette *fadePal, Palette *destPal, int rate);

	void setTextColorMap(const uint8 *cmap) {}
	int getRectSize(int w, int h) { return w * h; }

	void setFadeTableIndex(int index) { _fadeDataIndex = (CLIP(index, 0, 7) << 8); }
	void createFadeTable(uint8 *palData, uint8 *dst, uint8 rootColor, uint8 weight);
	uint8 *getFadeTable(int index) { return (index >= 0 && index < 5) ? &_fadeData[index << 8] : 0; }

protected:
	int16 _dsX1, _dsX2, _dsY1, _dsY2;
	bool _shapeFadeMode[2];
	uint16 _shapeFadeInternal;
	uint8 *_fadeData;
	int _fadeDataIndex;

	uint8 *_dsTempPage;

	static const ScreenDim _screenDimTable[];
	static const int _screenDimTableCount;

	ScreenDim **_customDimTable;
	int _curDimIndex;

private:
	void drawShapeSetPixel(uint8 *dst, uint8 c);
	void scaleShapeProcessLine(uint8 *&dst, const uint8 *&src);

	int _dsDiv, _dsRem, _dsScaleTmp;
};

}	// End of namespace Kyra

#endif // ENABLE_EOB

#endif


