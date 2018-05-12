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
 *  Unit tests for our DEFLATE decompressor (which uses zlib).
 */

#include "gtest/gtest.h"

#include "src/common/deflate.h"
#include "src/common/memreadstream.h"
#include "src/common/error.h"

// Percy Bysshe Shelley's "Ozymandias"
static const char *kDataUncompressed =
	"I met a traveller from an antique land\n"
	"Who said: Two vast and trunkless legs of stone\n"
	"Stand in the desert. Near them, on the sand,\n"
	"Half sunk, a shattered visage lies, whose frown,\n"
	"And wrinkled lip, and sneer of cold command,\n"
	"Tell that its sculptor well those passions read\n"
	"Which yet survive, stamped on these lifeless things,\n"
	"The hand that mocked them and the heart that fed:\n"
	"And on the pedestal these words appear:\n"
	"'My name is Ozymandias, king of kings:\n"
	"Look on my works, ye Mighty, and despair!'\n"
	"Nothing beside remains. Round the decay\n"
	"Of that colossal wreck, boundless and bare\n"
	"The lone and level sands stretch far away.";

// Percy Bysshe Shelley's "Ozymandias", compressed using DEFLATE
static const byte kDataCompressed[] = {
	0x2D,0x52,0xC9,0x6E,0xA4,0x30,0x10,0xBD,0xF3,0x15,0x95,0x53,0x2E,0xA8,0x3F,0xA0,
	0x6F,0x73,0x9B,0x48,0x59,0xA4,0x24,0xD2,0x9C,0xAB,0x71,0xD1,0x58,0xD8,0x2E,0xC6,
	0x65,0x40,0xE4,0xEB,0xF3,0x6C,0x5A,0x42,0x20,0xD5,0xF2,0xB6,0xE2,0x85,0xA2,0x14,
	0x62,0x2A,0x99,0x37,0x09,0x41,0x32,0x8D,0x59,0x23,0x71,0xC2,0x53,0xFC,0xFF,0x55,
	0x28,0x70,0x72,0xDD,0xBF,0x49,0xC9,0xD8,0xBB,0x2B,0x7D,0xEF,0x4A,0x1B,0x1B,0x76,
	0x92,0xC3,0xD6,0x9A,0xE6,0x20,0x66,0x14,0xE4,0x6E,0xA4,0x23,0x59,0xD1,0x24,0xDD,
	0x57,0xA9,0x5D,0x9F,0xA8,0x4C,0x42,0x4E,0x4C,0x72,0xB9,0xD0,0xBB,0x70,0xAE,0x85,
	0xD8,0x93,0x9E,0x1D,0xC3,0x54,0xDF,0xFD,0xE5,0x80,0x3D,0x00,0xF5,0xD0,0x61,0x13,
	0x97,0x22,0x59,0x1C,0x6D,0xDE,0xF8,0x0E,0x7A,0x2F,0xD6,0xD3,0x3E,0xA9,0x49,0x95,
	0xB6,0xA7,0xBE,0xFB,0x03,0xEC,0x3D,0xFB,0xCA,0xEC,0xD0,0x5F,0xFA,0xA6,0xC5,0x92,
	0x40,0x3D,0x24,0x0C,0x1A,0x1C,0x5E,0x31,0x36,0xF4,0x6F,0xB8,0x02,0x19,0x17,0xF2,
	0xC5,0xC8,0x86,0x35,0x2C,0x45,0x33,0xED,0x67,0xB9,0xA2,0x2E,0x6C,0xE6,0x35,0x19,
	0x65,0xE1,0xEA,0xD4,0x0F,0x13,0x1D,0x08,0xC5,0xD6,0xBC,0xF9,0x4D,0x7A,0x78,0xE2,
	0xB8,0x80,0xEA,0x54,0x6D,0x55,0xD3,0x28,0xCD,0x75,0x99,0x7C,0xBA,0x1B,0x48,0x60,
	0x66,0x6A,0x81,0x54,0xA2,0xA8,0xC3,0x2C,0xAE,0x59,0x3D,0x53,0xAA,0x5D,0x98,0x2F,
	0x67,0x7B,0x14,0x77,0x6D,0x1E,0x1E,0x29,0x00,0x5A,0x40,0x11,0x1E,0xE0,0xBB,0x66,
	0x67,0xC4,0xCB,0x82,0x8D,0x6B,0xF7,0xFC,0x76,0x50,0xE2,0x28,0xE4,0x8D,0x3E,0x7E,
	0x8E,0xEA,0xC9,0x33,0x02,0x99,0x41,0x5C,0xCD,0xD6,0xAF,0x5D,0xBB,0x57,0xD5,0xB9,
	0xE2,0xC5,0xA3,0xEE,0xCF,0x18,0x38,0x84,0xDE,0xFC,0x7D,0x2A,0xC7,0x99,0x0E,0x28,
	0x16,0xF6,0xF9,0xE9,0xB9,0x7B,0xD7,0xA6,0x9A,0x6E,0x62,0xDE,0x09,0x4C,0x47,0xF6,
	0xC9,0x2E,0xF4,0xA9,0xEB,0x43,0xAB,0x93,0x81,0x8F,0xEE,0x63,0x3C,0xE5,0x22,0x4E,
	0x35,0x83,0xBC,0x3D,0xCB,0x80,0x1B,0xDD,0xEA,0x5C,0x73,0x5F,0x71,0x6F,0x9C,0xA5,
	0xD9,0x0F,0xB8,0x7B,0xAB,0x04,0xC1,0x8F,0xD4,0x4E,0x8B,0xB8,0x4B,0x96,0x82,0x38,
	0x47,0x5C,0x9E,0x77,0x3E,0x2E,0xBF
};

