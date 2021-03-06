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

/** @file
 *  An FMOD SampleBank, found in Dragon Age: Origins as FSB files.
 */

#ifndef SOUND_FMODSAMPLEBANK_H
#define SOUND_FMODSAMPLEBANK_H

#include "src/common/ustring.h"
#include "src/common/scopedptr.h"
#include "src/common/readstream.h"

namespace Sound {

class RewindableAudioStream;

/** Class to hold audio resource data of an FMOD samplebank file.
 *
 *  An FSB file is a samplebank, i.e. an archive containing one or more
 *  audio files. It's part of FMOD sound middleware, and commonly used
 *  together with FEV files, which define events and effect chains.
 *
 *  FSB files are found in Dragon Age: Origins.
 *
 *  Only version 4 of the FSB format is supported, because that's the
 *  version used by Dragon Age: Origins.
 */
class FMODSampleBank {
public:
	FMODSampleBank(Common::SeekableReadStream *fsb);
	FMODSampleBank(const Common::UString &name);
	~FMODSampleBank() { }

	/** Return the number of sample files. */
	size_t getSampleCount() const;

	/** Return the name of a sample. */
	const Common::UString &getSampleName(size_t index) const;
	/** Does a sample with this name exist in the sample bank? */
	bool hasSample(const Common::UString &name) const;

	/** Return the audio stream of a sample, by index. */
	RewindableAudioStream *getSample(size_t index) const;
	/** Return the audio stream of a sample, by name. */
	RewindableAudioStream *getSample(const Common::UString &name) const;

private:
	struct Sample {
		Common::UString name; ///< Name of the sample.

		uint32 length; ///< Length of the sample, in audio samples.

		uint32 loopStart;
		uint32 loopEnd;

		uint32 flags;

		int32  defFreq;
		uint16 defVol;
		int16  defPan;
		uint16 defPri;

		uint16 channels;

		float minDistance;
		float maxDistance;

		int32  varFreq;
		uint16 varVol;
		int16  varPan;

		size_t offset; ///< Offset to the sample within the FSB.
		size_t size;   ///< Size of the sample in bytes.
	};


	Common::ScopedPtr<Common::SeekableReadStream> _fsb;

	std::vector<Sample> _samples;

	std::map<Common::UString, const Sample *> _sampleMap;


	void load(Common::SeekableReadStream &fsb);

	RewindableAudioStream *getSample(const Sample &sample) const;
};

} // End of namespace Sound

#endif // SOUND_FMODSAMPLEBANK_H
