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
    printf("\n Wywolanie move_triggerAI::Reset()");
    moveTimer   = 10000;
    pieceStance = PIECE_NONE;
    unitToMove = 0;
    MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);
}

void move_triggerAI::SpellHit(Unit *caster,const SpellEntry *spell)
{
    printf("\n Wywolanie SpellHit(Unit *caster,const SpellEntry *spell)");
    if (!MedivhGUID)
        MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);

    printf("\nmove_triggerAI:SpellHit(...): stance: %i, medivh: %u, id: %i\n", pieceStance, MedivhGUID, spell->Id);
    if (pieceStance != PIECE_NONE || !MedivhGUID)
        return;

    if(spell->Id == SPELL_MOVE_1 || spell->Id == SPELL_MOVE_2 || spell->Id == SPELL_MOVE_3 || spell->Id == SPELL_MOVE_4 ||
       spell->Id == SPELL_MOVE_5 || spell->Id == SPELL_MOVE_6 || spell->Id == SPELL_MOVE_7 || spell->Id == SPELL_CHANGE_FACING)
    {
        boss_MedivhAI * medivh = (boss_MedivhAI*)(m_creature->GetUnit(*m_creature, MedivhGUID));
        if (medivh)
        {
            if (medivh->CanMoveTo(m_creature->GetGUID(), caster->GetGUID()))
            {
                medivh->AddTriggerToMove(m_creature->GetGUID(), caster->GetGUID(), caster->isPossessedByPlayer() ? true : false);

                DoCast(m_creature, SPELL_MOVE_PREVISUAL);

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
            DoSay("Medivh not found ... you can't move", LANG_UNIVERSAL, m_creature);
    }
}

void move_triggerAI::MakeMove()
{
    printf("\n Wywolanie MakeMove()");
    ChessPiecesStances tmpStance = pieceStance;

    moveTimer = 10000;
    pieceStance = PIECE_NONE;
    Unit * temp = m_creature->GetUnit(*m_creature, unitToMove);

    if (!temp || !temp->isAlive())
    {
        unitToMove = 0;
        return;
    }

    switch (tmpStance)
    {
    case PIECE_MOVE:
        DoCast(m_creature,SPELL_MOVE_MARKER);
        temp->StopMoving();
        temp->GetMotionMaster()->Clear();
        temp->GetMotionMaster()->MovePoint(0, wLoc.x, wLoc.y, wLoc.z);
        break;
    case PIECE_CHANGE_FACING:
        temp->SetInFront(m_creature);
        temp = NULL;
        temp = m_creature->GetUnit(*m_creature, MedivhGUID);
        if (temp)
        {
            ((boss_MedivhAI*)temp)->SetOrientation(unitToMove);
        }
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
}

void npc_chesspieceAI::EnterEvadeMode()
{
    printf("\n Wywolanie EnterEvadeMode()");
    // to prevent reset chess pieces after unpossess
    if (pInstance->GetData(DATA_CHESS_EVENT) == IN_PROGRESS ||
        pInstance->GetData(DATA_CHESS_EVENT) == SPECIAL)
        return;

    ScriptedAI::EnterEvadeMode();
}

bool npc_chesspieceAI::IsHealingSpell(uint32 spell)
{
    printf("\n Wywolanie IsHealingSpell(uint32 spell)");
    if (spell == SPELL_BISHOP_A_1 || spell == SPELL_BISHOP_H_1)
        return true;

    return false;
}

int npc_chesspieceAI::GetAbilityRange(uint32 spell)
{
    printf("\n Wywolanie GetAbilityRange(uint32 spell)");
    switch(spell)
    {
        case SPELL_QUEEN_H_1:
        case SPELL_QUEEN_A_1:
            return 20;

        case SPELL_BISHOP_H_1:
        case SPELL_BISHOP_A_1:
        case SPELL_QUEEN_H_2:
        case SPELL_QUEEN_A_2:
            return 25;
        default:
            return 0;
    }
}

void npc_chesspieceAI::SetSpellsAndCooldowns()
{
    printf("\n Wywolanie SetSpellsAndCooldowns()");
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
    printf("\n Wywolanie npc_chesspieceAI::Reset()");
    ReturnToHome = true;
    InGame = true;
    CanMove = false;
    m_creature->setActive(true);

    SetSpellsAndCooldowns();

    ability1Chance = urand(ABILITY_1_CHANCE_MIN, ABILITY_1_CHANCE_MAX);
    ability2Chance = urand(ABILITY_2_CHANCE_MIN, ABILITY_2_CHANCE_MAX);

    nextTryTimer = urand(500, 5000);
}

void npc_chesspieceAI::MovementInform(uint32 MovementType, uint32 Data)
{
    printf("\n Wywolanie MovementInform(uint32 MovementType, uint32 Data)");
    if (MovementType != POINT_MOTION_TYPE)
        return;

    this->npc_medivh = m_creature->GetMap()->GetCreature(this->MedivhGUID);
    if (npc_medivh)
        ((boss_MedivhAI*)npc_medivh)->SetOrientation(m_creature->GetGUID());
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
    printf("\n Wywolanie JustRespawned()");
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

void npc_chesspieceAI::OnCharmed(bool apply)
{
    printf("\n Wywolanie OnCharmed(bool apply)");
    // Place to disable rotate and move for player on possess
    m_creature->SetInCombatState(false);
}

void npc_chesspieceAI::UpdateAI(const uint32 diff)
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

    if(!ReturnToHome) // for fail
        ReturnToHome = true;

    if(!InGame)
        return;

    if(!m_creature->isPossessed())
    {
        //TO DO: add shared cooldown for some spells
        if (ability1Timer < diff)
        {
            if (urand(0, ABILITY_CHANCE_MAX) > ability1Chance)
            {
                Unit * medivh = m_creature->GetUnit(*m_creature, MedivhGUID);
                if (!medivh)
                    return;

                uint64 victim = ((boss_MedivhAI*)medivh)->GetSpellTarget(m_creature->GetGUID(), GetAbilityRange(ability1ID), IsHealingSpell(ability1ID));

                Unit * uVictim = m_creature->GetUnit(*m_creature, victim);
                AddSpellToCast(uVictim, ability1ID);
                ability1Timer = ability1Cooldown;
            }
            else
                ability1Timer = nextTryTimer;
        }
        else
            ability1Timer -= diff;

        if (ability2Timer < diff)
        {
            if (urand(0, ABILITY_CHANCE_MAX) > ability2Chance)
            {
                Unit * medivh = m_creature->GetUnit(*m_creature, this->MedivhGUID);
                if (!medivh)
                    return;

                uint64 victim = ((boss_MedivhAI*)medivh)->GetSpellTarget(m_creature->GetGUID(), GetAbilityRange(ability2ID), IsHealingSpell(ability2ID));

                Unit * uVictim = m_creature->GetUnit(*m_creature, victim);
                AddSpellToCast(uVictim, ability2ID);
                ability2Timer = ability2Cooldown;
            }
            else
                ability2Timer = nextTryTimer;
        }
        else
            ability2Timer -= diff;

        /*if (attackTimer < diff)
        {
            attackTimer = attackCooldown;
            Unit * medivh = m_creature->GetUnit(*m_creature, this->MedivhGUID);

            if (!medivh)
                return;

            Unit * uVictim = m_creature->GetUnit(*m_creature, ((boss_MedivhAI*)medivh)->GetMeleeTarget(m_creature->GetGUID()));
            if (uVictim)
                m_creature->DealDamage(uVictim, attackDamage, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        else
            attackTimer -= diff;*/

    }
    //DoMeleeAttackIfReady();

    CastNextSpellIfAnyAndReady();
}

void npc_chesspieceAI::DamageTaken(Unit * done_by, uint32 &damage)
{
    printf("\n Wywolanie DamageTaken(Unit * done_by, uint32 &damage)");
    if (damage > m_creature->GetHealth())
    {
        //damage = 0;
        if (m_creature->isPossessed())
            m_creature->RemoveAurasDueToSpell(SPELL_POSSES_CHESSPIECE);

        this->npc_medivh = m_creature->GetMap()->GetCreature(this->MedivhGUID);
        if (npc_medivh)
            ((boss_MedivhAI*)npc_medivh)->RemoveChessPieceFromBoard(m_creature->GetGUID());
        else
        {
            DoSay("I'm dying ... but Medivh not found ...", LANG_UNIVERSAL, m_creature, false);
            //damage = m_creature->GetHealth() + 1;
        }
    }
}

//Medivh AI

boss_MedivhAI::boss_MedivhAI(Creature *c) : ScriptedAI(c)
{
    pInstance = ((instance_karazhan*)m_creature->GetInstanceData());
    m_creature->GetPosition(wLoc);
    tpLoc.x = -11108.2;
    tpLoc.y = -1841.56;
    tpLoc.z = 229.625;
    tpLoc.o = 5.39745;

    this->chanceToMove = urand(MIN_MOVE_CHANCE, MAX_MOVE_CHANCE);

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

    // Horde
    // A0
    chessBoard[0][0].position.x = -11077.7;
    chessBoard[0][0].position.y = -1849.0;

    // B0
    chessBoard[0][1].position.x = -11074.0;
    chessBoard[0][1].position.y = -1853.3;

    // C0
    chessBoard[0][2].position.x = -11070.8;
    chessBoard[0][2].position.y = -1857.7;

    // D0
    chessBoard[0][3].position.x = -11067.0;
    chessBoard[0][3].position.y = -1861.9;

    // E0
    chessBoard[0][4].position.x = -11063.6;
    chessBoard[0][4].position.y = -1866.5;

    // F0
    chessBoard[0][5].position.x = -11060.3;
    chessBoard[0][5].position.y = -1870.8;

    // G0
    chessBoard[0][6].position.x = -11056.7;
    chessBoard[0][6].position.y = -1875.0;

    // H0
    chessBoard[0][7].position.x = -11053.4;
    chessBoard[0][7].position.y = -1879.6;

    // A1
    chessBoard[1][0].position.x = -11082.1;
    chessBoard[1][0].position.y = -1852.4;

    // B1
    chessBoard[1][1].position.x = -11078.5;
    chessBoard[1][1].position.y = -1856.9;

    // C1
    chessBoard[1][2].position.x = -11075.0;
    chessBoard[1][2].position.y = -1861.1;

    // D1
    chessBoard[1][3].position.x = -11071.5;
    chessBoard[1][3].position.y = -1865.5;

    // E1
    chessBoard[1][4].position.x = -11067.8;
    chessBoard[1][4].position.y = -1869.9;

    // F1
    chessBoard[1][5].position.x = -11064.3;
    chessBoard[1][5].position.y = -1874.3;

    // G1
    chessBoard[1][6].position.x = -11061.0;
    chessBoard[1][6].position.y = -1878.6;

    // H1
    chessBoard[1][7].position.x = -11057.4;
    chessBoard[1][7].position.y = -1883.0;
    // end Horde

    // Empty
    // A2
    chessBoard[2][0].position.x = -11086.5;
    chessBoard[2][0].position.y = -1855.8;

    // B2
    chessBoard[2][1].position.x = -11083.0;
    chessBoard[2][1].position.y = -1860.6;

    // C2
    chessBoard[2][2].position.x = -11079.6;
    chessBoard[2][2].position.y = -1864.8;

    // D2
    chessBoard[2][3].position.x = -11076.2;
    chessBoard[2][3].position.y = -1868.9;

    // E2
    chessBoard[2][4].position.x = -11072.5;
    chessBoard[2][4].position.y = -1873.5;

    // F2
    chessBoard[2][5].position.x = -11069.1;
    chessBoard[2][5].position.y = -1877.8;

    // G2
    chessBoard[2][6].position.x = -11065.5;
    chessBoard[2][6].position.y = -1882.3;

    // H2
    chessBoard[2][7].position.x = -11062.2;
    chessBoard[2][7].position.y = -1886.5;

    // A3
    chessBoard[2][0].position.x = -11090.6;
    chessBoard[2][0].position.y = -1859.3;

    // B3
    chessBoard[3][1].position.x = -11087.0;
    chessBoard[3][1].position.y = -1863.7;

    // C3
    chessBoard[3][2].position.x = -11083.4;
    chessBoard[3][2].position.y = -1868.1;

    // D3
    chessBoard[3][3].position.x = -11080.4;
    chessBoard[3][3].position.y = -1872.4;

    // E3
    chessBoard[3][4].position.x = -11076.7;
    chessBoard[3][4].position.y = -1876.7;

    // F3
    chessBoard[3][5].position.x = -11073.2;
    chessBoard[3][5].position.y = -1889.4;

    // G3
    chessBoard[3][6].position.x = -11069.9;
    chessBoard[3][6].position.y = -1885.6;

    // H3
    chessBoard[3][7].position.x = -11066.2;
    chessBoard[3][7].position.y = -1889.9;

    // A4
    chessBoard[4][0].position.x = -11095.0;
    chessBoard[4][0].position.y = -1862.8;

    // B4
    chessBoard[4][1].position.x = -11091.5;
    chessBoard[4][1].position.y = -1867.3;

    // C4
    chessBoard[4][2].position.x = -11088.1;
    chessBoard[4][2].position.y = -1871.2;

    // D4
    chessBoard[4][3].position.x = -11084.3;
    chessBoard[4][3].position.y = -1875.8;

    // E4
    chessBoard[4][4].position.x = -11081.1;
    chessBoard[4][4].position.y = -1880.2;

    // F4
    chessBoard[4][5].position.x = -11077.7;
    chessBoard[4][5].position.y = -1884.6;

    // G4
    chessBoard[4][6].position.x = -11074.2;
    chessBoard[4][6].position.y = -1888.9;

    // H4
    chessBoard[4][7].position.x = -11070.6;
    chessBoard[4][7].position.y = -1893.4;

    // A5
    chessBoard[5][0].position.x = -11099.5;
    chessBoard[5][0].position.y = -1866.3;

    // B5
    chessBoard[5][1].position.x = -11095.7;
    chessBoard[5][1].position.y = -1870.7;

    // C5
    chessBoard[5][2].position.x = -11092.3;
    chessBoard[5][2].position.y = -1875.0;

    // D5
    chessBoard[5][3].position.x = -11088.8;
    chessBoard[5][3].position.y = -1879.5;

    // E5
    chessBoard[5][4].position.x = -11085.2;
    chessBoard[5][4].position.y = -1883.9;

    // F5
    chessBoard[5][5].position.x = -11082.1;
    chessBoard[5][5].position.y = -1888.3;

    // G5
    chessBoard[5][6].position.x = -11078.5;
    chessBoard[5][6].position.y = -1892.5;

    // H5
    chessBoard[5][7].position.x = -11075.0;
    chessBoard[5][7].position.y = -1897.0;
    // end Empty

    // Alliance
    // A6
    chessBoard[6][0].position.x = -11103.7;
    chessBoard[6][0].position.y = -1869.6;

    // B6
    chessBoard[6][1].position.x = -11100.2;
    chessBoard[6][1].position.y = -1874.0;

    // C6
    chessBoard[6][2].position.x = -11096.8;
    chessBoard[6][2].position.y = -1878.2;

    // D6
    chessBoard[6][3].position.x = -11093.4;
    chessBoard[6][3].position.y = -1882.6;

    // E6
    chessBoard[6][4].position.x = -11089.7;
    chessBoard[6][4].position.y = -1887.0;

    // F6
    chessBoard[6][5].position.x = -11086.2;
    chessBoard[6][5].position.y = -1891.5;

    // G6
    chessBoard[6][6].position.x = -11082.9;
    chessBoard[6][6].position.y = -1895.8;

    // H6
    chessBoard[6][7].position.x = -11079.4;
    chessBoard[6][7].position.y = -1900.2;

    // A7
    chessBoard[7][0].position.x = -11108.1;
    chessBoard[7][0].position.y = -1872.9;

    // B7
    chessBoard[7][1].position.x = -11104.4;
    chessBoard[7][1].position.y = -1877.7;

    // C7
    chessBoard[7][2].position.x = -11101.0;
    chessBoard[7][2].position.y = -1881.2;

    // D7
    chessBoard[7][3].position.x = -11097.6;
    chessBoard[7][3].position.y = -1886.4;

    // E7
    chessBoard[7][4].position.x = -11094.1;
    chessBoard[7][4].position.y = -1890.6;

    // F7
    chessBoard[7][5].position.x = -11090.5;
    chessBoard[7][5].position.y = -1895.0;

    // G7
    chessBoard[7][6].position.x = -11087.2;
    chessBoard[7][6].position.y = -1899.6;

    // H7
    chessBoard[7][7].position.x = -11083.7;
    chessBoard[7][7].position.y = -1903.9;

    //end Alliance
}

int boss_MedivhAI::GetMoveRange(uint64 piece)
{
    printf("\n Wywolanie GetMoveRange(uint64 piece)");
    return (GetMoveRange(m_creature->GetUnit(*m_creature, piece)));
}

int boss_MedivhAI::GetMoveRange(Unit * piece)
{
    printf("\n Wywolanie GetMoveRange(Unit * piece)");
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

int boss_MedivhAI::GetCountOfEnemyInMelee(uint64 piece)
{
    printf("\n Wywolanie GetCountOfEnemyInMelee(uint64 piece)");
    int tmpCount = 0;


    int tmpI = -1, tmpJ = -1;

    //search for position in tab of piece

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].piece == piece)
            {
                tmpI = i; tmpJ = j;
                break;
            }
        }
        //if we find location of piece
        if (tmpI >= 0 && tmpJ >= 0)
            break;
    }

    if (tmpI+1 < 8)
        if (chessBoard[tmpI+1][tmpJ].piece)
            tmpCount++;

    if (tmpI-1 >= 0)
        if (chessBoard[tmpI-1][tmpJ].piece)
            tmpCount++;

    if (tmpJ+1 < 8)
        if (chessBoard[tmpI][tmpJ+1].piece)
            tmpCount++;

    if (tmpJ-1 >= 0)
        if (chessBoard[tmpI][tmpJ-1].piece)
            tmpCount++;

    return tmpCount;
}

