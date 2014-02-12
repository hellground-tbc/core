/*
 * Copyright (C) 2008-2014 Hellground <http://hellground.net/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef HELLGROUND_PIPEWRAPPER_H
#define HELLGROUND_PIPEWRAPPER_H

#include "Common.h"
#include "Util.h"

#include <ace/SPIPE_Stream.h>
#include <ace/FIFO_Send.h>
#include <ace/FIFO_Recv.h>

class ByteBuffer;

namespace VMAP
{
    typedef ACE_Thread_Mutex LockType;
    typedef ACE_Guard<LockType> Guard;

    template<class STREAM>
    class _PipeWrapper
    {
    public:
        explicit _PipeWrapper() : m_stream(0), m_connected(false) {}
        virtual ~_PipeWrapper();

        bool IsConnected() { return m_connected; }
        virtual void Close();

    protected:
        bool m_connected;
        STREAM *m_stream;
    };

    template<class STREAM>
    class _SendPipeWrapper : public _PipeWrapper<STREAM>
    {
    public:
        explicit _SendPipeWrapper() {}
        ~_SendPipeWrapper() {}

        virtual void SendPacket(ByteBuffer &packet);
        virtual void Connect(const char* name, int32 *id = 0);
    };

    template<class STREAM>
    class _RecvPipeWrapper : public _PipeWrapper<STREAM>
    {
    public:
        explicit _RecvPipeWrapper() : m_eof(false) {}
        ~_RecvPipeWrapper() {}

        virtual ByteBuffer RecvPacket();
        virtual void Accept(const char* name, int32 *id = 0);
        bool Eof() { return m_eof; }

    protected:
        bool m_eof;

    private:
        uint8 m_buffer[100];

        bool recv(ByteBuffer &packet, uint32 size);
    };

    template<class STREAM>
    class _SynchronizedSendPipeWrapper : public _SendPipeWrapper<STREAM>
    {
    public:
        explicit _SynchronizedSendPipeWrapper() {}
        ~_SynchronizedSendPipeWrapper() {}

        void SendPacket(ByteBuffer &packet);
        void Connect(const char* name, int32 *id = 0);

    private:
        LockType m_lock;
    };

    template<class STREAM>
    class _SynchronizedRecvPipeWrapper : public _RecvPipeWrapper<STREAM>
    {
    public:
        explicit _SynchronizedRecvPipeWrapper() {}
        ~_SynchronizedRecvPipeWrapper() {}

        ByteBuffer RecvPacket();
        void Accept(const char* name, int32 *id = 0);

    private:
        LockType m_lock;
    };

}

#include "PipeWrapperImpl.h"

namespace VMAP
{
#if (defined ACE_HAS_STREAM_PIPES) || (PLATFORM == PLATFORM_WINDOWS)
    #define USING_STREAM_PIPES
    typedef _RecvPipeWrapper<ACE_SPIPE_Stream> RecvPipeWrapper;
    typedef _SendPipeWrapper<ACE_SPIPE_Stream> SendPipeWrapper;
    typedef _SynchronizedSendPipeWrapper<ACE_SPIPE_Stream> SynchronizedSendPipeWrapper;
    typedef _SynchronizedRecvPipeWrapper<ACE_SPIPE_Stream> SynchronizedRecvPipeWrapper;
#else
    #define USING_FIFO_PIPES
    typedef _RecvPipeWrapper<ACE_FIFO_Recv> RecvPipeWrapper;
    typedef _SendPipeWrapper<ACE_FIFO_Send> SendPipeWrapper;
    typedef _SynchronizedSendPipeWrapper<ACE_FIFO_Send> SynchronizedSendPipeWrapper;
    typedef _SynchronizedRecvPipeWrapper<ACE_FIFO_Recv> SynchronizedRecvPipeWrapper;
#endif
}


#endif
