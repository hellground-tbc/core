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
    //onMarker    = NULL;
    //EndMarker   = false;
    moveTimer   = 10000;
    pieceStance = PIECE_NONE;
    unitToMove = 0;
    MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);
}

void move_triggerAI::SpellHit(Unit *caster,const SpellEntry *spell)
{
    if (pieceStance != PIECE_NONE || !MedivhGUID)
        return;

    if(spell->Id == SPELL_MOVE_1 || spell->Id == SPELL_MOVE_2 || spell->Id == SPELL_MOVE_3 || spell->Id == SPELL_MOVE_4 ||
       spell->Id == SPELL_MOVE_5 || spell->Id == SPELL_MOVE_6 || spell->Id == SPELL_MOVE_7 || spell->Id == SPELL_CHANGE_FACING)
    {
        //if(onMarker != NULL || EndMarker)
        //    return;

        //EndMarker = true;
        //onMarker = caster;

        boss_MedivhAI * medivh = (boss_MedivhAI*)(m_creature->GetUnit(*m_creature, MedivhGUID));
        if (medivh)
        {
            if (medivh->CanMoveTo(m_creature->GetGUID(), caster->GetGUID()))
            {
                medivh->AddTriggerToMove(m_creature->GetGUID(), caster->GetGUID(), caster->GetTypeId() == TYPEID_PLAYER ? true : false);

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
        //unitToMove->SetInFront(m_creature);
        //unitToMove->SetUInt64Value(UNIT_FIELD_TARGET, m_creature->GetGUID());
        //unitToMove->
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

    /*if(unitToMove)
    {
        if(unitToMove->isAlive()) // Better check it here so we don't need to find start and end marker when chess die in move without reaching destination point :]
            return;
        else
            Reset();
    }*/
}


//Chesspieces AI

npc_chesspieceAI::npc_chesspieceAI(Creature *c) : Scripted_NoMovementAI(c)
{
    pInstance = ((ScriptedInstance*)m_creature->GetInstanceData());

    SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_POSSES_CHESSPIECE);
    if(TempSpell)
        TempSpell->Effect[0] = 0;  // Disable bind sight effect from SPELL_POSSES_CHESSPIECE. We handle all with dummy and charm effect.
}

/*void npc_chesspieceAI::MoveInLineOfSight(Unit *who)
{
    if( !m_creature->getVictim() && who->isTargetableForAttack() )
    {
        if (!m_creature->canFly() && m_creature->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
            return;

        if( m_creature->IsWithinDistInMap(who, AGGRO_RANGE) && m_creature->IsWithinLOSInMap(who) && InGame)
            AttackStart(who);
    }
}*/

/*void npc_chesspieceAI::Aggro(Unit *Unit)
{
    MedivhGUID = pInstance->GetData64(DATA_CHESS_ECHO_OF_MEDIVH);
    npc_medivh = m_creature->GetUnit(*m_creature, MedivhGUID);

    if(npc_medivh)
    {
        switch(pInstance->GetData(CHESS_EVENT_TEAM))
        {
            case ALLIANCE:
                if(m_creature->GetEntry() == NPC_KING_H)
                    DoScriptText(SCRIPTTEXT_MEDIVH_CHECK, npc_medivh);
                else if(m_creature->GetEntry() == NPC_KING_A)
                    DoScriptText(SCRIPTTEXT_PLAYER_CHECK, npc_medivh);
            break;
            case HORDE:
                if(m_creature->GetEntry() == NPC_KING_A)
                    DoScriptText(SCRIPTTEXT_MEDIVH_CHECK, npc_medivh);
                else if(m_creature->GetEntry() == NPC_KING_H)
                    DoScriptText(SCRIPTTEXT_PLAYER_CHECK, npc_medivh);
            break;
        }
    }
}*/

void npc_chesspieceAI::EnterEvadeMode()
{
    // to prevent reset chess pieces after unpossess
    if (pInstance->GetData(DATA_CHESS_EVENT) == IN_PROGRESS ||
        pInstance->GetData(DATA_CHESS_EVENT) == SPECIAL)
        return;

    ScriptedAI::EnterEvadeMode();
}

bool npc_chesspieceAI::IsHealingSpell(uint32 spell)
{
    if (spell == SPELL_BISHOP_A_1 || spell == SPELL_BISHOP_H_1)
        return true;

    return false;
}

int npc_chesspieceAI::GetAbilityRange(uint32 spell)
{
    switch(spell)
    {
        /*
        case SPELL_KING_H_1:
        case SPELL_KING_A_1:
        case SPELL_KNIGHT_H_1:
        case SPELL_KNIGHT_A_1:
        case SPELL_ROOK_H_1:
        case SPELL_ROOK_A_1:
        case SPELL_PAWN_H_1:
        case SPELL_PAWN_A_1:
        case SPELL_KING_H_2:
        case SPELL_KING_A_2:
        case SPELL_BISHOP_H_2:
        case SPELL_BISHOP_A_2:
        case SPELL_KNIGHT_H_2:
        case SPELL_KNIGHT_A_2:
        case SPELL_ROOK_H_2:
        case SPELL_ROOK_A_2:
        case SPELL_PAWN_H_2:
        case SPELL_PAWN_A_2:
            return 0;*/

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
    ReturnToHome = true;
    //Heal_Timer = 7000;
    InGame = true;
    CanMove = false;
    //NextMove_Timer = 5000; // wait 4.5s for first moves
    m_creature->setActive(true);

    /*start_marker = NULL;
    end_marker = NULL;*/

    SetSpellsAndCooldowns();

    ability1Chance = urand(ABILITY_1_CHANCE_MIN, ABILITY_1_CHANCE_MAX);
    ability2Chance = urand(ABILITY_2_CHANCE_MIN, ABILITY_2_CHANCE_MAX);

    nextTryTimer = urand(500, 5000);
}

void npc_chesspieceAI::MovementInform(uint32 MovementType, uint32 Data)
{
    if (MovementType != POINT_MOTION_TYPE)
        return;

    this->npc_medivh = m_creature->GetMap()->GetCreature(this->MedivhGUID);
    if (npc_medivh)
        ((boss_MedivhAI*)npc_medivh)->SetOrientation(m_creature->GetGUID());

    /*if(start_marker)
    {
        ((move_triggerAI*)start_marker->AI())->Reset();
      //  DoSay("Start Cell - Clear!",LANG_UNIVERSAL,false); // only for debug purposes :]
    }*/
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

        if (attackTimer < diff)
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
            attackTimer -= diff;

    }
    //DoMeleeAttackIfReady();

    CastNextSpellIfAnyAndReady();
}

void npc_chesspieceAI::SpellHitTarget(Unit *target, const SpellEntry *spell)
{
    if(target->GetEntry() != TRIGGER_ID || (spell->Id != SPELL_MOVE_1
       && spell->Id != SPELL_MOVE_2 && spell->Id != SPELL_MOVE_3 && spell->Id != SPELL_MOVE_4
       && spell->Id != SPELL_MOVE_5 && spell->Id != SPELL_MOVE_6 && spell->Id != SPELL_MOVE_7) )
        return;

    /*if(m_creature->isPossessed())
    {
        Creature *marker = (Creature*)target;
        if(marker && ((move_triggerAI*)marker->AI())->onMarker == m_creature)
        {
            start_marker = end_marker;
            end_marker = (Creature*)target;
        }
    }*/
}

void npc_chesspieceAI::DamageTaken(Unit * done_by, uint32 &damage)
{
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
}

int boss_MedivhAI::GetMoveRange(uint64 piece)
{
    return (GetMoveRange(m_creature->GetUnit(*m_creature, piece)));
}

int boss_MedivhAI::GetMoveRange(Unit * piece)
{
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

    return false;
}

bool boss_MedivhAI::IsMedivhsPiece(uint64 unit)
{
    for (std::list<uint64>::iterator i = medivhSidePieces.begin(); i != medivhSidePieces.end(); ++i)
    {
        if (*i == unit)
            return true;
    }

    return false;
}

bool boss_MedivhAI::IsInMoveList(uint64 unit, bool trigger)
{
    if (!trigger)
    {
        for (std::list<ChessSquare>::iterator i = moveList.begin(); i != moveList.end(); ++i)
        {
            if ((*i).piece == unit)
                return true;
        }
    }
    else
    {
        for (std::list<ChessSquare>::iterator i = moveList.begin(); i != moveList.end(); ++i)
        {
            if ((*i).trigger == unit)
                return true;
        }
    }

    return false;
}

bool boss_MedivhAI::IsInRange(uint64 from, uint64 to, int range)
{
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
    eventStarted = false;
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
    Unit * uPiece = m_creature->GetMap()->GetCreature(piece);
    RemoveChessPieceFromBoard(uPiece);
}

void boss_MedivhAI::RemoveChessPieceFromBoard(Unit * piece)
{
    if (!piece)
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



void boss_MedivhAI::PrepareBoardForEvent()
{
    //DoSay("zaczynam przygotowywanie planszy", LANG_UNIVERSAL, m_creature);
    //printf("\nzaczynam przygotowywanie planszy\n");
    if (chessBoard[0][0].trigger != 0)
        return;

    Creature * current;
    Creature * next;
    float posX, posY, posXc, posYc;
    std::list<Creature*> tmpList;

    //printf("\nRozmiary: tmpList: %i, forChessList: %i\n", tmpList.size(), pInstance->forChessList.size());

    for (std::list<uint64>::iterator i = pInstance->forChessList.begin(); i != pInstance->forChessList.end(); i++)
    {
        tmpList.push_back(m_creature->GetMap()->GetCreature((*i)));
        //DoSay(".", LANG_UNIVERSAL, m_creature);
    }

    //printf("\nRozmiary: tmpList: %i, forChessList: %i\n", tmpList.size(), pInstance->forChessList.size());

    //return;
    //DoSay("szukanie pierwszego", LANG_UNIVERSAL, m_creature);
    //printf("\nszukanie pierwszego\n");
    //add first trigger to list (from that point we will start searching and adding to chessBoard oder triggers)
    for (std::list<Creature*>::iterator itr = tmpList.begin(); itr != tmpList.end(); ++itr)
    {
        //printf("\n1\n");
        posX = (*itr)->GetPositionX();
        if (posX < -11050 && posX > -11056)
        {
            //printf("\n1.1\n");
            //DoSay("znalazlem pierwszego", LANG_UNIVERSAL, m_creature);
            //printf("\nznalazlem pierwszego\n");
            //printf("\n1.2\n");
            current = (*itr);
            posXc = current->GetPositionX();
            posYc = current->GetPositionY();
            //printf("\n1.3\n");
            if (current->GetEntry() == TRIGGER_ID)
                chessBoard[0][0].trigger = current->GetGUID();
            else
            {
                if (IsChessPiece(current))
                {
                    chessBoard[0][0].piece = current->GetGUID();

                    if (IsMedivhsPiece(current))
                        medivhSidePieces.push_back(current->GetGUID());

                    if (current->getFaction() == A_FACTION)
                        chessBoard[0][0].ori = CHESS_ORI_N;
                    else
                        chessBoard[0][0].ori = CHESS_ORI_S;
                }
            }
            //printf("\n1.4\n");
            //current->CastSpell(current, SPELL_MOVE_MARKER, true);




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
            //DoSay("niema currenta 2", LANG_UNIVERSAL, m_creature);
            //printf("\nniema currenta 2\n");
            continue;
        }

        for (std::list<Creature*>::iterator itr = tmpList.begin(); itr != tmpList.end(); ++itr)
        {
            //printf("\n3\n");
            next = (*itr);
            //printf("\n4\n");
            if (!next)
            {
                //DoSay("niema Next 1", LANG_UNIVERSAL, m_creature);
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
                    {
                        chessBoard[i + 1][0].piece = next->GetGUID();

                        if (IsMedivhsPiece(next))
                            medivhSidePieces.push_back(next->GetGUID());

                        if (current->getFaction() == A_FACTION)
                            chessBoard[i + 1][0].ori = CHESS_ORI_N;
                        else
                            chessBoard[i + 1][0].ori = CHESS_ORI_S;
                    }
                //next->CastSpell(next, SPELL_MOVE_MARKER, true);
                //printf("\nznalazlem z lewej: %i   n: %s\n", i, next->GetName());

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
            //DoSay("niema currenta 3", LANG_UNIVERSAL, m_creature);
            //printf("\nniema currenta 3\n");
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
                        if (IsChessPiece(next))
                        {
                            chessBoard[i][j].piece = next->GetGUID();

                            if (IsMedivhsPiece(next))
                                medivhSidePieces.push_back(next->GetGUID());

                            if (next->getFaction() == A_FACTION)
                                chessBoard[i][j].ori = CHESS_ORI_N;
                            else
                                chessBoard[i][j].ori = CHESS_ORI_S;
                        }

                    //next->CastSpell(next, SPELL_MOVE_MARKER, true);
                    //printf("\nznalazlem z reszty: [%i][%i]\n", i, j);

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
                //DoSay("niema currenta 4", LANG_UNIVERSAL, m_creature);
                //printf("\nniema currenta 4\n");
                break;
            }
            //printf("\n14\n");
        }
        current = NULL;
    }
}

void boss_MedivhAI::TeleportPlayer(Player * player)
{
    if (player)
        player->TeleportTo(tpLoc);
}
void boss_MedivhAI::TeleportPlayer(uint64 player)
{
    if(player)
    {
        if(Unit *tmpPlayer = Unit::GetUnit(*m_creature, player))
            if(tmpPlayer->GetTypeId() == TYPEID_PLAYER)
                ((Player*)tmpPlayer)->TeleportTo(tpLoc);
    }
}

void boss_MedivhAI::TeleportPlayers()
{
    Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
        if (itr->getSource() && itr->getSource()->HasAura(SPELL_POSSES_CHESSPIECE, 0))
            TeleportPlayer(itr->getSource());
}

void boss_MedivhAI::AddPlayerToTeleportList(Player * player)
{
    if (player)
        tpList.push_back(player->GetGUID());
}

void boss_MedivhAI::TeleportListedPlayers()
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

void boss_MedivhAI::ApplyDebuffsOnRaidMembers()
{
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
    //DoSay("StartEvent", LANG_UNIVERSAL, m_creature);
    PrepareBoardForEvent();
    TeleportPlayers();
    ApplyDebuffsOnRaidMembers();

    DoZoneInCombat();
    eventStarted = true;
}

void boss_MedivhAI::UpdateAI(const uint32 diff)
{
    /*if (!enabled && pInstance->GetData(DATA_CURATOR_EVENT) == DONE)
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        enabled = true;
    }*/

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
            tmpPiece->SetOrientation(tmpOri);
        }
    }
    else
        chessBoard[tmpi][tmpj].ori = ori;
}

Unit * boss_MedivhAI::FindTrigger(uint64 piece)
{
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
    /*Creature * cTrigger = m_creature->GetCreature(*m_creature, trigger);

    if (!cTrigger || !ChessSquereIsEmpty(trigger))
        return false;

    Unit * cPieceTrigger = FindTrigger(piece);
    Unit * cPiece = m_creature->GetUnit(*m_creature, piece);

    if (!cPieceTrigger)
        return false;

    bool canMove = false;

    switch (cPiece->GetEntry())
    {
        case NPC_PAWN_A:
        case NPC_PAWN_H:
        case NPC_KING_A:
        case NPC_KING_H:
        case NPC_BISHOP_A:
        case NPC_BISHOP_H:
        case NPC_ROOK_A:
        case NPC_ROOK_H:
            if (cTrigger->GetDistance(cPieceTrigger) <= 8)
                canMove = true;
            break;
        case NPC_KNIGHT_A:
        case NPC_KNIGHT_H:
            if (cTrigger->GetDistance(cPieceTrigger) <= 15)
                canMove = true;
            break;
        case NPC_QUEEN_A:
        case NPC_QUEEN_H:
            if (cTrigger->GetDistance(cPieceTrigger) <= 20)
                canMove = true;
            break;
        default:
            canMove = false;
            break;
    }

    return canMove;*/

    int moveRange = GetMoveRange(piece);
    bool isInRange = IsInRange(piece, trigger, moveRange);
    bool isEmpty = ChessSquareIsEmpty(trigger);

    printf("\nCanMoveTo: moveRange %i, isInRange %i, isEmpty %i", moveRange, isInRange, isEmpty);

    return isInRange && isEmpty;
}

void boss_MedivhAI::AddTriggerToMove(uint64 trigger, uint64 piece, bool player)
{
    ChessSquare tmp;
    tmp.piece = piece;
    tmp.trigger = trigger;

    moveList.push_back(tmp);

    uint16 tmpChance = urand(0, 100);

    //check, if tmpChance is higher than chanceToMove then medivh also can move one of his pieces
    if (player && tmpChance > chanceToMove)
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

        ChessSquare temp;
        temp.piece = chosenGUID;
        temp.trigger = chosenTriggerGUID;

        moveList.push_back(temp);
    }
}

//other

bool GossipHello_npc_chesspiece(Player* player, Creature* _Creature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)_Creature->GetInstanceData());

    //if(pInstance->GetData(DATA_CHESS_EVENT) != IN_PROGRESS)
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
