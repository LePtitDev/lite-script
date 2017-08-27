/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/*
    Copyright (C) 2017 LePtitDev
    All rights reserved.

    This software may be modified and distributed under the terms
    of the BSD license.  See the LICENSE file for details.

    Author: Arthur Ferré <leptitdev.com>
*/
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#ifndef LITESCRIPT_STREAMER_HPP
#define LITESCRIPT_STREAMER_HPP

#include <cstdint>
#include <iostream>

namespace LiteScript {

    // Class to simplify writing on little endian
    class OStreamer {

        ///////////////////////
        ////// ATTRIBUTE //////
        ///////////////////////

        // Output stream reference
        std::ostream& stream;

    public:

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor
         *
         * @param s Output stream reference
         */
        OStreamer(std::ostream& s) : stream(s) {}

        /**
         * Copy constructor
         *
         * @param s Output streamer
         */
        OStreamer(const OStreamer& s) : stream(s.stream) {}

        /////////////////////
        ////// METHODS //////
        /////////////////////

        ////// STATIC METHODS //////

        template <typename T>
        inline static void Write(std::ostream& stream, T v) { stream << v; }

        ////// NON STATIC METHOD //////

        // Left shift operator overloading
        template <typename T>
        OStreamer& operator<<(const T& v) {
            Write<T>(this->stream, v);
            return *this;
        }

    };

    template <>
    inline void OStreamer::Write(std::ostream& stream, unsigned char v) { stream << v; }
    template <>
    inline void OStreamer::Write(std::ostream& stream, char v) { stream << v; }
    template <>
    inline void OStreamer::Write(std::ostream& stream, unsigned short v) {
        stream << (uint8_t)(v & 0xff);
        stream << (uint8_t)((v >> 8) & 0xff);
    }
    template <>
    inline void OStreamer::Write(std::ostream& stream, short v) { Write<unsigned short>(stream, (unsigned short)v); }
    template <>
    inline void OStreamer::Write(std::ostream& stream, unsigned int v) {
        Write(stream, (unsigned short)(v & 0xffff));
        Write(stream, (unsigned short)((v >> 16) & 0xffff));
    }
    template <>
    inline void OStreamer::Write(std::ostream& stream, int v) { Write<unsigned int>(stream, (unsigned int)v); }
    template <>
    inline void OStreamer::Write(std::ostream& stream, float v) { Write<unsigned int>(stream, *(unsigned int *)&v); }

    // Class to simplify reading on little endian
    class IStreamer {

        ///////////////////////
        ////// ATTRIBUTE //////
        ///////////////////////

        // Output stream reference
        std::istream& stream;

    public:

        //////////////////////////
        ////// CONSTRUCTORS //////
        //////////////////////////

        /**
         * Basic constructor
         *
         * @param s Input stream reference
         */
        IStreamer(std::istream& s) : stream(s) {}

        /**
         * Copy constructor
         *
         * @param s Input streamer
         */
        IStreamer(const IStreamer& s) : stream(s.stream) {}

        /////////////////////
        ////// METHODS //////
        /////////////////////

        ////// STATIC METHODS //////

        template <typename T>
        inline static T Read(std::istream& stream) { return (T)stream.get(); }

        ////// NON STATIC METHOD //////

        // Right shift operator overloading
        template <typename T>
        IStreamer& operator>>(T& v) {
            v = Read<T>(this->stream);
            return *this;
        }

    };

    template <>
    inline unsigned char IStreamer::Read(std::istream& stream) { return (unsigned char)stream.get(); }
    template <>
    inline char IStreamer::Read(std::istream& stream) { return (char)stream.get(); }
    template <>
    inline unsigned short IStreamer::Read(std::istream& stream) {
        return (unsigned short)Read<unsigned char>(stream) | ((unsigned short)Read<unsigned char>(stream) << 8);
    }
    template <>
    inline short IStreamer::Read(std::istream& stream) { return (short)Read<unsigned short>(stream); }
    template <>
    inline unsigned int IStreamer::Read(std::istream& stream) {
        return (unsigned int)Read<unsigned short>(stream) | ((unsigned int)Read<unsigned short>(stream) << 16);
    }
    template <>
    inline int IStreamer::Read(std::istream& stream) { return (int)Read<unsigned int>(stream); }
    template <>
    inline float IStreamer::Read(std::istream& stream) {
        unsigned int r = Read<unsigned int>(stream);
        return *(float *)&r;
    }

}

#endif //LITESCRIPT_STREAMER_HPP
