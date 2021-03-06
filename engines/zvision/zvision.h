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
 *
 */

#ifndef ZVISION_ZVISION_H
#define ZVISION_ZVISION_H

#include "zvision/detection.h"
#include "zvision/utility/clock.h"
#include "zvision/core/search_manager.h"

#include "common/random.h"
#include "common/events.h"

#include "engines/engine.h"

#include "graphics/pixelformat.h"

#include "gui/debugger.h"

namespace Video {
class VideoDecoder;
}

namespace ZVision {

struct ZVisionGameDescription;
class Console;
class ScriptManager;
class RenderManager;
class CursorManager;
class StringManager;
class SaveManager;
class RlfAnimation;
class MenuHandler;
class TextRenderer;
class Subtitle;
class MidiManager;

class ZVision : public Engine {
public:
	ZVision(OSystem *syst, const ZVisionGameDescription *gameDesc);
	~ZVision();

public:
	/**
	 * A Rectangle centered inside the actual window. All in-game coordinates
	 * are given in this coordinate space. Also, all images are clipped to the
	 * edges of this Rectangle
	 */
	const Common::Rect &_workingWindow;
	const Graphics::PixelFormat _pixelFormat;

private:
	enum {
		WINDOW_WIDTH = 640,
		WINDOW_HEIGHT = 480,

		//Zork nemesis working window sizes
		ZNM_WORKING_WINDOW_WIDTH  = 512,
		ZNM_WORKING_WINDOW_HEIGHT = 320,

		//ZGI(and default) working window sizes
		WORKING_WINDOW_WIDTH  = 640,
		WORKING_WINDOW_HEIGHT = 344,

		ROTATION_SCREEN_EDGE_OFFSET = 60,
		MAX_ROTATION_SPEED = 400, // Pixels per second

		KEYBUF_SIZE = 20
	};

	Console *_console;
	const ZVisionGameDescription *_gameDescription;

	const int _desiredFrameTime;

	// We need random numbers
	Common::RandomSource *_rnd;

	// Managers
	ScriptManager *_scriptManager;
	RenderManager *_renderManager;
	CursorManager *_cursorManager;
	SaveManager *_saveManager;
	StringManager *_stringManager;
	MenuHandler *_menu;
	SearchManager *_searchManager;
	TextRenderer *_textRenderer;
	MidiManager *_midiManager;

	// Clock
	Clock _clock;

	// Audio ID
	int _audioId;

	// To prevent allocation every time we process events
	Common::Event _event;

	const Common::Rect _workingWindow_ZGI;
	const Common::Rect _workingWindow_ZNM;

	int _rendDelay;
	int16 _mouseVelocity;
	int16 _kbdVelocity;
	bool _halveDelay;

	uint8 _cheatBuff[KEYBUF_SIZE];
public:
	uint32 getFeatures() const;
	Common::Language getLanguage() const;
	Common::Error run();
	void pauseEngineIntern(bool pause);

	ScriptManager *getScriptManager() const {
		return _scriptManager;
	}
	RenderManager *getRenderManager() const {
		return _renderManager;
	}
	CursorManager *getCursorManager() const {
		return _cursorManager;
	}
	SaveManager *getSaveManager() const {
		return _saveManager;
	}
	StringManager *getStringManager() const {
		return _stringManager;
	}
	SearchManager *getSearchManager() const {
		return _searchManager;
	}
	TextRenderer *getTextRenderer() const {
		return _textRenderer;
	}
	MidiManager *getMidiManager() const {
		return _midiManager;
	}
	Common::RandomSource *getRandomSource() const {
		return _rnd;
	}
	ZVisionGameId getGameId() const {
		return _gameDescription->gameId;
	}

	uint8 getZvisionKey(Common::KeyCode scummKeyCode);

	/**
	 * Play a video until it is finished. This is a blocking call. It will call
	 * _clock.stop() when the video starts and _clock.start() when the video finishes.
	 * It will also consume all events during video playback.
	 *
	 * @param videoDecoder    The video to play
	 * @param destRect        Where to put the video. (In working window coords)
	 * @param skippable       If true, the video can be skipped at any time using [Spacebar]
	 */
	void playVideo(Video::VideoDecoder &videoDecoder, const Common::Rect &destRect = Common::Rect(0, 0, 0, 0), bool skippable = true, Subtitle *sub = NULL);

	void rotateTo(int16 to, int16 time);

	Common::String generateSaveFileName(uint slot);
	Common::String generateAutoSaveFileName();

	bool askQuestion(const Common::String &str);
	void delayedMessage(const Common::String &str, uint16 milsecs);
	void timedMessage(const Common::String &str, uint16 milsecs);

	void setRenderDelay(uint);
	bool canRender();

	void loadSettings();
	void saveSettings();

	void menuBarEnable(uint16 menus);
	uint16 getMenuBarEnable();

	bool ifQuit();

	void checkBorders();
	void showDebugMsg(const Common::String &msg, int16 delay = 3000);
private:
	void initialize();
	void initFonts();

	void parseStrFile(const Common::String fileName);

	/** Called every frame from ZVision::run() to process any events from EventMan */
	void processEvents();

	void onMouseMove(const Common::Point &pos);
	void updateRotation();

	void registerDefaultSettings();
	void shortKeys(Common::Event);

	void cheatCodes(uint8 key);
	void pushKeyToCheatBuf(uint8 key);
	bool checkCode(const char *code);
	uint8 getBufferedKey(uint8 pos);
};

} // End of namespace ZVision

#endif
