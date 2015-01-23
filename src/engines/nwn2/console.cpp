/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file engines/nwn2/console.cpp
 *  NWN2 (debug) console.
 */

#include <boost/bind.hpp>

#include "common/ustring.h"
#include "common/util.h"

#include "aurora/resman.h"

#include "graphics/aurora/fontman.h"

#include "engines/nwn2/console.h"
#include "engines/nwn2/module.h"
#include "engines/nwn2/area.h"

namespace Engines {

namespace NWN2 {

Console::Console() : ::Engines::Console(Graphics::Aurora::kSystemFontMono, 13),
	_module(0), _maxSizeMusic(0) {

	registerCommand("listmusic", boost::bind(&Console::cmdListMusic, this, _1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic", boost::bind(&Console::cmdStopMusic, this, _1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic", boost::bind(&Console::cmdPlayMusic, this, _1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
}

Console::~Console() {
}

void Console::setModule(Module *module) {
	_module = module;
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateMusic();
}

void Console::updateMusic() {
	_music.clear();
	_maxSizeMusic = 0;

	std::list<Aurora::ResourceManager::ResourceID> music;
	ResMan.getAvailableResources(Aurora::kFileTypeBMU, music);

	for (std::list<Aurora::ResourceManager::ResourceID>::const_iterator m = music.begin();
	     m != music.end(); ++m) {

		_music.push_back(m->name);

		_maxSizeMusic = MAX(_maxSizeMusic, _music.back().size());
	}

	setArguments("playmusic", _music);
}

void Console::cmdListMusic(const CommandLine &UNUSED(cl)) {
	updateMusic();
	printList(_music, _maxSizeMusic);
}

void Console::cmdStopMusic(const CommandLine &UNUSED(cl)) {
	if (!_module || !_module->_currentArea)
		return;

	_module->_currentArea->stopAmbientMusic();
}

void Console::cmdPlayMusic(const CommandLine &cl) {
	if (!_module || !_module->_currentArea)
		return;

	_module->_currentArea->playAmbientMusic(cl.args);
}

} // End of namespace NWN2

} // End of namespace Engines
