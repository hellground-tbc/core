#include "precompiled.h"
#include "chess_event.h"


//trigger AI
move_triggerAI::move_triggerAI(Creature *c) : ScriptedAI(c)
{
    pInstance = ((ScriptedInstance*)m_creature->GetInstanceData());
    m_creature->GetPosition(wLoc);
}

void move_triggerAI::Reset()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie move_triggerAI::Reset()");
    #endif
    moveTimer   = 10000;
    pieceStance = PIECE_NONE;
    unitToMove = 0;
    MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);
}

void move_triggerAI::SpellHit(Unit *caster,const SpellEntry *spell)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SpellHit(Unit *caster = %i,const SpellEntry *spell = %i)", caster ? 1 : 0, spell ? 1 : 0);
    #endif
    if (!MedivhGUID)
        MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);
    #ifdef CHESS_DEBUG_INFO
    printf("\nmove_triggerAI:SpellHit(...): stance: %i, medivh: %u, id: %i\n", pieceStance, MedivhGUID, spell->Id);
    #endif
    if (pieceStance != PIECE_NONE || !MedivhGUID)
        return;

    if(spell->Id == SPELL_MOVE_1 || spell->Id == SPELL_MOVE_2 || spell->Id == SPELL_MOVE_3 || spell->Id == SPELL_MOVE_4 ||
       spell->Id == SPELL_MOVE_5 || spell->Id == SPELL_MOVE_6 || spell->Id == SPELL_MOVE_7 || spell->Id == SPELL_CHANGE_FACING)
    {
        boss_MedivhAI * medivh = (boss_MedivhAI*)(m_creature->GetCreature(MedivhGUID)->AI());
        if (medivh)
        {
            if (spell->Id == SPELL_CHANGE_FACING || medivh->CanMoveTo(m_creature->GetGUID(), caster->GetGUID()))
            {
                medivh->AddTriggerToMove(m_creature->GetGUID(), caster->GetGUID(), caster->GetCharmerOrOwnerPlayerOrPlayerItself() ? true : false);

                me->CastSpell(m_creature, SPELL_MOVE_PREVISUAL, false);

                unitToMove = caster->GetGUID();

                if (spell->Id == SPELL_CHANGE_FACING)
                    pieceStance = PIECE_CHANGE_FACING;
                else
                    pieceStance = PIECE_MOVE;
            }
            else
                m_creature->Say("You can't move to me", LANG_UNIVERSAL, caster->GetGUID());
        }
        else
            me->Say("Medivh not found ... you can't move", LANG_UNIVERSAL, NULL);
    }
}

void move_triggerAI::MakeMove()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie MakeMove()");
    #endif
    ChessPiecesStances tmpStance = pieceStance;

    moveTimer = 10000;
    pieceStance = PIECE_NONE;
    Creature * temp = m_creature->GetCreature(unitToMove);
    Creature * temp2 = m_creature->GetCreature(MedivhGUID);

    if (!temp || !temp->isAlive())
    {
        unitToMove = 0;
        return;
    }

    switch (tmpStance)
    {
        case PIECE_MOVE:
            me->CastSpell(m_creature, SPELL_MOVE_MARKER, false);
            temp->StopMoving();
            temp->GetMotionMaster()->Clear();
            temp->GetMotionMaster()->MovePoint(0, wLoc.coord_x, wLoc.coord_y, wLoc.coord_z);

            if (temp2)
                ((boss_MedivhAI*)temp2->AI())->ChangePlaceInBoard(unitToMove, me->GetGUID());
            break;
        case PIECE_CHANGE_FACING:
            if (temp2)
                ((boss_MedivhAI*)temp2->AI())->ChangePieceFacing(temp, me);
            break;
        default:
            break;
    }

    unitToMove = 0;
}

void move_triggerAI::UpdateAI(const uint32 diff)
{
    //to prevent situation when in some case trigger isn't in move list and never move

    if(pInstance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS)
        return;

    if (pieceStance)
    {
        if (moveTimer < diff)
            MakeMove();
        else
            moveTimer -= diff;
    }
}

//Chesspieces AI

npc_chesspieceAI::npc_chesspieceAI(Creature *c) : Scripted_NoMovementAI(c)
{
    pInstance = ((ScriptedInstance*)m_creature->GetInstanceData());

    SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_POSSES_CHESSPIECE);
    if(TempSpell)
        TempSpell->Effect[0] = 0;  // Disable bind sight effect from SPELL_POSSES_CHESSPIECE. We handle all with dummy and charm effect.
    m_creature->setActive(true);
}

void npc_chesspieceAI::EnterEvadeMode()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie EnterEvadeMode()");
    #endif
    // to prevent reset chess pieces after unpossess
    //if (pInstance->GetData(DATA_CHESS_EVENT) == IN_PROGRESS ||
    //    pInstance->GetData(DATA_CHESS_EVENT) == SPECIAL)
        return;
}

void npc_chesspieceAI::SetSpellsAndCooldowns()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SetSpellsAndCooldowns()");
    #endif
    switch(m_creature->GetEntry())
    {
        case NPC_KING_A:
            ability1ID = SPELL_KING_A_1;
            ability1Timer = CD_KING_1;
            ability1Cooldown = CD_KING_1;

            ability2ID = SPELL_KING_A_2;
            ability2Timer = CD_KING_2;
            ability2Cooldown = CD_KING_2;

            attackDamage = KING_LLANE_ATTACK;
            break;

        case NPC_KING_H:
            ability1ID = SPELL_KING_H_1 ;
            ability1Timer = CD_KING_1;
            ability1Cooldown = CD_KING_1;

            ability2ID = SPELL_KING_H_2;
            ability2Timer = CD_KING_2;
            ability2Cooldown = CD_KING_2;

            attackDamage = WARCHIEF_ATTACK;
            break;

        case NPC_QUEEN_A:
            ability1ID = SPELL_QUEEN_A_1 ;
            ability1Timer = CD_QUEEN_1;
            ability1Cooldown = CD_QUEEN_1;

            ability2ID = SPELL_QUEEN_A_2;
            ability2Timer = CD_QUEEN_2;
            ability2Cooldown = CD_QUEEN_2;

            attackDamage = CONJURER_ATTACK;
            break;

        case NPC_QUEEN_H:
            ability1ID = SPELL_QUEEN_H_1 ;
            ability1Timer = CD_QUEEN_1;
            ability1Cooldown = CD_QUEEN_1;

            ability2ID = SPELL_QUEEN_H_2;
            ability2Timer = CD_QUEEN_2;
            ability2Cooldown = CD_QUEEN_2;

            attackDamage = WARLOCK_ATTACK;
            break;

        case NPC_BISHOP_A:
            ability1ID = SPELL_BISHOP_A_1 ;
            ability1Timer = CD_BISHOP_1;
            ability1Cooldown = CD_BISHOP_1;

            ability2ID = SPELL_BISHOP_A_2;
            ability2Timer = CD_BISHOP_2;
            ability2Cooldown = CD_BISHOP_2;

            attackDamage = CLERIC_ATTACK;
            break;

        case NPC_BISHOP_H:
            ability1ID = SPELL_BISHOP_H_1 ;
            ability1Timer = CD_BISHOP_1;
            ability1Cooldown = CD_BISHOP_1;

            ability2ID = SPELL_BISHOP_H_2;
            ability2Timer = CD_BISHOP_2;
            ability2Cooldown = CD_BISHOP_2;

            attackDamage = NECROLYTE_ATTACK;
            break;

        case NPC_KNIGHT_A:
            ability1ID = SPELL_KNIGHT_A_1 ;
            ability1Timer = CD_KNIGHT_1;
            ability1Cooldown = CD_KNIGHT_1;

            ability2ID = SPELL_KNIGHT_A_2;
            ability2Timer = CD_KNIGHT_2;
            ability2Cooldown = CD_KNIGHT_2;

            attackDamage = ELEMENTAL_ATTACK2;  //?
            break;

        case NPC_KNIGHT_H:
            ability1ID = SPELL_KNIGHT_H_1 ;
            ability1Timer = CD_KNIGHT_1;
            ability1Cooldown = CD_KNIGHT_1;

            ability2ID = SPELL_KNIGHT_H_2;
            ability2Timer = CD_KNIGHT_2;
            ability2Cooldown = CD_KNIGHT_2;

            attackDamage = WOLF_ATTACK;
            break;

        case NPC_ROOK_A:
            ability1ID = SPELL_ROOK_A_1 ;
            ability1Timer = CD_ROOK_1;
            ability1Cooldown = CD_ROOK_1;

            ability2ID = SPELL_ROOK_A_2;
            ability2Timer = CD_ROOK_2;
            ability2Cooldown = CD_ROOK_2;

            attackDamage = ELEMENTAL_ATTACK;
            break;

        case NPC_ROOK_H:
            ability1ID = SPELL_ROOK_H_1 ;
            ability1Timer = CD_ROOK_1;
            ability1Cooldown = CD_ROOK_1;

            ability2ID = SPELL_ROOK_H_2;
            ability2Timer = CD_ROOK_2;
            ability2Cooldown = CD_ROOK_2;

            attackDamage = DEMON_ATTACK;
            break;

        case NPC_PAWN_A:
            ability1ID = SPELL_PAWN_A_1 ;
            ability1Timer = CD_PAWN_1;
            ability1Cooldown = CD_PAWN_1;

            ability2ID = SPELL_PAWN_H_2;
            ability2Timer = CD_PAWN_2;
            ability2Cooldown = CD_PAWN_2;

            attackDamage = FOOTMAN_ATTACK;
            break;

        case NPC_PAWN_H:
            ability1ID = SPELL_PAWN_H_1 ;
            ability1Timer = CD_PAWN_1;
            ability1Cooldown = CD_PAWN_1;

            ability2ID = SPELL_PAWN_H_2;
            ability2Timer = CD_PAWN_2;
            ability2Cooldown = CD_PAWN_2;

            attackDamage = GRUNT_ATTACK;
            break;
        default:
            break;
    }

    attackTimer = attackCooldown;
}

void npc_chesspieceAI::Reset()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie npc_chesspieceAI::Reset()");
    #endif
    ReturnToHome = true;
    InGame = true;
    CanMove = false;
    m_creature->setActive(true);

    SetSpellsAndCooldowns();

    MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);

    ability1Chance = urand(ABILITY_1_CHANCE_MIN, ABILITY_1_CHANCE_MAX);
    ability2Chance = urand(ABILITY_2_CHANCE_MIN, ABILITY_2_CHANCE_MAX);

    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

    nextTryTimer = urand(500, 5000);
}

void npc_chesspieceAI::MovementInform(uint32 MovementType, uint32 Data)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie MovementInform(uint32 MovementType = %u, uint32 Data = %u)", MovementType, Data);
    #endif
    if (MovementType != POINT_MOTION_TYPE)
        return;

    npc_medivh = m_creature->GetCreature(MedivhGUID);
    if (npc_medivh)
        ((boss_MedivhAI*)npc_medivh->AI())->SetOrientation(m_creature->GetGUID());
}

void npc_chesspieceAI::MoveInLineOfSight(Unit *unit)
{
    if (m_creature->isPossessed())
    {
        m_creature->StopMoving();

        return;
    }
}

void npc_chesspieceAI::JustRespawned()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie JustRespawned()");
    #endif

    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
}

void npc_chesspieceAI::OnCharmed(bool apply)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie OnCharmed(bool apply = %i)", apply);
    #endif
    // Place to disable rotate and move for player on possess
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
}

