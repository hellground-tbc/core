/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Chess_Event
SD%Complete: xx
SDComment: Based on Hectolight script.
SDCategory: Karazhan

  I think locking and unlocking cells for unit movement is done. only need to add code to unlock it when creature dies :]

TODO:
 - Disable rotate for all chess pieces
 - Disable movement for unit controlled by player
 - Teleport players to the balkony when possesing creature
 - Improve finding creature in front and strafe ( maybe when rotate will be disabled it won't be needed :])
 - Update instance_karazhan for now it was done only to start it ;]
 - Implement Attack Creature when is in Front or Strafe(for every chesspieces ??) with proper factions.
 - Implement in instance_karazhan dead chess_piece count per side to allow Medivh to cheat when is loosing.(Or different terms was used as a treshold for cheat ?)
 - Prevent Reset() call when player unpossess creature.
 - Set proper position on left or right side from chess board for killed units.
 - and many more ...
EndScriptData */

#include "precompiled.h"
#include "def_karazhan.h"
#include "instance_karazhan.h"

#define A_FACTION  1690
#define H_FACTION  1691

#define GOSSIP_POSSES "[ Control this Chesspiece ]" // Maybe should be different gossip, visuals will be fixed at the end :]

#define ECHO_OF_MEDIVH_ID 16816

#define EVENT_START "[PH] Start Chess Event" // Maybe should be different gossip, visuals will be fixed at the end :]
#define NPC_ATTACK_RADIUS 7
#define AGGRO_RANGE 1
#define TRIGGER_ID 22519
#define SEARCH_RANGE 5
#define DUST_COVERED_CHEST 185119

enum SCRIPTTEXTs
{
    SCRIPTTEXT_AT_EVENT_START   =  -1650000,
    SCRIPTTEXT_LOSE_KNIGHT_P    =  -1650001,
    SCRIPTTEXT_LOSE_KNIGHT_M    =  -1650002,
    SCRIPTTEXT_LOSE_PAWN_P_1    =  -1650003,
    SCRIPTTEXT_LOSE_PAWN_P_2    =  -1650004,
    SCRIPTTEXT_LOSE_PAWN_P_3    =  -1650005,
    SCRIPTTEXT_LOSE_PAWN_M_1    =  -1650006,
    SCRIPTTEXT_LOSE_PAWN_M_2    =  -1650007,
    SCRIPTTEXT_LOSE_PAWN_M_3    =  -1650008,
    SCRIPTTEXT_LOSE_QUEEN_P     =  -1650009,
    SCRIPTTEXT_LOSE_QUEEN_M     =  -1650010,
    SCRIPTTEXT_LOSE_BISHOP_P    =  -1650011,
    SCRIPTTEXT_LOSE_BISHOP_M    =  -1650012,
    SCRIPTTEXT_LOSE_ROOK_P      =  -1650013,
    SCRIPTTEXT_LOSE_ROOK_M      =  -1650014,
    SCRIPTTEXT_PLAYER_CHECK     =  -1650015,
    SCRIPTTEXT_MEDIVH_CHECK     =  -1650016,
    SCRIPTTEXT_PLAYER_WIN       =  -1650017,
    SCRIPTTEXT_MEDIVH_WIN       =  -1650018,
    SCRIPTTEXT_MEDIVH_CHEAT_1   =  -1650019,
    SCRIPTTEXT_MEDIVH_CHEAT_2   =  -1650020,
    SCRIPTTEXT_MEDIVH_CHEAT_3   =  -1650021
};

enum NPCs
{
    NPC_MEDIVH   = 16816,
    NPC_PAWN_H   = 17469,
    NPC_PAWN_A   = 17211,
    NPC_KNIGHT_H = 21748,
    NPC_KNIGHT_A = 21664,
    NPC_QUEEN_H  = 21750,
    NPC_QUEEN_A  = 21683,
    NPC_BISHOP_H = 21747,
    NPC_BISHOP_A = 21682,
    NPC_ROOK_H   = 21726,
    NPC_ROOK_A   = 21160,
    NPC_KING_H   = 21752,
    NPC_KING_A   = 21684,
    NPC_STATUS   = 22520
};

enum ChessEventSpells
{
    BISHOP_HEAL_H  = 37456,
    BISHOP_HEAL_A  = 37455,
    SPELL_MOVE_1   = 37146,
    SPELL_MOVE_2   = 30012,
    SPELL_MOVE_3   = 37144,
    SPELL_MOVE_4   = 37148,
    SPELL_MOVE_5   = 37151,
    SPELL_MOVE_6   = 37152,
    SPELL_MOVE_7   = 37153,

