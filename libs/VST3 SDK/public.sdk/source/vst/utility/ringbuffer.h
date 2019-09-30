//-----------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/vst/utility/ringbuffer.h
// Created by  : Steinberg, 01/2018
// Description : Simple RingBuffer with one reader and one writer thread
//
//-----------------------------------------------------------------------------
// LICENSE
// (c) 2018, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this
//     software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/ftypes.h"

#include <atomic>
#include <vector>

//------------------------------------------------------------------------
namespace Steinberg {
namespace OneReaderOneWriter {

//------------------------------------------------------------------------
/** Ringbuffer
 *
 *	A ringbuffer supporting one reader and one writer thread
 */
template <typename ItemT>
class RingBuffer
{
private:
	using AtomicIndex = std::atomic<uint32>;
	using StorageT = std::vector<ItemT>;

	StorageT buffer;
	AtomicIndex readPosition {0u};
	AtomicIndex writePosition {0u};

public:
	/** Default constructor
	 *
	 *	@param initialNumberOfItems initial ring buffer size
	 */
	RingBuffer (size_t initialNumberOfItems = 0) noexcept
	{
		if (initialNumberOfItems)
			resize (initialNumberOfItems);
	}

	/** resize
	 *
	 *	note that you have to make sure that no other thread is reading or writing while calling
	 *	this method
	 *	@param newNumberOfItems resize buffer
	 */
	void resize (size_t newNumberOfItems) noexcept { buffer.resize (newNumberOfItems); }

	/** push a new item into the ringbuffer
	 *
	 *	@item item to push
	 *	@return true on success or false on buffer overflow
	 */
	bool push (ItemT&& item) noexcept
	{
		auto pos = writePosition.load ();
		if (pos == (readPosition.load () - 1))
			return false; // full

		buffer[pos] = std::move (item);
		++pos;
		if (pos >= buffer.size ())
			pos = 0u;

		writePosition.store (pos);
		return true;
	}

	/** push a new item into the ringbuffer
	 *
	 *	@item item to push
	 *	@return true on success or false on buffer overflow
	 */
	bool push (const ItemT& item) noexcept
	{
		auto pos = writePosition.load ();
		if (pos == (readPosition.load () - 1))
			return false; // full

		buffer[pos] = item;
		++pos;
		if (pos >= buffer.size ())
			pos = 0u;

		writePosition.store (pos);
		return true;
	}

	/** pop an item out of the ringbuffer
	 *
	 *	@item item
	 *	@return true on success or false if ringbuffer is empty
	 */
	bool pop (ItemT& item) noexcept
	{
		auto pos = readPosition.load ();
		if (pos == writePosition.load ())
			return false; // empty
		item = std::move (buffer[pos]);
		++pos;
		if (pos >= buffer.size ())
			pos = 0;
		readPosition.store (pos);
		return true;
	}
};

//------------------------------------------------------------------------
} // OneReaderOneWriter
} // Steinberg