void npc_chesspieceAI::SpellHit(Unit * caster, const SpellEntry * spell)
{
    if (spell->Id == SPELL_MOVE_MARKER)
    {
        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        ((Creature*)me)->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
}

void npc_chesspieceAI::UpdateAI(const uint32 diff)
{
    if(pInstance->GetData(DATA_CHESS_EVENT) == DONE || pInstance->GetData(DATA_CHESS_EVENT) == FAIL)
    {
        if(m_creature->isInCombat())
            m_creature->CombatStop();

        if(m_creature->isPossessed())
            m_creature->RemoveCharmedOrPossessedBy(m_creature->GetCharmer());

        if(m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    if(pInstance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS)
        return;

    if(!InGame)
        return;

    #ifdef DISSABLE_MEDIVH_PIECES_SPELLS
    return;
    #endif

    if((me->getFaction() == A_FACTION && pInstance->GetData(CHESS_EVENT_TEAM) == HORDE) ||
       (me->getFaction() == H_FACTION && pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE))
    {
        bool ab1 = false, ab2 = false;

        if (ability1Timer < diff)
        {
            if (urand(0, ABILITY_CHANCE_MAX) > ability1Chance)
                ab1 = true;
            else
                ability1Timer = nextTryTimer;
        }
        else
            ability1Timer -= diff;

        if (ability2Timer < diff)
        {
            if (urand(0, ABILITY_CHANCE_MAX) > ability2Chance)
                ab2 = true;
            else
                ability2Timer = nextTryTimer;
        }
        else
            ability2Timer -= diff;

        if (ab1 && ab2)
        {
            if (urand(0, RAND_MAX_VAL) < RAND_MAX_VAL/2)
                ab2 = false;
            else
                ab1 = false;
        }

        if (ab1)
        {
            Creature * medivh = m_creature->GetCreature(MedivhGUID);
            if (!medivh)
                return;

            uint64 victim = ((boss_MedivhAI*)medivh->AI())->GetSpellTarget(m_creature->GetGUID(), ability1ID);

            if (victim)
            {
                Unit * uVictim = m_creature->GetUnit(*m_creature, victim);
                AddSpellToCast(uVictim, ability1ID);
            }

            ability1Timer = ability1Cooldown;
            if (ability2Timer < SHARED_COOLDOWN)
                ability2Timer = SHARED_COOLDOWN;
        }
        else if (ab2)
        {
            Creature * medivh = m_creature->GetCreature(MedivhGUID);
            if (!medivh)
                return;

            uint64 victim = ((boss_MedivhAI*)medivh->AI())->GetSpellTarget(m_creature->GetGUID(), ability2ID);
            if (victim)
            {
                Unit * uVictim = m_creature->GetUnit(*m_creature, victim);
                AddSpellToCast(uVictim, ability2ID);
            }

            ability2Timer = ability2Cooldown;
            if (ability1Timer < SHARED_COOLDOWN)
                ability1Timer = SHARED_COOLDOWN;
        }
    }

    if (attackTimer < diff)
    {
        attackTimer = attackCooldown;
        Creature * medivh = m_creature->GetCreature(MedivhGUID);

        if (!medivh)
            return;

        Unit * uVictim = m_creature->GetUnit(((boss_MedivhAI*)medivh->AI())->GetMeleeTarget(m_creature->GetGUID()));
        if (uVictim)
            m_creature->DealDamage(uVictim, attackDamage, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }
    else
        attackTimer -= diff;

    CastNextSpellIfAnyAndReady();
}

void npc_chesspieceAI::JustDied(Unit * killer)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie JustDied(Unit * killer = %i)", killer ? 1 : 0);
    #endif

    Player * tmpP = me->GetCharmerOrOwnerPlayerOrPlayerItself();
    if (tmpP)
        tmpP->RemoveAurasDueToSpell(SPELL_POSSES_CHESSPIECE);

    m_creature->RemoveAurasDueToSpell(SPELL_POSSES_CHESSPIECE);

    if (pInstance->GetData(DATA_CHESS_EVENT) == IN_PROGRESS)
    {
        npc_medivh = m_creature->GetCreature(MedivhGUID);
        if (npc_medivh)
            ((boss_MedivhAI*)npc_medivh->AI())->RemoveChessPieceFromBoard(m_creature);
        else
            me->Say("I'm dying ... but Medivh not found ...", LANG_UNIVERSAL, NULL);
    }
}

//Medivh AI

boss_MedivhAI::boss_MedivhAI(Creature *c) : ScriptedAI(c)
{
    pInstance = ((instance_karazhan*)m_creature->GetInstanceData());
    m_creature->GetPosition(wLoc);
    tpLoc.coord_x = -11108.2;
    tpLoc.coord_y = -1841.56;
    tpLoc.coord_z = 229.625;
    tpLoc.orientation = 5.39745;

    this->chanceToMove = urand(MIN_MOVE_CHANCE, MAX_MOVE_CHANCE);

    chanceToSelfMove = urand(MIN_SELF_MOVE_CHANCE, MAX_SELF_MOVE_CHANCE);

    /*
          A  B  C  D  E  F  G  H
        0 H  H  H  H  H  H  H  H
        1 H  H  H  H  H  H  H  H
        2 E  E  E  E  E  E  E  E
        3 E  E  E  E  E  E  E  E
        4 E  E  E  E  E  E  E  E
        5 E  E  E  E  E  E  E  E
        6 A  A  A  A  A  A  A  A
        7 A  A  A  A  A  A  A  A
    */

    // calc positions:

    for (uint8 i = 0; i < 8; ++i)
    {
        for (uint8 j = 0; j < 8; ++j)
        {
            chessBoard[i][j].position.coord_x = -11077.66 + 3.48 * j - 4.32 * i ;
            chessBoard[i][j].position.coord_y = -1849.02 - 4.365 * j - 3.41 * i;
            chessBoard[i][j].position.coord_z = 221.1;
            chessBoard[i][j].position.mapid = me->GetMapId();
        }
    }
    int j = 15;
    for (int i = 0; i < 16; ++i)
    {
        allianceSideDeadWP[0][i] = (i < 8 ? ALLIANCE_DEAD_X2 : ALLIANCE_DEAD_X1) - 2.2 * 0.75 * (j < 8 ? j : j - 8);
        allianceSideDeadWP[1][i] = (i < 8 ? ALLIANCE_DEAD_Y2 : ALLIANCE_DEAD_Y1) - 1.7 * 0.75 * (j < 8 ? j : j - 8);
        hordeSideDeadWP[0][i] = (i < 8 ? HORDE_DEAD_X2 : HORDE_DEAD_X1) + 2.2 * 0.75 * (j < 8 ? j : j - 8);
        hordeSideDeadWP[1][i] = (i < 8 ? HORDE_DEAD_Y2 : HORDE_DEAD_Y1) + 1.7 * 0.75 * (j < 8 ? j : j - 8);
        j--;
    }
}

int boss_MedivhAI::GetMoveRange(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetMoveRange(uint64 piece = %u)", piece);
    #endif
    return (GetMoveRange(m_creature->GetUnit(*m_creature, piece)));
}

int boss_MedivhAI::GetMoveRange(Unit * piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetMoveRange(Unit * piece = %i)", piece ? 1 : 0);
    #endif
    if (!piece)
        return 0;

    switch (piece->GetEntry())
    {
        case NPC_PAWN_A:
        case NPC_PAWN_H:
        case NPC_KING_A:
        case NPC_KING_H:
        case NPC_BISHOP_A:
        case NPC_BISHOP_H:
        case NPC_ROOK_A:
        case NPC_ROOK_H:
            return 8;

        case NPC_KNIGHT_A:
        case NPC_KNIGHT_H:
            return 15;

        case NPC_QUEEN_A:
        case NPC_QUEEN_H:
            return 20;

        default:
            break;
    }

    return 0;
}

bool boss_MedivhAI::Enemy(uint64 piece1, uint64 piece2)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie Enemy(uint64 piece1 = %u, uint64 piece2 = %u)", piece1, piece2);
    #endif
    Creature * tmp1, * tmp2;

    if (!piece1 || !piece2)
        return false;

    tmp1 = me->GetCreature(piece1);
    tmp2 = me->GetCreature(piece2);

    if (!tmp1 || !tmp2)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("piece1: %i, %i  | piece2: %i, %i", piece1, tmp1 ? 1 : 0, piece2 , tmp2 ? 1 : 0);
        #endif
        me->Say("Cosik pionkow 2 nie znalazlem do sprawdzenia", LANG_UNIVERSAL, NULL);
        return false;
    }

    return tmp1->getFaction() != tmp2->getFaction();
}

int boss_MedivhAI::GetCountOfEnemyInMelee(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetCountOfEnemyInMelee(uint64 piece = %u)", piece);
    #endif

    int tmpCount = 0, tmpI = -1, tmpJ = -1;

    //search for position in tab of piece

    if (!FindPlaceInBoard(piece, tmpI, tmpJ))
        return 0;

    if (tmpI+1 < 8)
        if (Enemy(piece, chessBoard[tmpI+1][tmpJ].piece))
            tmpCount++;

    if (tmpI-1 >= 0)
        if (Enemy(piece, chessBoard[tmpI-1][tmpJ].piece))
            tmpCount++;

    if (tmpJ+1 < 8)
        if (Enemy(piece, chessBoard[tmpI][tmpJ+1].piece))
            tmpCount++;

    if (tmpJ-1 >= 0)
        if (Enemy(piece, chessBoard[tmpI][tmpJ-1].piece))
            tmpCount++;

    return tmpCount;
}

int boss_MedivhAI::GetLifePriority(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetLifePriority(uint64 piece = %u)", piece);
    #endif

    Unit * uPiece = m_creature->GetUnit(*m_creature, piece);

    if (!uPiece)
        return 0;

    int tmpPriority = 0;

    switch (uPiece->GetEntry())
    {
        case NPC_PAWN_A:
        case NPC_PAWN_H:
            tmpPriority += 5;
            break;
        case NPC_KING_A:
        case NPC_KING_H:
            tmpPriority += 15;
            break;
        case NPC_BISHOP_A:
        case NPC_BISHOP_H:
            tmpPriority += 25;
            break;
        case NPC_ROOK_A:
        case NPC_ROOK_H:
            tmpPriority += 10;
            break;
        case NPC_KNIGHT_A:
        case NPC_KNIGHT_H:
            tmpPriority += 15;
            break;
        case NPC_QUEEN_A:
        case NPC_QUEEN_H:
            tmpPriority += 20;
            break;
        default:
            break;
    }

    tmpPriority *= (1- (uPiece->GetHealth()/uPiece->GetMaxHealth()));

    return tmpPriority;
}

int boss_MedivhAI::GetAttackPriority(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetAttackPriority(uint64 piece = %u)", piece);
    #endif

    Unit * uPiece = m_creature->GetUnit(*m_creature, piece);

    if (!uPiece)
        return 0;

    int tmpPriority = 0;

    switch (uPiece->GetEntry())
    {
        case NPC_PAWN_A:
        case NPC_PAWN_H:
            tmpPriority += 5;
            break;
        case NPC_KING_A:
        case NPC_KING_H:
            tmpPriority += 15;
            break;
        case NPC_BISHOP_A:
        case NPC_BISHOP_H:
            tmpPriority += 15;
            break;
        case NPC_ROOK_A:
        case NPC_ROOK_H:
            tmpPriority += 5;
            break;
        case NPC_KNIGHT_A:
        case NPC_KNIGHT_H:
            tmpPriority += 5;
            break;
        case NPC_QUEEN_A:
        case NPC_QUEEN_H:
            tmpPriority += 10;
            break;
        default:
            break;
    }

    tmpPriority *= (1- (uPiece->GetHealth()/uPiece->GetMaxHealth()));

    return tmpPriority;
}

bool boss_MedivhAI::IsEmptySquareInRange(uint64 piece, int range)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsEmptySquareInRange(uint64 piece = %u, int range = %i)", piece, range);
    #endif

    if (!piece || !range)
        return false;

    int tmpI = -1, tmpJ = -1, tmpOffsetI, tmpOffsetJ;
    int i;

    //search for position in tab of piece

    if (!FindPlaceInBoard(piece, tmpI, tmpJ))
    {
        Creature * uPiece = m_creature->GetCreature(piece);

        if (uPiece)
            uPiece->Say("IsEmptySquareInRange(..) : Nie znaleziono mnie na planszy !!", LANG_UNIVERSAL, NULL);
        else
            me->Say("IsEmptySquareInRange(..) : Nie znalazlem pionka !!", LANG_UNIVERSAL, NULL);
        return false;
    }

    switch (range)
    {
        case 25:
            for (i = 0; i < OFFSET25COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab25[i][0];
                tmpOffsetJ = tmpJ + offsetTab25[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    if (!chessBoard[tmpOffsetI][tmpOffsetJ].piece)
                        return true;
            }
        case 20:
            for (i = 0; i < OFFSET20COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab20[i][0];
                tmpOffsetJ = tmpJ + offsetTab20[i][1];
                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    if (!chessBoard[tmpOffsetI][tmpOffsetJ].piece)
                        return true;
            }
        case 15:
            for (i = 0; i < OFFSET15COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab15[i][0];
                tmpOffsetJ = tmpJ + offsetTab15[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    if (!chessBoard[tmpOffsetI][tmpOffsetJ].piece)
                        return true;
            }
        case 8:
            for (i = 0; i < OFFSET8COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab8[i][0];
                tmpOffsetJ = tmpJ + offsetTab8[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    if (!chessBoard[tmpOffsetI][tmpOffsetJ].piece)
                        return true;
            }
            break;
        default:
            break;
    }

    return false;
}
bool boss_MedivhAI::IsOnSelfSpell(uint32 spell)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsOnSelfSpell(uint32 spell = %u)", spell);
    #endif

    switch (spell)
    {
        case SPELL_KING_H_2:
        case SPELL_KING_A_2:
        case SPELL_ROOK_H_2:
        case SPELL_ROOK_A_2:
        case SPELL_PAWN_H_2:
        case SPELL_PAWN_A_2:
            return true;

        default:
            return false;
    }

    return false;
}

bool boss_MedivhAI::IsPositive(uint32 spell)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsPositive(uint32 spell = %u)", spell);
    #endif

    switch (spell)
    {
        case SPELL_PAWN_A_2:
        case SPELL_PAWN_H_2:
        case SPELL_ROOK_A_2:
        case SPELL_ROOK_H_2:
        case SPELL_KING_A_2:
        case SPELL_KING_H_2:
        case SPELL_BISHOP_A_1:
        case SPELL_BISHOP_H_1:
            return true;

        default:
            return false;
    }

    return false;
}

int boss_MedivhAI::GetAbilityRange(uint32 spell)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetAbilityRange(uint32 spell = %u)", spell);
    #endif
    /*returns:
    0   - spell needs check if there are any target in melee range, if yes then casts spell on self
    5   - spell needs target in front of caster
    6   - spell needs target in 1 -> 3 targets in front (clave/swipe)
    20  - long range (7x7 - 5 squares) - normal check
    25  - long range (7x7 - 1 squares) - normal check

    0                   - caster;
    1 + 2 + 3           - return 0
    2                   - return 5
    3 + 2               - return 6
    4 + 1 + 2 + 3       - return 20
    5 + 4 + 1 + 2 + 3   - return 25

     5 4 4 4 4 4 5
     4 4 4 4 4 4 4
     4 4 3 2 3 4 4
     4 4 1 0 1 4 4
     4 4 1 1 1 4 4
     4 4 4 4 4 4 4
     5 4 4 4 4 4 5

    ranges for spells can be wrong !
    */

    switch (spell)
    {
        case SPELL_KING_H_1:
        case SPELL_KING_A_1:
            return 6;

        case SPELL_QUEEN_H_1:
        case SPELL_QUEEN_A_1:
        case SPELL_QUEEN_A_2:
        case SPELL_QUEEN_H_2:
            return 25;

        case SPELL_BISHOP_A_1:
        case SPELL_BISHOP_H_1:
            return 20;

        case SPELL_PAWN_H_1:
        case SPELL_PAWN_A_1:
        case SPELL_KNIGHT_H_1:
        case SPELL_KNIGHT_A_1:
            return 5;

        default:
            return 0;
    }

    return 0;
}

