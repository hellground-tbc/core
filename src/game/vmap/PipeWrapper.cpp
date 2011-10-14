#include "PipeWrapper.h"

namespace VMAP
{
    Logger Logger::logger = Logger();


    template<>
    void _SendPipeWrapper<ACE_SPIPE_Stream>::Connect(const char* name, int32 *id)
    {
        if(m_connected)
            return;

        ACE_SPIPE_Addr addr;
        if (id)
        {
            char addr_buf[50];
            sprintf(addr_buf, "%s_%u", name, *id);
            addr.set(addr_buf);
        } 
        else
            addr.set(name);

        ACE_SPIPE_Connector connetor = ACE_SPIPE_Connector();


        m_stream = new ACE_SPIPE_Stream();

        // block until connected
        while(true)
        {
            if (connetor.connect(*m_stream, addr) == -1)
            {
                if(ACE_OS::last_error() != ERROR_CONNECT_NO_PIPE)
                {
                    sLog.outError("Connect: failed to connect to stream %s because of error %d", addr.get_path_name(), ACE_OS::last_error());
                    return;
                }
            }
            else
            {
                m_connected = true;
                return;
            }
            ACE_Thread::yield();
        }
    }

    template<>
    void _SendPipeWrapper<ACE_FIFO_Send>::Connect(const char* name, int32 *id)
    {
        if(m_connected)
            return;

        char addr_buf[50];
        if (id)
            sprintf(addr_buf, "%s_%u", name, *id);
        else
            sprintf(addr_buf, "%s", name);

        

        m_stream = new ACE_FIFO_Send();
        while(true)
        {

            if(m_stream->open(addr_buf) == -1)
            {
                if(ACE_OS::last_error() != ERROR_CONNECT_NO_PIPE)
                {
                    sLog.outError("Connect: failed to connect to stream %s because of error %d", addr_buf, ACE_OS::last_error());
                    delete m_stream;
                    m_stream = 0;
                }
                
            } 
            else
            {
                m_connected = true;
                return;
            }
            ACE_Thread::yield();
        }
    }

    template<>
    void _RecvPipeWrapper<ACE_SPIPE_Stream>::Accept(const char* name, int32 *id)
    {
        if(m_connected)
            return;

        ACE_SPIPE_Addr addr;
        if (id)
        {
            char addr_buf[50];
            sprintf(addr_buf, "%s_%u", name, *id);
            addr.set(addr_buf);
        } 
        else
            addr.set(name);

        m_stream = new ACE_SPIPE_Stream();

        ACE_SPIPE_Acceptor acceptor = ACE_SPIPE_Acceptor(addr);
        if(acceptor.accept(*m_stream) == -1)
        {
            sLog.outError("Accept: failed to accept on stream %s becaus of error %d", addr.get_path_name(), ACE_OS::last_error());
            delete m_stream;
            m_stream = 0;
        }
        else
            m_connected = true;
    }

    template<>
    void _RecvPipeWrapper<ACE_FIFO_Recv>::Accept(const char *name, int32 *id)
    {
        if(m_connected)
            return;

        char addr_buf[50];
        if (id)
            sprintf(addr_buf, "%s_%u", name, *id);
        else
            sprintf(addr_buf, "%s", name);

        m_stream = new ACE_FIFO_Recv();

        if(m_stream->open(addr_buf) == -1)
        {
            sLog.outError("Connect: failed to accept to stream %s because of error %d", addr_buf, ACE_OS::last_error());
            delete m_stream;
            m_stream = 0;
        }
        m_connected = true;

        printf("Accept stream name %s\n", addr_buf);
    }

}