int boss_MedivhAI::GetLifePriority(uint64 piece)
{
    printf("\n Wywolanie GetLifePriority(uint64 piece)");
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
    printf("\n Wywolanie GetAttackPriority(uint64 piece)");
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
    printf("\n Wywolanie IsEmptySquareInRange(uint64 piece, int range)");
    if (!piece || !range)
        return false;

    int tmpI = -1, tmpJ = -1, tmpOffsetI, tmpOffsetJ;
    int8 i;

    //search for position in tab of piece

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].piece == piece)
            {
                tmpI = i; tmpJ = j;
                break;
            }
        }
        //if we find location of piece
        if (tmpI >= 0 && tmpJ >= 0)
            break;
    }

    if (tmpI < 0 || tmpJ < 0)
    {
        Unit * uPiece = m_creature->GetUnit(*m_creature, piece);

        if (uPiece)
            ((ScriptedAI*)uPiece)->DoSay("IsEmptySquareInRange(..) : Nie znaleziono mnie na planszy !!", LANG_UNIVERSAL, uPiece,  false);
        return false;
    }

    switch (range)
    {
        case 20:
            for (i = 0; i < OFFSET20COUNT; i++)
            {
                tmpOffsetI = offsetTab20[i][0];
                tmpOffsetJ = offsetTab20[i][1];
                if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                    tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                {
                    if (!chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece)
                        return true;
                }
            }
        case 15:
            for (i = 0; i < OFFSET15COUNT; i++)
            {
                tmpOffsetI = offsetTab15[i][0];
                tmpOffsetJ = offsetTab15[i][1];
                if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                    tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                {
                    if (!chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece)
                        return true;
                }
            }
        case 8:
            for (i = 0; i < OFFSET8COUNT; i++)
            {
                tmpOffsetI = offsetTab8[i][0];
                tmpOffsetJ = offsetTab8[i][1];
                if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                    tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                {
                    if (!chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece)
                        return true;
                }
            }
            break;
        default:
            break;
    }

    return false;
}

