#ifndef _PIPEWRAPPER_H
#define _PIPEWRAPPER_H

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
        virtual void Connect(const char* name, int32 id = -1);        
    };

    template<class STREAM>
    class _RecvPipeWrapper : public _PipeWrapper<STREAM>
    {
    public:
        explicit _RecvPipeWrapper() : m_eof(false) {}
        ~_RecvPipeWrapper() {}

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

    template<class STREAM>
    class _SynchronizedSendPipeWrapper : public _SendPipeWrapper<STREAM>
    {
    public:
        explicit _SynchronizedSendPipeWrapper() {}
        ~_SynchronizedSendPipeWrapper() {}

        void SendPacket(ByteBuffer &packet);
        void Connect(const char* name, int32 id = -1);        

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
            printf("!!!!ERROR!!!!!\n");
            UTF8PRINTF(stdout, fmt,);
            printf("\n!!!!!!!\n");
        }
    };

}

#include "PipeWrapperImpl.h"

namespace VMAP
{
#ifdef afdsadf
    typedef _RecvPipeWrapper<ACE_SPIPE_Stream> RecvPipeWrapper;
    typedef _SendPipeWrapper<ACE_SPIPE_Stream> SendPipeWrapper;
    typedef _SynchronizedSendPipeWrapper<ACE_SPIPE_Stream> SynchronizedSendPipeWrapper;
    typedef _SynchronizedRecvPipeWrapper<ACE_SPIPE_Stream> SynchronizedRecvPipeWrapper;
#else
    typedef _RecvPipeWrapper<ACE_FIFO_Recv> RecvPipeWrapper;
    typedef _SendPipeWrapper<ACE_FIFO_Send> SendPipeWrapper;
    typedef _SynchronizedSendPipeWrapper<ACE_FIFO_Send> SynchronizedSendPipeWrapper;
    typedef _SynchronizedRecvPipeWrapper<ACE_FIFO_Recv> SynchronizedRecvPipeWrapper;
#endif
}


#endif