    SPELL_MOVE_PREVISUAL = 32745,
    //SPELL_AI_FACE_NEARBY_ENEMY = 37787,

    SPELL_CHANGE_FACING     = 30284,
    SPELL_MOVE_MARKER       = 32261,
    SPELL_POSSES_CHESSPIECE = 30019,
    SPELL_IN_GAME           = 30532,
    SPELL_GAME_IN_SESSION   = 39331,
    SPELL_RECENTLY_IN_GAME  = 30529,
    SPELL_HAND_OF_MEDIVH    = 39339, // 1st cheat: AOE spell burn cell under enemy chesspieces.
    SPELL_FURY_OF_MEDIVH    = 39383  // 2nd cheat: Berserk own chesspieces.
    // 3rd cheat: set own creatures to max health
};

enum ChessPiecesStances
{
    PIECE_NONE          = 0,
    PIECE_MOVE          = 1,
    PIECE_CHANGE_FACING = 2
    //PIECE_DIE           = 3
};

struct ChessSquare
{
    uint64 piece;//GUID;
    uint64 trigger;//GUID;

    ChessSquare()
    {
        piece = 0;
        trigger = 0;
    }
};

struct TRINITY_DLL_DECL move_triggerAI : public ScriptedAI
{
    move_triggerAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)m_creature->GetInstanceData());
        m_creature->GetPosition(wLoc);
    }

    ScriptedInstance* pInstance;

    WorldLocation wLoc;

    Unit * onMarker;
    bool EndMarker;

    int32 moveTimer;
    Unit * unitToMove;
    ChessPiecesStances pieceStance;

    void Reset()
    {
        onMarker    = NULL;
        EndMarker   = false;
        moveTimer   = 10000;
        pieceStance = PIECE_NONE;
        unitToMove = NULL;
    }

    void Aggro(Unit *){}

    void SpellHit(Unit *caster,const SpellEntry *spell)
    {
        if(spell->Id == SPELL_CHANGE_FACING)
        {
            //caster->SetInFront(m_creature);
        }

        if(spell->Id == SPELL_MOVE_1 || spell->Id == SPELL_MOVE_2 || spell->Id == SPELL_MOVE_3 || spell->Id == SPELL_MOVE_4 ||
           spell->Id == SPELL_MOVE_5 || spell->Id == SPELL_MOVE_6 || spell->Id == SPELL_MOVE_7 || spell->Id == SPELL_CHANGE_FACING)
        {

            if(onMarker != NULL || EndMarker)
                return;

            EndMarker = true;
            onMarker = caster;

            if (pieceStance != PIECE_NONE)
                return;

            DoCast(m_creature, SPELL_MOVE_PREVISUAL);

            if (spell->Id == SPELL_CHANGE_FACING)
            {
                pieceStance = PIECE_CHANGE_FACING;
            }
            else
            {
                unitToMove = caster;
                pieceStance = PIECE_MOVE;
            }
        }
    }

    void MakeMove()
    {
        ChessPiecesStances tmpStance = pieceStance;

        moveTimer = 10000;
        pieceStance = PIECE_NONE;

        if (!unitToMove || (unitToMove && !unitToMove->isAlive()))
            return;

        switch (tmpStance)
        {
        case PIECE_MOVE:
            DoCast(m_creature,SPELL_MOVE_MARKER);
            unitToMove->StopMoving();
            unitToMove->GetMotionMaster()->Clear();
            unitToMove->GetMotionMaster()->MovePoint(0, wLoc.x, wLoc.y, wLoc.z);
            break;
        case PIECE_CHANGE_FACING:
            //unitToMove->SetInFront(m_creature);
            unitToMove->SetUInt64Value(UNIT_FIELD_TARGET, m_creature->GetGUID());
            break;
        default:
            break;
        }

        unitToMove = NULL;
    }

    void UpdateAI(const uint32 diff)
    {
        if(pInstance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS)
            return;

        if (pieceStance)
        {
            if (moveTimer < diff)
                MakeMove();
            else
                moveTimer -= diff;
        }

        if(onMarker)
        {
            if(onMarker->isAlive()) // Better check it here so we don't need to find start and end marker when chess die in move without reaching destination point :]
                return;
            else
                Reset();
        }
    }
};