uint64 boss_MedivhAI::GetSpellTarget(uint64 caster, int range, bool heal)
{
    printf("\n Wywolanie GetSpellTarget(uint64 caster, int range, bool heal)");
    if (!range)
        return caster;

    int8 tmpI = -1, tmpJ = -1, i, tmpOffsetI, tmpOffsetJ;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].piece == caster)
            {
                tmpI = i; tmpJ = j;
                break;
            }
        }
        //if we find location of piece
        if (tmpI >= 0 && tmpJ >= 0)
            break;
    }

    if (tmpI < 0 || tmpJ < 0)
    {
        Unit * uCaster = m_creature->GetUnit(*m_creature, caster);
        if (uCaster)
            ((ScriptedAI*)uCaster)->DoSay("GetSpellTarget(..) : Nie znaleziono mnie na planszy !!", LANG_UNIVERSAL, uCaster, false);
        return 0;
    }

    int priority = START_PRIORITY, prevPriority = 0;

    std::list<Priority> tmpList;
    std::list<uint64> tmpPossibleTargetsList;
    uint64 tmpGUID;

    if (heal)
    {
        //create possible target list

        switch (range)
        {
            case 25:
                for (i = 0; i < OFFSET25COUNT; i++)
                {
                    tmpOffsetI = offsetTab25[i][0];
                    tmpOffsetJ = offsetTab25[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 20:
                for (i = 0; i < OFFSET20COUNT; i++)
                {
                    tmpOffsetI = offsetTab20[i][0];
                    tmpOffsetJ = offsetTab20[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 15:
                for (i = 0; i < OFFSET15COUNT; i++)
                {
                    tmpOffsetI = offsetTab15[i][0];
                    tmpOffsetJ = offsetTab15[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 8:
                for (i = 0; i < OFFSET8COUNT; i++)
                {
                    tmpOffsetI = offsetTab8[i][0];
                    tmpOffsetJ = offsetTab8[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece;

                        if (tmpGUID && IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
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
    else        //if !heal
    {
        //create possible targets list

        switch (range)
        {
            case 25:
                for (i = 0; i < OFFSET25COUNT; i++)
                {
                    tmpOffsetI = offsetTab25[i][0];
                    tmpOffsetJ = offsetTab25[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 20:
                for (i = 0; i < OFFSET20COUNT; i++)
                {
                    tmpOffsetI = offsetTab20[i][0];
                    tmpOffsetJ = offsetTab20[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 15:
                for (i = 0; i < OFFSET15COUNT; i++)
                {
                    tmpOffsetI = offsetTab15[i][0];
                    tmpOffsetJ = offsetTab15[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
                    }
                }
            case 8:
                for (i = 0; i < OFFSET8COUNT; i++)
                {
                    tmpOffsetI = offsetTab8[i][0];
                    tmpOffsetJ = offsetTab8[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        tmpGUID = chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece;

                        if (tmpGUID && !IsMedivhsPiece(tmpGUID))
                            tmpPossibleTargetsList.push_back(tmpGUID);
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
    printf("\n Wywolanie GetMeleeTarget(uint64 piece)");
    int8 tmpi, tmpj;    //temporary piece position

    for (int8 i = 0; i < 8; i++)
    {
        for (int8 j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].piece == piece)
            {
                tmpi = j;
                tmpj = j;
                break;
            }
        }
    }

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
    printf("\n Wywolanie IsChessPiece(Unit * unit)");
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

bool boss_MedivhAI::IsMedivhsPiece(Unit * unit)
{
    printf("\n Wywolanie IsMedivhsPiece(Unit * unit)");
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

    return false;
}

bool boss_MedivhAI::IsMedivhsPiece(uint64 unit)
{
    printf("\n Wywolanie IsMedivhsPiece(uint64 unit)");
    for (std::list<uint64>::iterator i = medivhSidePieces.begin(); i != medivhSidePieces.end(); ++i)
    {
        if (*i == unit)
            return true;
    }

    return false;
}

bool boss_MedivhAI::IsInMoveList(uint64 unit, bool trigger)
{
    printf("\n Wywolanie IsInMoveList(uint64 unit, bool trigger)");
    if (!trigger)
    {
        for (std::list<ChessTile>::iterator i = moveList.begin(); i != moveList.end(); ++i)
        {
            if ((*i).piece == unit)
                return true;
        }
    }
    else
    {
        for (std::list<ChessTile>::iterator i = moveList.begin(); i != moveList.end(); ++i)
        {
            if ((*i).trigger == unit)
                return true;
        }
    }

    return false;
}

bool boss_MedivhAI::IsInRange(uint64 from, uint64 to, int range)
{
    printf("\n Wywolanie IsInRange(uint64 from, uint64 to, int range)");
    if (!from || !to || !range)
        return false;

    int8 tmpI = -1, tmpJ = -1, i, tmpOffsetI, tmpOffsetJ;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].piece == from)
            {
                tmpI = i; tmpJ = j;
                break;
            }
        }
        //if we find location of piece
        if (tmpI >= 0 && tmpJ >= 0)
            break;
    }

    if (tmpI < 0 || tmpJ < 0)
    {
        Unit * uFrom = m_creature->GetUnit(*m_creature, from);
        if (uFrom)
            ((ScriptedAI*)uFrom)->DoSay("GetSpellTarget(..) : Nie znaleziono mnie na planszy !!", LANG_UNIVERSAL, uFrom, false);
        return false;
    }

    switch (range)
    {
        case 25:
            for (i = 0; i < OFFSET25COUNT; i++)
            {
                tmpOffsetI = offsetTab25[i][0];
                tmpOffsetJ = offsetTab25[i][1];

                if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                    tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                {
                    if (chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece == to ||
                        chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger == to)
                        return true;
                }
            }
        case 20:
            for (i = 0; i < OFFSET20COUNT; i++)
            {
                tmpOffsetI = offsetTab20[i][0];
                tmpOffsetJ = offsetTab20[i][1];

                if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                    tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                {
                   if (chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece == to ||
                        chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger == to)
                        return true;
                }
            }
        case 15:
            for (i = 0; i < OFFSET15COUNT; i++)
            {
                tmpOffsetI = offsetTab15[i][0];
                tmpOffsetJ = offsetTab15[i][1];

                if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                    tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                {
                    if (chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece == to ||
                        chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger == to)
                        return true;
                }
            }
        case 8:
            for (i = 0; i < OFFSET8COUNT; i++)
            {
                tmpOffsetI = offsetTab8[i][0];
                tmpOffsetJ = offsetTab8[i][1];

                if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                    tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                {
                   if (chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].piece == to ||
                        chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger == to)
                        return true;
                }
            }
            break;
        default:
            break;
    }

    return false;
}

void boss_MedivhAI::Reset()
{
    printf("\n Wywolanie boss_MedivhAI::Reset()");
    eventStarted = false;
    miniEvent = false;
    miniEventState = MINI_EVENT_KING;
    miniEventTimer = 5000;
    hordePieces = 16;
    alliancePieces = 16;
    medivhSidePieces.clear();
    tpList.clear();
    moveList.clear();

    //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    //enabled = false;
}

void boss_MedivhAI::SayChessPieceDied(Unit * piece)
{
    printf("\n Wywolanie SayChessPieceDied(Unit * piece)");
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

                switch(rand()%3)
                {
                    case 0: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_1, m_creature);break;
                    case 1: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_2, m_creature);break;
                    case 2: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_3, m_creature);break;
                }
                break;

            case NPC_PAWN_A:

                switch(rand()%2)
                {
                    case 0: DoScriptText(SCRIPTTEXT_LOSE_PAWN_M_1, m_creature);break;
                    case 1: DoScriptText(SCRIPTTEXT_LOSE_PAWN_M_2, m_creature);break;
                }
                break;

            case NPC_KING_H:

                DoScriptText(SCRIPTTEXT_MEDIVH_WIN, m_creature);
                pInstance->SetData(DATA_CHESS_EVENT,FAIL);
                break;

            case NPC_KING_A:

                DoScriptText(SCRIPTTEXT_PLAYER_WIN, m_creature);
                pInstance->SetData(DATA_CHESS_EVENT,DONE);
                m_creature->SummonGameObject(DUST_COVERED_CHEST,-11058,-1903,221,2.24,0,0,0,0,7200000);
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

                switch(rand()%3)
                {
                    case 0: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_1, m_creature);break;
                    case 1: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_2, m_creature);break;
                    case 2: DoScriptText(SCRIPTTEXT_LOSE_PAWN_P_3, m_creature);break;
                }
                break;

            case NPC_PAWN_H:

                switch(rand()%2)
                {
                    case 0: DoScriptText(SCRIPTTEXT_LOSE_PAWN_M_1, m_creature);break;
                    case 1: DoScriptText(SCRIPTTEXT_LOSE_PAWN_M_2, m_creature);break;
                }
                break;

            case NPC_KING_A:

                DoScriptText(SCRIPTTEXT_MEDIVH_WIN, m_creature);
                pInstance->SetData(DATA_CHESS_EVENT,FAIL);
                break;

            case NPC_KING_H:

                DoScriptText(SCRIPTTEXT_PLAYER_WIN, m_creature);
                pInstance->SetData(DATA_CHESS_EVENT,DONE);
                m_creature->SummonGameObject(DUST_COVERED_CHEST,-11058,-1903,221,2.24,0,0,0,0,7200000);
                break;

            default:
                break;
        }
    }
}

void boss_MedivhAI::RemoveChessPieceFromBoard(uint64 piece)
{
    printf("\n Wywolanie RemoveChessPieceFromBoard(uint64 piece)");
    return;
    Unit * uPiece = m_creature->GetMap()->GetCreature(piece);
    RemoveChessPieceFromBoard(uPiece);
}

void boss_MedivhAI::RemoveChessPieceFromBoard(Unit * piece)
{
    printf("\n Wywolanie RemoveChessPieceFromBoard(Unit * piece)");
    //if (!piece)
        return;

    if (piece->getFaction() == A_FACTION)
    {
        --alliancePieces;
        piece->Relocate(hordeSideDeadWP[alliancePieces][0], hordeSideDeadWP[alliancePieces][1], POSITION_Z, hordeSideDeadOrientation);
    }
    else
    {
        --hordePieces;
        piece->Relocate(allianceSideDeadWP[hordePieces][0], allianceSideDeadWP[hordePieces][1], POSITION_Z, allianceSideDeadOrientation);
    }

    piece->CombatStop();
    piece->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    piece->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    piece->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

    ((npc_chesspieceAI*)piece)->InGame = false;
    piece->setActive(false);

    uint64 tmpGUID = piece->GetGUID();

    for (int8 i = 0; i < 8; i++)
        for (int8 j = 0; j < 8; j++)
            if (chessBoard[i][j].piece == tmpGUID)
            {
                chessBoard[i][j].piece = 0;
                return;
            }
}



void boss_MedivhAI::SpawnPawns()
{
    printf("\n Wywolanie SpawnPawns()");
    Creature * tmp[2][8];

    //printf("\nSpawnPawns(): before for");
    for (int i = 0; i < 8; i++)
    {
        //printf("\nSpawnPawns(): in for: i: %i", i);
        tmp[0][i] = m_creature->SummonCreature(NPC_PAWN_A, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);
        tmp[1][i] = m_creature->SummonCreature(NPC_PAWN_H, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);
        //printf("\n if 1");
        if (tmp[0][i])
        {
            chessBoard[6][i].piece = tmp[0][i]->GetGUID();
            tmp[0][i]->SetReactState(REACT_PASSIVE);
            tmp[0][i]->GetMotionMaster()->MovePoint(0, chessBoard[6][i].position.x, chessBoard[6][i].position.y, chessBoard[6][i].position.z);
        }
        chessBoard[6][i].ori = CHESS_ORI_N;
        //printf("\n if 2");
        if (tmp[1][i])
        {
            chessBoard[1][i].piece = tmp[1][i]->GetGUID();
            tmp[1][i]->SetReactState(REACT_PASSIVE);
            tmp[1][i]->GetMotionMaster()->MovePoint(0, chessBoard[1][i].position.x, chessBoard[1][i].position.y, chessBoard[1][i].position.z);
        }
        chessBoard[1][i].ori = CHESS_ORI_S;
    }
    //printf("\n after for");
    if (pInstance)
    {
        //printf("\n if 1");
        if (pInstance->GetData(CHESS_EVENT_TEAM) == A_FACTION)
        {
            //printf("\n if 1.1");
            for (int i = 0; i < 8; i++)
            {
                //printf("\n if 1.1 for: i: %i", i);
                medivhSidePieces.push_back(tmp[1][i]->GetGUID());
            }
        }
        else
        {
            //printf("\n if 1.2");
            for (int i = 0; i < 8; i++)
            {
                //printf("\n if 1.2 for: i: %i", i);
                medivhSidePieces.push_back(tmp[0][i]->GetGUID());
            }
        }
    }
    miniEventTimer = 20000;
    //printf("\n end SpawnPawns()");
}

void boss_MedivhAI::SpawnRooks()
{
    printf("\n Wywolanie SpawnRooks()");
    Creature * tmp1, * tmp2, * tmp3, * tmp4;

    tmp1 = m_creature->SummonCreature(NPC_ROOK_A, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp1)
    {
        chessBoard[7][0].piece = tmp1->GetGUID();
        chessBoard[7][0].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][0].position.x, chessBoard[7][0].position.y, chessBoard[7][0].position.z);
    }

    tmp2 = m_creature->SummonCreature(NPC_ROOK_A, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp2)
    {
        chessBoard[7][7].piece = tmp2->GetGUID();
        chessBoard[7][7].ori = CHESS_ORI_N;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[7][7].position.x, chessBoard[7][7].position.y, chessBoard[7][7].position.z);
    }


    tmp3 = m_creature->SummonCreature(NPC_ROOK_H, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp3)
    {
        chessBoard[0][0].piece = tmp3->GetGUID();
        chessBoard[0][0].ori = CHESS_ORI_S;
        tmp3->SetReactState(REACT_PASSIVE);
        tmp3->GetMotionMaster()->MovePoint(0, chessBoard[0][0].position.x, chessBoard[0][0].position.y, chessBoard[0][0].position.z);
    }

    tmp4 = m_creature->SummonCreature(NPC_ROOK_H, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp4)
    {
        chessBoard[0][7].piece = tmp4->GetGUID();
        chessBoard[0][7].ori = CHESS_ORI_S;
        tmp4->SetReactState(REACT_PASSIVE);
        tmp4->GetMotionMaster()->MovePoint(0, chessBoard[0][7].position.x, chessBoard[0][7].position.y, chessBoard[0][7].position.z);
    }

    if (pInstance && tmp1 && tmp2 && tmp3 && tmp4)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == A_FACTION)
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
    printf("\n Wywolanie SpawnKnights()");
    Creature * tmp1, * tmp2, * tmp3, * tmp4;

    tmp1 = m_creature->SummonCreature(NPC_KNIGHT_A, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp1)
    {
        chessBoard[7][1].piece = tmp1->GetGUID();
        chessBoard[7][1].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][1].position.x, chessBoard[7][1].position.y, chessBoard[7][1].position.z);
    }

    tmp2 = m_creature->SummonCreature(NPC_KNIGHT_A, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp2)
    {
        chessBoard[7][6].piece = tmp2->GetGUID();
        chessBoard[7][6].ori = CHESS_ORI_N;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[7][6].position.x, chessBoard[7][6].position.y, chessBoard[7][6].position.z);
    }


    tmp3 = m_creature->SummonCreature(NPC_KNIGHT_H, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp3)
    {
        chessBoard[0][1].piece = tmp3->GetGUID();
        chessBoard[0][1].ori = CHESS_ORI_S;
        tmp3->SetReactState(REACT_PASSIVE);
        tmp3->GetMotionMaster()->MovePoint(0, chessBoard[0][1].position.x, chessBoard[0][1].position.y, chessBoard[0][1].position.z);
    }

    tmp4 = m_creature->SummonCreature(NPC_KNIGHT_H, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp4)
    {
        chessBoard[0][6].piece = tmp4->GetGUID();
        chessBoard[0][6].ori = CHESS_ORI_S;
        tmp4->SetReactState(REACT_PASSIVE);
        tmp4->GetMotionMaster()->MovePoint(0, chessBoard[0][6].position.x, chessBoard[0][6].position.y, chessBoard[0][6].position.z);
    }

    if (pInstance && tmp1 && tmp2 && tmp3 && tmp4)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == A_FACTION)
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
    printf("\n Wywolanie SpawnBishops()");
    Creature * tmp1, * tmp2, * tmp3, * tmp4;

    tmp1 = m_creature->SummonCreature(NPC_BISHOP_A, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp1)
    {
        chessBoard[7][2].piece = tmp1->GetGUID();
        chessBoard[7][2].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][2].position.x, chessBoard[7][2].position.y, chessBoard[7][2].position.z);
    }

    tmp2 = m_creature->SummonCreature(NPC_BISHOP_A, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp2)
    {
        chessBoard[7][5].piece = tmp2->GetGUID();
        chessBoard[7][5].ori = CHESS_ORI_N;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[7][5].position.x, chessBoard[7][5].position.y, chessBoard[7][5].position.z);
    }


    tmp3 = m_creature->SummonCreature(NPC_BISHOP_H, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp3)
    {
        chessBoard[0][2].piece = tmp3->GetGUID();
        chessBoard[0][2].ori = CHESS_ORI_S;
        tmp3->SetReactState(REACT_PASSIVE);
        tmp3->GetMotionMaster()->MovePoint(0, chessBoard[0][2].position.x, chessBoard[0][2].position.y, chessBoard[0][2].position.z);
    }

    tmp4 = m_creature->SummonCreature(NPC_BISHOP_H, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp4)
    {
        chessBoard[0][5].piece = tmp4->GetGUID();
        chessBoard[0][5].ori = CHESS_ORI_S;
        tmp4->SetReactState(REACT_PASSIVE);
        tmp4->GetMotionMaster()->MovePoint(0, chessBoard[0][5].position.x, chessBoard[0][5].position.y, chessBoard[0][5].position.z);
    }

    if (pInstance && tmp1 && tmp2 && tmp3 && tmp4)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == A_FACTION)
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
    printf("\n Wywolanie SpawnQueens()");
    Creature * tmp1, * tmp2;

    tmp1 = m_creature->SummonCreature(NPC_QUEEN_A, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp1)
    {
        chessBoard[7][3].piece = tmp1->GetGUID();
        chessBoard[7][3].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][3].position.x, chessBoard[7][3].position.y, chessBoard[7][3].position.z);
    }

    tmp2 = m_creature->SummonCreature(NPC_QUEEN_H, SPAWN_POS, ORI_W, TEMPSUMMON_DEAD_DESPAWN, 0);

    if (tmp2)
    {
        chessBoard[0][3].piece = tmp2->GetGUID();
        chessBoard[0][3].ori = CHESS_ORI_S;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[0][3].position.x, chessBoard[0][3].position.y, chessBoard[0][3].position.z);
    }

    if (pInstance && tmp1 && tmp2)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == A_FACTION)
            medivhSidePieces.push_back(tmp2->GetGUID());
        else
            medivhSidePieces.push_back(tmp1->GetGUID());
    }
    miniEventTimer = 5000;
}