uint64 boss_MedivhAI::GetSpellTarget(uint64 caster, uint32 spell)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetSpellTarget(uint64 caster = %u, uint32 spell = %u)", caster, spell);
    #endif
    if (IsOnSelfSpell(spell))
        return caster;

    int tmpI = -1, tmpJ = -1, i, tmpOffsetI, tmpOffsetJ;

    if (!FindPlaceInBoard(caster, tmpI, tmpJ))
    {
        Creature * uCaster = m_creature->GetCreature(caster);
        if (uCaster)
            uCaster->Say("GetSpellTarget(..) : Nie znaleziono mnie na planszy !!", LANG_UNIVERSAL, NULL);
        return 0;
    }

    int priority = START_PRIORITY, prevPriority = 0;

    std::list<Priority> tmpList;
    std::list<uint64> tmpPossibleTargetsList;
    uint64 tmpGUID;

    if (IsPositive(spell))
    {
        //create possible target list

        switch (GetAbilityRange(spell))
        {
            case 25:
                for (i = 0; i < OFFSET25COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab25[i][0];
                    tmpOffsetJ = tmpJ + offsetTab25[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 20:
                for (i = 0; i < OFFSET20COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab20[i][0];
                    tmpOffsetJ = tmpJ + offsetTab20[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 15:
                for (i = 0; i < OFFSET15COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab15[i][0];
                    tmpOffsetJ = tmpJ + offsetTab15[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 8:
                for (i = 0; i < OFFSET8COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab8[i][0];
                    tmpOffsetJ = tmpJ + offsetTab8[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
                break;
            case 6: // 3 targets in front
                switch (chessBoard[tmpI][tmpJ].ori)
                {
                    case CHESS_ORI_E:
                        if (tmpJ + 1 >= 0)
                        {
                            if (!Enemy(caster, chessBoard[tmpI][tmpJ + 1].piece))
                                return caster;

                            if (tmpI - 1 >= 0 && !Enemy(caster, chessBoard[tmpI - 1][tmpJ + 1].piece))
                                return caster;

                            if (tmpI + 1 < 8 && !Enemy(caster, chessBoard[tmpI + 1][tmpJ + 1].piece))
                                return caster;
                        }
                        break;
                    case CHESS_ORI_N:
                        if (tmpI - 1 >= 0)
                        {
                            if (!Enemy(caster, chessBoard[tmpI - 1][tmpJ].piece))
                                return caster;

                            if (tmpJ - 1 >= 0 && !Enemy(caster, chessBoard[tmpI - 1][tmpJ - 1].piece))
                                return caster;

                            if (tmpJ + 1 < 8 && !Enemy(caster, chessBoard[tmpI - 1][tmpJ + 1].piece))
                                return caster;
                        }
                        break;
                    case CHESS_ORI_S:
                        if (tmpI + 1 >= 0)
                        {
                            if (!Enemy(caster, chessBoard[tmpI + 1][tmpJ].piece))
                                return caster;

                            if (tmpJ - 1 >= 0 && !Enemy(caster, chessBoard[tmpI + 1][tmpJ - 1].piece))
                                return caster;

                            if (tmpJ + 1 < 8 && !Enemy(caster, chessBoard[tmpI + 1][tmpJ + 1].piece))
                                return caster;
                        }
                        break;
                    case CHESS_ORI_W:
                        if (tmpJ - 1 >= 0)
                        {
                            if (!Enemy(caster, chessBoard[tmpI][tmpJ - 1].piece))
                                return caster;

                            if (tmpI - 1 >= 0 && !Enemy(caster, chessBoard[tmpI - 1][tmpJ - 1].piece))
                                return caster;

                            if (tmpI + 1 < 8 && !Enemy(caster, chessBoard[tmpI + 1][tmpJ - 1].piece))
                                return caster;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case 5: // 1 target in front
                switch (chessBoard[tmpI][tmpJ].ori)
                {
                    case CHESS_ORI_E:
                        if (tmpJ + 1 >= 0)
                            if (!Enemy(caster, chessBoard[tmpI][tmpJ + 1].piece))
                                return chessBoard[tmpI][tmpJ + 1].piece;
                        break;
                    case CHESS_ORI_N:
                        if (tmpI - 1 >= 0)
                            if (!Enemy(caster, chessBoard[tmpI - 1][tmpJ].piece))
                                return chessBoard[tmpI - 1][tmpJ].piece;
                        break;
                    case CHESS_ORI_S:
                        if (tmpI + 1 >= 0)
                            if (!Enemy(caster, chessBoard[tmpI + 1][tmpJ].piece))
                                return chessBoard[tmpI + 1][tmpJ].piece;
                        break;
                    case CHESS_ORI_W:
                        if (tmpJ - 1 >= 0)
                            if (!Enemy(caster, chessBoard[tmpI][tmpJ - 1].piece))
                                return chessBoard[tmpI][tmpJ - 1].piece;
                        break;
                    default:
                        break;
                }
                break;
            case 0: // check if is any piece in melee
                for (i = 0; i < OFFSET8COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab8[i][0];
                    tmpOffsetJ = tmpJ + offsetTab8[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            return caster;
                    }
                }
                break;
            default:
                break;
        }

        if (tmpPossibleTargetsList.empty())
            return 0;

        // calculate and add priority

        int prioritySum = 0;

        for (std::list<uint64>::iterator i = tmpPossibleTargetsList.begin(); i != tmpPossibleTargetsList.end(); ++i)
        {
            Priority tempPriority;
            tempPriority.prior = START_PRIORITY;
            tempPriority.GUID = *i;

            tempPriority.prior += GetCountOfEnemyInMelee(*i) * MELEE_PRIORITY + urand(0, RAND_PRIORITY) + GetLifePriority(*i);

            prioritySum += tempPriority.prior;
            tmpList.push_back(tempPriority);
        }

        int chosen = urand(0, prioritySum), prevPrior = 0;

        for (std::list<Priority>::iterator i = tmpList.begin(); i!= tmpList.end(); ++i)
        {
            if (prevPrior < chosen && (*i).prior >= chosen)
                return (*i).GUID;
            prevPrior = (*i).prior;
        }
    }
    else        //if !positive
    {
        //create possible targets list

        switch (GetAbilityRange(spell))
        {
            case 25:
                for (i = 0; i < OFFSET25COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab25[i][0];
                    tmpOffsetJ = tmpJ + offsetTab25[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 20:
                for (i = 0; i < OFFSET20COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab20[i][0];
                    tmpOffsetJ = tmpJ + offsetTab20[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 15:
                for (i = 0; i < OFFSET15COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab15[i][0];
                    tmpOffsetJ = tmpJ + offsetTab15[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 8:
                for (i = 0; i < OFFSET8COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab8[i][0];
                    tmpOffsetJ = tmpJ + offsetTab8[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
                break;
            case 6: // 3 targets in front
                switch (chessBoard[tmpI][tmpJ].ori)
                {
                    case CHESS_ORI_E:
                        if (tmpJ + 1 >= 0)
                        {
                            if (Enemy(caster, chessBoard[tmpI][tmpJ + 1].piece))
                                return caster;

                            if (tmpI - 1 >= 0 && Enemy(caster, chessBoard[tmpI - 1][tmpJ + 1].piece))
                                return caster;

                            if (tmpI + 1 < 8 && Enemy(caster, chessBoard[tmpI + 1][tmpJ + 1].piece))
                                return caster;
                        }
                        break;
                    case CHESS_ORI_N:
                        if (tmpI - 1 >= 0)
                        {
                            if (Enemy(caster, chessBoard[tmpI - 1][tmpJ].piece))
                                return caster;

                            if (tmpJ - 1 >= 0 && Enemy(caster, chessBoard[tmpI - 1][tmpJ - 1].piece))
                                return caster;

                            if (tmpJ + 1 < 8 && Enemy(caster, chessBoard[tmpI - 1][tmpJ + 1].piece))
                                return caster;
                        }
                        break;
                    case CHESS_ORI_S:
                        if (tmpI + 1 >= 0)
                        {
                            if (Enemy(caster, chessBoard[tmpI + 1][tmpJ].piece))
                                return caster;

                            if (tmpJ - 1 >= 0 && Enemy(caster, chessBoard[tmpI + 1][tmpJ - 1].piece))
                                return caster;

                            if (tmpJ + 1 < 8 && Enemy(caster, chessBoard[tmpI + 1][tmpJ + 1].piece))
                                return caster;
                        }
                        break;
                    case CHESS_ORI_W:
                        if (tmpJ - 1 >= 0)
                        {
                            if (Enemy(caster, chessBoard[tmpI][tmpJ - 1].piece))
                                return caster;

                            if (tmpI - 1 >= 0 && Enemy(caster, chessBoard[tmpI - 1][tmpJ - 1].piece))
                                return caster;

                            if (tmpI + 1 < 8 && Enemy(caster, chessBoard[tmpI + 1][tmpJ - 1].piece))
                                return caster;
                        }
                        break;
                    default:
                        break;
                }
                break;
            case 5: // 1 target in front
                switch (chessBoard[tmpI][tmpJ].ori)
                {
                    case CHESS_ORI_E:
                        if (tmpJ + 1 >= 0)
                            if (Enemy(caster, chessBoard[tmpI][tmpJ + 1].piece))
                                return chessBoard[tmpI][tmpJ + 1].piece;
                        break;
                    case CHESS_ORI_N:
                        if (tmpI - 1 >= 0)
                            if (Enemy(caster, chessBoard[tmpI - 1][tmpJ].piece))
                                return chessBoard[tmpI - 1][tmpJ].piece;
                        break;
                    case CHESS_ORI_S:
                        if (tmpI + 1 >= 0)
                            if (Enemy(caster, chessBoard[tmpI + 1][tmpJ].piece))
                                return chessBoard[tmpI + 1][tmpJ].piece;
                        break;
                    case CHESS_ORI_W:
                        if (tmpJ - 1 >= 0)
                            if (Enemy(caster, chessBoard[tmpI][tmpJ - 1].piece))
                                return chessBoard[tmpI][tmpJ - 1].piece;
                        break;
                    default:
                        break;
                }
                break;
            case 0: // check if is any piece in melee
                for (i = 0; i < OFFSET8COUNT; i++)
                {
                    tmpOffsetI = tmpI + offsetTab8[i][0];
                    tmpOffsetJ = tmpJ + offsetTab8[i][1];

                    if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                        tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpOffsetI][tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            return caster;
                    }
                }
                break;
            default:
                break;
        }

        // calculate and add priority

        int prioritySum = 0;

        for (std::list<uint64>::iterator i = tmpPossibleTargetsList.begin(); i != tmpPossibleTargetsList.end(); ++i)
        {
            Priority tempPriority;
            tempPriority.prior = START_PRIORITY;
            tempPriority.GUID = *i;

            tempPriority.prior += GetCountOfEnemyInMelee(*i) * MELEE_PRIORITY + urand(0, RAND_PRIORITY) + GetAttackPriority(*i);

            prioritySum += tempPriority.prior;
            tmpList.push_back(tempPriority);
        }

        int chosen = urand(0, prioritySum), prevPrior = 0;

        for (std::list<Priority>::iterator i = tmpList.begin(); i!= tmpList.end(); ++i)
        {
            if (prevPrior < chosen && (*i).prior >= chosen)
                return (*i).GUID;
            prevPrior = (*i).prior;
        }
    }

    return 0;
}

uint64 boss_MedivhAI::GetMeleeTarget(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetMeleeTarget(uint64 piece = %u)", piece);
    #endif
    int tmpi, tmpj;    //temporary piece position

    FindPlaceInBoard(piece, tmpi, tmpj);

    switch (chessBoard[tmpi][tmpj].ori)
    {
        case CHESS_ORI_N:
            if (tmpi < 7)
            {
                //front
                if (chessBoard[tmpi + 1][tmpj].piece)
                    return chessBoard[tmpi + 1][tmpj].piece;

                //strafe
                if (tmpj < 7 && chessBoard[tmpi + 1][tmpj + 1].piece)
                    return chessBoard[tmpi + 1][tmpj + 1].piece;

                if (tmpj > 0 && chessBoard[tmpi + 1][tmpj - 1].piece)
                    return chessBoard[tmpi + 1][tmpj - 1].piece;
            }
            break;
        case CHESS_ORI_E:
            if (tmpj < 7)
            {
                //front
                if (chessBoard[tmpi][tmpj + 1].piece)
                    return chessBoard[tmpi][tmpj + 1].piece;

                //strafe
                if (tmpi < 7 && chessBoard[tmpi + 1][tmpj + 1].piece)
                    return chessBoard[tmpi + 1][tmpj + 1].piece;

                if (tmpi > 0 && chessBoard[tmpi - 1][tmpj + 1].piece)
                    return chessBoard[tmpi - 1][tmpj + 1].piece;
            }
            break;
        case CHESS_ORI_S:
            if (tmpi > 0)
            {
                //front
                if (chessBoard[tmpi - 1][tmpj].piece)
                    return chessBoard[tmpi - 1][tmpj].piece;

                //strafe
                if (tmpj < 7 && chessBoard[tmpi - 1][tmpj + 1].piece)
                    return chessBoard[tmpi - 1][tmpj + 1].piece;

                if (tmpj > 0 && chessBoard[tmpi - 1][tmpj - 1].piece)
                    return chessBoard[tmpi - 1][tmpj - 1].piece;
            }
            break;
        case CHESS_ORI_W:
            if (tmpj > 0)
            {
                //front
                if (chessBoard[tmpi][tmpj - 1].piece)
                    return chessBoard[tmpi][tmpj - 1].piece;

                //strafe
                if (tmpi < 7 && chessBoard[tmpi + 1][tmpj - 1].piece)
                    return chessBoard[tmpi + 1][tmpj - 1].piece;

                if (tmpi > 0 && chessBoard[tmpi - 1][tmpj - 1].piece)
                    return chessBoard[tmpi - 1][tmpj - 1].piece;
            }
            break;
        default:
            break;
    }

    return 0;
}


bool boss_MedivhAI::IsChessPiece(Unit * unit)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsChessPiece(Unit * unit = %i)", unit ? 1 : 0);
    #endif

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

bool boss_MedivhAI::IsKing(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsKing(uint64 piece = %u)", piece);
    #endif
    return IsKing(me->GetCreature(piece));
}

bool boss_MedivhAI::IsHealer(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsHealer(uint64 piece = %u)", piece);
    #endif
    return IsHealer(me->GetCreature(piece));
}

bool boss_MedivhAI::IsKing(Creature * piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsKing(Creature * piece = %i)", piece ? 1 : 0);
    #endif

    if (!piece)
        return false;

    switch (piece->GetEntry())
    {
        case NPC_KING_H:
        case NPC_KING_A:
            return true;

        default:
            return false;
    }

    return false;
}

bool boss_MedivhAI::IsHealer(Creature * piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsHealer(Creature * piece = %i)", piece ? 1 : 0);
    #endif

    if (!piece)
        return false;

    switch (piece->GetEntry())
    {
        case NPC_BISHOP_H:
        case NPC_BISHOP_A:
            return true;

        default:
            return false;
    }

    return false;
}

bool boss_MedivhAI::IsMedivhsPiece(Unit * unit)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsMedivhsPiece(Unit * unit = %i)", unit ? 1 : 0);
    #endif

    if (unit)
    {
        switch (pInstance->GetData(CHESS_EVENT_TEAM))
        {
            case ALLIANCE:
                if (unit->getFaction() == H_FACTION)
                    return true;
                break;
            case HORDE:
                if (unit->getFaction() == A_FACTION)
                    return true;
                break;
        }
    }
    else
        me->Say("IsMedivhsPiece: Cos unita niema", LANG_UNIVERSAL, NULL);

    return false;
}

bool boss_MedivhAI::IsMedivhsPiece(uint64 unit)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsMedivhsPiece(uint64 unit = %u)", unit);
    #endif

    for (std::list<uint64>::iterator i = medivhSidePieces.begin(); i != medivhSidePieces.end(); ++i)
        if ((*i) == unit)
            return true;

    return false;
}

bool boss_MedivhAI::IsInMoveList(uint64 unit, bool trigger)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsInMoveList(uint64 unit = %u, bool trigger = %i)", unit, trigger);
    #endif

    if (!trigger)
    {
        for (std::list<ChessTile>::iterator i = moveList.begin(); i != moveList.end(); ++i)
            if ((*i).piece == unit)
                return true;
    }
    else
    {
        for (std::list<ChessTile>::iterator i = moveList.begin(); i != moveList.end(); ++i)
            if ((*i).trigger == unit)
                return true;
    }

    return false;
}

bool boss_MedivhAI::IsInMoveRange(uint64 from, uint64 to, int range)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie IsInMoveRange(uint64 from = %u, uint64 to = %u, int range = %i)", from, to, range);
    #endif

    if (!from || !to || !range)
        return false;

    int tmpI = -1, tmpJ = -1, i, tmpOffsetI, tmpOffsetJ;

    if (!FindPlaceInBoard(from, tmpI, tmpJ))
    {
        Creature * uFrom = m_creature->GetCreature(from);
        if (uFrom)
            uFrom->Say("GetSpellTarget(..) : Nie znaleziono mnie na planszy !!", LANG_UNIVERSAL, NULL);
        else
            me->Say("GetSpellTarget(..) : uFrom sie zapodzial", LANG_UNIVERSAL, NULL);
        return false;
    }

    switch (range)
    {
        case 25:
            for (i = 0; i < OFFSET25COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab25[i][0];
                tmpOffsetJ = tmpJ + offsetTab25[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    if (chessBoard[tmpOffsetI][tmpOffsetJ].piece == to ||
                        chessBoard[tmpOffsetI][tmpOffsetJ].trigger == to)
                        return true;
            }
        case 20:
            for (i = 0; i < OFFSET20COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab20[i][0];
                tmpOffsetJ = tmpJ + offsetTab20[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    if (chessBoard[tmpOffsetI][tmpOffsetJ].piece == to ||
                        chessBoard[tmpOffsetI][tmpOffsetJ].trigger == to)
                        return true;
            }
        case 15:
            for (i = 0; i < OFFSET15COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab15[i][0];
                tmpOffsetJ = tmpJ + offsetTab15[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    if (chessBoard[tmpOffsetI][tmpOffsetJ].piece == to ||
                        chessBoard[tmpOffsetI][tmpOffsetJ].trigger == to)
                        return true;
            }
        case 8:
            for (i = 0; i < OFFSET8COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab8[i][0];
                tmpOffsetJ = tmpJ + offsetTab8[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                    if (chessBoard[tmpOffsetI][tmpOffsetJ].piece == to ||
                        chessBoard[tmpOffsetI][tmpOffsetJ].trigger == to)
                        return true;
            }
            break;
        default:
            me->Say("Jakis dziwaczny range", LANG_UNIVERSAL, NULL);
            break;
    }

    return false;
}

void boss_MedivhAI::Reset()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie boss_MedivhAI::Reset()");
    #endif

    eventStarted = false;
    miniEventState = MINI_EVENT_NONE;
    miniEventTimer = 5000;
    endGameEventState = GAMEEND_NONE;
    endEventTimer = 2500;
    hordePieces = 16;
    alliancePieces = 16;
    moveTimer = 60000;
    medivhSidePieces.clear();
    tpList.clear();
    moveList.clear();

    m_creature->CastSpell((Unit*)NULL, SPELL_GAME_IN_SESSION, false);

    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
}

void boss_MedivhAI::SayChessPieceDied(Unit * piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SayChessPieceDied(Unit * piece)");
    #endif

    if(pInstance->GetData(CHESS_EVENT_TEAM) == HORDE)
    {
        switch(piece->GetEntry())
        {
            case NPC_ROOK_H:
                DoScriptText(SCRIPTTEXT_LOSE_ROOK_P, m_creature);
                break;
            case NPC_ROOK_A:
                DoScriptText(SCRIPTTEXT_LOSE_ROOK_M, m_creature);
                break;
            case NPC_QUEEN_H:
                DoScriptText(SCRIPTTEXT_LOSE_QUEEN_P, m_creature);
                break;
            case NPC_QUEEN_A:
                DoScriptText(SCRIPTTEXT_LOSE_QUEEN_M, m_creature);
                break;
            case NPC_BISHOP_H:
                DoScriptText(SCRIPTTEXT_LOSE_BISHOP_P, m_creature);
                break;
            case NPC_BISHOP_A:
                DoScriptText(SCRIPTTEXT_LOSE_BISHOP_M, m_creature);
                break;
            case NPC_KNIGHT_H:
                DoScriptText(SCRIPTTEXT_LOSE_KNIGHT_P, m_creature);
                break;
            case NPC_KNIGHT_A:
                DoScriptText(SCRIPTTEXT_LOSE_KNIGHT_M, m_creature);
                break;
            case NPC_PAWN_H:

                DoScriptText(RAND(SCRIPTTEXT_LOSE_PAWN_P_1, SCRIPTTEXT_LOSE_PAWN_P_2, SCRIPTTEXT_LOSE_PAWN_P_3), m_creature);
                break;

            case NPC_PAWN_A:

                DoScriptText(RAND(SCRIPTTEXT_LOSE_PAWN_M_1, SCRIPTTEXT_LOSE_PAWN_M_2), m_creature);
                break;

            case NPC_KING_H:

                DoScriptText(SCRIPTTEXT_MEDIVH_WIN, m_creature);
                pInstance->SetData(DATA_CHESS_EVENT, FAIL);
                endGameEventState = GAMEEND_MEDIVH_WIN;
                endEventTimer = 2500;
                endEventCount = 0;
                break;

            case NPC_KING_A:

                DoScriptText(SCRIPTTEXT_PLAYER_WIN, m_creature);
                pInstance->SetData(DATA_CHESS_EVENT, DONE);
                endGameEventState = GAMEEND_MEDIVH_LOSE;
                endEventTimer = 2500;
                endEventCount = 0;
                m_creature->SummonGameObject(DUST_COVERED_CHEST, -11058, -1903, 221, 2.24, 0, 0, 0, 0, 7200000);
                break;

            default:
                break;
        }
    }
    else
    {
        switch(piece->GetEntry())
        {
            case NPC_ROOK_A:
                DoScriptText(SCRIPTTEXT_LOSE_ROOK_P, m_creature);
                break;
            case NPC_ROOK_H:
                DoScriptText(SCRIPTTEXT_LOSE_ROOK_M, m_creature);
                break;
            case NPC_QUEEN_A:
                DoScriptText(SCRIPTTEXT_LOSE_QUEEN_P, m_creature);
                break;
            case NPC_QUEEN_H:
                DoScriptText(SCRIPTTEXT_LOSE_QUEEN_M, m_creature);
                break;
            case NPC_BISHOP_A:
                DoScriptText(SCRIPTTEXT_LOSE_BISHOP_P, m_creature);
                break;
            case NPC_BISHOP_H:
                DoScriptText(SCRIPTTEXT_LOSE_BISHOP_M, m_creature);
                break;
            case NPC_KNIGHT_A:
                DoScriptText(SCRIPTTEXT_LOSE_KNIGHT_P, m_creature);
                break;
            case NPC_KNIGHT_H:
                DoScriptText(SCRIPTTEXT_LOSE_KNIGHT_M, m_creature);
                break;
            case NPC_PAWN_A:

                DoScriptText(RAND(SCRIPTTEXT_LOSE_PAWN_P_1, SCRIPTTEXT_LOSE_PAWN_P_2, SCRIPTTEXT_LOSE_PAWN_P_3), m_creature);
                break;

            case NPC_PAWN_H:

                DoScriptText(RAND(SCRIPTTEXT_LOSE_PAWN_M_1, SCRIPTTEXT_LOSE_PAWN_M_2), m_creature);
                break;

            case NPC_KING_A:

                DoScriptText(SCRIPTTEXT_MEDIVH_WIN, m_creature);
                pInstance->SetData(DATA_CHESS_EVENT, FAIL);
                endGameEventState = GAMEEND_MEDIVH_WIN;
                endEventTimer = 2500;
                endEventCount = 0;
                break;

            case NPC_KING_H:

                DoScriptText(SCRIPTTEXT_PLAYER_WIN, m_creature);
                pInstance->SetData(DATA_CHESS_EVENT, DONE);
                endGameEventState = GAMEEND_MEDIVH_LOSE;
                endEventTimer = 2500;
                endEventCount = 0;
                m_creature->SummonGameObject(DUST_COVERED_CHEST, -11058, -1903, 221, 2.24, 0, 0, 0, 0, 7200000);
                break;

            default:
                break;
        }
    }
}

void boss_MedivhAI::RemoveChessPieceFromBoard(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie RemoveChessPieceFromBoard(uint64 piece = %u)", piece);
    #endif

    RemoveChessPieceFromBoard(m_creature->GetCreature(piece));
}

void boss_MedivhAI::RemoveChessPieceFromBoard(Creature * piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie RemoveChessPieceFromBoard(Unit * piece = %i)", piece ? 1 : 0);
    #endif

    if (!piece)
    {
        me->Say("RemoveChesPieceFromBoard(..) : pionka wcielo", LANG_UNIVERSAL, NULL);
        return;
    }

    Creature * tmpC;
    uint32 tmpEntry = GetDeadEntryForPiece(piece->GetEntry());

    #ifdef CHESS_DEBUG_INFO
    printf("\ntmpEntry: %u", tmpEntry);
    #endif
    if (piece->getFaction() == A_FACTION)
    {
        --alliancePieces;
        tmpC = me->SummonCreature(tmpEntry, allianceSideDeadWP[0][alliancePieces], allianceSideDeadWP[1][alliancePieces], POSITION_Z, ORI_W, TEMPSUMMON_CORPSE_DESPAWN, 0);
    }
    else
    {
        --hordePieces;
        tmpC = me->SummonCreature(tmpEntry, hordeSideDeadWP[0][hordePieces], hordeSideDeadWP[1][hordePieces], POSITION_Z, ORI_W, TEMPSUMMON_CORPSE_DESPAWN, 0);
    }

    if (tmpC)
    {
        tmpC->CombatStop();
        tmpC->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        tmpC->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        tmpC->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
    }

    int tmpI = -1, tmpJ = -1;
    uint64 tmpGUID = piece->GetGUID();

    if (FindPlaceInBoard(tmpGUID, tmpI, tmpJ))
        chessBoard[tmpI][tmpJ].piece = 0;

    bool medivhPiece = false;

    for (std::list<uint64>::iterator itr = medivhSidePieces.begin(); itr != medivhSidePieces.end();)
    {
        std::list<uint64>::iterator tmpItr = itr;
        ++itr;

        if ((*tmpItr) == tmpGUID)
        {
            medivhSidePieces.erase(tmpItr);
            medivhPiece = true;
            if (tmpC)
                unusedMedivhPieces.push_back(tmpC->GetGUID());
            break;
        }
    }

    if (!medivhPiece && tmpC)
        unusedPlayerPieces.push_back(tmpC->GetGUID());

    SayChessPieceDied(piece);
}

void boss_MedivhAI::SpawnPawns()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SpawnPawns()");
    #endif

    Creature * tmp[2][8];

    for (int i = 0; i < 8; i++)
    {
        tmp[0][i] = m_creature->SummonCreature(NPC_PAWN_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
        tmp[1][i] = m_creature->SummonCreature(NPC_PAWN_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
        if (tmp[0][i])
        {
            #ifdef CHESS_DEBUG_INFO
            printf("\nPawns [0][%i] : %u ",i, tmp[0][i]->GetGUID());
            #endif
            chessBoard[6][i].piece = tmp[0][i]->GetGUID();
            tmp[0][i]->SetReactState(REACT_PASSIVE);
            tmp[0][i]->GetMotionMaster()->MovePoint(0, chessBoard[6][i].position.coord_x, chessBoard[6][i].position.coord_y, chessBoard[6][i].position.coord_z);
        }
        chessBoard[6][i].ori = CHESS_ORI_N;
        if (tmp[1][i])
        {
            #ifdef CHESS_DEBUG_INFO
            printf("\nPawns [1][%i] : %u ",i, tmp[0][i]->GetGUID());
            #endif
            chessBoard[1][i].piece = tmp[1][i]->GetGUID();
            tmp[1][i]->SetReactState(REACT_PASSIVE);
            tmp[1][i]->GetMotionMaster()->MovePoint(0, chessBoard[1][i].position.coord_x, chessBoard[1][i].position.coord_y, chessBoard[1][i].position.coord_z);
        }
        chessBoard[1][i].ori = CHESS_ORI_S;
    }

    if (pInstance)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE)
            for (int i = 0; i < 8; i++)
                medivhSidePieces.push_back(tmp[1][i]->GetGUID());
        else
            for (int i = 0; i < 8; i++)
                medivhSidePieces.push_back(tmp[0][i]->GetGUID());
    }

    miniEventTimer = 10000;
}

void boss_MedivhAI::SpawnRooks()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SpawnRooks()");
    #endif
    Creature * tmp1, * tmp2, * tmp3, * tmp4;

    tmp1 = m_creature->SummonCreature(NPC_ROOK_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp1)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nRooks [0] : %u ", tmp1->GetGUID());
        #endif
        chessBoard[7][0].piece = tmp1->GetGUID();
        chessBoard[7][0].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][0].position.coord_x, chessBoard[7][0].position.coord_y, chessBoard[7][0].position.coord_z);
    }

    tmp2 = m_creature->SummonCreature(NPC_ROOK_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp2)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nRooks [1] : %u ", tmp2->GetGUID());
        #endif
        chessBoard[7][7].piece = tmp2->GetGUID();
        chessBoard[7][7].ori = CHESS_ORI_N;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[7][7].position.coord_x, chessBoard[7][7].position.coord_y, chessBoard[7][7].position.coord_z);
    }


    tmp3 = m_creature->SummonCreature(NPC_ROOK_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp3)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nRooks [2] : %u ", tmp3->GetGUID());
        #endif
        chessBoard[0][0].piece = tmp3->GetGUID();
        chessBoard[0][0].ori = CHESS_ORI_S;
        tmp3->SetReactState(REACT_PASSIVE);
        tmp3->GetMotionMaster()->MovePoint(0, chessBoard[0][0].position.coord_x, chessBoard[0][0].position.coord_y, chessBoard[0][0].position.coord_z);
    }

    tmp4 = m_creature->SummonCreature(NPC_ROOK_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp4)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nRooks [3] : %u ", tmp4->GetGUID());
        #endif
        chessBoard[0][7].piece = tmp4->GetGUID();
        chessBoard[0][7].ori = CHESS_ORI_S;
        tmp4->SetReactState(REACT_PASSIVE);
        tmp4->GetMotionMaster()->MovePoint(0, chessBoard[0][7].position.coord_x, chessBoard[0][7].position.coord_y, chessBoard[0][7].position.coord_z);
    }

    if (pInstance && tmp1 && tmp2 && tmp3 && tmp4)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE)
        {
            medivhSidePieces.push_back(tmp3->GetGUID());
            medivhSidePieces.push_back(tmp4->GetGUID());
        }
        else
        {
            medivhSidePieces.push_back(tmp1->GetGUID());
            medivhSidePieces.push_back(tmp2->GetGUID());
        }
    }
    miniEventTimer = 5000;
}

void boss_MedivhAI::SpawnKnights()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SpawnKnights()");
    #endif
    Creature * tmp1, * tmp2, * tmp3, * tmp4;

    tmp1 = m_creature->SummonCreature(NPC_KNIGHT_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp1)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nKnights [0] : %u ", tmp1->GetGUID());
        #endif
        chessBoard[7][1].piece = tmp1->GetGUID();
        chessBoard[7][1].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][1].position.coord_x, chessBoard[7][1].position.coord_y, chessBoard[7][1].position.coord_z);
    }

    tmp2 = m_creature->SummonCreature(NPC_KNIGHT_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp2)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nKnights [1] : %u ", tmp2->GetGUID());
        #endif
        chessBoard[7][6].piece = tmp2->GetGUID();
        chessBoard[7][6].ori = CHESS_ORI_N;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[7][6].position.coord_x, chessBoard[7][6].position.coord_y, chessBoard[7][6].position.coord_z);
    }


    tmp3 = m_creature->SummonCreature(NPC_KNIGHT_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp3)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nKnights [2] : %u ", tmp3->GetGUID());
        #endif
        chessBoard[0][1].piece = tmp3->GetGUID();
        chessBoard[0][1].ori = CHESS_ORI_S;
        tmp3->SetReactState(REACT_PASSIVE);
        tmp3->GetMotionMaster()->MovePoint(0, chessBoard[0][1].position.coord_x, chessBoard[0][1].position.coord_y, chessBoard[0][1].position.coord_z);
    }

    tmp4 = m_creature->SummonCreature(NPC_KNIGHT_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp4)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nKnights [3] : %u ", tmp4->GetGUID());
        #endif
        chessBoard[0][6].piece = tmp4->GetGUID();
        chessBoard[0][6].ori = CHESS_ORI_S;
        tmp4->SetReactState(REACT_PASSIVE);
        tmp4->GetMotionMaster()->MovePoint(0, chessBoard[0][6].position.coord_x, chessBoard[0][6].position.coord_y, chessBoard[0][6].position.coord_z);
    }

    if (pInstance && tmp1 && tmp2 && tmp3 && tmp4)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE)
        {
            medivhSidePieces.push_back(tmp3->GetGUID());
            medivhSidePieces.push_back(tmp4->GetGUID());
        }
        else
        {
            medivhSidePieces.push_back(tmp1->GetGUID());
            medivhSidePieces.push_back(tmp2->GetGUID());
        }
    }
    miniEventTimer = 5000;
}