struct TRINITY_DLL_DECL npc_chesspieceAI : public Scripted_NoMovementAI
{
    npc_chesspieceAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = ((ScriptedInstance*)m_creature->GetInstanceData());

        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_POSSES_CHESSPIECE);
        if(TempSpell)
            TempSpell->Effect[0] = 0;  // Disable bind sight effect from SPELL_POSSES_CHESSPIECE. We handle all with dummy and charm effect.
    }

    ScriptedInstance* pInstance;

    Unit *npc_medivh;

    bool ReturnToHome;
    bool InGame;
    bool CanMove;

    uint32 Heal_Timer;
    uint32 NextMove_Timer;
    uint64 MedivhGUID;


    Creature *start_marker, *end_marker;

    std::list<Unit *> PossibleMoveUnits;

    /*void MoveInLineOfSight(Unit *who)
    {
        if( !m_creature->getVictim() && who->isTargetableForAttack() )
        {
            if (!m_creature->canFly() && m_creature->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                return;

            if( m_creature->IsWithinDistInMap(who, AGGRO_RANGE) && m_creature->IsWithinLOSInMap(who) && InGame)
                AttackStart(who);
        }
    }*/

    void Aggro(Unit *Unit)
    {
        MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);
        npc_medivh = Unit::GetUnit(*m_creature, MedivhGUID);

        if(npc_medivh)
        {
            switch(pInstance->GetData(CHESS_EVENT_TEAM))
            {
                case ALLIANCE:
                    if(m_creature->GetEntry() == NPC_KING_H)
                        DoScriptText(SCRIPTTEXT_MEDIVH_CHECK,npc_medivh);
                    else if(m_creature->GetEntry() == NPC_KING_A)
                        DoScriptText(SCRIPTTEXT_PLAYER_CHECK,npc_medivh);
                break;
                case HORDE:
                    if(m_creature->GetEntry() == NPC_KING_A)
                        DoScriptText(SCRIPTTEXT_MEDIVH_CHECK,npc_medivh);
                    else if(m_creature->GetEntry() == NPC_KING_H)
                        DoScriptText(SCRIPTTEXT_PLAYER_CHECK,npc_medivh);
                break;
            }
        }
    }

    void Reset()
    {
        ReturnToHome = true;
        Heal_Timer = 7000;
        InGame = true;
        CanMove = false;
        NextMove_Timer = 5000; // wait 4.5s for first moves
        m_creature->setActive(true);

        start_marker = NULL;
        end_marker = NULL;
    }

    void MovementInform(uint32 MovementType, uint32 Data)
    {
        if(MovementType != POINT_MOTION_TYPE)
            return;

        if(start_marker)
        {
            ((move_triggerAI*)start_marker->AI())->Reset();
          //  DoSay("Start Cell - Clear!",LANG_UNIVERSAL,false); // only for debug purposes :]
        }
    }

    void MoveInLineOfSight(Unit *unit)
    {
        if (m_creature->isPossessed())
        {
            m_creature->StopMoving();

            return;
        }
    }

    void JustRespawned()
    {
        //not finally - just a presentation - need 32place two side of chesstable
        float angle = m_creature->GetOrientation();
        float pos_x = -11066;
        float pos_y = -1898;
        int move_lenght = 2*rand()%10;
        float new_x = pos_x + move_lenght * cos(angle);
        float new_y = pos_y + move_lenght * sin(angle);
        m_creature->Relocate(new_x,new_y,221,2.24);
        m_creature->CombatStop();
        m_creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
    }

    void OnCharmed(bool apply)
    {
        // Place to disable rotate and move for player on possess
    }

    void JustDied(Unit* killer)
    {
        MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);
        npc_medivh = Unit::GetUnit(*m_creature, MedivhGUID);

        if(npc_medivh && pInstance->GetData(CHESS_EVENT_TEAM) == HORDE)
        {
            switch(m_creature->GetEntry())
            {
                case NPC_ROOK_H:   DoScriptText(SCRIPTTEXT_LOSE_ROOK_P,npc_medivh);     break;
                case NPC_ROOK_A:   DoScriptText(SCRIPTTEXT_LOSE_ROOK_M,npc_medivh);     break;
                case NPC_QUEEN_H:  DoScriptText(SCRIPTTEXT_LOSE_QUEEN_P,npc_medivh);   break;
                case NPC_QUEEN_A:  DoScriptText(SCRIPTTEXT_LOSE_QUEEN_M,npc_medivh);   break;
                case NPC_BISHOP_H: DoScriptText(SCRIPTTEXT_LOSE_BISHOP_P,npc_medivh); break;
                case NPC_BISHOP_A: DoScriptText(SCRIPTTEXT_LOSE_BISHOP_M,npc_medivh); break;
                case NPC_KNIGHT_H: DoScriptText(SCRIPTTEXT_LOSE_KNIGHT_P,npc_medivh); break;
                case NPC_KNIGHT_A: DoScriptText(SCRIPTTEXT_LOSE_KNIGHT_M,npc_medivh); break;
                case NPC_PAWN_H:

                    switch(rand()%3)
                    {
                        case 0: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_1,npc_medivh);break;
                        case 1: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_2,npc_medivh);break;
                        case 2: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_3,npc_medivh);break;
                    }
                    break;

                case NPC_PAWN_A:

                    switch(rand()%2)
                    {
                        case 0: DoScriptText(SCRIPTTEXT_LOSE_PAWN_M_1,npc_medivh);break;
                        case 1: DoScriptText(SCRIPTTEXT_LOSE_PAWN_M_2,npc_medivh);break;
                    }
                    break;

                case NPC_KING_H:

                    DoScriptText(SCRIPTTEXT_MEDIVH_WIN,npc_medivh);
                    pInstance->SetData(DATA_CHESS_EVENT,FAIL);
                    break;

                case NPC_KING_A:

                    DoScriptText(SCRIPTTEXT_PLAYER_WIN,npc_medivh);
                    pInstance->SetData(DATA_CHESS_EVENT,DONE);
                    m_creature->SummonGameObject(DUST_COVERED_CHEST,-11058,-1903,221,2.24,0,0,0,0,7200000);
                    break;

                default: break;
            }
        }
        else if(npc_medivh)
        {
            switch(m_creature->GetEntry())
            {
                case NPC_ROOK_A:   DoScriptText(SCRIPTTEXT_LOSE_ROOK_P,npc_medivh);break;
                case NPC_ROOK_H:   DoScriptText(SCRIPTTEXT_LOSE_ROOK_M,npc_medivh);break;
                case NPC_QUEEN_A:  DoScriptText(SCRIPTTEXT_LOSE_QUEEN_P,npc_medivh);break;
                case NPC_QUEEN_H:  DoScriptText(SCRIPTTEXT_LOSE_QUEEN_M,npc_medivh);break;
                case NPC_BISHOP_A: DoScriptText(SCRIPTTEXT_LOSE_BISHOP_P,npc_medivh);break;
                case NPC_BISHOP_H: DoScriptText(SCRIPTTEXT_LOSE_BISHOP_M,npc_medivh);break;
                case NPC_KNIGHT_A: DoScriptText(SCRIPTTEXT_LOSE_KNIGHT_P,npc_medivh);break;
                case NPC_KNIGHT_H: DoScriptText(SCRIPTTEXT_LOSE_KNIGHT_M,npc_medivh);break;
                case NPC_PAWN_A:

                    switch(rand()%3)
                    {
                        case 0: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_1,npc_medivh);break;
                        case 1: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_2,npc_medivh);break;
                        case 2: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_3,npc_medivh);break;
                    }
                    break;

                case NPC_PAWN_H:

                    switch(rand()%2)
                    {
                        case 0: DoScriptText(SCRIPTTEXT_LOSE_PAWN_M_1,npc_medivh);break;
                        case 1: DoScriptText(SCRIPTTEXT_LOSE_PAWN_M_2,npc_medivh);break;
                    }
                    break;

                case NPC_KING_A:

                    DoScriptText(SCRIPTTEXT_MEDIVH_WIN,npc_medivh);
                    pInstance->SetData(DATA_CHESS_EVENT,FAIL);
                    break;

                case NPC_KING_H:

                    DoScriptText(SCRIPTTEXT_PLAYER_WIN,npc_medivh);
                    pInstance->SetData(DATA_CHESS_EVENT,DONE);
                    m_creature->SummonGameObject(DUST_COVERED_CHEST,-11058,-1903,221,2.24,0,0,0,0,7200000);
                    break;

                default: break;
            }
        }
        else
            DoSay("Medivh not found / bad data",LANG_UNIVERSAL,false); // Only for me for debug :]

        /*if(m_creature->isPossessed())
        {
            m_creature->RemoveAurasDueToSpell(SPELL_POSSES_CHESSPIECE);
            m_creature->RemoveCharmedOrPossessedBy(m_creature->GetCharmer());
        }*/

        InGame = false;
        m_creature->setActive(false);
        m_creature->Respawn();
    }

    bool CanGoTo(Unit* target)
    {
        if (!target)
            return false;

        if (m_creature->isPossessed())
        {
            if (((Player*)m_creature->GetCharmer())->GetTeam() == ALLIANCE)
            {

            }
            else
            {

            }
        }

        return true;
    }

    void UpdateAI(const uint32 diff)
    {
        if(pInstance->GetData(DATA_CHESS_EVENT) == DONE || pInstance->GetData(DATA_CHESS_EVENT) == FAIL)
        {
            if(m_creature->isInCombat())
                m_creature->CombatStop();

            if(m_creature->isPossessed())
                m_creature->RemoveCharmedOrPossessedBy(m_creature->GetCharmer());

            if(m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE))
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);

            if(ReturnToHome)
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveTargetedHome();
                ReturnToHome = false;
            }
        }

        if(pInstance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS)
            return;

        //if(!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_ROTATE))
        //    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_ROTATE); // that state disallow player to use spell's from pet bar. Need to change that and also need to disable move :]

        if(!ReturnToHome) // for fail
            ReturnToHome = true;

        if(!InGame)
            return;

        if(!m_creature->isPossessed())
        {
            /*if(!CanMove)
            {
                if(NextMove_Timer < diff)
                {
                    CanMove = true;

                    start_marker = end_marker;
                    end_marker = NULL;

                    NextMove_Timer = 10000;
                }else NextMove_Timer -= diff;
            }*/

            /*if(!m_creature->getVictim())
            {
                if(!CanMove)
                    return;

                PossibleMoveUnits = FindPossibleMoveUnits();

                if(PossibleMoveUnits.empty())
                    return;

                std::list<Unit*>::iterator i = PossibleMoveUnits.begin();
                advance ( i ,rand()%PossibleMoveUnits.size());

                end_marker = (Creature*)(*i);

                if(((move_triggerAI*)end_marker->AI())->onMarker != NULL && !((move_triggerAI*)end_marker->AI())->EndMarker)
                {
                    for(uint8 x = 0; x<PossibleMoveUnits.size();x++)
                    {
                        i = PossibleMoveUnits.begin();
                        advance ( i, x);

                        end_marker = (Creature*)(*i);

                        if(!((move_triggerAI*)end_marker->AI())->EndMarker)
                        {
                            DoCast((*i),SPELL_MOVE_1);
                            break;
                        }
                    }
                }
                else
                    DoCast((*i),SPELL_MOVE_1);

                CanMove = false;
                NextMove_Timer = 6000;
            }

            switch(m_creature->GetEntry())
            {
                case NPC_BISHOP_A:

                    if(Heal_Timer <= diff)
                    {
                        Unit *target = DoSelectLowestHpFriendly(25,5000); //dont select player controlled unit

                        if(target)
                            DoCast(target,BISHOP_HEAL_A);

                        Heal_Timer = 21000;
                    }else Heal_Timer -= diff;
                    break;

                case NPC_BISHOP_H:

                    if(Heal_Timer <= diff)
                    {
                        Unit *target = DoSelectLowestHpFriendly(25,5000); //dont select player controlled unit

                        if(target)
                            DoCast(target,BISHOP_HEAL_H);

                        Heal_Timer = 21000;
                    }else Heal_Timer -= diff;
                    break;

                default:        break;
            }
            if(m_creature->getVictim() || !m_creature->IsWithinDistInMap(m_creature->getVictim(), NPC_ATTACK_RADIUS))
            {
                m_creature->CombatStop();
                return;
            }*/
        }
        //DoMeleeAttackIfReady();
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if(target->GetEntry() != TRIGGER_ID || (spell->Id != SPELL_MOVE_1
           && spell->Id != SPELL_MOVE_2 && spell->Id != SPELL_MOVE_3 && spell->Id != SPELL_MOVE_4
           && spell->Id != SPELL_MOVE_5 && spell->Id != SPELL_MOVE_6 && spell->Id != SPELL_MOVE_7) )
            return;

        if(m_creature->isPossessed())
        {
            Creature *marker = (Creature*)target;
            if(marker && ((move_triggerAI*)marker->AI())->onMarker == m_creature)
            {
                start_marker = end_marker;
                end_marker = (Creature*)target;
            }
        }
    }
};

