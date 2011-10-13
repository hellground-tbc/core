#ifndef _PIPEWRAPPER_H
#define _PIPEWRAPPER_H

#include "Common.h"
#include "Util.h"
#include <ace/SPIPE_Stream.h>


class ByteBuffer;

namespace VMAP
{
    typedef ACE_Thread_Mutex LockType;
    typedef ACE_Guard<LockType> Guard;


    class PipeWrapper 
    {
    public:
        explicit PipeWrapper() : m_counter(0), m_bufferSize(0), m_eof(false), m_connected(false) {}
        virtual ByteBuffer RecvPacket();
        virtual void SendPacket(ByteBuffer &packet);
        bool Eof() { return m_eof; }

        void Connect(const char* name, int32 id = -1);
        void Accept(const char* name, int32 id = -1);

        bool IsConnected() { return m_connected; }

    protected:
        bool m_eof;
        bool m_connected;

    private:
        int32 m_counter;
        int32 m_bufferSize;
        uint8 m_buffer[100];

        bool recv(ByteBuffer &packet, uint32 size);

        ACE_SPIPE_Stream m_stream;
    };


    class SynchronizedPipeWrapper : public PipeWrapper
    {
    public:
        explicit SynchronizedPipeWrapper() {}

        ByteBuffer RecvPacket();
        void SendPacket(ByteBuffer &packet);

    private:
        LockType m_readLock;
        LockType m_sendLock;
    };

// temporary

    class Logger
    {
    public:
        static Logger& Instance() { return logger; }
        static Logger logger;
        void outError(const char *fmt, ...) 
        {
            UTF8PRINTF(stdout, fmt,);
            printf("\n");
        }
    };

}

#endif
