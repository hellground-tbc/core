#ifndef CHESS_EVENT
#define CHESS_EVENT

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

#define allianceSideDeadOrientation     1
#define hordeSideDeadOrientation        1
#define POSITION_Z                      221

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
    SPELL_FURY_OF_MEDIVH    = 39383,  // 2nd cheat: Berserk own chesspieces.
    // 3rd cheat: set own creatures to max health


    MOVE_NEAR_UNIT          = 30417,
    SNAPSHOT_TIMER          = 37440,
    FIND_ENEMY_ALLIANCE_M   = 32303,
    ACTION_TIMER            = 37504,
    GET_EMPTY_SQUARE        = 30418
};

enum AttackSpells
{
    ATTACK              = 6603,
    ATTACK_TIMER        = 32226,
    TAKE_ACTION         = 32225,

    //ally
    ELEMENTAL_ATTACK    = 37142,
    ELEMENTAL_ATTACK2   = 37143,
    FOOTMAN_ATTACK_DMG  = 32247,
    FOOTMAN_ATTACK      = 32227,
    CLERIC_ATTACK       = 37147,
    CONJURER_ATTACK     = 37149,
    KING_LLANE_ATTACK   = 37150,

    //horde
    GRUNT_ATTACK        = 32228,
    NECROLYTE_ATTACK    = 37337,
    WARLOCK_ATTACK      = 37345,
    WOLF_ATTACK         = 37339,
    DEMON_ATTACK        = 37220,
    WARCHIEF_ATTACK     = 37348
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

float hordeSideDeadWP[2][16] = {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},         //X coord
                                {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};        //Y coord

float allianceSideDeadWP[2][16] =  {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},     //Xcoord
                                    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};    //Ycoord


struct TRINITY_DLL_DECL move_triggerAI : public ScriptedAI
{
    move_triggerAI(Creature *c);

    ScriptedInstance* pInstance;

    WorldLocation wLoc;

    //Unit * onMarker;
    //bool EndMarker;

    int32 moveTimer;
    Unit * unitToMove;
    ChessPiecesStances pieceStance;

    void Reset();

    void Aggro(Unit *){}

    void SpellHit(Unit *caster,const SpellEntry *spell);

    void MakeMove();

    void UpdateAI(const uint32 diff);
};

struct TRINITY_DLL_DECL npc_chesspieceAI : public Scripted_NoMovementAI
{
    npc_chesspieceAI(Creature *c);

    ScriptedInstance* pInstance;

    Unit *npc_medivh;

    bool ReturnToHome;
    bool InGame;
    bool CanMove;

    //uint32 Heal_Timer;
    //uint32 NextMove_Timer;
    uint64 MedivhGUID;


    //Creature *start_marker, *end_marker;

    //std::list<Unit *> PossibleMoveUnits;

    //void MoveInLineOfSight(Unit *who);

    void Aggro(Unit *Unit);

    void EnterEvadeMode();

    void Reset();

    void MovementInform(uint32 MovementType, uint32 Data);

    void MoveInLineOfSight(Unit *unit);

    void JustRespawned();

    void OnCharmed(bool apply);

    void JustDied(Unit* killer){}

    void UpdateAI(const uint32 diff);

    void SpellHitTarget(Unit *target, const SpellEntry *spell);

    void DamageTaken(Unit * done_by, uint32 &damage);
};

struct TRINITY_DLL_DECL boss_MedivhAI : public ScriptedAI
{
    boss_MedivhAI(Creature *c);

    instance_karazhan* pInstance;

    int16 hordePieces;
    int16 alliancePieces;

    ChessSquare chessBoard[8][8];

    std::list<int64> medivhSidePieces; //alive pieces guids

    bool eventStarted;

    WorldLocation wLoc;
    WorldLocation tpLoc;

    std::list<uint64> tpList;

    void Reset();

    void Aggro(Unit *){}

    void SayChessPieceDied(Unit * piece);

    void RemoveChessPieceFromBoard(uint64 piece);

    void RemoveChessPieceFromBoard(Unit * piece);

    bool IsChessPiece(Unit * unit);

    bool IsMedivhsPiece(Unit * unit);

    void PrepareBoardForEvent();

    void TeleportPlayer(Player * player);

    void TeleportPlayer(uint64 player);

    void TeleportPlayers();

    void AddPlayerToTeleportList(Player * player);

    void TeleportListedPlayers();

    void ApplyDebuffsOnRaidMembers();

    void StartEvent();

    void UpdateAI(const uint32 diff);
};

#endif