void boss_MedivhAI::SpawnBishops()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SpawnBishops()");
    #endif
    Creature * tmp1, * tmp2, * tmp3, * tmp4;

    tmp1 = m_creature->SummonCreature(NPC_BISHOP_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp1)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nBishops [0] : %u ", tmp1->GetGUID());
        #endif
        chessBoard[7][2].piece = tmp1->GetGUID();
        chessBoard[7][2].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][2].position.coord_x, chessBoard[7][2].position.coord_y, chessBoard[7][2].position.coord_z);
    }

    tmp2 = m_creature->SummonCreature(NPC_BISHOP_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp2)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nBishops [1] : %u ", tmp2->GetGUID());
        #endif
        chessBoard[7][5].piece = tmp2->GetGUID();
        chessBoard[7][5].ori = CHESS_ORI_N;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[7][5].position.coord_x, chessBoard[7][5].position.coord_y, chessBoard[7][5].position.coord_z);
    }


    tmp3 = m_creature->SummonCreature(NPC_BISHOP_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp3)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nBishops [2] : %u ", tmp3->GetGUID());
        #endif
        chessBoard[0][2].piece = tmp3->GetGUID();
        chessBoard[0][2].ori = CHESS_ORI_S;
        tmp3->SetReactState(REACT_PASSIVE);
        tmp3->GetMotionMaster()->MovePoint(0, chessBoard[0][2].position.coord_x, chessBoard[0][2].position.coord_y, chessBoard[0][2].position.coord_z);
    }

    tmp4 = m_creature->SummonCreature(NPC_BISHOP_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp4)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nBishops [3] : %u ", tmp4->GetGUID());
        #endif
        chessBoard[0][5].piece = tmp4->GetGUID();
        chessBoard[0][5].ori = CHESS_ORI_S;
        tmp4->SetReactState(REACT_PASSIVE);
        tmp4->GetMotionMaster()->MovePoint(0, chessBoard[0][5].position.coord_x, chessBoard[0][5].position.coord_y, chessBoard[0][5].position.coord_z);
    }

    if (pInstance && tmp1 && tmp2 && tmp3 && tmp4)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE)
        {
            medivhSidePieces.push_back(tmp3->GetGUID());
            medivhSidePieces.push_back(tmp4->GetGUID());
        }
        else
        {
            medivhSidePieces.push_back(tmp1->GetGUID());
            medivhSidePieces.push_back(tmp2->GetGUID());
        }
    }

    miniEventTimer = 5000;
}

