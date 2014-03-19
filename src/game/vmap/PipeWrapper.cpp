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

#include "PipeWrapper.h"
#include "Log.h"
#include <ace/OS_NS_unistd.h>

namespace VMAP
{
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
                    sLog.outLog(LOG_DEFAULT, "ERROR: Connect: failed to connect to stream %s because of error %d", addr.get_path_name(), ACE_OS::last_error());
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
                    sLog.outLog(LOG_DEFAULT, "ERROR: Connect: failed to connect to stream %s because of error %d", addr_buf, ACE_OS::last_error());
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
            sLog.outLog(LOG_DEFAULT, "ERROR: Accept: failed to accept on stream %s becaus of error %d", addr.get_path_name(), ACE_OS::last_error());
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
            sLog.outLog(LOG_DEFAULT, "ERROR: Connect: failed to accept to stream %s because of error %d", addr_buf, ACE_OS::last_error());
            delete m_stream;
            m_stream = 0;
        }
        m_connected = true;
    }

}