struct TRINITY_DLL_DECL boss_MedivhAI : public ScriptedAI
{
    boss_MedivhAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((instance_karazhan*)m_creature->GetInstanceData());
        m_creature->GetPosition(wLoc);
        tpLoc.x = -11108.2;
        tpLoc.y = -1841.56;
        tpLoc.z = 229.625;
        tpLoc.o = 5.39745;
    }

    instance_karazhan* pInstance;

    int16 hordePieces;
    int16 alliancePieces;

    ChessSquare chessBoard[8][8];

    bool eventStarted;

    WorldLocation wLoc;
    WorldLocation tpLoc;

    std::list<uint64> tpList;

    void Reset()
    {
        eventStarted = false;
        hordePieces = 16;
        alliancePieces = 16;
    }

    void Aggro(Unit *){}

    void RemoveChessPieceFromBoard(uint64 piece)
    {

    }

    void RemoveChessPieceFromBoard(Unit * piece)
    {

    }

    bool IsChessPiece(Unit * unit)
    {
        switch (unit->GetEntry())
        {
            case NPC_BISHOP_A:
            case NPC_BISHOP_H:
            case NPC_KING_A:
            case NPC_KING_H:
            case NPC_KNIGHT_A:
            case NPC_KNIGHT_H:
            case NPC_PAWN_A:
            case NPC_PAWN_H:
            case NPC_QUEEN_A:
            case NPC_QUEEN_H:
            case NPC_ROOK_A:
            case NPC_ROOK_H:
                return true;
            default:
                return false;
        }

        return false;
    }

    void PrepareBoardForEvent()
    {
        DoSay("zaczynam przygotowywanie planszy", LANG_UNIVERSAL, m_creature);
        printf("\nzaczynam przygotowywanie planszy\n");
        if (chessBoard[0][0].trigger != 0)
            return;

        Creature * current;
        Creature * next;
        int16 i, j;
        float posX, posY, posXc, posYc;
        std::list<Creature*> tmpList;

        printf("\nRozmiary: tmpList: %i, forChessList: %i\n", tmpList.size(), pInstance->forChessList.size());

        for (std::list<uint64>::iterator i = pInstance->forChessList.begin(); i != pInstance->forChessList.end(); i++)
        {
            tmpList.push_back(m_creature->GetMap()->GetCreature((*i)));
            //DoSay(".", LANG_UNIVERSAL, m_creature);
        }

        printf("\nRozmiary: tmpList: %i, forChessList: %i\n", tmpList.size(), pInstance->forChessList.size());

        //return;
        DoSay("szukanie pierwszego", LANG_UNIVERSAL, m_creature);
        printf("\nszukanie pierwszego\n");
        //add first trigger to list (from that point we will start searching and adding to chessBoard oder triggers)
        for (std::list<Creature*>::iterator itr = tmpList.begin(); itr != tmpList.end(); ++itr)
        {
            //printf("\n1\n");
            posX = (*itr)->GetPositionX();
            if (posX < -11050 && posX > -11056)
            {
                //printf("\n1.1\n");
                DoSay("znalazlem pierwszego", LANG_UNIVERSAL, m_creature);
                printf("\nznalazlem pierwszego\n");
                //printf("\n1.2\n");
                current = (*itr);
                posXc = current->GetPositionX();
                posYc = current->GetPositionY();
                //printf("\n1.3\n");
                if (current->GetEntry() == TRIGGER_ID)
                    chessBoard[0][0].trigger = current->GetGUID();
                else
                    if (IsChessPiece(current))
                        chessBoard[0][0].piece = current->GetGUID();
                //printf("\n1.4\n");
                current->CastSpell(current, SPELL_MOVE_MARKER, true);
                //tmpList.erase(itr);
                //printf("\n1.5\n");
            }
            //printf("\n2\n");
        }

        for (int i = 0; i < 7; i++)
        {

            if (i < 7 && chessBoard[i][0].trigger != 0)
                current = m_creature->GetCreature(*m_creature, chessBoard[i][0].trigger);

            if (!current)
            {
                DoSay("niema currenta 2", LANG_UNIVERSAL, m_creature);
                printf("\nniema currenta 2\n");
                continue;
            }

            for (std::list<Creature*>::iterator itr = tmpList.begin(); itr != tmpList.end(); ++itr)
            {
                //printf("\n3\n");
                next = (*itr);
                //printf("\n4\n");
                if (!next)
                {
                    DoSay("niema Next 1", LANG_UNIVERSAL, m_creature);
                    //printf("\nniema next 1\n");
                    continue;
                }
                //printf("\n5\n");
                if (((next->GetPositionX() < (current->GetPositionX() - 2)) && (next->GetPositionX() > (current->GetPositionX() - 4))) &&
                    ((next->GetPositionY() > (current->GetPositionY() + 3 )) && (next->GetPositionY() < (current->GetPositionY() + 6))))
                {
                    //printf("\n5.1\n");
                    if (next->GetEntry() == TRIGGER_ID)
                        chessBoard[i + 1][0].trigger = next->GetGUID();
                    else
                        if (IsChessPiece(current))
                            chessBoard[i + 1][0].piece = next->GetGUID();
                    next->CastSpell(next, SPELL_MOVE_MARKER, true);
                    printf("\nznalazlem z lewej: %i   n: %s\n", i, next->GetName());
                    //printf("\n5.2\n");
                    //tmpList.erase(itr);
                    //printf("\n5.3\n");
                }
               // printf("\n6\n");
            }
            current = NULL;
        }

        for (int i = 0; i < 8; i++)
        {
            //printf("\n7\n");
            if (chessBoard[i][0].trigger != 0)
                current = m_creature->GetCreature(*m_creature, chessBoard[i][0].trigger);

            if (!current)
            {
                DoSay("niema currenta 3", LANG_UNIVERSAL, m_creature);
                printf("\nniema currenta 3\n");
                continue;
            }
            for (int j = 1; j < 8; j++)
            {
                //printf("\n8\n");
                for (std::list<Creature*>::iterator itr = tmpList.begin(); itr != tmpList.end(); ++itr)
                {
                    //printf("\n9\n");
                    next = (*itr);
                    //printf("\n10\n");
                    if (!next)
                        continue;

                    if (((next->GetPositionX() < (current->GetPositionX() - 3)) && (next->GetPositionX() > (current->GetPositionX() - 6))) &&
                        ((next->GetPositionY() < (current->GetPositionY() - 2)) && (next->GetPositionY() > (current->GetPositionY() - 6))))
                    {
                        //printf("\n10.1\n");
                        if (next->GetEntry() == TRIGGER_ID)
                            chessBoard[i][j].trigger = next->GetGUID();
                        else
                            if (IsChessPiece(current))
                                chessBoard[i][j].piece = next->GetGUID();
                        next->CastSpell(next, SPELL_MOVE_MARKER, true);
                        printf("\nznalazlem z reszty: [%i][%i]\n", i, j);
                        //printf("\n10.2\n");
                        //tmpList.erase(itr);
                        //printf("\n10.3\n");
                    }
                    //printf("\n11\n");
                }
                //printf("\n12\n");
                current = NULL;
                if (j < 7 && chessBoard[i][j].trigger != 0)
                    current = m_creature->GetCreature(*m_creature, chessBoard[i][j].trigger);
                //printf("\n13\n");
                if (j < 7 && !current)
                {
                    DoSay("niema currenta 4", LANG_UNIVERSAL, m_creature);
                    printf("\nniema currenta 4\n");
                    break;
                }
                //printf("\n14\n");
            }
            current = NULL;
        }
    }

    void TeleportPlayer(Player * player)
    {
        if (player)
            player->TeleportTo(tpLoc);
    }
    void TeleportPlayer(uint64 player)
    {
        if(player)
        {
            if(Unit *tmpPlayer = Unit::GetUnit(*m_creature, player))
                if(tmpPlayer->GetTypeId() == TYPEID_PLAYER)
                    ((Player*)tmpPlayer)->TeleportTo(tpLoc);
        }
    }

    void TeleportPlayers()
    {
        Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
            if (itr->getSource() && itr->getSource()->HasAura(SPELL_POSSES_CHESSPIECE, 0))
                TeleportPlayer(itr->getSource());
    }

    void AddPlayerToTeleportList(Player * player)
    {
        if (player)
            tpList.push_back(player->GetGUID());
    }

    void TeleportListedPlayers()
    {
        for (std::list<uint64>::iterator itr = tpList.begin(); itr != tpList.end(); ++itr)
        {
            Unit* tmpPl = Unit::GetUnit(*m_creature, (*itr));
            if (tmpPl && tmpPl->GetTypeId() == TYPEID_PLAYER && tmpPl->HasAura(SPELL_POSSES_CHESSPIECE, 0))
            {
                TeleportPlayer((*itr));
                tpList.erase(itr);
            }
        }
    }

    void ApplyDebuffsOnRaidMembers()
    {
        DoSay("ApplyDebuffsOnRaidMembers() poczatek", LANG_UNIVERSAL, m_creature);
        Player * tmp;
        m_creature->CastSpell(m_creature, SPELL_GAME_IN_SESSION, true);
        Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
        {
            DoSay("ApplyDebuffsOnRaidMembers() for", LANG_UNIVERSAL, m_creature);
            tmp = itr->getSource();
            if (tmp && !tmp->isGameMaster() && tmp->isAlive())
            {
                DoSay("ApplyDebuffsOnRaidMembers() if poczatek", LANG_UNIVERSAL, m_creature);
                //tmp->CastSpell(tmp, SPELL_IN_GAME, true);
                //tmp->CastSpell(tmp, SPELL_GAME_IN_SESSION, true);
                m_creature->CastSpell(tmp, SPELL_GAME_IN_SESSION, true);
                tmp->AddAura(SPELL_GAME_IN_SESSION, tmp);
                //tmp->SetInCombatWith(m_creature);
                DoSay("ApplyDebuffsOnRaidMembers() if koniec", LANG_UNIVERSAL, m_creature);
            }
        }
        DoSay("ApplyDebuffsOnRaidMembers() koniec", LANG_UNIVERSAL, m_creature);
    }

    void StartEvent()
    {
        DoSay("StartEvent", LANG_UNIVERSAL, m_creature);
        PrepareBoardForEvent();
        TeleportPlayers();
        ApplyDebuffsOnRaidMembers();

        DoZoneInCombat();
        eventStarted = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(DATA_CHESS_EVENT) == IN_PROGRESS && !eventStarted)
            StartEvent();

        if (!eventStarted)
            return;
    }
};