void boss_MedivhAI::SpawnKings()
{
    printf("\n Wywolanie SpawnKings()");
    Creature * tmp1, * tmp2;

    tmp1 = m_creature->SummonCreature(NPC_KING_A, SPAWN_POS, ORI_W, TEMPSUMMON_MANUAL_DESPAWN, 0);

    if (tmp1)
    {
        chessBoard[7][4].piece = tmp1->GetGUID();
        chessBoard[7][4].ori = CHESS_ORI_N;
        tmp1->SetReactState(REACT_PASSIVE);
        tmp1->GetMotionMaster()->MovePoint(0, chessBoard[7][4].position.x, chessBoard[7][4].position.y, chessBoard[7][4].position.z);
    }

    tmp2 = m_creature->SummonCreature(NPC_KING_H, SPAWN_POS, ORI_W, TEMPSUMMON_MANUAL_DESPAWN, 0);

    if (tmp2)
    {
        chessBoard[0][4].piece = tmp2->GetGUID();
        chessBoard[0][4].ori = CHESS_ORI_S;
        tmp2->SetReactState(REACT_PASSIVE);
        tmp2->GetMotionMaster()->MovePoint(0, chessBoard[0][4].position.x, chessBoard[0][4].position.y, chessBoard[0][4].position.z);
    }

    if (pInstance && tmp1 && tmp2)
    {
        if (pInstance->GetData(CHESS_EVENT_TEAM) == A_FACTION)
            medivhSidePieces.push_back(tmp2->GetGUID());
        else
            medivhSidePieces.push_back(tmp1->GetGUID());
    }
    miniEventTimer = 5000;
}