void boss_MedivhAI::SpawnQueens()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SpawnQueens()");
    #endif
    Creature * tmp1, * tmp2;

    tmp1 = m_creature->SummonCreature(NPC_QUEEN_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp1)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nQuens [0] : %u ", tmp1->GetGUID());
        #endif
        chessBoard[7][3].piece = tmp1->GetGUID();
        chessBoard[7][3].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][3].position.coord_x, chessBoard[7][3].position.coord_y, chessBoard[7][3].position.coord_z);
    }

    tmp2 = m_creature->SummonCreature(NPC_QUEEN_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp2)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nQuens [1] : %u ", tmp2->GetGUID());
        #endif
        chessBoard[0][3].piece = tmp2->GetGUID();
        chessBoard[0][3].ori = CHESS_ORI_S;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[0][3].position.coord_x, chessBoard[0][3].position.coord_y, chessBoard[0][3].position.coord_z);
    }

    if (pInstance && tmp1 && tmp2)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE)
            medivhSidePieces.push_back(tmp2->GetGUID());
        else
            medivhSidePieces.push_back(tmp1->GetGUID());
    }
    miniEventTimer = 5000;
}

void boss_MedivhAI::SpawnKings()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SpawnKings()");
    #endif
    Creature * tmp1, * tmp2;

    tmp1 = m_creature->SummonCreature(NPC_KING_A, SPAWN_POS, ORI_N, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp1)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nKings [0] : %u", tmp1->GetGUID());
        #endif
        chessBoard[7][4].piece = tmp1->GetGUID();
        chessBoard[7][4].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][4].position.coord_x, chessBoard[7][4].position.coord_y, chessBoard[7][4].position.coord_z);
    }

    tmp2 = m_creature->SummonCreature(NPC_KING_H, SPAWN_POS, ORI_S, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

    if (tmp2)
    {
        #ifdef CHESS_DEBUG_INFO
        printf("\nKings [1] : %u ", tmp2->GetGUID());
        #endif
        chessBoard[0][4].piece = tmp2->GetGUID();
        chessBoard[0][4].ori = CHESS_ORI_S;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[0][4].position.coord_x, chessBoard[0][4].position.coord_y, chessBoard[0][4].position.coord_z);
    }

    if (pInstance && tmp1 && tmp2)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE)
            medivhSidePieces.push_back(tmp2->GetGUID());
        else
            medivhSidePieces.push_back(tmp1->GetGUID());
    }
    miniEventTimer = 5000;
}

