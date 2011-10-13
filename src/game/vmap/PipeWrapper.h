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
        explicit PipeWrapper() : m_stream(0), m_connected(false) {}
        virtual ~PipeWrapper();

        bool IsConnected() { return m_connected; }

    protected:
        bool m_connected;
        ACE_SPIPE_Stream *m_stream;
    };


    class SendPipeWrapper : public PipeWrapper
    {
    public:
        explicit SendPipeWrapper() {}
        ~SendPipeWrapper() {}
        
        virtual void SendPacket(ByteBuffer &packet);
        virtual void Connect(const char* name, int32 id = -1);        
    };

    class RecvPipeWrapper : public PipeWrapper
    {
    public:
        explicit RecvPipeWrapper() : m_eof(false) {}
        ~RecvPipeWrapper() {}

        virtual ByteBuffer RecvPacket();
        virtual void Accept(const char* name, int32 id = -1);
        bool Eof() { return m_eof; }

    protected:
        bool m_eof;

    private:
        //int32 m_counter;
        //int32 m_bufferSize;
        uint8 m_buffer[100];
        

        bool recv(ByteBuffer &packet, uint32 size);
    };


    class SynchronizedSendPipeWrapper : public SendPipeWrapper
    {
    public:
        explicit SynchronizedSendPipeWrapper() {}
        ~SynchronizedSendPipeWrapper() {}

        void SendPacket(ByteBuffer &packet);
        void Connect(const char* name, int32 id = -1);        

    private:
        LockType m_lock;
    };

    class SynchronizedRecvPipeWrapper : public RecvPipeWrapper
    {
    public:
        explicit SynchronizedRecvPipeWrapper() {}
        ~SynchronizedRecvPipeWrapper() {}

        ByteBuffer RecvPacket();
        void Accept(const char* name, int32 id = -1);

    private:
        LockType m_lock;
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