void boss_MedivhAI::SpawnTriggers()
{
    printf("\n Wywolanie SpawnTriggers()");
    Creature * tmp = NULL;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            tmp = m_creature->SummonCreature(TRIGGER_ID, chessBoard[i][j].position.x, chessBoard[i][j].position.y, chessBoard[i][j].position.z, ORI_W, TEMPSUMMON_MANUAL_DESPAWN, 0);
            if (tmp)
            {
                chessBoard[i][j].trigger = tmp->GetGUID();
                chessBoard[i][j].ori = CHESS_ORI_N;
                tmp->SetReactState(REACT_PASSIVE);
            }
        }
    }
}

void boss_MedivhAI::PrepareBoardForEvent()
{
    printf("\n Wywolanie PrepareBoardForEvent()");
    // Horde Tower
    allowedPositions[NPC_ROOK_H].push_back(std::pair<int, int>(0, 0));
    allowedPositions[NPC_ROOK_H].push_back(std::pair<int, int>(0, 7));
    // Horde Horse
    allowedPositions[NPC_KNIGHT_H].push_back(std::pair<int, int>(0, 1));
    allowedPositions[NPC_KNIGHT_H].push_back(std::pair<int, int>(0, 6));
    // Horde Laufer
    allowedPositions[NPC_BISHOP_H].push_back(std::pair<int, int>(0, 2));
    allowedPositions[NPC_BISHOP_H].push_back(std::pair<int, int>(0, 5));
    // Horde Queen
    allowedPositions[NPC_QUEEN_H].push_back(std::pair<int, int>(0, 3));
    // Horde King
    allowedPositions[NPC_KING_H].push_back(std::pair<int, int>(0, 4));
    // Horde Pawn
    allowedPositions[NPC_PAWN_H].push_back(std::pair<int, int>(1, 0));
    allowedPositions[NPC_PAWN_H].push_back(std::pair<int, int>(1, 1));
    allowedPositions[NPC_PAWN_H].push_back(std::pair<int, int>(1, 2));
    allowedPositions[NPC_PAWN_H].push_back(std::pair<int, int>(1, 3));
    allowedPositions[NPC_PAWN_H].push_back(std::pair<int, int>(1, 4));
    allowedPositions[NPC_PAWN_H].push_back(std::pair<int, int>(1, 5));
    allowedPositions[NPC_PAWN_H].push_back(std::pair<int, int>(1, 6));
    allowedPositions[NPC_PAWN_H].push_back(std::pair<int, int>(1, 7));

    // Alliance Tower
    allowedPositions[NPC_ROOK_A].push_back(std::pair<int, int>(7, 0));
    allowedPositions[NPC_ROOK_A].push_back(std::pair<int, int>(7, 7));
    // Alliance Horse
    allowedPositions[NPC_KNIGHT_A].push_back(std::pair<int, int>(7, 1));
    allowedPositions[NPC_KNIGHT_A].push_back(std::pair<int, int>(7, 6));
    // Alliance Laufer
    allowedPositions[NPC_BISHOP_A].push_back(std::pair<int, int>(7, 2));
    allowedPositions[NPC_BISHOP_A].push_back(std::pair<int, int>(7, 5));
    // Alliance Queen
    allowedPositions[NPC_QUEEN_A].push_back(std::pair<int, int>(7, 3));
    // Alliance King
    allowedPositions[NPC_KING_A].push_back(std::pair<int, int>(7, 4));
    // Alliance Pawn
    allowedPositions[NPC_PAWN_A].push_back(std::pair<int, int>(6, 0));
    allowedPositions[NPC_PAWN_A].push_back(std::pair<int, int>(6, 1));
    allowedPositions[NPC_PAWN_A].push_back(std::pair<int, int>(6, 2));
    allowedPositions[NPC_PAWN_A].push_back(std::pair<int, int>(6, 3));
    allowedPositions[NPC_PAWN_A].push_back(std::pair<int, int>(6, 4));
    allowedPositions[NPC_PAWN_A].push_back(std::pair<int, int>(6, 5));
    allowedPositions[NPC_PAWN_A].push_back(std::pair<int, int>(6, 6));
    allowedPositions[NPC_PAWN_A].push_back(std::pair<int, int>(6, 7));

    // Triggers
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            allowedPositions[TRIGGER_ID].push_back(std::pair<int, int>(i, j));
        }
    }

    SpawnTriggers();
    SpawnKings();
}

