/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/area.cpp
 *  An area.
 */

#include "engines/kotor/area.h"
#include "engines/kotor/placeable.h"

#include "common/error.h"
#include "common/ustring.h"
#include "common/stream.h"

#include "engines/engine.h"
#include "engines/util.h"

#include "aurora/resman.h"
#include "aurora/util.h"

#include "graphics/aurora/model_kotor.h"

static const uint32 kAREID = MKID_BE('ARE ');
static const uint32 kGITID = MKID_BE('GIT ');

namespace Engines {

namespace KotOR {

Area::Area(const ModelLoader &modelLoader) : _modelLoader(&modelLoader) {
}

Area::~Area() {
	for (std::vector<Graphics::Aurora::Model *>::iterator model = _models.begin(); model != _models.end(); ++model)
		delete *model;

	for (std::list<Placeable *>::iterator plc = _placeables.begin(); plc != _placeables.end(); ++plc)
		delete *plc;
}

void Area::load(const Common::UString &name) {
	loadLYT(name); // Room layout
	loadVIS(name); // Room visibilities

	loadARE(name); // Statics
	loadGIT(name); // Dynamics

	loadModels(name);
}

void Area::loadLYT(const Common::UString &name) {
	Common::SeekableReadStream *lyt = 0;
	try {
		if (!(lyt = ResMan.getResource(name, Aurora::kFileTypeLYT)))
			throw Common::Exception("No such LYT \"%s\"", name.c_str());

		_lyt.load(*lyt);

		delete lyt;
	} catch (...) {
		delete lyt;
		throw;
	}
}

void Area::loadVIS(const Common::UString &name) {
	Common::SeekableReadStream *vis = 0;
	try {
		if (!(vis = ResMan.getResource(name, Aurora::kFileTypeVIS)))
			throw Common::Exception("No such VIS \"%s\"", name.c_str());

		_vis.load(*vis);

		delete vis;
	} catch (...) {
		delete vis;
		throw;
	}
}

void Area::loadARE(const Common::UString &name) {
	Aurora::GFFFile are;
	loadGFF(are, name, Aurora::kFileTypeARE, kAREID);
}

void Area::loadGIT(const Common::UString &name) {
	Aurora::GFFFile git;
	loadGFF(git, name, Aurora::kFileTypeGIT, kGITID);

	Aurora::GFFFile::StructRange gitTop = git.structRange();
	for (Aurora::GFFFile::StructIterator it = gitTop.first; it != gitTop.second; ++it) {

		if (it->getLabel() == "Placeable List") {
			Aurora::GFFFile::ListRange placeables = git.listRange(it->getListIndex());
			for (Aurora::GFFFile::ListIterator plc = placeables.first; plc != placeables.second; ++plc)
				loadPlaceable(plc);
		}

	}
}

void Area::loadPlaceable(Aurora::GFFFile::ListIterator &placeable) {
	Common::UString resref;
	float x = 0.0, y = 0.0, z = 0.0;
	float bearing = 0.0;

	for (Aurora::GFFFile::StructIterator it = placeable->first; it != placeable->second; ++it) {
		if      (it->getLabel() == "TemplateResRef")
			resref = it->getString();
		else if (it->getLabel() == "X")
			x = it->getDouble();
		else if (it->getLabel() == "Y")
			y = it->getDouble();
		else if (it->getLabel() == "Z")
			z = it->getDouble();
		else if (it->getLabel() == "Bearing")
			bearing = it->getDouble();
	}

	if (resref.empty())
		throw Common::Exception("Placeable without a template");

	Placeable *place = 0;
	try {
		place = new Placeable(*_modelLoader);

		place->load(resref);
		place->setPosition(x, y, z);
		place->setBearing(bearing);

	} catch (...) {
		delete place;
		throw;
	}

	_placeables.push_back(place);
}

void Area::loadModels(const Common::UString &name) {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	_models.resize(rooms.size());
	for (size_t i = 0; i < rooms.size(); i++)
		_models[i] = 0;

	for (size_t i = 0; i < rooms.size(); i++) {
		const Aurora::LYTFile::Room &room = rooms[i];

		if (room.model == "****")
			// No model for that room
			continue;

		try {
			_models[i] = (*_modelLoader)(room.model);
		} catch (Common::Exception &e) {
			e.add("Can't load model \"%s\" for area \"%s\"", room.model.c_str(), name.c_str());
			throw e;
		} catch (...) {
			throw;
		}

		_models[i]->setPosition(room.x, room.y, room.z);
	}
}

void Area::show() {
	for (std::vector<Graphics::Aurora::Model *>::iterator model = _models.begin(); model != _models.end(); ++model)
		if (*model)
			(*model)->show();
}

void Area::hide() {
	for (std::vector<Graphics::Aurora::Model *>::iterator model = _models.begin(); model != _models.end(); ++model)
		if (*model)
			(*model)->hide();
}

void Area::setPosition(float x, float y, float z) {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	for (size_t i = 0; i < rooms.size(); i++)
		if (_models[i])
			_models[i]->setPosition(rooms[i].x + x, rooms[i].y + y, rooms[i].z + z);
}

void Area::setOrientation(float x, float y, float z) {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	for (size_t i = 0; i < rooms.size(); i++)
		if (_models[i])
			_models[i]->setOrientation(x, y, z);
}

} // End of namespace KotOR

} // End of namespace Engines