void boss_MedivhAI::SpawnTriggers()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SpawnTriggers()");
    #endif
    Creature * tmp = NULL;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            tmp = m_creature->SummonCreature(TRIGGER_ID, chessBoard[i][j].position.coord_x, chessBoard[i][j].position.coord_y, chessBoard[i][j].position.coord_z, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (tmp)
            {
                #ifdef CHESS_DEBUG_INFO
                printf("\nTriggers [%i][%i] : %u ", i, j, tmp->GetGUID());
                #endif
                chessBoard[i][j].trigger = tmp->GetGUID();
                if (i > 1 && i < 6)
                {
                    chessBoard[i][j].piece = 0;
                    chessBoard[i][j].ori = CHESS_ORI_CHOOSE;
                }
                tmp->SetReactState(REACT_PASSIVE);
            }
        }
    }
}

void boss_MedivhAI::PrepareBoardForEvent()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie PrepareBoardForEvent()");
    #endif

    SpawnTriggers();
    SpawnKings();
}

void boss_MedivhAI::TeleportPlayer(Player * player)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie TeleportPlayer(Player * player = %i)", player ? 1 : 0);
    #endif
    if (player)
        player->TeleportTo(tpLoc);
}
void boss_MedivhAI::TeleportPlayer(uint64 player)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie TeleportPlayer(uint64 player = %u)", player);
    #endif
    if (player)
    {
        if (Unit *tmpPlayer = Unit::GetUnit(*m_creature, player))
            if (tmpPlayer->GetTypeId() == TYPEID_PLAYER)
                ((Player*)tmpPlayer)->TeleportTo(tpLoc);
    }
}

void boss_MedivhAI::TeleportPlayers()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie TeleportPlayers()");
    #endif
    Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
        if (itr->getSource() && itr->getSource()->HasAura(SPELL_POSSES_CHESSPIECE, 0))
            TeleportPlayer(itr->getSource());
}

void boss_MedivhAI::AddPlayerToTeleportList(Player * player)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie AddPlayerToTeleportList(Player* player = %i)", player ? 1 : 0);
    #endif
    if (player)
        tpList.push_back(player->GetGUID());
}

void boss_MedivhAI::TeleportListedPlayers()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie TeleportListedPlayers()");
    #endif
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

void boss_MedivhAI::ApplyDebuffsOnRaidMembers()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie ApplyDebuffsOnRaidMembers()");
    #endif
    Player * tmp;
    m_creature->CastSpell(m_creature, SPELL_GAME_IN_SESSION, true);
    Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
    {
        tmp = itr->getSource();
        if (tmp && !tmp->isGameMaster() && tmp->isAlive())
        {
            //tmp->CastSpell(tmp, SPELL_IN_GAME, true);
            //tmp->CastSpell(tmp, SPELL_GAME_IN_SESSION, true);
            m_creature->CastSpell(tmp, SPELL_GAME_IN_SESSION, true);
            tmp->AddAura(SPELL_GAME_IN_SESSION, tmp);
            //tmp->SetInCombatWith(m_creature);
        }
    }
}

void boss_MedivhAI::StartMiniEvent()
{
    miniEventState = MINI_EVENT_KING;
    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
}

void boss_MedivhAI::StartEvent()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie StartEvent()");
    #endif
    TeleportPlayers();
    ApplyDebuffsOnRaidMembers();

    DoZoneInCombat();
    eventStarted = true;
    addPieceToMoveCheckTimer = 10000;
}

void boss_MedivhAI::UpdateAI(const uint32 diff)
{
    if (miniEventState)
    {
        if (miniEventTimer < diff)
        {
            switch(miniEventState)
            {
                case MINI_EVENT_KING:
                    PrepareBoardForEvent();
                    miniEventState = MINI_EVENT_QUEEN;
                    break;
                case MINI_EVENT_QUEEN:
                    SpawnQueens();
                    miniEventState = MINI_EVENT_BISHOP;
                    break;
                case MINI_EVENT_BISHOP:
                    SpawnBishops();
                    miniEventState = MINI_EVENT_KNIGHT;
                    break;
                case MINI_EVENT_KNIGHT:
                    SpawnKnights();
                    miniEventState = MINI_EVENT_ROOK;
                    break;
                case MINI_EVENT_ROOK:
                    SpawnRooks();
                    miniEventState = MINI_EVENT_PAWN;
                    break;
                case MINI_EVENT_PAWN:
                    SpawnPawns();
                    miniEventState = MINI_EVENT_END;
                    break;
                default:
                    miniEventState = MINI_EVENT_NONE;
                    #ifdef CHESS_DEBUG_INFO
                    printf("\nTablica:");
                    #endif
                    Creature * tmpC;
                    for (int i = 0; i < 8; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            ChangePieceFacing(chessBoard[i][j].piece, chessBoard[4][j].trigger);
                            #ifdef CHESS_DEBUG_INFO
                            printf("\ni: %i, j: %i, piece: %u, trigger: %u", i, j, chessBoard[i][j].piece, chessBoard[i][j].trigger);
                            #endif
                            if (tmpC = me->GetCreature(chessBoard[i][j].piece))
                            {
                                tmpC->CastSpell(tmpC, SPELL_MOVE_MARKER, false);
                                tmpC->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
                            }
                        }
                    }
                    moveTimer = 10000;
                    break;
            }
        }
        else
            miniEventTimer -= diff;

        return;
    }

    if (endGameEventState)
    {
        if (endEventTimer < diff)
        {
            Creature * tmpC;
            switch (endGameEventState)
            {
                case GAMEEND_MEDIVH_WIN:
                    for (std::list<uint64>::iterator itr = unusedMedivhPieces.begin(); itr != unusedMedivhPieces.end(); ++itr)
                        if (tmpC = me->GetCreature(*itr))
                            tmpC->HandleEmoteCommand(RAND(EMOTE_ONESHOT_CHEER, EMOTE_ONESHOT_APPLAUD));

                    for (std::list<uint64>::iterator itr = unusedPlayerPieces.begin(); itr != unusedPlayerPieces.end(); ++itr)
                        if (tmpC = me->GetCreature(*itr))
                            tmpC->HandleEmoteCommand(EMOTE_ONESHOT_CRY);

                    if (endEventCount >= 5)
                        endGameEventState = GAMEEND_CLEAR_BOARD;
                    break;
                case GAMEEND_MEDIVH_LOSE:
                    for (std::list<uint64>::iterator itr = unusedPlayerPieces.begin(); itr != unusedPlayerPieces.end(); ++itr)
                        if (tmpC = me->GetCreature(*itr))
                            tmpC->HandleEmoteCommand(RAND(EMOTE_ONESHOT_CHEER, EMOTE_ONESHOT_APPLAUD));

                    for (std::list<uint64>::iterator itr = unusedMedivhPieces.begin(); itr != unusedMedivhPieces.end(); ++itr)
                        if (tmpC = me->GetCreature(*itr))
                            tmpC->HandleEmoteCommand(EMOTE_ONESHOT_CRY);

                    if (endEventCount >= 5)
                        endGameEventState = GAMEEND_CLEAR_BOARD;
                    break;
                case GAMEEND_CLEAR_BOARD:
                    for (int i = 0; i < 8; ++i)
                    {
                        for (int j = 0; j < 8; ++j)
                        {
                            if (tmpC = me->GetCreature(chessBoard[i][j].piece))
                            {
                                tmpC->SetVisibility(VISIBILITY_OFF);
                                tmpC->DestroyForNearbyPlayers();
                                tmpC->Kill(tmpC, false);
                            }

                            if (tmpC = me->GetCreature(chessBoard[i][j].trigger))
                            {
                                tmpC->SetVisibility(VISIBILITY_OFF);
                                tmpC->DestroyForNearbyPlayers();
                                tmpC->Kill(tmpC, false);
                            }

                            chessBoard[i][j].piece = 0;
                            chessBoard[i][j].trigger = 0;
                            chessBoard[i][j].ori = CHESS_ORI_CHOOSE;
                        }
                    }

                    for (std::list<uint64>::iterator itr = unusedMedivhPieces.begin(); itr != unusedMedivhPieces.end(); ++itr)
                    {
                        if (tmpC = me->GetCreature(*itr))
                        {
                            tmpC->SetVisibility(VISIBILITY_OFF);
                            tmpC->DestroyForNearbyPlayers();
                            tmpC->Kill(tmpC, false);
                        }
                    }

                    for (std::list<uint64>::iterator itr = unusedPlayerPieces.begin(); itr != unusedPlayerPieces.end(); ++itr)
                    {
                        if (tmpC = me->GetCreature(*itr))
                        {
                            tmpC->SetVisibility(VISIBILITY_OFF);
                            tmpC->DestroyForNearbyPlayers();
                            tmpC->Kill(tmpC, false);
                        }
                    }

                    unusedMedivhPieces.clear();
                    unusedPlayerPieces.clear();
                    medivhSidePieces.clear();
                    EnterEvadeMode();
                    break;
            }
            endEventCount++;
            endEventTimer = 2500;
        }
        else
            endEventTimer -= diff;

        return;
    }

    if (!eventStarted)
        return;

    if (moveTimer <= diff)
    {
        MakeMoves();
        moveTimer = urand(6000, 8000);
    }
    else
        moveTimer -= diff;

    if (addPieceToMoveCheckTimer <= diff)
    {
        if (urand(0, 100) < chanceToSelfMove)
            ChoosePieceToMove();

        addPieceToMoveCheckTimer = ADD_PIECE_TO_MOVE_TIMER;
    }
    else
        addPieceToMoveCheckTimer -= diff;
}