void boss_MedivhAI::TeleportPlayer(Player * player)
{
    printf("\n Wywolanie TeleportPlayer(Player * player)");
    if (player)
        player->TeleportTo(tpLoc);
}
void boss_MedivhAI::TeleportPlayer(uint64 player)
{
    printf("\n Wywolanie TeleportPlayer(uint64 player)");
    if(player)
    {
        if(Unit *tmpPlayer = Unit::GetUnit(*m_creature, player))
            if(tmpPlayer->GetTypeId() == TYPEID_PLAYER)
                ((Player*)tmpPlayer)->TeleportTo(tpLoc);
    }
}

void boss_MedivhAI::TeleportPlayers()
{
    printf("\n Wywolanie TeleportPlayers()");
    Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
        if (itr->getSource() && itr->getSource()->HasAura(SPELL_POSSES_CHESSPIECE, 0))
            TeleportPlayer(itr->getSource());
}

void boss_MedivhAI::AddPlayerToTeleportList(Player * player)
{
    printf("\n Wywolanie AddPlayerToTeleportList(Player* player)");
    if (player)
        tpList.push_back(player->GetGUID());
}

void boss_MedivhAI::TeleportListedPlayers()
{
    printf("\n Wywolanie TeleportListedPlayers()");
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
    printf("\n Wywolanie ApplyDebuffsOnRaidMembers()");
    //DoSay("ApplyDebuffsOnRaidMembers() poczatek", LANG_UNIVERSAL, m_creature);
    Player * tmp;
    m_creature->CastSpell(m_creature, SPELL_GAME_IN_SESSION, true);
    Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
    {
        //DoSay("ApplyDebuffsOnRaidMembers() for", LANG_UNIVERSAL, m_creature);
        tmp = itr->getSource();
        if (tmp && !tmp->isGameMaster() && tmp->isAlive())
        {
            //DoSay("ApplyDebuffsOnRaidMembers() if poczatek", LANG_UNIVERSAL, m_creature);
            //tmp->CastSpell(tmp, SPELL_IN_GAME, true);
            //tmp->CastSpell(tmp, SPELL_GAME_IN_SESSION, true);
            m_creature->CastSpell(tmp, SPELL_GAME_IN_SESSION, true);
            tmp->AddAura(SPELL_GAME_IN_SESSION, tmp);
            //tmp->SetInCombatWith(m_creature);
            //DoSay("ApplyDebuffsOnRaidMembers() if koniec", LANG_UNIVERSAL, m_creature);
        }
    }
    //DoSay("ApplyDebuffsOnRaidMembers() koniec", LANG_UNIVERSAL, m_creature);
}