bool GossipHello_npc_chesspiece(Player* player, Creature* _Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    //if(pInstance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS)
    //    return true;

    if(pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE && _Creature->getFaction() != A_FACTION)
        return true;

    if(pInstance->GetData(CHESS_EVENT_TEAM) == HORDE && _Creature->getFaction() != H_FACTION)
        return true;

    if(!_Creature->isPossessedByPlayer())
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_POSSES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(8990, _Creature->GetGUID());
    }

    return true;
}

bool GossipSelect_npc_chesspiece(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    if(action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CastSpell(_Creature, SPELL_POSSES_CHESSPIECE, false);
    }

    player->CLOSE_GOSSIP_MENU();

    return true;
}
bool GossipHello_npc_echo_of_medivh(Player* player, Creature* _Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    if(pInstance->GetData(DATA_CHESS_EVENT) == FAIL)
        pInstance->SetData(DATA_CHESS_EVENT,NOT_STARTED);

    if(pInstance->GetData(DATA_CHESS_EVENT) == DONE) //for testing
        pInstance->SetData(DATA_CHESS_EVENT,NOT_STARTED);

    if(pInstance->GetData(DATA_CHESS_EVENT) == IN_PROGRESS)
    {
        player->ADD_GOSSIP_ITEM(3, "Reset Event", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    }

    if(pInstance->GetData(DATA_CHESS_EVENT) == NOT_STARTED)
    {
        player->ADD_GOSSIP_ITEM(0, EVENT_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (player->isGameMaster())
            player->ADD_GOSSIP_ITEM(2, "Start Debug Mode", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    }

    player->SEND_GOSSIP_MENU(8990, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_echo_of_medivh(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    if(action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        DoScriptText(SCRIPTTEXT_AT_EVENT_START,_Creature);
        pInstance->SetData(DATA_CHESS_EVENT, IN_PROGRESS);
        pInstance->SetData(CHESS_EVENT_TEAM, player->GetTeam());
        _Creature->GetMotionMaster()->MoveRandom(10);
    }

    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pInstance->SetData(DATA_CHESS_EVENT, SPECIAL);
        ((ScriptedAI*)_Creature)->DoSay("Debug mode on", LANG_UNIVERSAL, _Creature);
    }

    player->CLOSE_GOSSIP_MENU();

    return true;
}

CreatureAI* GetAI_npc_Medivh(Creature * _Creature)
{
    return new boss_MedivhAI (_Creature);
}

CreatureAI* GetAI_npc_chesspiece(Creature *_Creature)
{
    return new npc_chesspieceAI (_Creature);
}

CreatureAI* GetAI_move_trigger(Creature *_Creature)
{
    return new move_triggerAI (_Creature);
}

void AddSC_chess_event()
{
    Script* newscript;

    newscript = new Script;
    newscript->GetAI = &GetAI_npc_chesspiece;
    newscript->Name = "npc_chesspiece";
    newscript->pGossipHello = GossipHello_npc_chesspiece;
    newscript->pGossipSelect = GossipSelect_npc_chesspiece;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_echo_of_medivh";
    newscript->GetAI = &GetAI_npc_Medivh;
    newscript->pGossipHello = GossipHello_npc_echo_of_medivh;
    newscript->pGossipSelect = GossipSelect_npc_echo_of_medivh;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "chess_move_trigger";
    newscript->GetAI = &GetAI_move_trigger;
    newscript->RegisterSelf();
}
