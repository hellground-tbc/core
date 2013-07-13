#ifndef _WARDEN_CHAT_H
#define _WARDEN_CHAT_H

class WardenBase;
class WorldSession;

class WardenChat : WardenBase
{
    public:
        WardenChat();
        ~WardenChat();

        void Init(WorldSession *pClient, BigNumber *K);
        ClientWardenModule *GetModuleForClient(WorldSession *session);
        void InitializeModule();
        void RequestHash();
        void HandleHashResult(ByteBuffer &buff);
        void RequestData();
        void HandleData(ByteBuffer &buff);
};
#endif