void boss_MedivhAI::StartEvent()
{
    printf("\n Wywolanie StartEvent()");
    miniEvent = true;
    TeleportPlayers();
    ApplyDebuffsOnRaidMembers();

    DoZoneInCombat();
    eventStarted = true;
}

void boss_MedivhAI::UpdateAI(const uint32 diff)
{
    if(miniEvent)
    {
        if (miniEventTimer < diff)
        {
            switch(miniEventState)
            {
                case MINI_EVENT_KING:
                    PrepareBoardForEvent();
                    break;
                case MINI_EVENT_QUEEN:
                    SpawnQueens();
                    break;
                case MINI_EVENT_BISHOP:
                    SpawnBishops();
                    break;
                case MINI_EVENT_KNIGHT:
                    SpawnKnights();
                    break;
                case MINI_EVENT_ROOK:
                    SpawnRooks();
                    break;
                case MINI_EVENT_PAWN:
                    SpawnPawns();
                    break;
                default:
                    miniEvent = false;
                    printf("\nTablica:");
                    for (int i = 0; i < 8; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            SetOrientation(chessBoard[i][j].piece, chessBoard[i][j].ori);
                            printf("\ni: %i, j: %i, piece: %u, trigger: %u", i, j, chessBoard[i][j].piece, chessBoard[i][j].trigger);
                        }
                    }
                    break;
            }
            miniEventState++;
        }
        else
            miniEventTimer -= diff;

        return;
    }

    if (!eventStarted && pInstance->GetData(DATA_CHESS_EVENT) == IN_PROGRESS)
    {
        StartEvent();
        return;
    }

    if (!eventStarted)
        return;

    //TODO: add piece move ai
}

void boss_MedivhAI::SetOrientation(uint64 piece, ChessOrientation ori)
{
    int8 tmpi = -1, tmpj = -1;    //temp piece location in array

    for (int8 i = 0; i < 8; i++)
    {
        for(int8 j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].piece == piece)
            {
                tmpi = i;
                tmpj = j;
                break;
            }
        }
        if (tmpi > -1)
            break;
    }

    if (tmpi < 0 || tmpj < 0)
        return;

    if (ori == CHESS_ORI_CHOOSE)
    {
        float tmpN, tmpS, tmpE, tmpW;
        float pieceOri;

        Unit * tmpPiece = m_creature->GetUnit(*m_creature, piece);
        if (tmpPiece)
        {
            pieceOri = tmpPiece->GetOrientation();

            if (pieceOri > CHESS_ORI_N)
                tmpN = pieceOri - CHESS_ORI_N;
            else
                tmpN = CHESS_ORI_N - pieceOri;

            if (pieceOri > CHESS_ORI_E)
                tmpE = pieceOri - CHESS_ORI_E;
            else
                tmpE = CHESS_ORI_E - pieceOri;

            if (pieceOri > CHESS_ORI_S)
                tmpS = pieceOri - CHESS_ORI_S;
            else
                tmpS = CHESS_ORI_S - pieceOri;

            if (pieceOri > CHESS_ORI_W)
                tmpW = pieceOri - CHESS_ORI_W;
            else
                tmpW = CHESS_ORI_W - pieceOri;

            ChessOrientation tmpOri = CHESS_ORI_N;
            float min = tmpN;

            if (tmpE < min)
            {
                min = tmpE;
                tmpOri = CHESS_ORI_E;
            }

            if (tmpS < min)
            {
                min = tmpS;
                tmpOri = CHESS_ORI_S;
            }

            if (tmpW < min)
            {
                min = tmpW;
                tmpOri = CHESS_ORI_W;
            }

            chessBoard[tmpi][tmpj].ori = tmpOri;
        }
    }
    else
        chessBoard[tmpi][tmpj].ori = ori;

    Creature * cPiece = m_creature->GetMap()->GetCreature(piece);
    if (cPiece)
    {

        switch (chessBoard[tmpi][tmpj].ori)
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

        Map::PlayerList const& players = m_creature->GetMap()->GetPlayers();

        if (!players.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->getSource())
                    cPiece->SendMonsterMove(cPiece->GetPositionX(), cPiece->GetPositionY(), cPiece->GetPositionZ(), 0, plr);
            }
        }
    }
}

Unit * boss_MedivhAI::FindTrigger(uint64 piece)
{
    printf("\n Wywolanie FindTrigger(uint64 piece)");
    for (int8 i = 0; i < 8; i++)
    {
        for (int8 j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].piece == piece)
                return m_creature->GetUnit(*m_creature, chessBoard[i][j].trigger);
        }
    }

    return NULL;
}

bool boss_MedivhAI::ChessSquareIsEmpty(uint64 trigger)
{
    printf("\n Wywolanie ChessSquareIsEmpty(uint64 trigger)");
    for (int8 i = 0; i < 8; i++)
    {
        for (int8 j = 0; j < 8; j++)
        {
            if (chessBoard[i][j].trigger == trigger)
            {
                if (chessBoard[i][j].piece || IsInMoveList(trigger, true))
                    return false;
                else
                    return true;
            }
        }
    }

    return false;
}

