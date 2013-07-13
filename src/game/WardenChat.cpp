#include "WardenChat.h"
#include "WorldSession.h"

WardenChat::WardenChat()
{
}

WardenChat::~WardenChat()
{
}

void WardenChat::Init(WorldSession *pClient, BigNumber *K)
{
}

ClientWardenModule *WardenChat::GetModuleForClient(WorldSession *session)
{
    ClientWardenModule *mod = new ClientWardenModule;
    return mod;
}

void WardenChat::InitializeModule()
{
}

void WardenChat::RequestHash()
{
}

void WardenChat::HandleHashResult(ByteBuffer &buff)
{
}

void WardenChat::RequestData()
{
}

void WardenChat::HandleData(ByteBuffer &buff)
{
}