GTEST_TEST(DEFLATE, decompressBuf) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed);

	const byte *decompressed =
		Common::decompressDeflate(kDataCompressed, kSizeCompressed, kSizeDecompressed, Common::kWindowBitsMaxRaw);
	ASSERT_NE(decompressed, static_cast<const byte *>(0));

	for (size_t i = 0; i < kSizeDecompressed; i++)
		EXPECT_EQ(decompressed[i], kDataUncompressed[i]) << "At index " << i;

	delete[] decompressed;
}

GTEST_TEST(DEFLATE, decompressBufWithoutDecompressedSize) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed);

	size_t size = 0;
	const byte *decompressed =
		Common::decompressDeflateWithoutOutputSize(kDataCompressed, kSizeCompressed, size, Common::kWindowBitsMaxRaw, 128);
	ASSERT_NE(decompressed, static_cast<const byte *>(0));
	ASSERT_EQ(size, kSizeDecompressed);

	for (size_t i = 0; i < kSizeDecompressed; i++)
		EXPECT_EQ(decompressed[i], kDataUncompressed[i]) << "At index " << i;

	delete[] decompressed;
}

GTEST_TEST(DEFLATE, decompressStream) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed);

	Common::MemoryReadStream compressed(kDataCompressed);

	Common::SeekableReadStream *decompressed =
		Common::decompressDeflate(compressed, kSizeCompressed, kSizeDecompressed, Common::kWindowBitsMaxRaw);
	ASSERT_NE(decompressed, static_cast<Common::SeekableReadStream *>(0));

	ASSERT_EQ(decompressed->size(), kSizeDecompressed);

	for (size_t i = 0; i < kSizeDecompressed; i++)
		EXPECT_EQ(decompressed->readByte(), kDataUncompressed[i]) << "At index " << i;

	delete decompressed;
}

GTEST_TEST(DEFLATE, decompressStreamWithoutDecompressedSize) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed);

	Common::MemoryReadStream compressed(kDataCompressed);

	Common::SeekableReadStream *decompressed =
			Common::decompressDeflateWithoutOutputSize(compressed, kSizeCompressed, Common::kWindowBitsMaxRaw, 128);
	ASSERT_NE(decompressed, static_cast<Common::SeekableReadStream *>(0));

	ASSERT_EQ(decompressed->size(), kSizeDecompressed);

	for (size_t i = 0; i < kSizeDecompressed; i++)
		EXPECT_EQ(decompressed->readByte(), kDataUncompressed[i]) << "At index " << i;

	delete decompressed;
}

GTEST_TEST(DEFLATE, decompressFailOutputSmall) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed) / 2;

	EXPECT_THROW(Common::decompressDeflate(kDataCompressed, kSizeCompressed,
	                                       kSizeDecompressed, Common::kWindowBitsMaxRaw),
	             Common::Exception);
}

GTEST_TEST(DEFLATE, decompressFailOutputBig) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed);
	static const size_t kSizeDecompressed = strlen(kDataUncompressed) * 2;

	EXPECT_THROW(Common::decompressDeflate(kDataCompressed, kSizeCompressed,
	                                       kSizeDecompressed, Common::kWindowBitsMaxRaw),
	             Common::Exception);
}

GTEST_TEST(DEFLATE, decompressFailInputCut) {
	static const size_t kSizeCompressed   = sizeof(kDataCompressed) / 2;
	static const size_t kSizeDecompressed = strlen(kDataUncompressed);

	EXPECT_THROW(Common::decompressDeflate(kDataCompressed, kSizeCompressed,
	                                       kSizeDecompressed, Common::kWindowBitsMaxRaw),
	             Common::Exception);
}