bool boss_MedivhAI::CanMoveTo(uint64 trigger, uint64 piece)
{
    printf("\n Wywolanie CanMoveTo(uint64 trigger, uint64 piece)");
    printf("\nCanMoveTo: trigger %u, piece %u", trigger, piece);

    if (!trigger || !piece)
        return false;

    printf("\nCMT: Tablica:");
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("\ni: %i, j: %i, piece: %u, trigger: %u", i, j, chessBoard[i][j].piece, chessBoard[i][j].trigger);
        }
    }

    int moveRange = GetMoveRange(piece);
    bool inRange = IsInRange(piece, trigger, moveRange);
    bool isEmpty = ChessSquareIsEmpty(trigger);

    printf("\nCanMoveTo: moveRange %i, isInRange %i, isEmpty %i", moveRange, inRange, isEmpty);

    return inRange && isEmpty;
}

void boss_MedivhAI::AddTriggerToMove(uint64 trigger, uint64 piece, bool player)
{
    printf("\n Wywolanie AddTriggerToMove(uint64 trigger, uint64 piece, bool player)");
    ChessTile tmp;
    tmp.piece = piece;
    tmp.trigger = trigger;

    moveList.push_back(tmp);

    uint16 tmpChance = urand(0, 100);

    //check, if tmpChance is higher than chanceToMove then medivh also can move one of his pieces
    if (player && tmpChance < chanceToMove)
    {
        std::list<Priority> tmpList;
        std::list<uint64> emptySquareList;

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
            tempPriority.prior = START_PRIORITY;
            tempPriority.GUID = *i;

            if (IsEmptySquareInRange(*i, GetMoveRange(*i)))
            {
                //if piece can move anywhere then modify move priority

                tempPriority.prior += GetCountOfEnemyInMelee(*i) * MELEE_PRIORITY + urand(0, RAND_PRIORITY) + GetLifePriority(*i);
            }
            else
                tempPriority.prior = 0;

            prioritySum += tempPriority.prior;
            tmpList.push_back(tempPriority);
        }

        int chosen = urand(0, prioritySum), prevPrior = 0;
        uint64 chosenGUID = 0;

        for (std::list<Priority>::iterator i = tmpList.begin(); i!= tmpList.end(); ++i)
        {
            if (prevPrior < chosen && (*i).prior >= chosen)
            {
                chosenGUID = (*i).GUID;
                break;
            }
            prevPrior = (*i).prior;
        }

        // create empty square list

        int8 tmpI = -1, tmpJ = -1, i, j, tmpOffsetI, tmpOffsetJ;

        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (chessBoard[i][j].piece == chosenGUID)
                {
                    tmpI = i; tmpJ = j;
                    break;
                }
            }
            //if we find location of piece
            if (tmpI >= 0 && tmpJ >= 0)
                break;
        }

        if (tmpI < 0 || tmpJ < 0)
        {
            Unit * uChosen = m_creature->GetUnit(*m_creature, chosenGUID);
            if (uChosen)
                ((ScriptedAI*)uChosen)->DoSay("AddTriggerToMove(..) : Nie znaleziono mnie na planszy !!", LANG_UNIVERSAL, uChosen, false);
            return;
        }

        switch (GetMoveRange(chosenGUID))
        {
            case 20:
                for (i = 0; i < OFFSET20COUNT; i++)
                {
                    tmpOffsetI = offsetTab20[i][0];
                    tmpOffsetJ = offsetTab20[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                       if (ChessSquareIsEmpty(chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger))
                            emptySquareList.push_back(chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger);
                    }
                }
            case 15:
                for (i = 0; i < OFFSET15COUNT; i++)
                {
                    tmpOffsetI = offsetTab15[i][0];
                    tmpOffsetJ = offsetTab15[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                        if (ChessSquareIsEmpty(chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger))
                            emptySquareList.push_back(chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger);
                    }
                }
            case 8:
                for (i = 0; i < OFFSET8COUNT; i++)
                {
                    tmpOffsetI = offsetTab8[i][0];
                    tmpOffsetJ = offsetTab8[i][1];

                    if (tmpI + tmpOffsetI >= 0 && tmpI + tmpOffsetI < 8 &&
                        tmpJ + tmpOffsetJ >= 0 && tmpJ + tmpOffsetJ < 8)
                    {
                       if (ChessSquareIsEmpty(chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger))
                            emptySquareList.push_back(chessBoard[tmpI + tmpOffsetI][tmpJ + tmpOffsetJ].trigger);
                    }
                }
                break;
            default:
                break;
        }

        if (emptySquareList.empty())
            return;

        // calculate and add square move priority
        tmpList.clear();
        prioritySum = 0;

        for (std::list<uint64>::iterator i = emptySquareList.begin(); i != emptySquareList.end(); ++i)
        {
            Priority tmpPrior;
            tmpPrior.prior = START_PRIORITY;
            tmpPrior.GUID = (*i);

            switch (GetCountOfEnemyInMelee(*i))
            {
                case 1:
                    tmpPrior.prior += 25;
                    break;
                case 2:
                    tmpPrior.prior += 10;
                    break;
                case 3:
                    tmpPrior.prior -= 10;
                    break;
                case 4:
                    tmpPrior.prior -= 25;
                    break;
                default:
                    break;
            }
            //add bonus prior if *i is heal/king ??
            prioritySum += tmpPrior.prior;
            tmpList.push_back(tmpPrior);
        }

        chosen = urand(0, prioritySum);
        prevPrior = 0;
        uint64 chosenTriggerGUID = 0;

        for (std::list<Priority>::iterator i = tmpList.begin(); i!= tmpList.end(); ++i)
        {
            if (prevPrior < chosen && (*i).prior >= chosen)
            {
                chosenTriggerGUID = (*i).GUID;
                break;
            }
            prevPrior = (*i).prior;
        }

        ChessTile temp;
        temp.piece = chosenGUID;
        temp.trigger = chosenTriggerGUID;

        moveList.push_back(temp);
    }
}

//other

bool GossipHello_npc_chesspiece(Player* player, Creature* _Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    //if(pInstance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS &&
    //   _Creature->GetEntry() != NPC_KING_A && _Creature->GetEntry() != NPC_KING_H)
    //    return true;

    if(pInstance->GetData(CHESS_EVENT_TEAM) == ALLIANCE && _Creature->getFaction() != A_FACTION)
        return true;

    if(pInstance->GetData(CHESS_EVENT_TEAM) == HORDE && _Creature->getFaction() != H_FACTION)
        return true;

    if(!(_Creature->isPossessedByPlayer()))
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
        //if (_Creature->GetEntry() == NPC_KING_A || _Creature->GetEntry() == NPC_KING_H)
        //    pInstance->SetData(DATA_CHESS_EVENT, IN_PROGRESS);

        player->CastSpell(_Creature, SPELL_POSSES_CHESSPIECE, false);
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
        pInstance->SetData(DATA_CHESS_EVENT,NOT_STARTED);

    if(pInstance->GetData(DATA_CHESS_EVENT) == DONE) //for testing
        pInstance->SetData(DATA_CHESS_EVENT,NOT_STARTED);

    if(pInstance->GetData(DATA_CHESS_EVENT) == NOT_STARTED)
    {
        player->ADD_GOSSIP_ITEM(0, EVENT_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        //if (player->isGameMaster())
        //    player->ADD_GOSSIP_ITEM(2, "Start Debug Mode", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
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
