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
 * $URL$
 * $Id$
 *
 */

#include "common/system.h"
#include "common/util.h"

#include "parallaction/parallaction.h"
#include "parallaction/sound.h"

namespace Parallaction {

enum MenuOptions {
	kMenuPart0 = 0,
	kMenuPart1 = 1,
	kMenuPart2 = 2,
	kMenuPart3 = 3,
	kMenuPart4 = 4,
	kMenuLoadGame = 5,
	kMenuQuit = 6
};

const char *partNames[] = {
	"PART0",
	"PART1",
	"PART2",
	"PART3",
	"PART4"
};

const char *partFirstLocation[] = {
	"intro",
	"museo",
	"start",
	"bolscoi",
	"treno"
};

int Parallaction_br::init() {

	_screenWidth = 640;
	_screenHeight = 400;

	if (getGameType() == GType_BRA) {
		if (getPlatform() == Common::kPlatformPC) {
			_disk = new DosDisk_br(this);
			_disk->setLanguage(2);					// NOTE: language is now hardcoded to English. Original used command-line parameters.
		} else
			error("unsupported platform for Big Red Adventure");
	} else
		error("unknown game type");

	_soundMan = new DummySoundMan(this);

	_activeZone2 = 0;

	initResources();
	initFonts();
	initCursors();
	initOpcodes();
	initParsers();

	_part = -1;

	Parallaction::init();

	return 0;
}

Parallaction_br::~Parallaction_br() {
	freeFonts();

	delete _dinoCursor;
	delete _dougCursor;
	delete _donnaCursor;

}

void Parallaction_br::callFunction(uint index, void* parm) {
	assert(index < 6);	// magic value 6 is maximum # of callables for Big Red Adventure

	(this->*_callables[index])(parm);
}

int Parallaction_br::go() {
	splash("dyna");
	splash("core");

	// TODO: load progress value from special save game
	_progress = 3;

	while ((_engineFlags & kEngineQuit) == 0) {

//		initCharacter();

		int option = showMenu();
		switch (option) {
		case kMenuQuit:
			_engineFlags |= kEngineQuit;
			break;

		case kMenuLoadGame:
			warning("loadgame not yet implemented");
			break;

		default:
			_part = option;
			_disk->selectArchive(partNames[_part]);
			startPart();
			break;
		}


		runGame();

		freePart();
//		freeLocation();
//		freeCharacter();

	}

	return 0;
}

void Parallaction_br::splash(const char *name) {

	_gfx->clearScreen();
	_gfx->setBackground(kBackgroundSlide, name, 0, 0);
	_gfx->_backgroundInfo.x = (_screenWidth - _gfx->_backgroundInfo.width) >> 1;
	_gfx->_backgroundInfo.y = (_screenHeight - _gfx->_backgroundInfo.height) >> 1;
	_gfx->updateScreen();
	_system->delayMillis(600);

	Palette blackPal;
	Palette pal(_gfx->_backgroundInfo.palette);
	for (uint i = 0; i < 64; i++) {
		pal.fadeTo(blackPal, 1);
		_gfx->setPalette(pal);
		_gfx->updateScreen();
		_system->delayMillis(20);
	}

}

#define MENUITEMS_X			250
#define MENUITEMS_Y			200

#define MENUITEM_WIDTH		190
#define MENUITEM_HEIGHT		18

void Parallaction_br::renderMenuItem(Graphics::Surface &surf, const char *text) {
	surf.create(MENUITEM_WIDTH, MENUITEM_HEIGHT, 1);
	_menuFont->setColor(0);
	_menuFont->drawString((byte*)surf.getBasePtr(5, 2), MENUITEM_WIDTH, text);
}

void Parallaction_br::invertMenuItem(Graphics::Surface &surf) {
	for (int i = 0; i < surf.w * surf.h; i++)
		((byte*)surf.pixels)[i] ^= 0xD;
}

int Parallaction_br::showMenu() {
	// TODO: filter menu entries according to progress in game
#if 0
	_gfx->clearScreen(Gfx::kBitFront);
#endif
	BackgroundInfo info;

	Graphics::Surface	_menuItems[7];

	const char *menuStrings[7] = {
		"SEE INTRO",
		"NEW GAME",
		"SAVED GAME",
		"EXIT TO DOS",
		"PART 2",
		"PART 3",
		"PART 4"
	};

	MenuOptions options[7] = {
		kMenuPart0,
		kMenuPart1,
		kMenuLoadGame,
		kMenuQuit,
		kMenuPart2,
		kMenuPart3,
		kMenuPart4
	};
#if 0
	_disk->loadSlide(info, "tbra");
	_gfx->setPalette(info.palette);
	_gfx->flatBlitCnv(&info.bg, 20, 50, Gfx::kBitFront);
#endif
	int availItems = 4 + _progress;

	for (int i = 0; i < availItems; i++)
		renderMenuItem(_menuItems[i], menuStrings[i]);

	int selectedItem = -1, oldSelectedItem = -2;

	setMousePointer(0);

	while (true) {

		if ((_mouseButtons == kMouseLeftUp) && selectedItem >= 0)
			break;

		readInput();

		if ((_mousePos.x > MENUITEMS_X) && (_mousePos.x < (MENUITEMS_X+MENUITEM_WIDTH)) && (_mousePos.y > MENUITEMS_Y)) {
			selectedItem = (_mousePos.y - MENUITEMS_Y) / MENUITEM_HEIGHT;

			if (!(selectedItem < availItems))
				selectedItem = -1;
		} else
			selectedItem = -1;


		if (selectedItem != oldSelectedItem) {

			if (selectedItem >= 0 && selectedItem < availItems)
				invertMenuItem(_menuItems[selectedItem]);

			if (oldSelectedItem >= 0 && oldSelectedItem < availItems)
				invertMenuItem(_menuItems[oldSelectedItem]);

			Common::Rect r(MENUITEM_WIDTH, MENUITEM_HEIGHT);
#if 0
			for (int i = 0; i < availItems; i++) {
				r.moveTo(MENUITEMS_X, MENUITEMS_Y + i * 20);
				_gfx->copyRect(Gfx::kBitFront, r, (byte*)_menuItems[i].pixels, _menuItems[i].pitch);
			}
#endif
			oldSelectedItem = selectedItem;
		}

		_gfx->updateScreen();
		_system->delayMillis(20);
	}

	_system->showMouse(false);

	info.bg.free();

	for (int i = 0; i < availItems; i++)
		_menuItems[i].free();

	return options[selectedItem];
}


void Parallaction_br::initFonts() {

	_menuFont = _disk->loadFont("russia");
	_dialogueFont = _disk->loadFont("comic");
	_labelFont = _menuFont;

}

void Parallaction_br::freeFonts() {

	delete _menuFont;
	delete _dialogueFont;

	return;
}

void Parallaction_br::initCursors() {

	_dinoCursor = _disk->loadPointer("pointer1");
	_dougCursor = _disk->loadPointer("pointer2");
	_donnaCursor = _disk->loadPointer("pointer3");

	_mouseArrow = _donnaCursor;

}

void Parallaction_br::setMousePointer(int16 index) {

	Common::Rect r;
	_mouseArrow->getRect(0, r);

	_system->setMouseCursor(_mouseArrow->getData(0), r.width(), r.height(), 0, 0, 0);
	_system->showMouse(true);

}

void Parallaction_br::initPart() {

	memset(_counters, 0, ARRAYSIZE(_counters));

	_globalTable = _disk->loadTable("global");
	_objectsNames = _disk->loadTable("objects");
	_countersNames = _disk->loadTable("counters");

//	_disk->loadObjects("icone.ico");

}

void Parallaction_br::freePart() {

	delete _globalTable;
	delete _objectsNames;
	delete _countersNames;

	_globalTable = 0;
	_objectsNames = 0;
	_countersNames = 0;
}

void Parallaction_br::startPart() {

	initPart();

	strcpy(_location._name, partFirstLocation[_part]);

	parseLocation("common");
	parseLocation(_location._name);

}

void Parallaction_br::changeLocation(char *location) {

}

void Parallaction_br::changeCharacter(const char *name) {

}


void Parallaction_br::setArrowCursor() {



}

void Parallaction_br::setInventoryCursor(int pos) {



}

} // namespace Parallaction