void boss_MedivhAI::SetOrientation(uint64 piece, ChessOrientation ori)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie SetOrientation(uint64 piece = %u, ChessOrientation ori = %i)", piece, ori);
    #endif
    int tmpi = -1, tmpj = -1;    //temp piece location in array

    if (!FindPlaceInBoard(piece, tmpi, tmpj))
        return;

    if (ori == CHESS_ORI_CHOOSE)
    {
        float tmpN = 99, tmpS = 99, tmpE = 99, tmpW = 99;
        float pieceOri;

        Creature * tmpPiece = m_creature->GetCreature(piece);
        if (tmpPiece)
        {
            pieceOri = tmpPiece->GetOrientation();

            if (pieceOri > ORI_N && pieceOri <= ORI_W)
            {
                tmpN = pieceOri - ORI_N;
                tmpW = ORI_W - pieceOri;

                if (tmpN < tmpW)
                    ori = CHESS_ORI_N;
                else
                    ori = CHESS_ORI_W;
            }
            else if (pieceOri > ORI_W && pieceOri <= ORI_S)
            {
                tmpW = pieceOri - ORI_W;
                tmpS = ORI_S - pieceOri;

                if (tmpW < tmpS)
                    ori = CHESS_ORI_W;
                else
                    ori = CHESS_ORI_S;
            }
            else if (pieceOri > ORI_S && pieceOri <= ORI_E)
            {
                tmpS = pieceOri - ORI_S;
                tmpE = ORI_E - pieceOri;

                if (tmpS < tmpE)
                    ori = CHESS_ORI_S;
                else
                    ori = CHESS_ORI_E;
            }
            else if (pieceOri > ORI_E)
            {
                tmpE = pieceOri - ORI_E;
                tmpN = 6.28 + ORI_N - pieceOri;

                if (tmpE < tmpN)
                    ori = CHESS_ORI_E;
                else
                    ori = CHESS_ORI_N;
            }
            else if (pieceOri < ORI_N)
            {
                tmpW = 6.28 + pieceOri - ORI_E;
                tmpN = ORI_N - pieceOri;

                if (tmpE < tmpN)
                    ori = CHESS_ORI_E;
                else
                    ori = CHESS_ORI_N;
            }
        }
        #ifdef CHESS_DEBUG_INFO
        printf("\npieceori: %f, tmpW: %f, tmpE: %f, tmpN: %f, tmpS: %f", pieceOri, tmpW, tmpE, tmpN, tmpS);
        #endif
    }

    #ifdef CHESS_DEBUG_INFO
    printf("\nori: %i", ori);
    #endif

    chessBoard[tmpi][tmpj].ori = ori;

    Creature * cPiece = m_creature->GetMap()->GetCreature(piece);
    if (cPiece)
    {

        switch (ori)
        {
            case CHESS_ORI_N:
                cPiece->SetOrientation(ORI_N);
                break;
            case CHESS_ORI_E:
                cPiece->SetOrientation(ORI_E);
                break;
            case CHESS_ORI_S:
                cPiece->SetOrientation(ORI_S);
                break;
            case CHESS_ORI_W:
                cPiece->SetOrientation(ORI_W);
                break;
            default:
                break;
        }

        me->GetMap()->CreatureRelocation(cPiece, chessBoard[tmpi][tmpj].position.coord_x, chessBoard[tmpi][tmpj].position.coord_y, chessBoard[tmpi][tmpj].position.coord_z, cPiece->GetOrientation());

        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();

        if (!players.isEmpty())
            for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                if (Player* plr = itr->getSource())
                    cPiece->SendMonsterMove(chessBoard[tmpi][tmpj].position.coord_x, chessBoard[tmpi][tmpj].position.coord_y, chessBoard[tmpi][tmpj].position.coord_z, 0, plr);
    }
}

Creature * boss_MedivhAI::FindTrigger(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie FindTrigger(uint64 piece = %u)", piece);
    #endif
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].piece == piece)
                return m_creature->GetCreature(chessBoard[i][j].trigger);
        }
    }

    return NULL;
}

bool boss_MedivhAI::ChessSquareIsEmpty(uint64 trigger)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie ChessSquareIsEmpty(uint64 trigger = %u)", trigger);
    #endif
    if (IsInMoveList(trigger, true))
        return false;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].trigger == trigger)
            {
                if (chessBoard[i][j].piece)
                    return false;
                else
                    return true;
            }
        }
    }

    return false;
}

bool boss_MedivhAI::ChessSquareIsEmpty(int i, int j)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie ChessSquareIsEmpty(int i = %i, int j = %i)", i, j);
    #endif
    if (IsInMoveList(chessBoard[i][j].trigger))
        return false;

    if (chessBoard[i][j].piece)
        return false;

    return true;
}

bool boss_MedivhAI::CanMoveTo(uint64 trigger, uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie CanMoveTo(uint64 trigger = %u, uint64 piece = %u)", trigger, piece);
    #endif
    if (!trigger || !piece)
        return false;

    int moveRange = GetMoveRange(piece);
    bool inRange = IsInMoveRange(piece, trigger, moveRange);
    bool isEmpty = ChessSquareIsEmpty(trigger);
    #ifdef CHESS_DEBUG_INFO
    printf("\nCanMoveTo: moveRange %i, isInRange %i, isEmpty %i", moveRange, inRange, isEmpty);
    #endif
    return inRange && isEmpty;
}

void boss_MedivhAI::AddTriggerToMove(uint64 trigger, uint64 piece, bool player)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie AddTriggerToMove(uint64 trigger = %u, uint64 piece = %u, bool player = %i)", trigger, piece, player);
    #endif
    ChessTile tmp;
    tmp.piece = piece;
    tmp.trigger = trigger;

    moveList.push_back(tmp);

    uint16 tmpChance = urand(0, 100);

    //check, if tmpChance is higher than chanceToMove then medivh also can move one of his pieces
    if (player && tmpChance < chanceToMove)
        ChoosePieceToMove();
}

void boss_MedivhAI::MakeMoves()
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie MakeMoves()");
    #endif
    std::list<ChessTile>::iterator tmpItr;
    Creature * tmpC;
    for (std::list<ChessTile>::iterator itr = moveList.begin(); itr != moveList.end();)
    {
        tmpItr = itr;
        ++itr;

        if (tmpC = me->GetCreature((*tmpItr).trigger))
            ((move_triggerAI*)tmpC->AI())->MakeMove();

        moveList.erase(tmpItr);
    }
}

void boss_MedivhAI::ChangePlaceInBoard(uint64 piece, uint64 destTrigger)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie ChangePlaceInBoard(uint64 piece = %u, uint64 destTrigger = %u)", piece, destTrigger);
    #endif
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (chessBoard[i][j].piece == piece && chessBoard[i][j].trigger != destTrigger)
            {
                chessBoard[i][j].piece = 0;
                chessBoard[i][j].ori = CHESS_ORI_CHOOSE;
            }

            if (chessBoard[i][j].trigger == destTrigger)
            {
                chessBoard[i][j].piece = piece;
                chessBoard[i][j].ori = CHESS_ORI_CHOOSE;
            }
        }
    }
}

void boss_MedivhAI::ChangePieceFacing(uint64 piece, uint64 destTrigger)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie ChangePieceFacing(uint64 piece = %u, uint64 destTrigger = %u)", piece, destTrigger);
    #endif
    ChangePieceFacing(me->GetCreature(piece), me->GetCreature(destTrigger));
}

void boss_MedivhAI::ChangePieceFacing(Creature * piece, Creature * destTrigger)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie ChangePieceFacing(Creature * piece = %i, Creature * destTrigger = %i)", piece ? 1 : 0, destTrigger ? 1 : 0);
    #endif

    if (!piece || !destTrigger)
        return;

    piece->SetInFront(destTrigger);
    SetOrientation(piece->GetGUID());

    int tmpI = -1, tmpJ = -1;

    if (!FindPlaceInBoard(piece->GetGUID(), tmpI, tmpJ))
        return;

    Creature * tmpC = NULL;
    switch (chessBoard[tmpI][tmpJ].ori)
    {
        case CHESS_ORI_N:
            tmpC = me->GetCreature(chessBoard[tmpI - 1][tmpJ].trigger);
            break;
        case CHESS_ORI_E:
            tmpC = me->GetCreature(chessBoard[tmpI][tmpJ + 1].trigger);
            break;
        case CHESS_ORI_S:
            tmpC = me->GetCreature(chessBoard[tmpI + 1][tmpJ].trigger);
            break;
        case CHESS_ORI_W:
            tmpC = me->GetCreature(chessBoard[tmpI][tmpJ - 1].trigger);
            break;
    }

    if (tmpC)
        piece->SetFacingToObject(tmpC);
}

uint32 boss_MedivhAI::GetMoveSpell(uint64 piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetMoveSpell(uint64 piece = %u)", piece);
    #endif
    return GetMoveSpell(me->GetCreature(piece));
}

uint32 boss_MedivhAI::GetMoveSpell(Creature * piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie GetMoveSpell(Creature * piece = %i)", piece ? 1 : 0);
    #endif
    if (!piece)
        return 0;

    switch (piece->GetEntry())
    {
        case NPC_QUEEN_A:
        case NPC_QUEEN_H:
            return SPELL_MOVE_4;
        case NPC_KING_A:
        case NPC_KING_H:
            return SPELL_MOVE_5;
        case NPC_BISHOP_A:
        case NPC_BISHOP_H:
            return SPELL_MOVE_6;
        case NPC_KNIGHT_A:
        case NPC_KNIGHT_H:
            return SPELL_MOVE_3;
        case NPC_ROOK_A:
        case NPC_ROOK_H:
            return SPELL_MOVE_7;
        case NPC_PAWN_A:
        case NPC_PAWN_H:
            return SPELL_MOVE_1;
    }

    return 0;
}

bool boss_MedivhAI::FindPlaceInBoard(uint64 unit, int & i, int & j)
{
    #ifdef CHESS_DEBUG_INFO
    printf("\nWywolanie: FindPlaceInBoard(uint64 unit = %u, int & i = %i, int & j = %i)", unit, i, j);
    #endif
    for (int x = 0; x < 8; ++x)
    {
        for (int y = 0; y < 8; ++y)
        {
            if (chessBoard[x][y].piece == unit || chessBoard[x][y].trigger == unit)
            {
                i = x;
                j = y;
                return true;
            }
        }
    }
    return false;
}

