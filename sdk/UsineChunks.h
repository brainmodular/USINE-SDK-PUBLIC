//-----------------------------------------------------------------------------
///@file UsineChunks.h
///
///@author
///	BrainModular team
///
///@brief
///	Lightweight typed reader over Usine chunks
///
///@IMPORTANT
///	This file is part of the Usine Hollyhock CPP SDK
///
///@LICENSE
/// Copyright (C) 2026 BrainModular, BeSpline, Adamson
///
///-----------------------------------------------------------------------------

// Include once, no more
#pragma once

#include <cmath>
#include <string>
#include <vector>
#include "UsineEventClass.h"

/// @addtogroup UsineChunks
/// @{

//----------------------------------------------------------------------------
/// @class UsineChunk
/// @brief Read typed values from a named Usine chunk.
///
/// The module knows the chunk name and the type of each item, so each read is a single
/// call with no knowledge of flow types or events needed.
/// Reading never creates the chunk nor the item: a missing chunk or item makes the getters return false.
/// @note Not thread-safe: each object owns one scratch event reused by every read.
class UsineChunk
{
public:
    /// @param module The owning module, used to call the sdk chunk functions.
    /// @param chunkName The name of the chunk to read from. The chunk doesn't need to exist yet.
    UsineChunk(const UserModuleBase* module, std::string chunkName)
        : m_module(module), m_name(std::move(chunkName))
    {
    }

    ~UsineChunk()
    {
        if (m_scratch.event() != nullptr)
            m_scratch.destroyEvent();
    }

    UsineChunk(const UsineChunk&) = delete;
    UsineChunk& operator=(const UsineChunk&) = delete;

    /// @return The chunk name.
    const std::string& name() const { return m_name; }

    /// @return true if the chunk exists. Never creates the chunk.
    bool exists() const { return m_module->sdkChunkExist(cname()) != FALSE; }

    /// @param item The item name.
    /// @return true if both the chunk and the item exist.
    bool has(const char* item) const { return m_module->sdkChunkItemExist(cname(), (AnsiCharPtr)item) != FALSE; }

    /// Read a float array item.
    /// @return false (out cleared) if the chunk or the item is missing.
    bool getFloats(const char* item, std::vector<float>& out)
    {
        out.clear();
        if (!read(item, ftDataFloat)) return false;
        const int n = m_scratch.getSize();
        out.resize(size_t(n > 0 ? n : 0));
        for (int i = 0; i < int(out.size()); i++)
            out[size_t(i)] = float(m_scratch.getArrayData(i));
        return true;
    }

    /// Read an integer array item.
    /// @return false (out cleared) if the chunk or the item is missing.
    bool getInts(const char* item, std::vector<int>& out)
    {
        out.clear();
        if (!read(item, ftDataInteger)) return false;
        const int n = m_scratch.getSize();
        out.resize(size_t(n > 0 ? n : 0));
        for (int i = 0; i < int(out.size()); i++)
            out[size_t(i)] = int(lroundf(float(m_scratch.getArrayData(i))));
        return true;
    }

    /// Read a text item (comma text is returned as is).
    /// @return false (out cleared) if the chunk or the item is missing.
    bool getText(const char* item, std::string& out)
    {
        out.clear();
        if (!read(item, ftTextGeneral)) return false;
        const char* p = m_scratch.getPChar();
        if (p != nullptr) out = p;
        return true;
    }

    /// Read the first value of a float item.
    /// @return The value, or fallback if the item is missing or empty.
    float getFloat(const char* item, float fallback = 0.0f)
    {
        if (!read(item, ftDataFloat) || m_scratch.getSize() < 1) return fallback;
        return float(m_scratch.getArrayData(0));
    }

    /// Read the first value of an integer item.
    /// @return The value, or fallback if the item is missing or empty.
    int getInt(const char* item, int fallback = 0)
    {
        if (!read(item, ftDataInteger) || m_scratch.getSize() < 1) return fallback;
        return int(lroundf(float(m_scratch.getArrayData(0))));
    }

private:
    AnsiCharPtr cname() const { return (AnsiCharPtr)m_name.c_str(); }

    // Read an item into m_scratch. The chunk pointer is fetched each time so a deleted/recreated chunk stays valid.
    bool read(const char* item, TFlowType flow)
    {
        // also false when the chunk itself is missing, so sdkGetChunk below never creates it
        if (!has(item)) return false;

        TUsineChunkPtr chunk = m_module->sdkGetChunk(cname());
        if (chunk == nullptr) return false;

        // allocated once: createEvent does not free a former event
        if (m_scratch.event() == nullptr) m_scratch.createEvent(nullptr, 1);

        m_module->sdkChunkOperation(chunk, (AnsiCharPtr)item, flow, m_scratch, coGetItem, -1);
        return true;
    }

    const UserModuleBase* m_module;
    std::string           m_name;
    UsineEventClass       m_scratch;
};

/// @}