void boss_MedivhAI::ChoosePieceToMove()
{
    #ifdef DISSABLE_MEDIVH_PIECES_MOVEMENT
    return;
    #endif

    #ifdef CHESS_DEBUG_INFO
    printf("\n Wywolanie ChoosePieceToMove()");
    #endif

    //TODO: add some kind of check to test if actual position is better than other positions

    std::list<Priority> tmpList;
    std::list<ChessPosition> emptySquareList;

    int prioritySum = 0;

    //Select Medivh piece to move
    //tmpList used to store move priority
    for (std::list<uint64>::iterator i = medivhSidePieces.begin(); i != medivhSidePieces.end(); ++i)
    {
        //check neighbours and modify priority
        //higher priority for pieces with more than 1 enemy in melee range
        //higher priority for healers if enemy in melee range  ??
        //set priority 0 for pieces which can't move anywhere
        Priority tempPriority;
        tempPriority.prior = 0;
        tempPriority.GUID = *i;

        if (IsEmptySquareInRange(*i, GetMoveRange(*i)))
        {
            //if piece can move anywhere then modify move priority

            tempPriority.prior = START_PRIORITY + GetCountOfEnemyInMelee(*i) * MELEE_PRIORITY + urand(0, RAND_PRIORITY) + GetLifePriority(*i);
        }

        prioritySum += tempPriority.prior;
        tmpList.push_back(tempPriority);
    }
    #ifdef CHESS_DEBUG_INFO
    printf("\ntempPriorityList size: %i, priorsum: %i", tmpList.size(), prioritySum);
    #endif
    int chosen = urand(0, prioritySum), prevPrior = 0;
    ChessPosition chosenPiece;

    for (std::list<Priority>::iterator i = tmpList.begin(); i!= tmpList.end(); ++i)
    {
        if (prevPrior < chosen && prevPrior + (*i).prior >= chosen)
        {
            chosenPiece.GUID = (*i).GUID;
            break;
        }
        prevPrior += (*i).prior;
    }

    // create empty square list

    int tmpI = -1, tmpJ = -1, i, j, tmpOffsetI, tmpOffsetJ;

    FindPlaceInBoard(chosenPiece.GUID, tmpI, tmpJ);

    if (tmpI < 0 || tmpJ < 0)
    {
        Creature * uChosen = m_creature->GetCreature(chosenPiece.GUID);
        if (uChosen)
            uChosen->Say("AddTriggerToMove(..) : Nie znaleziono mnie na planszy !!", LANG_UNIVERSAL, NULL);
        else
            me->Say("wybrany nie znaleziony", LANG_UNIVERSAL, NULL);
        return;
    }
    #ifdef CHESS_DEBUG_INFO
    printf("\nChosenGUID: %u | tmpI %i, tmpJ %i", chosenPiece.GUID, tmpI, tmpJ);
    #endif
    switch (GetMoveRange(chosenPiece.GUID))
    {
        case 25:
            for (i = 0; i < OFFSET25COUNT; ++i)
            {
                tmpOffsetI = tmpI + offsetTab25[i][0];
                tmpOffsetJ = tmpJ + offsetTab25[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                {
                   if (ChessSquareIsEmpty(tmpOffsetI, tmpOffsetJ))
                        emptySquareList.push_back(ChessPosition(chessBoard[tmpOffsetI][tmpOffsetJ].trigger, tmpOffsetI, tmpOffsetJ));
                }
            }
        case 20:
            for (i = 0; i < OFFSET20COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab20[i][0];
                tmpOffsetJ = tmpJ + offsetTab20[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                {
                   if (ChessSquareIsEmpty(tmpOffsetI, tmpOffsetJ))
                        emptySquareList.push_back(ChessPosition(chessBoard[tmpOffsetI][tmpOffsetJ].trigger, tmpOffsetI, tmpOffsetJ));
                }
            }
        case 15:
            for (i = 0; i < OFFSET15COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab15[i][0];
                tmpOffsetJ = tmpJ + offsetTab15[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                {
                   if (ChessSquareIsEmpty(tmpOffsetI, tmpOffsetJ))
                        emptySquareList.push_back(ChessPosition(chessBoard[tmpOffsetI][tmpOffsetJ].trigger, tmpOffsetI, tmpOffsetJ));
                }
            }
        case 8:
            for (i = 0; i < OFFSET8COUNT; i++)
            {
                tmpOffsetI = tmpI + offsetTab8[i][0];
                tmpOffsetJ = tmpJ + offsetTab8[i][1];

                if (tmpOffsetI >= 0 && tmpOffsetI < 8 &&
                    tmpOffsetJ >= 0 && tmpOffsetJ < 8)
                {
                   if (ChessSquareIsEmpty(tmpOffsetI, tmpOffsetJ))
                        emptySquareList.push_back(ChessPosition(chessBoard[tmpOffsetI][tmpOffsetJ].trigger, tmpOffsetI, tmpOffsetJ));
                }
            }
            break;
        default:
            break;
    }
    #ifdef CHESS_DEBUG_INFO
    printf("\nemptySquareList size: %i", emptySquareList.size());
    #endif
    if (emptySquareList.empty())
        return;

    // calculate and add square move priority
    tmpList.clear();
    prioritySum = 0;

    for (std::list<ChessPosition>::iterator i = emptySquareList.begin(); i != emptySquareList.end(); ++i)
    {
        Priority tmpPrior;
        tmpPrior.prior = START_PRIORITY;
        tmpPrior.GUID = (*i).GUID;

        switch (GetCountOfEnemyInMelee((*i).GUID))
        {
            case 1:
                tmpPrior.prior += MELEE_ENEMY_COUNT_PRIOR_MOD_1;
                break;
            case 2:
                tmpPrior.prior += MELEE_ENEMY_COUNT_PRIOR_MOD_2;
                break;
            case 3:
                tmpPrior.prior += MELEE_ENEMY_COUNT_PRIOR_MOD_3;
                break;
            case 4:
                tmpPrior.prior += MELEE_ENEMY_COUNT_PRIOR_MOD_4;
                break;
            default:
                break;
        }

        // modify priority for moving back
        if (pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE)
        {
            if ((*i).i > chosenPiece.i)
                tmpPrior.prior += MOVE_BACK_PRIOR_MOD;
        }
        else
        {
            if ((*i).i < chosenPiece.i)
                tmpPrior.prior += MOVE_BACK_PRIOR_MOD;
        }

        // modify priority for moving to left or right
        if ((*i).j != chosenPiece.j)
            tmpPrior.prior += MOVE_STRAFE_PRIOR_MOD;

        if (IsKing((*i).GUID))
            tmpPrior.prior += ATTACK_KING_PRIOR;

        if (IsHealer((*i).GUID))
            tmpPrior.prior += ATTACK_HEALER_PRIOR;

        prioritySum += tmpPrior.prior;
        tmpList.push_back(tmpPrior);
    }

    chosen = urand(0, prioritySum);
    prevPrior = 0;
    uint64 chosenTriggerGUID = 0;

    for (std::list<Priority>::iterator i = tmpList.begin(); i!= tmpList.end(); ++i)
    {
        if (prevPrior < chosen && prevPrior + (*i).prior >= chosen)
        {
            chosenTriggerGUID = (*i).GUID;
            break;
        }
        prevPrior += (*i).prior;
    }

    Creature * tmpC = me->GetCreature(chosenPiece.GUID);
    Creature * tmpT = me->GetCreature(chosenTriggerGUID);

    if (tmpC && tmpT)
        tmpC->CastSpell(tmpT, GetMoveSpell(tmpC), false);
    else
    {
        #ifdef CHESS_DEBUG_INFO
        printf("chosenGUID: %i, %i | chosenTriggerGUID: %i, %i", chosenPiece.GUID, tmpC ? 1 : 0, chosenTriggerGUID, tmpT ? 1 : 0);
        #endif
        me->Say("Cosik mi sie wybrany pionek albo trigger gdzies zgubil", LANG_UNIVERSAL, NULL);
    }
}

uint32 boss_MedivhAI::GetDeadEntryForPiece(Creature * piece)
{
    #ifdef CHESS_DEBUG_INFO
    printf("GetDeadEntryForPiece(Creature * piece = %i)", piece);
    #endif
    if (!piece)
        return 0;

    return GetDeadEntryForPiece(piece->GetEntry());
}

uint32 boss_MedivhAI::GetDeadEntryForPiece(uint32 entry)
{
    #ifdef CHESS_DEBUG_INFO
    printf("GetDeadEntryForPiece(uint32 entry = %u)", entry);
    #endif

    switch (entry)
    {
        case NPC_PAWN_H:
            return 16556;
        case NPC_PAWN_A:
            return 16567;
        case NPC_KNIGHT_H:
            return 16561;
        case NPC_KNIGHT_A:
            return 16569;
        case NPC_QUEEN_H:
            return 16557;
        case NPC_QUEEN_A:
            return 16572;
        case NPC_BISHOP_H:
            return 16560;
        case NPC_BISHOP_A:
            return 16571;
        case NPC_ROOK_H:
            return 16562;
        case NPC_ROOK_A:
            return 16570;
        case NPC_KING_H:
            return 16563;
        case NPC_KING_A:
            return 16581;
        default:
            return 0;
    }

    return 0;
}

//other

bool GossipHello_npc_chesspiece(Player* player, Creature* _Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    if (!pInstance)
        return false;

    //if(pInstance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS &&
    //   _Creature->GetEntry() != NPC_KING_A && _Creature->GetEntry() != NPC_KING_H)
    //    return true;

    if(pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE && _Creature->getFaction() != A_FACTION)
        return false;

    if(pInstance->GetData(CHESS_EVENT_TEAM) == HORDE && _Creature->getFaction() != H_FACTION)
        return false;

    if (player->HasAura(SPELL_RECENTLY_IN_GAME, 0))
    {
        player->SEND_GOSSIP_MENU(10505, _Creature->GetGUID());
        return false;
    }

    if(!(_Creature->isPossessedByPlayer()))
    {
        switch (_Creature->GetEntry())
        {
            case NPC_PAWN_H:
                player->ADD_GOSSIP_ITEM(0, "Control Orc Grunt", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10425, _Creature->GetGUID());
                break;
            case NPC_PAWN_A:
                player->ADD_GOSSIP_ITEM(0, "Control Human Footman", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(8952, _Creature->GetGUID());
                break;
            case NPC_KNIGHT_H:
                player->ADD_GOSSIP_ITEM(0, "Control Orc Wolf", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10439, _Creature->GetGUID());
                break;
            case NPC_KNIGHT_A:
                player->ADD_GOSSIP_ITEM(0, "Control Human Charger", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10414, _Creature->GetGUID());
                break;
            case NPC_QUEEN_H:
                player->ADD_GOSSIP_ITEM(0, "Control Orc Warlock", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10440, _Creature->GetGUID());
                break;
            case NPC_QUEEN_A:
                player->ADD_GOSSIP_ITEM(0, "Control Human Conjurer", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10417, _Creature->GetGUID());
                break;
            case NPC_BISHOP_H:
                player->ADD_GOSSIP_ITEM(0, "Control Orc Necrolyte", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10434, _Creature->GetGUID());
                break;
            case NPC_BISHOP_A:
                player->ADD_GOSSIP_ITEM(0, "Control Human Cleric", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10416, _Creature->GetGUID());
                break;
            case NPC_ROOK_H:
                player->ADD_GOSSIP_ITEM(0, "Control Summoned Daemon", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10426, _Creature->GetGUID());
                break;
            case NPC_ROOK_A:
                player->ADD_GOSSIP_ITEM(0, "Control Conjured Water Elemental", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10413, _Creature->GetGUID());
                break;
            case NPC_KING_H:
                player->ADD_GOSSIP_ITEM(0, "Control Warchief Blackhand", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10442, _Creature->GetGUID());
                break;
            case NPC_KING_A:
                player->ADD_GOSSIP_ITEM(0, "Control King Llane", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(10418, _Creature->GetGUID());
                break;
        }
    }

    return true;
}

bool GossipSelect_npc_chesspiece(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());
    if(action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        if (_Creature->GetEntry() == NPC_KING_A || _Creature->GetEntry() == NPC_KING_H)
        {
            pInstance->SetData(DATA_CHESS_EVENT, IN_PROGRESS);
            ((boss_MedivhAI*)(_Creature->GetCreature(pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH))->AI()))->StartEvent();
        }

        player->TeleportTo(_Creature->GetMapId(), -11108.2, -1841.56, 229.625, 5.39745);
        player->CastSpell(_Creature, SPELL_POSSES_CHESSPIECE, true);
    }

    player->CLOSE_GOSSIP_MENU();

    return true;
}

bool GossipHello_npc_echo_of_medivh(Player* player, Creature* _Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    if(pInstance->GetData(DATA_CHESS_EVENT) == IN_PROGRESS)
        return false;

    if(pInstance->GetData(DATA_CHESS_EVENT) == FAIL)
        pInstance->SetData(DATA_CHESS_EVENT, NOT_STARTED);

    if(pInstance->GetData(DATA_CHESS_EVENT) == DONE) //for testing
        pInstance->SetData(DATA_CHESS_EVENT, NOT_STARTED);

    if(pInstance->GetData(DATA_CHESS_EVENT) == NOT_STARTED)
    {
        player->ADD_GOSSIP_ITEM(0, EVENT_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        //if (player->isGameMaster())
        //    player->ADD_GOSSIP_ITEM(2, "Start Debug Mode", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    }

    player->SEND_GOSSIP_MENU(10506, _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_echo_of_medivh(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    if(action == GOSSIP_ACTION_INFO_DEF)
    {
        DoScriptText(SCRIPTTEXT_AT_EVENT_START,_Creature);
        //pInstance->SetData(DATA_CHESS_EVENT, IN_PROGRESS);

        ((boss_MedivhAI*)_Creature->AI())->StartMiniEvent();

        pInstance->SetData(CHESS_EVENT_TEAM, player->GetTeam());
        _Creature->GetMotionMaster()->MoveRandom(10);
    }

    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pInstance->SetData(DATA_CHESS_EVENT, SPECIAL);
        ((ScriptedAI*)_Creature->AI())->DoSay("Debug mode on", LANG_UNIVERSAL, _Creature);
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
    newscript->pGossipHello = &GossipHello_npc_chesspiece;
    newscript->pGossipSelect = &GossipSelect_npc_chesspiece;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_echo_of_medivh";
    newscript->GetAI = &GetAI_npc_Medivh;
    newscript->pGossipHello = &GossipHello_npc_echo_of_medivh;
    newscript->pGossipSelect = &GossipSelect_npc_echo_of_medivh;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "chess_move_trigger";
    newscript->GetAI = &GetAI_move_trigger;
    newscript->RegisterSelf();
}
