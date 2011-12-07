/*
 * processor.cpp
 *
 *  Created on: 2010-8-3
 *      Author: Argon
 */

#include "field.h"

void field::add_process(uint16 type, uint16 step, effect* peffect, group* target, ptr arg1, ptr arg2) {
	processor_unit new_unit;
	new_unit.type = type;
	new_unit.step = step;
	new_unit.peffect = peffect;
	new_unit.ptarget = target;
	new_unit.arg1 = arg1;
	new_unit.arg2 = arg2;
	core.subunits.push_back(new_unit);
}
int32 field::process() {
	if (core.subunits.size())
		core.units.splice(core.units.begin(), core.subunits);
	if (core.units.size() == 0)
		return PROCESSOR_END + pduel->bufferlen;
	processor_list::iterator it = core.units.begin();
	switch (it->type) {
	case PROCESSOR_ADJUST: {
		if (adjust_step(it->step))
			core.units.pop_front();
		else {
			core.units.begin()->step++;
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_TURN: {
		if (process_turn(it->step, it->arg1))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_WAIT: {
		core.units.pop_front();
		return PROCESSOR_WAITING + pduel->bufferlen;
	}
	case PROCESSOR_REFRESH_LOC: {
		if (refresh_location_info(it->step))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_BATTLECMD: {
		if (select_battle_command(it->step, it->arg1)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_IDLECMD: {
		if (select_idle_command(it->step, it->arg1)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_EFFECTYN: {
		if (select_effect_yes_no(it->step, it->arg1, (card*)it->ptarget)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_YESNO: {
		if (select_yes_no(it->step, it->arg1, it->arg2)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_OPTION: {
		if (select_option(it->step, it->arg1)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_CARD: {
		if (select_card(it->step, it->arg1 & 0xff, (it->arg1 >> 16) & 0xff, (it->arg2) & 0xff, (it->arg2 >> 16) & 0xff)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_CHAIN: {
		if (select_chain(it->step, it->arg1, it->arg2)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_DISFIELD:
	case PROCESSOR_SELECT_PLACE: {
		if (select_place(it->step, it->arg1 & 0xffff, it->arg2, (it->arg1 >> 16) & 0xffff)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_POSITION: {
		if (select_position(it->step, it->arg1 & 0xffff, it->arg2, (it->arg1 >> 16) & 0xffff)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_TRIBUTE_P: {
		if (select_tribute(it->step, it->arg1 & 0xff, (it->arg1 >> 16) & 0xff, (it->arg2) & 0xff, (it->arg2 >> 16) & 0xff)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SORT_CHAIN: {
		if (sort_chain(it->step, it->arg1)) {
			core.units.pop_front();
		} else {
			core.units.begin()->step++;
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_COUNTER: {
		if (select_counter(it->step, it->arg1, (it->arg2) & 0xffff, (it->arg2 >> 16) & 0xffff)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_SUM: {
		if (select_with_sum_limit(it->step, it->arg2 & 0xffff, it->arg1, it->arg2 >> 16)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SORT_CARD: {
		if (sort_card(it->step, it->arg1, it->arg2)) {
			core.units.pop_front();
			return pduel->bufferlen;
		} else {
			core.units.begin()->step = 1;
			return PROCESSOR_WAITING + pduel->bufferlen;
		}
	}
	case PROCESSOR_SELECT_RELEASE: {
		if (select_release_cards(it->step, it->arg1 & 0xff, (it->arg1 >> 16) & 0xff, (it->arg1 >> 24) & 0xff, (it->arg2) & 0xff, (it->arg2 >> 16) & 0xff))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_TRIBUTE: {
		if (select_tribute_cards(it->step, it->arg1 & 0xff, (it->arg1 >> 16) & 0xff, (it->arg2) & 0xff, (it->arg2 >> 16) & 0xff))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_POINT_EVENT: {
		if(process_point_event(it->step, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_QUICK_EFFECT: {
		if(process_quick_effect(it->step, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_IDLE_COMMAND: {
		if(process_idle_command(it->step))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_PHASE_EVENT: {
		if(process_phase_event(it->step, it->arg1))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_BATTLE_COMMAND: {
		if(process_battle_command(it->step))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_DAMAGE_PHASE: {
		if(process_damage_phase(it->step))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_ADD_CHAIN: {
		if (add_chain(it->step))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SOLVE_CHAIN: {
		if (solve_chain(it->step, it->arg1))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SOLVE_CONTINUOUS: {
		if (solve_continuous(it->step, it->peffect, it->arg1))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_EXECUTE_COST: {
		if (execute_cost(it->step, it->peffect, it->arg1)) {
			core.units.pop_front();
			core.solving_event.pop_front();
		} else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_EXECUTE_OPERATION: {
		if (execute_operation(it->step, it->peffect, it->arg1)) {
			core.units.pop_front();
			core.solving_event.pop_front();
		} else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_EXECUTE_TARGET: {
		if (execute_target(it->step, it->peffect, it->arg1)) {
			core.units.pop_front();
			core.solving_event.pop_front();
		} else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_DESTROY: {
		if (destroy(it->step, it->ptarget, it->peffect, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_RELEASE: {
		if (release(it->step, it->ptarget, it->peffect, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SENDTO: {
		if (send_to(it->step, it->ptarget, it->peffect, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_DESTROY_STEP: {
		if(destroy(it->step, it->ptarget, (card*)it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_RELEASE_STEP: {
		if (release(it->step, it->ptarget, (card*)it->arg1))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SENDTO_STEP: {
		if (send_to(it->step, it->ptarget, (card*)it->arg1))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_MOVETOFIELD: {
		if (move_to_field(it->step, (card*)it->ptarget, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_CHANGEPOS: {
		if (change_position(it->step, it->ptarget, it->peffect, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_OPERATION_REPLACE: {
		if (operation_replace(it->step, it->peffect, it->ptarget, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SUMMON_RULE: {
		if (summon(it->step, it->arg1, (card*)it->ptarget, it->peffect, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SPSUMMON_RULE: {
		if (special_summon_rule(it->step, it->arg1, (card*)it->ptarget))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SPSUMMON: {
		if (special_summon(it->step, it->peffect, it->arg1, it->ptarget))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_FLIP_SUMMON: {
		if (flip_summon(it->step, it->arg1, (card*)(it->ptarget)))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_MSET: {
		if (mset(it->step, it->arg1, (card*)(it->ptarget), it->peffect, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SSET: {
		if (sset(it->step, it->arg1, (card*)(it->ptarget)))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SPSUMMON_STEP: {
		if (special_summon_step(it->step, it->ptarget, (card*)(it->arg2)))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_DRAW	: {
		if (draw(it->step, it->peffect, it->arg1, (it->arg2 >> 28) & 0xf, (it->arg2 >> 24) & 0xf, it->arg2 & 0xffffff))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_DAMAGE: {
		int32 reason = it->arg1;
		effect* peffect = 0;
		card* pcard = 0;
		if(reason & REASON_BATTLE)
			pcard = (card*)it->peffect;
		else
			peffect = it->peffect;
		if (damage(it->step, peffect, reason, (it->arg2 >> 28) & 0xf, pcard, (it->arg2 >> 24) & 0xf, it->arg2 & 0xffffff))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_RECOVER: {
		if (recover(it->step, it->peffect, it->arg1, (it->arg2 >> 28) & 0xf, (it->arg2 >> 24) & 0xf, it->arg2 & 0xffffff))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_EQUIP: {
		if (equip(it->step, it->arg2 & 0xffff, (card*)it->arg1, (card*)it->ptarget, (it->arg2 >> 16)))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_GET_CONTROL: {
		if (get_control(it->step, it->peffect, (it->arg2 >> 24) & 0xff, (card*)it->ptarget, (it->arg2 >> 16) & 0xff, (it->arg2 >> 8) & 0xff, it->arg2 & 0xff)) {
			core.units.pop_front();
		} else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SWAP_CONTROL: {
		if (swap_control(it->step, it->peffect, (it->arg2 >> 24) & 0xff, (card*)it->ptarget, (card*)it->arg1, (it->arg2 >> 8) & 0xff, it->arg2 & 0xff)) {
			core.units.pop_front();
		} else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_PAY_LPCOST: {
		if (pay_lp_cost(it->step, it->arg1, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_REMOVE_COUNTER: {
		if (remove_counter(it->step, (ptr)it->peffect, (card*)it->ptarget, (it->arg1 >> 16) & 0xff, (it->arg1 >> 8) & 0xff, it->arg1 & 0xff, it->arg2 & 0xffff, it->arg2 >> 16)) {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_BOOLEAN);
			core.units.pop_front();
		} else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_DESTROY_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_DESTROY, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_RELEASE_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_RELEASE, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SENDTO_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SENDTO, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_CHANGEPOS_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_CHANGEPOS, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_ANNOUNCE_RACE: {
		if(announce_race(it->step, it->arg1 & 0xffff, it->arg1 >> 16, it->arg2)) {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		} else {
			core.units.begin()->step++;
		}
		return PROCESSOR_WAITING + pduel->bufferlen;
	}
	case PROCESSOR_ANNOUNCE_ATTRIB: {
		if(announce_attribute(it->step, it->arg1 & 0xffff, it->arg1 >> 16, it->arg2)) {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		} else {
			core.units.begin()->step++;
		}
		return PROCESSOR_WAITING + pduel->bufferlen;
	}
	case PROCESSOR_ANNOUNCE_CARD: {
		if(announce_card(it->step, it->arg1)) {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		} else {
			core.units.begin()->step++;
		}
		return PROCESSOR_WAITING + pduel->bufferlen;
	}
	case PROCESSOR_ANNOUNCE_NUMBER: {
		if(announce_number(it->step, it->arg1)) {
			pduel->lua->add_param(core.select_options[returns.ivalue[0]], PARAM_TYPE_INT);
			core.units.pop_front();
		} else {
			core.units.begin()->step++;
		}
		return PROCESSOR_WAITING + pduel->bufferlen;
	}
	case PROCESSOR_TOSS_DICE: {
		if (toss_dice(it->step, it->arg1, it->arg2)) {
			pduel->write_buffer8(MSG_TOSS_DICE);
			pduel->write_buffer8(it->arg1);
			pduel->write_buffer8(it->arg2);
			for(int32 i = 0; i < it->arg2; ++i) {
				pduel->lua->add_param(core.dice_result[i], PARAM_TYPE_INT);
				pduel->write_buffer8(core.dice_result[i]);
			}
			core.units.pop_front();
		} else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_TOSS_COIN: {
		if (toss_coin(it->step, it->arg1, it->arg2)) {
			pduel->write_buffer8(MSG_TOSS_COIN);
			pduel->write_buffer8(it->arg1);
			pduel->write_buffer8(it->arg2);
			for(int32 i = 0; i < it->arg2; ++i) {
				pduel->lua->add_param(core.coin_result[i], PARAM_TYPE_INT);
				pduel->write_buffer8(core.coin_result[i]);
			}
			core.units.pop_front();
		} else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_EFFECTYN_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_EFFECTYN, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_BOOLEAN);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_YESNO_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_YESNO, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_BOOLEAN);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_OPTION_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_OPTION, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			pduel->write_buffer8(MSG_HINT);
			pduel->write_buffer8(HINT_OPSELECTED);
			pduel->write_buffer8(it->arg1);
			pduel->write_buffer32(core.select_options[returns.ivalue[0]]);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_CARD_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_CARD, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			group* pgroup = pduel->new_group();
			card* pcard;
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				pcard = core.select_cards[returns.bvalue[i + 1]];
				pgroup->container.insert(pcard);
			}
			pduel->lua->add_param(pgroup, PARAM_TYPE_GROUP);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_PLACE_S:
		break;
	case PROCESSOR_SELECT_POSITION_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_POSITION, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_RELEASE_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_RELEASE, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			group* pgroup = pduel->new_group();
			card* pcard;
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				pcard = core.select_cards[returns.bvalue[i + 1]];
				pgroup->container.insert(pcard);
			}
			pduel->lua->add_param(pgroup, PARAM_TYPE_GROUP);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_TRIBUTE_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_TRIBUTE, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			group* pgroup = pduel->new_group();
			card* pcard;
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				pcard = core.select_cards[returns.bvalue[i + 1]];
				pgroup->container.insert(pcard);
			}
			pduel->lua->add_param(pgroup, PARAM_TYPE_GROUP);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SORT_CARDS_S:
		core.units.pop_front();
		break;
	case PROCESSOR_SELECT_TARGET: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_CARD, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			if(core.continuous_chain.size()) {
				if(!core.continuous_chain.rbegin()->target_cards) {
					core.continuous_chain.rbegin()->target_cards = pduel->new_group();
					core.continuous_chain.rbegin()->target_cards->is_readonly = TRUE;
				}
				for(int32 i = 0; i < returns.bvalue[0]; ++i)
					core.continuous_chain.rbegin()->target_cards->container.insert(core.select_cards[returns.bvalue[i + 1]]);
				pduel->lua->add_param(core.continuous_chain.rbegin()->target_cards, PARAM_TYPE_GROUP);
			} else if(core.current_chain.size()) {
				effect* peffect = core.current_chain.rbegin()->triggering_effect;
				if(!core.current_chain.rbegin()->target_cards) {
					core.current_chain.rbegin()->target_cards = pduel->new_group();
					core.current_chain.rbegin()->target_cards->is_readonly = TRUE;
				}
				group* tg = core.current_chain.rbegin()->target_cards;
				group* pret = pduel->new_group();
				for(int32 i = 0; i < returns.bvalue[0]; ++i) {
					tg->container.insert(core.select_cards[returns.bvalue[i + 1]]);
					pret->container.insert(core.select_cards[returns.bvalue[i + 1]]);
				}
				if(returns.bvalue[0] > 0 && peffect->flag & EFFECT_FLAG_CARD_TARGET) {
					for(int32 i = 0; i < returns.bvalue[0]; ++i) {
						card* pcard = core.select_cards[returns.bvalue[i + 1]];
						if(pcard->current.location & 0x30)
							move_card(pcard->current.controler, pcard, pcard->current.location, 0);
						pduel->write_buffer8(MSG_BECOME_TARGET);
						pduel->write_buffer8(1);
						pduel->write_buffer32(core.select_cards[returns.bvalue[i + 1]]->get_info_location());
					}
				}
				pduel->lua->add_param(pret, PARAM_TYPE_GROUP);
			}
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_FUSION: {
		if(it->step == 0) {
			event e;
			e.event_cards = it->ptarget;
			e.reason_effect = it->peffect;
			e.reason_player = it->arg1;
			core.fusion_materials.clear();
			if(!it->peffect) {
				core.units.pop_front();
				return pduel->bufferlen;
			}
			core.sub_solving_event.push_back(e);
			pduel->lua->add_param(it->arg2, PARAM_TYPE_CARD);
			pduel->lua->add_param(it->arg1 >> 16, PARAM_TYPE_INT);
			add_process(PROCESSOR_EXECUTE_OPERATION, 0, it->peffect, 0, it->arg1 & 0xffff, 0);
			core.units.begin()->step++;
		} else {
			group* pgroup = pduel->new_group();
			card_set::iterator cit;
			for(cit = core.fusion_materials.begin(); cit != core.fusion_materials.end(); ++cit) {
				pgroup->container.insert(*cit);
			}
			if(it->arg2)
				pgroup->container.insert((card*)it->arg2);
			pduel->lua->add_param(pgroup, PARAM_TYPE_GROUP);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_SYNCHRO: {
		if (select_synchro_material(it->step, it->arg1, (card*)it->ptarget, it->arg2))
			core.units.pop_front();
		else
			core.units.begin()->step++;
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_SUM_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_SUM, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			group* pgroup = pduel->new_group();
			card* pcard;
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				pcard = core.select_cards[returns.bvalue[i + 1]];
				pgroup->container.insert(pcard);
			}
			pduel->lua->add_param(pgroup, PARAM_TYPE_GROUP);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SELECT_DISFIELD_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SELECT_DISFIELD, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			int32 playerid = it->arg1 & 0xffff;
			int32 count = (it->arg1) >> 16;
			int32 dfflag = 0;
			uint8 p, l, s, pa = 0;
			for(int32 i = 0; i < count; ++i) {
				p = returns.bvalue[pa];
				l = returns.bvalue[pa + 1];
				s = returns.bvalue[pa + 2];
				dfflag |= 1 << (s + (p == playerid ? 0 : 16) + (l == LOCATION_MZONE ? 0 : 8));
				pa += 3;
			}
			pduel->lua->add_param(dfflag, PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SPSUMMON_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SPSUMMON, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SPSUMMON_STEP_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SPSUMMON_STEP, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_BOOLEAN);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SPSUMMON_COMP_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SPSUMMON, 1, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_RANDOM_SELECT_S: {
		uint32 count = it->arg2, i = 0, p = 0;
		field::card_set::iterator cit;
		group* pgroup = it->ptarget;
		if(count > pgroup->container.size())
			count = pgroup->container.size();
		if(count == 0) {
			core.units.pop_front();
			return pduel->bufferlen;
		}
		duel* pduel = pgroup->pduel;
		group* newgroup = pduel->new_group();
		if(count == pgroup->container.size())
			newgroup->container = pgroup->container;
		else {
			while(newgroup->container.size() < count) {
				i = pduel->get_next_integer(0, pgroup->container.size() - 1);
				for(p = 0, cit = pgroup->container.begin(); p < i; ++p, ++cit);
				newgroup->container.insert(*cit);
			}
		}
		pduel->lua->add_param(newgroup, PARAM_TYPE_GROUP);
		pduel->write_buffer8(MSG_RANDOM_SELECTED);
		pduel->write_buffer8(it->arg1);
		pduel->write_buffer8(count);
		for(cit = newgroup->container.begin(); cit != newgroup->container.end(); ++cit) {
			pduel->write_buffer32((*cit)->get_info_location());
		}
		core.units.pop_front();
		return pduel->bufferlen;
	}
	case PROCESSOR_DRAW_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_DRAW, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_DAMAGE_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_DAMAGE, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_RECOVER_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_RECOVER, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_EQUIP_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_EQUIP, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_BOOLEAN);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_GET_CONTROL_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_GET_CONTROL, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_BOOLEAN);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SWAP_CONTROL_S: {
		if(it->step == 0) {
			add_process(PROCESSOR_SWAP_CONTROL, 0, it->peffect, it->ptarget, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_BOOLEAN);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_DISCARD_HAND_S: {
		if(it->step == 0) {
			pduel->write_buffer8(MSG_HINT);
			pduel->write_buffer8(HINT_SELECTMSG);
			pduel->write_buffer8(it->arg1);
			if(((ptr)it->ptarget) & REASON_DISCARD)
				pduel->write_buffer32(501);
			else
				pduel->write_buffer32(504);
			add_process(PROCESSOR_SELECT_CARD, 0, 0, 0, it->arg1, it->arg2);
			core.units.begin()->step++;
		} else if(it->step == 1) {
			card_set cset;
			card* pcard;
			for(int32 i = 0; i < returns.bvalue[0]; ++i) {
				pcard = core.select_cards[returns.bvalue[i + 1]];
				cset.insert(pcard);
			}
			if(cset.size())
				send_to(&cset, core.reason_effect, (ptr)(it->ptarget), core.reason_player, it->arg1, LOCATION_GRAVE, 0, POS_FACEUP);
			else
				returns.ivalue[0] = 0;
			core.units.begin()->step++;
		} else {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_DISCARD_DECK_S: {
		if(discard_deck(it->step, it->arg1 & 0xff, it->arg1 >> 16, it->arg2)) {
			pduel->lua->add_param(returns.ivalue[0], PARAM_TYPE_INT);
			core.units.pop_front();
		} else {
			core.units.begin()->step++;
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_SORT_DECK_S: {
		uint8 sort_player = it->arg1 & 0xffff;
		uint8 target_player = it->arg1 >> 16;
		uint8 count = it->arg2, i = 0;
		if(count > player[target_player].list_main.size())
			count = player[target_player].list_main.size();
		if(it->step == 0) {
			core.select_cards.clear();
			for(auto clit = player[target_player].list_main.rbegin(); i < count; ++i, ++clit)
				core.select_cards.push_back(*clit);
			add_process(PROCESSOR_SORT_CARD, 0, 0, 0, sort_player, 0);
			core.units.begin()->step++;
		} else {
			if(returns.bvalue[0] != -1) {
				card* tc[16];
				for(i = 0; i < count; ++i)
					player[target_player].list_main.pop_back();
				for(i = 0; i < count; ++i)
					tc[(int32)returns.bvalue[i]] = core.select_cards[i];
				for(i = 0; i < count; ++i) {
					player[target_player].list_main.push_back(tc[count - i - 1]);
					tc[count - i - 1]->current.sequence = player[target_player].list_main.size() - 1;
				}
			}
			pduel->write_buffer8(MSG_REFRESH_DECK);
			pduel->write_buffer8(target_player);
			core.units.pop_front();
		}
		return pduel->bufferlen;
	}
	case PROCESSOR_REMOVEOL_S: {
		if(remove_overlay_card(it->step, (ptr)(it->peffect), (card*)(it->ptarget), it->arg1 >> 16,
		                       (it->arg1 >> 8) & 0xff, it->arg1 & 0xff, it->arg2 & 0xffff, it->arg2 >> 16)) {
			core.units.pop_front();
		} else {
			core.units.begin()->step++;
		}
		return pduel->bufferlen;
	}
	}
	return pduel->bufferlen;
}
int32 field::execute_cost(uint16 step, effect * triggering_effect, uint8 triggering_player) {
	if(!triggering_effect->cost) {
		core.solving_event.splice(core.solving_event.begin(), core.sub_solving_event);
		pduel->lua->params.clear();
		return TRUE;
	}
	if (step == 0) {
		core.solving_event.splice(core.solving_event.begin(), core.sub_solving_event);
		event e = *core.solving_event.begin();
		pduel->lua->add_param(1, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.reason_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.reason, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.reason_effect , PARAM_TYPE_EFFECT, true);
		pduel->lua->add_param(e.event_value, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.event_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.event_cards , PARAM_TYPE_GROUP, true);
		pduel->lua->add_param(triggering_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(triggering_effect, PARAM_TYPE_EFFECT, true);
		if(core.check_level == 0) {
			core.shuffle_deck_check[0] = FALSE;
			core.shuffle_deck_check[1] = FALSE;
			core.shuffle_hand_check[0] = FALSE;
			core.shuffle_hand_check[1] = FALSE;
		}
		core.shuffle_check_disabled = FALSE;
		core.check_level++;
	}
	core.reason_effect = triggering_effect;
	core.reason_player = triggering_player;
	uint32 count = pduel->lua->params.size();
	uint32 yield_value = 0;
	int32 result = pduel->lua->call_coroutine(triggering_effect->cost, count, &yield_value, step);
	returns.ivalue[0] = yield_value;
	if (result == COROUTINE_FINISH || result == COROUTINE_ERROR || result == OPERATION_FAIL) {
		core.reason_effect = 0;
		core.reason_player = PLAYER_NONE;
		core.check_level--;
		if(core.check_level == 0) {
			if(core.shuffle_hand_check[0])
				shuffle(0, LOCATION_HAND);
			if(core.shuffle_hand_check[1])
				shuffle(1, LOCATION_HAND);
			if(core.shuffle_deck_check[0])
				shuffle(0, LOCATION_DECK);
			if(core.shuffle_deck_check[1])
				shuffle(1, LOCATION_DECK);
		}
		core.shuffle_check_disabled = FALSE;
		return TRUE;
	}
	return FALSE;
}
int32 field::execute_operation(uint16 step, effect * triggering_effect, uint8 triggering_player) {
	if(!triggering_effect->operation) {
		core.solving_event.splice(core.solving_event.begin(), core.sub_solving_event);
		pduel->lua->params.clear();
		return TRUE;
	}
	if (step == 0) {
		core.solving_event.splice(core.solving_event.begin(), core.sub_solving_event);
		event e = *core.solving_event.begin();
		pduel->lua->add_param(e.reason_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.reason, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.reason_effect , PARAM_TYPE_EFFECT, true);
		pduel->lua->add_param(e.event_value, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.event_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.event_cards , PARAM_TYPE_GROUP, true);
		pduel->lua->add_param(triggering_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(triggering_effect, PARAM_TYPE_EFFECT, true);
		if(core.check_level == 0) {
			core.shuffle_deck_check[0] = FALSE;
			core.shuffle_deck_check[1] = FALSE;
			core.shuffle_hand_check[0] = FALSE;
			core.shuffle_hand_check[1] = FALSE;
		}
		core.shuffle_check_disabled = FALSE;
		core.check_level++;
	}
	core.reason_effect = triggering_effect;
	core.reason_player = triggering_player;
	uint32 count = pduel->lua->params.size();
	uint32 yield_value = 0;
	int32 result = pduel->lua->call_coroutine(triggering_effect->operation, count, &yield_value, step);
	returns.ivalue[0] = yield_value;
	if (result == COROUTINE_FINISH || result == COROUTINE_ERROR || result == OPERATION_FAIL) {
		core.reason_effect = 0;
		core.reason_player = PLAYER_NONE;
		core.check_level--;
		if(core.check_level == 0) {
			if(core.shuffle_hand_check[0])
				shuffle(0, LOCATION_HAND);
			if(core.shuffle_hand_check[1])
				shuffle(1, LOCATION_HAND);
			if(core.shuffle_deck_check[0])
				shuffle(0, LOCATION_DECK);
			if(core.shuffle_deck_check[1])
				shuffle(1, LOCATION_DECK);
		}
		core.shuffle_check_disabled = FALSE;
		return TRUE;
	}
	return FALSE;
}
int32 field::execute_target(uint16 step, effect * triggering_effect, uint8 triggering_player) {
	if(!triggering_effect->target) {
		core.solving_event.splice(core.solving_event.begin(), core.sub_solving_event);
		pduel->lua->params.clear();
		return TRUE;
	}
	if (step == 0) {
		core.solving_event.splice(core.solving_event.begin(), core.sub_solving_event);
		event e = *core.solving_event.begin();
		pduel->lua->add_param(1, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.reason_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.reason, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.reason_effect , PARAM_TYPE_EFFECT, true);
		pduel->lua->add_param(e.event_value, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.event_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(e.event_cards , PARAM_TYPE_GROUP, true);
		pduel->lua->add_param(triggering_player, PARAM_TYPE_INT, true);
		pduel->lua->add_param(triggering_effect, PARAM_TYPE_EFFECT, true);
		if(core.check_level == 0) {
			core.shuffle_deck_check[0] = FALSE;
			core.shuffle_deck_check[1] = FALSE;
			core.shuffle_hand_check[0] = FALSE;
			core.shuffle_hand_check[1] = FALSE;
		}
		core.shuffle_check_disabled = FALSE;
		core.check_level++;
	}
	core.reason_effect = triggering_effect;
	core.reason_player = triggering_player;
	uint32 count = pduel->lua->params.size();
	uint32 yield_value = 0;
	int32 result = pduel->lua->call_coroutine(triggering_effect->target, count, &yield_value, step);
	returns.ivalue[0] = yield_value;
	if (result == COROUTINE_FINISH || result == COROUTINE_ERROR || result == OPERATION_FAIL) {
		core.reason_effect = 0;
		core.reason_player = PLAYER_NONE;
		core.check_level--;
		if(core.check_level == 0) {
			if(core.shuffle_hand_check[0])
				shuffle(0, LOCATION_HAND);
			if(core.shuffle_hand_check[1])
				shuffle(1, LOCATION_HAND);
			if(core.shuffle_deck_check[0])
				shuffle(0, LOCATION_DECK);
			if(core.shuffle_deck_check[1])
				shuffle(1, LOCATION_DECK);
		}
		core.shuffle_check_disabled = FALSE;
		return TRUE;
	}
	return FALSE;
}
void field::raise_event(card * event_card, uint32 event_code, effect * reason_effect, uint32 reason, uint8 reason_player, uint8 event_player, uint32 event_value) {
	event new_event;
	if (event_card) {
		group* pgroup = pduel->new_group(event_card);
		pgroup->is_readonly = TRUE;
		new_event.event_cards = (void*)pgroup;
	} else
		new_event.event_cards = 0;
	new_event.event_code = event_code;
	new_event.reason_effect = reason_effect;
	new_event.reason = reason;
	new_event.reason_player = reason_player;
	new_event.event_player = event_player;
	new_event.event_value = event_value;
	core.queue_event.push_back(new_event);
}
void field::raise_event(card_set * event_cards, uint32 event_code, effect * reason_effect, uint32 reason, uint8 reason_player, uint8 event_player, uint32 event_value) {
	event new_event;
	if (event_cards) {
		group* pgroup = pduel->new_group();
		pgroup->is_readonly = TRUE;
		pgroup->container = *event_cards;
		new_event.event_cards = (void*)pgroup;
	} else new_event.event_cards = 0;
	new_event.event_code = event_code;
	new_event.reason_effect = reason_effect;
	new_event.reason = reason;
	new_event.reason_player = reason_player;
	new_event.event_player = event_player;
	new_event.event_value = event_value;
	core.queue_event.push_back(new_event);
}
void field::raise_single_event(card * event_card, uint32 event_code, effect * reason_effect, uint32 reason, uint8 reason_player, uint8 event_player, uint32 event_value) {
	event new_event;
	new_event.event_cards = (void*) event_card;
	new_event.event_code = event_code;
	new_event.reason_effect = reason_effect;
	new_event.reason = reason;
	new_event.reason_player = reason_player;
	new_event.event_player = event_player;
	new_event.event_value = event_value;
	core.single_event.push_back(new_event);
}
int32 field::check_event(uint32 code, event * pe) {
	event_list::iterator eit;
	for(eit = core.point_event.begin(); eit != core.point_event.end(); ++eit) {
		if(eit->event_code == code) {
			if(pe)
				*pe = *eit;
			return TRUE;
		}
	}
	for(eit = core.instant_event.begin(); eit != core.instant_event.end(); ++eit) {
		if(eit->event_code == code) {
			if(pe)
				*pe = *eit;
			return TRUE;
		}
	}
	return FALSE;
}
int32 field::check_hint_timing(effect* peffect) {
	int32 p = peffect->get_handler_player();
	if(p == 0)
		return (peffect->hint_timing[0] & core.hint_timing[0]) || (peffect->hint_timing[1] & core.hint_timing[1]);
	else
		return (peffect->hint_timing[0] & core.hint_timing[1]) || (peffect->hint_timing[1] & core.hint_timing[0]);
}
int32 field::process_phase_event(int16 step, int32 phase) {
	switch(step) {
	case 0: {
		if(infos.phase == PHASE_STANDBY && is_player_affected_by_effect(infos.turn_id, EFFECT_SKIP_SP)) {
			core.units.begin()->step = 14;
		}
		return FALSE;
	}
	case 1: {
		int32 phase_event = EVENT_PHASE + phase;
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.trigger_f_effect.equal_range(phase_event);
		effect* peffect;
		effect* solve_effect = 0;
		nil_event.event_code = phase_event;
		chain newchain;
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(solve_effect && peffect->get_handler_player() != infos.turn_player)
				continue;
			if(!peffect->is_activateable(peffect->handler->current.controler, nil_event))
				continue;
			if(peffect->get_handler_player() == infos.turn_player) {
				solve_effect = peffect;
				break;
			} else {
				if(!solve_effect)
					solve_effect = peffect;
			}
		}
		if(solve_effect) {
			newchain.flag = 0;
			newchain.chain_id = infos.field_id++;
			newchain.evt = nil_event;
			newchain.triggering_effect = solve_effect;
			newchain.triggering_controler = solve_effect->handler->current.controler;
			newchain.triggering_location = solve_effect->handler->current.location;
			newchain.triggering_sequence = solve_effect->handler->current.sequence;
			newchain.triggering_player = solve_effect->handler->current.controler;
			core.new_chains.push_back(newchain);
			solve_effect->handler->set_status(STATUS_CHAINING, TRUE);
			core.select_chains.clear();
			add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
			add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, FALSE, 1 - infos.turn_player);
			infos.priorities[0] = 0;
			infos.priorities[1] = 0;
			return FALSE;
		}
		core.units.begin()->step = 2;
		return FALSE;
	}
	case 2: {
		if(core.current_chain.size()) {
			core.chain_limit = 0;
			chain_array::iterator cait;
			for(cait = core.current_chain.begin(); cait != core.current_chain.end(); ++cait)
				cait->triggering_effect->handler->set_status(STATUS_CHAINING, FALSE);
			add_process(PROCESSOR_SOLVE_CHAIN, 0, 0, 0, FALSE, 0);
		}
		core.units.begin()->step = 0;
		return FALSE;
	}
	case 3: {
		int32 phase_event = EVENT_PHASE + phase;
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.trigger_o_effect.equal_range(phase_event);
		effect* peffect;
		nil_event.event_code = phase_event;
		chain newchain;
		core.tpchain.clear();
		core.ntpchain.clear();
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(!peffect->is_activateable(peffect->get_handler_player(), nil_event))
				continue;
			newchain.triggering_effect = peffect;
			if(peffect->get_handler_player() == infos.turn_player)
				core.tpchain.push_back(newchain);
			else
				core.ntpchain.push_back(newchain);
		}
		core.spe_effect[infos.turn_player] = core.tpchain.size();
		core.spe_effect[1 - infos.turn_player] = core.ntpchain.size();
		if(phase == PHASE_DRAW)
			core.hint_timing[infos.turn_player] = TIMING_DRAW_PHASE;
		else if(phase == PHASE_STANDBY)
			core.hint_timing[infos.turn_player] = TIMING_STANDBY_PHASE;
		else if(phase == PHASE_BATTLE)
			core.hint_timing[infos.turn_player] = TIMING_BATTLE_END;
		else core.hint_timing[infos.turn_player] = TIMING_END_PHASE;
		pr = effects.activate_effect.equal_range(EVENT_FREE_CHAIN);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(!peffect->is_chainable(peffect->get_handler_player()) || !peffect->is_activateable(peffect->get_handler_player(), nil_event))
				continue;
			newchain.triggering_effect = peffect;
			if(peffect->get_handler_player() == infos.turn_player) {
				if(check_hint_timing(peffect))
					core.spe_effect[infos.turn_player]++;
				core.tpchain.push_back(newchain);
			} else {
				if(check_hint_timing(peffect))
					core.spe_effect[1 - infos.turn_player]++;
				core.ntpchain.push_back(newchain);
			}
		}
		pr = effects.instant_o_effect.equal_range(EVENT_FREE_CHAIN);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(!peffect->is_chainable(peffect->get_handler_player()) || !peffect->is_activateable(peffect->get_handler_player(), nil_event))
				continue;
			newchain.triggering_effect = peffect;
			if(peffect->get_handler_player() == infos.turn_player) {
				core.tpchain.push_back(newchain);
				if(check_hint_timing(peffect))
					core.spe_effect[infos.turn_player]++;
			} else {
				core.ntpchain.push_back(newchain);
				if(check_hint_timing(peffect))
					core.spe_effect[1 - infos.turn_player]++;
			}
		}
		core.select_chains.clear();
		if(core.tpchain.size()) {
			for(chain_list::iterator clit = core.tpchain.begin(); clit != core.tpchain.end(); ++clit)
				core.select_chains.push_back(*clit);
			pduel->write_buffer8(MSG_HINT);
			pduel->write_buffer8(HINT_EVENT);
			pduel->write_buffer8(infos.turn_player);
			if(infos.phase == PHASE_DRAW)
				pduel->write_buffer32(20);
			else if(infos.phase == PHASE_STANDBY)
				pduel->write_buffer32(21);
			else if(infos.phase == PHASE_BATTLE)
				pduel->write_buffer32(25);
			else
				pduel->write_buffer32(26);
			add_process(PROCESSOR_SELECT_CHAIN, 0, 0, 0, infos.turn_player, core.spe_effect[infos.turn_player]);
			return FALSE;
		}
		core.units.begin()->step = 4;
		return FALSE;
	}
	case 4: {
		chain newchain;
		if(returns.ivalue[0] == -1)
			return FALSE;
		newchain = core.select_chains[returns.ivalue[0]];
		effect* peffect = newchain.triggering_effect;
		newchain.flag = 0;
		newchain.chain_id = infos.field_id++;
		newchain.evt = nil_event;
		newchain.triggering_effect = peffect;
		newchain.triggering_controler = peffect->handler->current.controler;
		newchain.triggering_location = peffect->handler->current.location;
		newchain.triggering_sequence = peffect->handler->current.sequence;
		newchain.triggering_player = peffect->handler->current.controler;
		core.new_chains.push_back(newchain);
		newchain.triggering_effect->handler->set_status(STATUS_CHAINING, TRUE);
		core.select_chains.clear();
		add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
		add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, FALSE, 1 - infos.turn_player);
		infos.priorities[0] = 0;
		infos.priorities[1] = 0;
		core.units.begin()->step = 1;
		return FALSE;
	}
	case 5: {
		core.select_chains.clear();
		if(core.ntpchain.size()) {
			for(chain_list::iterator clit = core.ntpchain.begin(); clit != core.ntpchain.end(); ++clit)
				core.select_chains.push_back(*clit);
			pduel->write_buffer8(MSG_HINT);
			pduel->write_buffer8(HINT_EVENT);
			pduel->write_buffer8(1 - infos.turn_player);
			if(infos.phase == PHASE_DRAW)
				pduel->write_buffer32(20);
			else if(infos.phase == PHASE_STANDBY)
				pduel->write_buffer32(21);
			else if(infos.phase == PHASE_BATTLE)
				pduel->write_buffer32(25);
			else
				pduel->write_buffer32(26);
			add_process(PROCESSOR_SELECT_CHAIN, 0, 0, 0, 1 - infos.turn_player, core.spe_effect[1 - infos.turn_player]);
			return FALSE;
		}
		core.units.begin()->step = 6;
		return FALSE;
	}
	case 6: {
		chain newchain;
		if(returns.ivalue[0] == -1)
			return FALSE;
		newchain = core.select_chains[returns.ivalue[0]];
		effect* peffect = newchain.triggering_effect;
		newchain.flag = 0;
		newchain.chain_id = infos.field_id++;
		newchain.evt = nil_event;
		newchain.triggering_effect = peffect;
		newchain.triggering_controler = peffect->handler->current.controler;
		newchain.triggering_location = peffect->handler->current.location;
		newchain.triggering_sequence = peffect->handler->current.sequence;
		newchain.triggering_player = peffect->handler->current.controler;
		core.new_chains.push_back(newchain);
		newchain.triggering_effect->handler->set_status(STATUS_CHAINING, TRUE);
		core.select_chains.clear();
		add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
		add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, FALSE, infos.turn_player);
		infos.priorities[0] = 0;
		infos.priorities[1] = 0;
		core.units.begin()->step = 1;
		return FALSE;
	}
	case 7: {
		core.hint_timing[infos.turn_player] = 0;
		int32 phase_event = EVENT_PHASE + phase;
		pair<effect_container::iterator, effect_container::iterator> pr;
		pr = effects.continuous_effect.equal_range(phase_event);
		effect_vector tp;
		effect_vector ntp;
		effect_vector::iterator eit;
		effect* peffect;
		nil_event.event_code = phase_event;
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			uint8 owner_player = peffect->get_handler_player();
			if(peffect->is_activateable(owner_player, nil_event)) {
				if(owner_player == infos.turn_player)
					tp.push_back(peffect);
				else
					ntp.push_back(peffect);
			}
		}
		for(eit = tp.begin(); eit != tp.end(); ++eit) {
			core.sub_solving_event.push_back(nil_event);
			add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, (*eit), 0, (*eit)->get_handler_player(), 0);
		}
		for(eit = ntp.begin(); eit != ntp.end(); ++eit) {
			core.sub_solving_event.push_back(nil_event);
			add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, (*eit), 0, (*eit)->get_handler_player(), 0);
		}
		if(core.sub_solving_event.size() == 0)
			core.units.begin()->step = 8;
		return FALSE;
	}
	case 8: {
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		core.units.begin()->step = 0;
		return FALSE;
	}
	case 9: {
		if(phase != PHASE_END) {
			core.units.begin()->step = 14;
			return FALSE;
		}
		int32 limit = 6;
		effect_set eset;
		filter_player_effect(infos.turn_player, EFFECT_HAND_LIMIT, &eset);
		if(eset.count)
			limit = eset.get_last()->get_value();
		int32 hd = player[infos.turn_player].list_hand.size();
		if(hd <= limit) {
			core.units.begin()->step = 14;
			return FALSE;
		}
		core.select_cards.clear();
		for(auto cit = player[infos.turn_player].list_hand.begin(); cit != player[infos.turn_player].list_hand.end(); ++cit)
			core.select_cards.push_back(*cit);
		add_process(PROCESSOR_SELECT_CARD, 0, 0, 0, infos.turn_player, hd - limit + ((hd - limit) << 16));
		return FALSE;
	}
	case 10: {
		card_set cset;
		for(int32 i = 0; i < returns.bvalue[0]; ++i)
			cset.insert(core.select_cards[returns.bvalue[i + 1]]);
		send_to(&cset, 0, REASON_RULE + REASON_DISCARD + REASON_ADJUST, infos.turn_player, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
		return FALSE;
	}
	case 11: {
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		core.units.begin()->step = 0;
		return FALSE;
	}
	case 15: {
		core.hint_timing[infos.turn_player] = 0;
		reset_phase(phase);
		adjust_all();
		return FALSE;
	}
	case 16: {
		core.new_fchain.clear();
		core.new_ochain.clear();
		core.instant_f_chain.clear();
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::process_point_event(int16 step, int32 special, int32 skip_new) {
	chain_list::iterator clit;
	switch(step) {
	case 0: {
		if(core.instant_event.size() == 0)
			return TRUE;
		core.select_chains.clear();
		core.point_event.splice(core.point_event.end(), core.instant_event);
		if(special) {
			core.units.begin()->step = 7;
			return FALSE;
		}
		core.new_fchain_s.splice(core.new_fchain_s.begin(), core.new_fchain);
		core.new_ochain_s.splice(core.new_ochain_s.begin(), core.new_ochain);
		//flip
		core.tpchain.clear();
		core.ntpchain.clear();
		for (clit = core.flip_chain.begin(); clit != core.flip_chain.end(); ++clit) {
			if(clit->triggering_effect->is_chainable(clit->triggering_player)
			        && clit->triggering_effect->is_activateable(clit->triggering_player, clit->evt, TRUE)) {
				if(clit->triggering_player == infos.turn_player)
					core.tpchain.push_back(*clit);
				else
					core.ntpchain.push_back(*clit);
				clit->triggering_effect->handler->set_status(STATUS_CHAINING, TRUE);
			}
		}
		if(core.tpchain.size() > 1)
			add_process(PROCESSOR_SORT_CHAIN, 0, 0, 0, 1, infos.turn_player);
		if(core.ntpchain.size() > 1)
			add_process(PROCESSOR_SORT_CHAIN, 0, 0, 0, 0, infos.turn_player);
		core.flip_chain.clear();
		return FALSE;
	}
	case 1: {
		core.new_chains.splice(core.new_chains.end(), core.tpchain);
		core.new_chains.splice(core.new_chains.end(), core.ntpchain);
		if(core.new_chains.size())
			add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 2: {
		//forced trigger
		for (clit = core.new_fchain_s.begin(); clit != core.new_fchain_s.end(); ++clit) {
			if(clit->triggering_effect->handler->current.reason & REASON_TEMPORARY)
				clit->triggering_player = clit->triggering_effect->handler->previous.controler;
			else
				clit->triggering_player = clit->triggering_effect->handler->current.controler;
			if(clit->triggering_effect->is_chainable(clit->triggering_player) && clit->triggering_effect->is_activateable(clit->triggering_player, clit->evt, TRUE)) {
				if(clit->triggering_player == infos.turn_player)
					core.tpchain.push_back(*clit);
				else
					core.ntpchain.push_back(*clit);
				clit->triggering_effect->handler->set_status(STATUS_CHAINING, TRUE);
			}
		}
		if(core.tpchain.size() > 1)
			add_process(PROCESSOR_SORT_CHAIN, 0, 0, 0, 1, infos.turn_player);
		if(core.ntpchain.size() > 1)
			add_process(PROCESSOR_SORT_CHAIN, 0, 0, 0, 0, infos.turn_player);
		core.new_fchain_s.clear();
		return FALSE;
	}
	case 3: {
		core.new_chains.splice(core.new_chains.end(), core.tpchain);
		core.new_chains.splice(core.new_chains.end(), core.ntpchain);
		if(core.new_chains.size())
			add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 4: {
		//optional trigger
		for (clit = core.new_ochain_s.begin(); clit != core.new_ochain_s.end(); ++clit) {
			if(clit->triggering_effect->handler->current.reason & REASON_TEMPORARY)
				clit->triggering_player = clit->triggering_effect->handler->previous.controler;
			else
				clit->triggering_player = clit->triggering_effect->handler->current.controler;
			if(clit->triggering_effect->is_chainable(clit->triggering_player) && clit->triggering_effect->is_activateable(clit->triggering_player, clit->evt, TRUE)) {
				if(clit->triggering_player == infos.turn_player)
					core.tpchain.push_back(*clit);
				else
					core.ntpchain.push_back(*clit);
				clit->triggering_effect->handler->set_status(STATUS_CHAINING, TRUE);
			}
		}
		if(core.tpchain.size() > 1)
			add_process(PROCESSOR_SORT_CHAIN, 0, 0, 0, 1, infos.turn_player);
		if(core.ntpchain.size() > 1)
			add_process(PROCESSOR_SORT_CHAIN, 0, 0, 0, 0, infos.turn_player);
		core.new_ochain_s.clear();
		return FALSE;
	}
	case 5: {
		bool act;
		if(core.tpchain.size()) {
			act = true;
			chain_list::iterator clit;
			chain_list::iterator tpit = core.tpchain.begin();
			if(!(tpit->triggering_effect->flag & EFFECT_FLAG_MULTIACT_HAND)) {
				for(clit = core.new_chains.begin(); clit != core.new_chains.end(); ++clit) {
					if(tpit->triggering_location == LOCATION_HAND) {
						if((clit->triggering_location == LOCATION_HAND && clit->triggering_player == tpit->triggering_player)
						        || clit->triggering_effect->handler->data.code == tpit->triggering_effect->handler->data.code) {
							act = false;
							break;
						}
					} else {
						if(clit->triggering_location == LOCATION_HAND && clit->triggering_player == tpit->triggering_player
						        && clit->triggering_effect->handler->data.code == tpit->triggering_effect->handler->data.code) {
							act = false;
							break;
						}
					}
				}
			}
			if(act)
				add_process(PROCESSOR_SELECT_EFFECTYN, 0, 0, (group*)tpit->triggering_effect->handler, tpit->triggering_player, 0);
			else returns.ivalue[0] = FALSE;
		} else if(core.ntpchain.size()) {
			act = true;
			chain_list::iterator clit;
			chain_list::iterator ntpit = core.ntpchain.begin();
			if(!(ntpit->triggering_effect->flag & EFFECT_FLAG_MULTIACT_HAND)) {
				for(clit = core.new_chains.begin(); clit != core.new_chains.end(); ++clit) {
					if(ntpit->triggering_location == LOCATION_HAND) {
						if((clit->triggering_location == LOCATION_HAND && clit->triggering_player == ntpit->triggering_player)
						        || clit->triggering_effect->handler->data.code == ntpit->triggering_effect->handler->data.code) {
							act = false;
							break;
						}
					} else {
						if(clit->triggering_location == LOCATION_HAND && clit->triggering_player == ntpit->triggering_player
						        && clit->triggering_effect->handler->data.code == ntpit->triggering_effect->handler->data.code) {
							act = false;
							break;
						}
					}
				}
			}
			if(act)
				add_process(PROCESSOR_SELECT_EFFECTYN, 0, 0, (group*)ntpit->triggering_effect->handler, ntpit->triggering_player, 0);
			else returns.ivalue[0] = FALSE;
		} else {
			core.units.begin()->step = 6;
			if(core.new_chains.size())
				add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
		}
		return FALSE;
	}
	case 6: {
		if(returns.ivalue[0]) {
			if(core.tpchain.size()) {
				core.new_chains.push_back(core.tpchain.front());
				core.tpchain.pop_front();
			} else {
				core.new_chains.push_back(core.ntpchain.front());
				core.ntpchain.pop_front();
			}
		} else {
			if(core.tpchain.size())
				core.tpchain.pop_front();
			else
				core.ntpchain.pop_front();
		}
		core.units.begin()->step = 4;
		return FALSE;
	}
	case 7: {
		/* Due to the new official rules, the priority of ignition effects when summon, special summon, flip summon is canceled
		//startup & quick effect
		event e;
		if(core.current_chain.size() == 0 &&
		        (check_event(EVENT_SUMMON_SUCCESS, &e) || check_event(EVENT_SPSUMMON_SUCCESS, &e) || check_event(EVENT_FLIP_SUMMON_SUCCESS, &e))
		        && e.reason_player == infos.turn_player) {
			effect* peffect;
			effect_container::iterator eit;
			chain newchain;
			event e;
			e.event_cards = 0;
			e.event_value = 0;
			e.event_player = PLAYER_NONE;
			e.reason_effect = 0;
			e.reason = 0;
			e.reason_player = PLAYER_NONE;
			for(eit = effects.startup_effect.begin(); eit != effects.startup_effect.end(); ++eit) {
				peffect = eit->second;
				e.event_code = peffect->code;
				if(peffect->handler->current.location == LOCATION_MZONE && peffect->is_chainable() && peffect->is_activateable(infos.turn_player, e)) {
					newchain.flag = 0;
					newchain.chain_id = infos.chain_id++;
					newchain.evt = e;
					newchain.triggering_controler = peffect->handler->current.controler;
					newchain.triggering_effect = peffect;
					newchain.triggering_location = peffect->handler->current.location;
					newchain.triggering_sequence = peffect->handler->current.sequence;
					newchain.triggering_player = infos.turn_player;
					core.select_chains.push_back(newchain);
				}
			}
		}
		*/
		return FALSE;
	}
	case 8: {
		infos.priorities[0] = 0;
		infos.priorities[1] = 0;
		if(core.current_chain.size() == 0)
			add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, special, infos.turn_player);
		else
			add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, special, 1 - core.current_chain.rbegin()->triggering_player);
		return FALSE;
	}
	case 9: {
		core.chain_limit = 0;
		if(core.current_chain.size()) {
			chain_array::iterator cait;
			for(cait = core.current_chain.begin(); cait != core.current_chain.end(); ++cait)
				cait->triggering_effect->handler->set_status(STATUS_CHAINING, FALSE);
			add_process(PROCESSOR_SOLVE_CHAIN, 0, 0, 0, skip_new, 0);
		} else {
			core.used_event.splice(core.used_event.end(), core.point_event);
			core.chain_limit_p = 0;
			reset_chain();
		}
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::process_quick_effect(int16 step, int32 special, uint8 priority) {
	effect_container::iterator eit;
	pair<effect_container::iterator, effect_container::iterator> pr;
	event_list::iterator evit;
	effect* peffect;
	chain newchain;
	switch(step) {
	case 0: {
		core.tpchain.clear();
		core.ntpchain.clear();
		if(!core.instant_f_chain.size())
			return FALSE;
		instant_f_list::iterator ifit;
		for(ifit = core.instant_f_chain.begin(); ifit != core.instant_f_chain.end(); ++ifit) {
			peffect = ifit->first;
			if(peffect->is_chainable(ifit->second.triggering_player) && peffect->handler->is_has_relation(peffect)) {
				if(ifit->second.triggering_player == infos.turn_player)
					core.tpchain.push_back(ifit->second);
				else
					core.ntpchain.push_back(ifit->second);
				peffect->handler->set_status(STATUS_CHAINING, TRUE);
			}
		}
		core.instant_f_chain.clear();
		if(core.tpchain.size() > 1)
			add_process(PROCESSOR_SORT_CHAIN, 0, 0, 0, 1, infos.turn_player);
		if(core.ntpchain.size() > 1)
			add_process(PROCESSOR_SORT_CHAIN, 0, 0, 0, 0, infos.turn_player);
		return FALSE;
	}
	case 1: {
		if(core.tpchain.size() == 0 && core.ntpchain.size() == 0) {
			chain newchain;
			for(evit = core.point_event.begin(); evit != core.instant_event.end();) {
				pr = effects.activate_effect.equal_range(evit->event_code);
				for(; pr.first != pr.second; ++pr.first) {
					peffect = pr.first->second;
					if(peffect->is_chainable(priority) && peffect->is_activateable(priority, *evit)) {
						newchain.flag = 0;
						newchain.chain_id = infos.field_id++;
						newchain.evt = *evit;
						newchain.triggering_controler = peffect->handler->current.controler;
						newchain.triggering_effect = peffect;
						newchain.triggering_location = peffect->handler->current.location;
						newchain.triggering_sequence = peffect->handler->current.sequence;
						newchain.triggering_player = priority;
						core.select_chains.push_back(newchain);
					}
				}
				pr = effects.instant_o_effect.equal_range(evit->event_code);
				for(; pr.first != pr.second; ++pr.first) {
					peffect = pr.first->second;
					if(peffect->is_chainable(priority) && peffect->is_activateable(priority, *evit)) {
						newchain.flag = 0;
						newchain.chain_id = infos.field_id++;
						newchain.evt = *evit;
						newchain.triggering_controler = peffect->handler->current.controler;
						newchain.triggering_effect = peffect;
						newchain.triggering_location = peffect->handler->current.location;
						newchain.triggering_sequence = peffect->handler->current.sequence;
						newchain.triggering_player = priority;
						core.select_chains.push_back(newchain);
					}
				}
				++evit;
				if(evit == core.point_event.end())
					evit = core.instant_event.begin();
			}
			core.spe_effect[priority] = core.select_chains.size();
			if(!special) {
				nil_event.event_code = EVENT_FREE_CHAIN;
				pr = effects.activate_effect.equal_range(EVENT_FREE_CHAIN);
				for(; pr.first != pr.second; ++pr.first) {
					peffect = pr.first->second;
					if(peffect->is_chainable(priority) && peffect->is_activateable(priority, nil_event)) {
						newchain.flag = 0;
						newchain.chain_id = infos.field_id++;
						newchain.evt = nil_event;
						newchain.triggering_controler = peffect->handler->current.controler;
						newchain.triggering_effect = peffect;
						newchain.triggering_location = peffect->handler->current.location;
						newchain.triggering_sequence = peffect->handler->current.sequence;
						newchain.triggering_player = priority;
						core.select_chains.push_back(newchain);
						if(check_hint_timing(peffect))
							core.spe_effect[priority]++;
					}
				}
			}
			pr = effects.instant_o_effect.equal_range(EVENT_FREE_CHAIN);
			for(; pr.first != pr.second; ++pr.first) {
				peffect = pr.first->second;
				if(peffect->is_chainable(priority) && peffect->is_activateable(priority, nil_event)) {
					newchain.flag = 0;
					newchain.chain_id = infos.field_id++;
					newchain.evt = nil_event;
					newchain.triggering_controler = peffect->handler->current.controler;
					newchain.triggering_effect = peffect;
					newchain.triggering_location = peffect->handler->current.location;
					newchain.triggering_sequence = peffect->handler->current.sequence;
					newchain.triggering_player = priority;
					core.select_chains.push_back(newchain);
					if(check_hint_timing(peffect))
						core.spe_effect[priority]++;
				}
			}
			if(core.current_chain.size() || (core.hint_timing[0]&TIMING_ATTACK) || (core.hint_timing[1]&TIMING_ATTACK))
				core.spe_effect[priority] = core.select_chains.size();
			if(!(core.duel_options & DUEL_NO_CHAIN_HINT) || core.select_chains.size())
				add_process(PROCESSOR_SELECT_CHAIN, 0, 0, 0, priority, core.spe_effect[priority]);
		} else {
			core.new_chains.splice(core.new_chains.end(), core.tpchain);
			core.new_chains.splice(core.new_chains.end(), core.ntpchain);
			add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
			add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, FALSE, 1 - core.new_chains.rbegin()->triggering_player);
			infos.priorities[0] = 0;
			infos.priorities[1] = 0;
			return TRUE;
		}
		return FALSE;
	}
	case 2: {
		if(core.select_chains.size() && returns.ivalue[0] != -1) {
			chain newchain = core.select_chains[returns.ivalue[0]];
			core.new_chains.push_back(newchain);
			newchain.triggering_effect->handler->set_status(STATUS_CHAINING, TRUE);
			add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
			add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, FALSE, 1 - priority);
			infos.priorities[0] = 0;
			infos.priorities[1] = 0;
		} else {
			infos.priorities[priority] = 1;
			if(!infos.priorities[0] || !infos.priorities[1])
				add_process(PROCESSOR_QUICK_EFFECT, 1, 0, 0, special, 1 - priority);
			else {
				core.hint_timing[0] = 0;
				core.hint_timing[1] = 0;
			}
		}
		core.select_chains.clear();
		return TRUE;
	}
	}
	return TRUE;
}

int32 field::process_instant_event() {
	if (core.queue_event.size() == 0)
		return TRUE;
	event_list::iterator elit;
	effect* peffect;
	chain newchain;
	effect_vector tp;
	effect_vector ntp;
	event_list tev;
	event_list ntev;
	effect_vector::iterator eit;
	event_list::iterator evit;
	pair<effect_container::iterator, effect_container::iterator> pr;
	for(elit = core.queue_event.begin(); elit != core.queue_event.end(); ++elit) {
		//continuous events
		pr = effects.continuous_effect.equal_range(elit->event_code);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			uint8 owner_player = peffect->get_handler_player();
			if(peffect->is_activateable(owner_player, *elit)) {
				if((peffect->flag & EFFECT_FLAG_DELAY) && core.chain_solving) {
					if(owner_player == infos.turn_player) {
						core.delayed_tp.push_back(peffect);
						core.delayed_tev.push_back(*elit);
					} else {
						core.delayed_ntp.push_back(peffect);
						core.delayed_ntev.push_back(*elit);
					}
				} else {
					if(owner_player == infos.turn_player) {
						tp.push_back(peffect);
						tev.push_back(*elit);
					} else {
						ntp.push_back(peffect);
						ntev.push_back(*elit);
					}
				}
			}
		}
		if(elit->event_code == EVENT_ADJUST || ((elit->event_code & 0xf000) == EVENT_PHASE_START))
			continue;
		//triggers
		pr = effects.trigger_f_effect.equal_range(elit->event_code);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(!peffect->is_condition_check(peffect->handler->current.controler, *elit))
				continue;
			peffect->handler->create_relation(peffect);
			newchain.flag = 0;
			newchain.chain_id = infos.field_id++;
			newchain.evt = *elit;
			newchain.triggering_effect = peffect;
			newchain.triggering_controler = peffect->handler->current.controler;
			newchain.triggering_location = peffect->handler->current.location;
			newchain.triggering_sequence = peffect->handler->current.sequence;
			newchain.triggering_player = peffect->handler->current.controler;
			core.new_fchain.push_back(newchain);
		}
		pr = effects.trigger_o_effect.equal_range(elit->event_code);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(!peffect->is_condition_check(peffect->handler->current.controler, *elit))
				continue;
			peffect->handler->create_relation(peffect);
			newchain.flag = 0;
			newchain.chain_id = infos.field_id++;
			newchain.evt = *elit;
			newchain.triggering_effect = peffect;
			newchain.triggering_controler = peffect->handler->current.controler;
			newchain.triggering_location = peffect->handler->current.location;
			newchain.triggering_sequence = peffect->handler->current.sequence;
			newchain.triggering_player = peffect->handler->current.controler;
			core.new_ochain.push_back(newchain);
		}
		//instant_f
		pr = effects.instant_f_effect.equal_range(elit->event_code);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(peffect->is_activateable(peffect->handler->current.controler, *elit)) {
				peffect->handler->create_relation(peffect);
				newchain.flag = 0;
				newchain.chain_id = infos.field_id++;
				newchain.evt = *elit;
				newchain.triggering_effect = peffect;
				newchain.triggering_controler = peffect->handler->current.controler;
				newchain.triggering_location = peffect->handler->current.location;
				newchain.triggering_sequence = peffect->handler->current.sequence;
				newchain.triggering_player = peffect->handler->current.controler;
				core.instant_f_chain[peffect] = newchain;
			}
		}
	}
	for(eit = tp.begin(), evit = tev.begin(); eit != tp.end(); ++eit, ++evit) {
		core.sub_solving_event.push_back(*evit);
		add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, (*eit), 0, (*eit)->get_handler_player(), 0);
	}
	for(eit = ntp.begin(), evit = ntev.begin(); eit != ntp.end(); ++eit, ++evit) {
		core.sub_solving_event.push_back(*evit);
		add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, (*eit), 0, (*eit)->get_handler_player(), 0);
	}
	core.instant_event.splice(core.instant_event.end(), core.queue_event);
	return TRUE;
}
int32 field::process_single_event() {
	pair<effect_container::iterator, effect_container::iterator> pr;
	if(core.single_event.size() == 0)
		return TRUE;
	card* starget;
	uint32 ev;
	effect_set eset;
	effect* peffect;
	effect_vector tp;
	effect_vector ntp;
	event_list tev;
	event_list ntev;
	effect_vector::iterator eit;
	event_list::iterator evit;
	for(event_list::iterator elit = core.single_event.begin(); elit != core.single_event.end(); ++elit) {
		starget = (card*)elit->event_cards;
		ev = elit->event_code;
		pr = starget->single_effect.equal_range(ev);
		event e = *elit;
		e.event_cards = 0;
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			if(!(peffect->type & EFFECT_TYPE_ACTIONS))
				continue;
			if((peffect->type & EFFECT_TYPE_FLIP) && e.event_value)
				continue;
			//continuous & trigger (single)
			if(peffect->type & EFFECT_TYPE_CONTINUOUS) {
				uint8 owner_player = peffect->get_handler_player();
				if(peffect->is_activateable(owner_player, e)) {
					if((peffect->flag & EFFECT_FLAG_DELAY) && core.chain_solving) {
						if(owner_player == infos.turn_player) {
							core.delayed_tp.push_back(peffect);
							core.delayed_tev.push_back(*elit);
						} else {
							core.delayed_ntp.push_back(peffect);
							core.delayed_ntev.push_back(*elit);
						}
					} else {
						if(owner_player == infos.turn_player) {
							tp.push_back(peffect);
							tev.push_back(e);
						} else {
							ntp.push_back(peffect);
							ntev.push_back(e);
						}
					}
				}
			} else {
				if(!peffect->is_condition_check(peffect->handler->current.controler, e))
					continue;
				peffect->handler->create_relation(peffect);
				chain newchain;
				newchain.flag = 0;
				newchain.chain_id = infos.field_id++;
				newchain.evt = e;
				newchain.triggering_effect = peffect;
				newchain.triggering_controler = peffect->handler->current.controler;
				newchain.triggering_location = peffect->handler->current.location;
				newchain.triggering_sequence = peffect->handler->current.sequence;
				if(peffect->handler->current.reason & REASON_TEMPORARY)
					newchain.triggering_player = peffect->handler->previous.controler;
				else
					newchain.triggering_player = newchain.triggering_controler;
				if(core.flip_delayed && ev == EVENT_FLIP) {
					if (peffect->type & EFFECT_TYPE_TRIGGER_O) {
						core.new_ochain_b.push_back(newchain);
					} else if (peffect->type & EFFECT_TYPE_TRIGGER_F) {
						core.new_fchain_b.push_back(newchain);
					} else
						core.flip_chain_b.push_back(newchain);
				} else {
					if (peffect->type & EFFECT_TYPE_TRIGGER_O) {
						core.new_ochain.push_back(newchain);
					} else if (peffect->type & EFFECT_TYPE_TRIGGER_F) {
						core.new_fchain.push_back(newchain);
					} else
						core.flip_chain.push_back(newchain);
				}
			}
		}
	}
	for(eit = tp.begin(), evit = tev.begin(); eit != tp.end(); ++eit, ++evit) {
		core.sub_solving_event.push_back(*evit);
		add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, (*eit), 0, (*eit)->get_handler_player(), 0);
	}
	for(eit = ntp.begin(), evit = ntev.begin(); eit != ntp.end(); ++eit, ++evit) {
		core.sub_solving_event.push_back(*evit);
		add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, (*eit), 0, (*eit)->get_handler_player(), 0);
	}
	core.single_event.clear();
	return TRUE;
}
int32 field::process_idle_command(uint16 step) {
	switch(step) {
	case 0: {
		card* pcard;
		effect_container::iterator eit;
		pair<effect_container::iterator, effect_container::iterator> pr;
		effect* peffect;
		core.select_chains.clear();
		chain newchain;
		nil_event.event_code = EVENT_FREE_CHAIN;
		core.to_bp = TRUE;
		core.to_ep = TRUE;
		if((!(core.duel_options & DUEL_ATTACK_FIRST_TURN) && infos.turn_id == 1) ||  infos.phase == PHASE_MAIN2 || is_player_affected_by_effect(infos.turn_player, EFFECT_CANNOT_BP))
			core.to_bp = FALSE;
		if(core.to_bp && infos.phase == PHASE_MAIN1 && is_player_affected_by_effect(infos.turn_player, EFFECT_CANNOT_EP))
			core.to_ep = FALSE;
		if((infos.phase == PHASE_MAIN1 && is_player_affected_by_effect(infos.turn_player, EFFECT_SKIP_M1))
		        || (infos.phase == PHASE_MAIN2 && is_player_affected_by_effect(infos.turn_player, EFFECT_SKIP_M2))) {
			if(core.to_bp && core.to_ep) {
				core.select_options.clear();
				core.select_options.push_back(80);
				core.select_options.push_back(81);
				add_process(PROCESSOR_SELECT_OPTION, 0, 0, 0, infos.turn_player, 0);
				core.units.begin()->step = 11;
			} else if(core.to_bp) {
				core.units.begin()->arg1 = 6;
				core.units.begin()->step = 10;
				reset_phase(infos.phase);
				adjust_all();
			} else {
				core.units.begin()->arg1 = 7;
				core.units.begin()->step = 10;
				reset_phase(infos.phase);
				adjust_all();
			}
			return FALSE;
		}
		pr = effects.activate_effect.equal_range(EVENT_FREE_CHAIN);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			newchain.triggering_effect = peffect;
			if(peffect->is_activateable(infos.turn_player, nil_event))
				core.select_chains.push_back(newchain);
		}
		pr = effects.instant_o_effect.equal_range(EVENT_FREE_CHAIN);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			newchain.triggering_effect = peffect;
			if(peffect->is_activateable(infos.turn_player, nil_event))
				core.select_chains.push_back(newchain);
		}
		for(eit = effects.startup_effect.begin(); eit != effects.startup_effect.end(); ++eit) {
			peffect = eit->second;
			newchain.triggering_effect = peffect;
			if(peffect->is_activateable(infos.turn_player, nil_event))
				core.select_chains.push_back(newchain);
		}
		core.summonable_cards.clear();
		for(auto clit = player[infos.turn_player].list_hand.begin(); clit != player[infos.turn_player].list_hand.end(); ++clit)
			if((*clit)->is_can_be_summoned(infos.turn_player, FALSE, 0))
				core.summonable_cards.push_back(*clit);
		for(int i = 0; i < 5; ++i) {
			if(player[infos.turn_player].list_mzone[i] && player[infos.turn_player].list_mzone[i]->is_can_be_summoned(infos.turn_player, FALSE, 0))
				core.summonable_cards.push_back(player[infos.turn_player].list_mzone[i]);
		}
		core.spsummonable_cards.clear();
		effect_set eset;
		filter_field_effect(EFFECT_SPSUMMON_PROC, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pcard = eset[i]->handler;
			if(pcard->current.controler == infos.turn_player && pcard->is_special_summonable(infos.turn_player))
				core.spsummonable_cards.push_back(pcard);
		}
		core.repositionable_cards.clear();
		for(int i = 0; i < 5; ++i) {
			pcard = player[infos.turn_player].list_mzone[i];
			if(pcard && ((pcard->is_position(POS_FACEUP) && pcard->is_capable_change_position(infos.turn_player)) || (pcard->is_position(POS_FACEDOWN) && pcard->is_can_be_flip_summoned(infos.turn_player))) )
				core.repositionable_cards.push_back(player[infos.turn_player].list_mzone[i]);
		}
		core.msetable_cards.clear();
		core.ssetable_cards.clear();
		for(auto clit = player[infos.turn_player].list_hand.begin(); clit != player[infos.turn_player].list_hand.end(); ++clit) {
			if((*clit)->is_setable_mzone(infos.turn_player, FALSE, 0))
				core.msetable_cards.push_back(*clit);
			if((*clit)->is_setable_szone(infos.turn_player))
				core.ssetable_cards.push_back(*clit);
		}
		add_process(PROCESSOR_SELECT_IDLECMD, 0, 0, 0, infos.turn_player, 0);
		return FALSE;
	}
	case 1: {
		uint32 ctype = returns.ivalue[0] & 0xffff;
		uint32 sel = returns.ivalue[0] >> 16;
		if(ctype == 5) {
			chain newchain = core.select_chains[sel];
			effect* peffect = newchain.triggering_effect;
			newchain.flag = 0;
			newchain.chain_id = infos.field_id++;
			newchain.evt.event_code = peffect->code;
			newchain.evt.event_player = PLAYER_NONE;
			newchain.evt.event_value = 0;
			newchain.evt.event_cards = 0;
			newchain.evt.reason = 0;
			newchain.evt.reason_effect = 0;
			newchain.evt.reason_player = PLAYER_NONE;
			newchain.triggering_controler = peffect->handler->current.controler;
			newchain.triggering_location = peffect->handler->current.location;
			newchain.triggering_sequence = peffect->handler->current.sequence;
			newchain.triggering_player = infos.turn_player;
			core.new_chains.push_back(newchain);
			peffect->handler->set_status(STATUS_CHAINING, TRUE);
			core.select_chains.clear();
			add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
			add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, FALSE, 1 - infos.turn_player);
			infos.priorities[0] = 0;
			infos.priorities[1] = 0;
			core.select_chains.clear();
			return FALSE;
		} else if(ctype == 0) {
			core.units.begin()->step = 4;
			return FALSE;
		} else if(ctype == 1) {
			core.units.begin()->step = 5;
			return FALSE;
		} else if(ctype == 2) {
			core.units.begin()->step = 6;
			return FALSE;
		} else if(ctype == 3) {
			core.units.begin()->step = 7;
			return FALSE;
		} else if(ctype == 4) {
			core.units.begin()->step = 8;
			return FALSE;
		} else {
			core.units.begin()->step = 9;
			pduel->write_buffer8(MSG_HINT);
			pduel->write_buffer8(HINT_EVENT);
			pduel->write_buffer8(1 - infos.turn_player);
			pduel->write_buffer32(23);
			core.select_chains.clear();
			core.hint_timing[infos.turn_player] = TIMING_MAIN_END;
			add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, FALSE, 1 - infos.turn_player);
			infos.priorities[infos.turn_player] = 1;
			infos.priorities[1 - infos.turn_player] = 0;
			core.units.begin()->arg1 = ctype;
			return FALSE;
		}
		return TRUE;
	}
	case 2: {
		core.chain_limit = 0;
		chain_array::iterator cait;
		for(cait = core.current_chain.begin(); cait != core.current_chain.end(); ++cait)
			cait->triggering_effect->handler->set_status(STATUS_CHAINING, FALSE);
		add_process(PROCESSOR_SOLVE_CHAIN, 0, 0, 0, FALSE, 0);
		core.units.begin()->step = -1;
		return FALSE;
	}
	case 5: {
		card* target = core.summonable_cards[returns.ivalue[0] >> 16];
		summon(infos.turn_player, target, 0, FALSE);
		core.units.begin()->step = -1;
		return FALSE;
	}
	case 6: {
		card* target = core.spsummonable_cards[returns.ivalue[0] >> 16];
		special_summon_rule(infos.turn_player, target);
		core.units.begin()->step = -1;
		return FALSE;
	}
	case 7: {
		card* target = core.repositionable_cards[returns.ivalue[0] >> 16];
		if(target->is_position(POS_FACEUP_ATTACK)) {
			core.phase_action = TRUE;
			change_position(target, 0, infos.turn_player, POS_FACEUP_DEFENCE, FALSE);
			adjust_all();
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, 0);
		} else if(target->is_position(POS_FACEUP_DEFENCE)) {
			core.phase_action = TRUE;
			change_position(target, 0, infos.turn_player, POS_FACEUP_ATTACK, FALSE);
			adjust_all();
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, 0);
		} else add_process(PROCESSOR_FLIP_SUMMON, 0, 0, (group*)target, target->current.controler, 0);
		target->set_status(STATUS_FORM_CHANGED, TRUE);
		core.units.begin()->step = -1;
		return FALSE;
	}
	case 8: {
		card* target = core.msetable_cards[returns.ivalue[0] >> 16];
		add_process(PROCESSOR_MSET, 0, 0, (group*)target, target->current.controler, 0);
		core.units.begin()->step = -1;
		return FALSE;
	}
	case 9: {
		card* target = core.ssetable_cards[returns.ivalue[0] >> 16];
		add_process(PROCESSOR_SSET, 0, 0, (group*)target, target->current.controler, 0);
		core.units.begin()->step = -1;
		return FALSE;
	}
	case 10: {
		//end announce
		core.chain_limit = 0;
		if(core.current_chain.size()) {
			chain_array::iterator cait;
			for(cait = core.current_chain.begin(); cait != core.current_chain.end(); ++cait)
				cait->triggering_effect->handler->set_status(STATUS_CHAINING, FALSE);
			add_process(PROCESSOR_SOLVE_CHAIN, 0, 0, 0, FALSE, 0);
			core.units.begin()->step = -1;
			return FALSE;
		}
		reset_phase(infos.phase);
		adjust_all();
		return FALSE;
	}
	case 11: {
		returns.ivalue[0] = core.units.begin()->arg1;
		return TRUE;
	}
	case 12: {
		if(returns.ivalue[0] == 0)
			core.units.begin()->arg1 = 6;
		else
			core.units.begin()->arg1 = 7;
		reset_phase(infos.phase);
		adjust_all();
		core.units.begin()->step = 10;
		return FALSE;
	}
	}
	return TRUE;
}
int32 field::process_battle_command(uint16 step) {
	switch(step) {
	case 0: {
		pair<effect_container::iterator, effect_container::iterator> pr;
		effect* peffect = 0;
		card* pcard = 0;
		core.select_chains.clear();
		chain newchain;
		nil_event.event_code = EVENT_FREE_CHAIN;
		core.chain_attack = FALSE;
		core.chain_attack_target = 0;
		core.attacker = 0;
		core.attack_target = 0;
		if(is_player_affected_by_effect(infos.turn_player, EFFECT_SKIP_BP)) {
			core.units.begin()->step = 29;
			core.units.begin()->arg1 = 2;
			add_process(PROCESSOR_PHASE_EVENT, 0, 0, 0, PHASE_BATTLE, 0);
			adjust_all();
			return FALSE;
		}
		pr = effects.activate_effect.equal_range(EVENT_FREE_CHAIN);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			newchain.triggering_effect = peffect;
			if(peffect->is_activateable(infos.turn_player, nil_event) && peffect->get_speed() > 1)
				core.select_chains.push_back(newchain);
		}
		pr = effects.instant_o_effect.equal_range(EVENT_FREE_CHAIN);
		for(; pr.first != pr.second; ++pr.first) {
			peffect = pr.first->second;
			newchain.triggering_effect = peffect;
			if(peffect->is_activateable(infos.turn_player, nil_event))
				core.select_chains.push_back(newchain);
		}
		core.attackable_cards.clear();
		card_vector first_attack;
		card_vector must_attack;
		for(uint32 i = 0; i < 5; ++i) {
			pcard = player[infos.turn_player].list_mzone[i];
			if(!pcard)
				continue;
			if(!pcard->is_capable_attack_announce(infos.turn_player))
				continue;
			core.select_cards.clear();
			get_attack_target(pcard, &core.select_cards);
			if(core.select_cards.size() == 0 && pcard->operation_param == 0)
				continue;
			core.attackable_cards.push_back(pcard);
			if(pcard->is_affected_by_effect(EFFECT_FIRST_ATTACK)) {
				if(first_attack.size()) {
					first_attack.clear();
					must_attack.clear();
					core.attackable_cards.clear();
					break;
				} else {
					core.attackable_cards.clear();
					core.attackable_cards.push_back(pcard);
					first_attack.push_back(pcard);
				}
			}
			if(pcard->is_affected_by_effect(EFFECT_MUST_ATTACK))
				must_attack.push_back(pcard);
		}
		core.to_m2 = TRUE;
		core.to_ep = TRUE;
		if(must_attack.size() || is_player_affected_by_effect(infos.turn_player, EFFECT_CANNOT_M2))
			core.to_m2 = FALSE;
		if(must_attack.size() || is_player_affected_by_effect(infos.turn_player, EFFECT_CANNOT_EP))
			core.to_ep = FALSE;
		add_process(PROCESSOR_SELECT_BATTLECMD, 0, 0, 0, infos.turn_player, 0);
		return FALSE;
	}
	case 1: {
		int32 ctype = returns.ivalue[0] & 0xffff;
		int32 sel = returns.ivalue[0] >> 16;
		if(ctype == 0) {
			chain newchain = core.select_chains[sel];
			effect* peffect = newchain.triggering_effect;
			newchain.flag = 0;
			newchain.chain_id = infos.field_id++;
			newchain.evt.event_code = peffect->code;
			newchain.evt.event_player = PLAYER_NONE;
			newchain.evt.event_value = 0;
			newchain.evt.event_cards = 0;
			newchain.evt.reason = 0;
			newchain.evt.reason_effect = 0;
			newchain.evt.reason_player = PLAYER_NONE;
			newchain.triggering_controler = peffect->handler->current.controler;
			newchain.triggering_location = peffect->handler->current.location;
			newchain.triggering_sequence = peffect->handler->current.sequence;
			newchain.triggering_player = infos.turn_player;
			core.new_chains.push_back(newchain);
			peffect->handler->set_status(STATUS_CHAINING, TRUE);
			core.select_chains.clear();
			add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
			add_process(PROCESSOR_QUICK_EFFECT, 0, 0, 0, FALSE, 1 - infos.turn_player);
			infos.priorities[0] = 0;
			infos.priorities[1] = 0;
			core.select_chains.clear();
			return FALSE;
		} else if(ctype == 1) {
			core.units.begin()->step = 2;
			core.attacker = core.attackable_cards[sel];
			core.attacker->set_status(STATUS_ATTACK_CANCELED, FALSE);
			core.phase_action = TRUE;
			//attack cost
			effect_set eset;
			filter_player_effect(infos.turn_player, EFFECT_ATTACK_COST, &eset, FALSE);
			core.attacker->filter_effect(EFFECT_ATTACK_COST, &eset);
			for(int32 i = 0; i < eset.count; ++i) {
				pduel->lua->add_param(core.attacker, PARAM_TYPE_CARD);
				pduel->lua->add_param(infos.turn_player, PARAM_TYPE_INT);
				if(!eset[i]->check_value_condition(2))
					continue;
				if(eset[i]->cost) {
					core.sub_solving_event.push_back(nil_event);
					add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, infos.turn_player, 0);
				}
			}
			return FALSE;
		} else {
			core.units.begin()->step = 29;
			core.units.begin()->arg1 = ctype;
			add_process(PROCESSOR_PHASE_EVENT, 0, 0, 0, PHASE_BATTLE, 0);
			adjust_all();
			return FALSE;
		}
		return TRUE;
	}
	case 2: {
		core.chain_limit = 0;
		chain_array::iterator cait;
		for(cait = core.current_chain.begin(); cait != core.current_chain.end(); ++cait)
			cait->triggering_effect->handler->set_status(STATUS_CHAINING, FALSE);
		add_process(PROCESSOR_SOLVE_CHAIN, 0, 0, 0, FALSE, 0);
		core.units.begin()->step = -1;
		return FALSE;
	}
	case 3: {
		//Filter Targers
		core.select_cards.clear();
		core.units.begin()->arg1 = FALSE;
		if(core.chain_attack && core.chain_attack_target) {
			core.attack_target = core.chain_attack_target;
			core.units.begin()->step = 6;
			return FALSE;
		}
		core.units.begin()->arg2 = get_attack_target(core.attacker, &core.select_cards, core.chain_attack);
		return FALSE;
	}
	case 4: {
		//confirm attack_target
		card_vector auto_be_attack;
		card* atarget;
		for(uint32 i = 0; i < 5; ++i) {
			atarget = player[1 - infos.turn_player].list_mzone[i];
			if(atarget && atarget->is_affected_by_effect(EFFECT_AUTO_BE_ATTACKED))
				auto_be_attack.push_back(atarget);
		}
		if(auto_be_attack.size()) {
			core.select_cards = auto_be_attack;
			if(core.select_cards.size() == 1)
				returns.bvalue[1] = 0;
			else
				add_process(MSG_SELECT_CARD, 0, 0, 0, 1 - infos.turn_player + 0x20000, 0x10001);
			core.units.begin()->step = 5;
			return FALSE;
		}
		if(core.attacker->operation_param) {
			for(int32 i = 0; i < 5; ++i) {
				if(player[1 - infos.turn_player].list_mzone[i]) {
					add_process(MSG_SELECT_YESNO, 0, 0, 0, infos.turn_player, 31);
					return FALSE;
				}
			}
			core.attack_target = 0;
			core.units.begin()->step = 6;
			return FALSE;
		}
		if(core.units.begin()->arg2) {
			if(core.select_cards.size() == 1)
				returns.bvalue[1] = 0;
			else
				add_process(MSG_SELECT_CARD, 0, 0, 0, 1 - infos.turn_player + 0x20000, 0x10001);
			core.units.begin()->step = 5;
			return FALSE;
		}
		add_process(MSG_SELECT_CARD, 0, 0, 0, infos.turn_player + 0x20000, 0x10001);
		core.units.begin()->step = 5;
		return FALSE;
	}
	case 5: {
		if(returns.ivalue[0]) {
			core.attack_target = 0;
			core.units.begin()->step = 6;
			return FALSE;
		} else {
			if(core.select_cards.size())
				add_process(MSG_SELECT_CARD, 0, 0, 0, infos.turn_player + 0x20000, 0x10001);
			else
				core.units.begin()->step = -1;
		}
		return FALSE;
	}
	case 6: {
		if(returns.ivalue[0] == -1) {
			core.units.begin()->step = -1;
			return FALSE;
		}
		core.attack_target = core.select_cards[returns.bvalue[1]];
		return FALSE;
	}
	case 7: {
		core.sub_attacker = 0;
		core.sub_attack_target = (card*)0xffffffff;
		core.attack_state[infos.turn_player] = TRUE;
		for(uint32 i = 0; i < 5; ++i) {
			if(player[1 - infos.turn_player].list_mzone[i])
				core.pre_field[i] = player[1 - infos.turn_player].list_mzone[i]->fieldid;
			else
				core.pre_field[i] = 0;
		}
		//core.units.begin()->arg1 ---> is rollbacked
		if(!core.units.begin()->arg1) {
			raise_single_event(core.attacker, EVENT_ATTACK_ANNOUNCE, 0, 0, 0, infos.turn_player, 0);
			raise_event(core.attacker, EVENT_ATTACK_ANNOUNCE, 0, 0, 0, infos.turn_player, 0);
		}
		pduel->write_buffer8(MSG_ATTACK);
		pduel->write_buffer32(core.attacker->get_info_location());
		if(core.attack_target) {
			raise_single_event(core.attack_target, EVENT_BE_BATTLE_TARGET, 0, 0, 0, 1 - infos.turn_player, 0);
			raise_event(core.attack_target, EVENT_BE_BATTLE_TARGET, 0, 0, 0, 1 - infos.turn_player, 0);
			pduel->write_buffer32(core.attack_target->get_info_location());
		} else
			pduel->write_buffer32(0);
		core.units.begin()->arg2 = (core.attacker->current.controler << 16) + core.attacker->fieldid;
		process_single_event();
		process_instant_event();
		core.hint_timing[infos.turn_player] = TIMING_ATTACK;
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 8: {
		bool rollback = false;
		bool atk_disabled = false;
		uint32 acon = core.units.begin()->arg2 >> 16;
		uint32 afid = core.units.begin()->arg2 & 0xffff;
		if(core.attacker->is_affected_by_effect(EFFECT_ATTACK_DISABLED)) {
			core.attacker->reset(EFFECT_ATTACK_DISABLED, RESET_CODE);
			atk_disabled = true;
			pduel->write_buffer8(MSG_ATTACK_DISABLED);
			core.attacker->set_status(STATUS_ATTACK_CANCELED, TRUE);
		}
		if(is_player_affected_by_effect(infos.turn_player, EFFECT_SKIP_BP)) {
			if(core.attacker->fieldid == afid) {
				if(!atk_disabled) {
					if(core.attack_target)
						core.attacker->attacked_cards[core.attack_target->fieldid] = core.attack_target;
					else
						core.attacker->attacked_cards[0] = 0;
				}
				core.attacker->attacked_count++;
				core.attacker->announce_count++;
			}
			core.units.begin()->step = 29;
			core.units.begin()->arg1 = 2;
			add_process(PROCESSOR_PHASE_EVENT, 0, 0, 0, PHASE_BATTLE, 0);
			adjust_all();
			return FALSE;
		}
		if(atk_disabled || !core.attacker->is_capable_attack() || core.attacker->is_status(STATUS_ATTACK_CANCELED)
		        || core.attacker->current.controler != acon || core.attacker->fieldid != afid) {
			if(core.attacker->fieldid == afid) {
				if(core.attack_target)
					core.attacker->announced_cards[core.attack_target->fieldid] = core.attack_target;
				else
					core.attacker->announced_cards[0] = 0;
				core.attacker->announce_count++;
				if(!atk_disabled) {
					if(core.attack_target)
						core.attacker->attacked_cards[core.attack_target->fieldid] = core.attack_target;
					else
						core.attacker->attacked_cards[0] = 0;
					core.attacker->attacked_count++;
				}
			}
			core.units.begin()->step = -1;
			reset_phase(PHASE_DAMAGE);
			return FALSE;
		}
		if(core.sub_attack_target != (card*)0xffffffff) {
			core.attacker->announce_count++;
			core.attacker->attacked_count++;
			pduel->write_buffer8(MSG_ATTACK);
			pduel->write_buffer32(core.attacker->get_info_location());
			if(core.sub_attack_target) {
				core.attacker->announced_cards[core.sub_attack_target->fieldid] = core.sub_attack_target;
				core.attacker->attacked_cards[core.sub_attack_target->fieldid] = core.sub_attack_target;
				pduel->write_buffer32(core.sub_attack_target->get_info_location());
			} else {
				core.attacker->announced_cards[0] = 0;
				core.attacker->attacked_cards[0] = 0;
				pduel->write_buffer32(0);
			}
			core.units.begin()->step = 9;
			return FALSE;
		}
		if(core.chain_attack && core.chain_attack_target) {
			core.attacker->announce_count++;
			if(!(core.chain_attack_target->current.location & LOCATION_MZONE)) {
				core.units.begin()->step = -1;
				reset_phase(PHASE_DAMAGE);
				return FALSE;
			}
			uint8 seq = core.chain_attack_target->current.sequence;
			if(core.pre_field[seq] != core.chain_attack_target->fieldid) {
				core.units.begin()->step = -1;
				reset_phase(PHASE_DAMAGE);
				return FALSE;
			}
			core.units.begin()->step = 9;
			return FALSE;
		}
		core.select_cards.clear();
		core.units.begin()->arg2 = get_attack_target(core.attacker, &core.select_cards, core.chain_attack);
		for(uint32 i = 0; i < 5; ++i) {
			if(player[1 - infos.turn_player].list_mzone[i]) {
				if(!core.pre_field[i] || core.pre_field[i] != player[1 - infos.turn_player].list_mzone[i]->fieldid) {
					rollback = true;
					break;
				}
			} else {
				if(core.pre_field[i]) {
					rollback = true;
					break;
				}
			}
		}
		if(!core.attack_target && !core.attacker->operation_param)
			rollback = true;
		if(!rollback) {
			core.attacker->announce_count++;
			core.attacker->attacked_count++;
			if(core.attack_target) {
				core.attacker->announced_cards[core.attack_target->fieldid] = core.attack_target;
				core.attacker->attacked_cards[core.attack_target->fieldid] = core.attack_target;
			} else {
				core.attacker->announced_cards[0] = 0;
				core.attacker->attacked_cards[0] = 0;
			}
			core.units.begin()->step = 9;
			return FALSE;
		}
		if(!core.select_cards.size() && !core.attacker->operation_param) {
			core.attacker->announce_count++;
			core.units.begin()->step = -1;
			reset_phase(PHASE_DAMAGE);
			return FALSE;
		}
		add_process(PROCESSOR_SELECT_YESNO, 0, 0, 0, infos.turn_player, 30);
		return FALSE;
	}
	case 9: {
		if(returns.ivalue[0]) {
			core.units.begin()->arg1 = TRUE;
			core.units.begin()->step = 3;
			return FALSE;
		}
		core.attacker->announce_count++;
		if(core.attack_target)
			core.attacker->announced_cards[core.attack_target->fieldid] = core.attack_target;
		else
			core.attacker->announced_cards[0] = 0;
		core.units.begin()->step = -1;
		reset_phase(PHASE_DAMAGE);
		return FALSE;
	}
	case 10: {
		infos.phase = PHASE_DAMAGE;
		pduel->write_buffer8(MSG_DAMAGE_STEP_START);
		core.units.begin()->arg1 = FALSE;
		core.damage_calculated = FALSE;
		core.selfdes_disabled = TRUE;
		core.flip_delayed = TRUE;
		if(core.sub_attacker)
			core.attacker = core.sub_attacker;
		if(core.sub_attack_target != (card*)0xffffffff)
			core.attack_target = core.sub_attack_target;
		core.pre_field[0] = core.attacker->fieldid;
		if(core.attack_target)
			core.pre_field[1] = core.attack_target->fieldid;
		else
			core.pre_field[1] = 0;
		raise_single_event(core.attacker, EVENT_BATTLE_START, 0, 0, 0, 0, 0);
		if(core.attack_target)
			raise_single_event(core.attack_target, EVENT_BATTLE_START, 0, 0, 0, 0, 1);
		raise_event((card*)0, EVENT_BATTLE_START, 0, 0, 0, 0, 0);
		process_single_event();
		process_instant_event();
		if(core.new_fchain.size() || core.new_ochain.size()) {
			core.hint_timing[infos.turn_player] = TIMING_DAMAGE_STEP;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
			core.units.begin()->arg1 = TRUE;
		}
		return FALSE;
	}
	case 11: {
		if(core.attacker->current.location != LOCATION_MZONE || core.attacker->fieldid != core.pre_field[0]
		        || (core.attack_target && (core.attack_target->current.location != LOCATION_MZONE || core.attack_target->fieldid != core.pre_field[1]))) {
			core.units.begin()->arg1 = 0;
			core.damage_calculated = TRUE;
			core.units.begin()->step = 20;
			return FALSE;
		}
		if(!core.attack_target) {
			core.units.begin()->step = 13;
			if(!core.units.begin()->arg1) {
				pduel->write_buffer8(MSG_HINT);
				pduel->write_buffer8(HINT_EVENT);
				pduel->write_buffer8(0);
				pduel->write_buffer32(40);
				pduel->write_buffer8(MSG_HINT);
				pduel->write_buffer8(HINT_EVENT);
				pduel->write_buffer8(1);
				pduel->write_buffer32(40);
				core.hint_timing[infos.turn_player] = TIMING_DAMAGE_STEP;
				add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
			}
			return FALSE;
		}
		core.sub_attacker = 0;
		core.sub_attack_target = (card*)0xffffffff;
		core.attacker->temp.position = core.attacker->current.position;
		core.attack_target->temp.position = core.attack_target->current.position;
		if(core.attack_target->is_position(POS_FACEDOWN)) {
			change_position(core.attack_target, 0, PLAYER_NONE, core.attack_target->current.position >> 1, 0, TRUE);
			adjust_all();
		}
		return FALSE;
	}
	case 12: {
		raise_single_event(core.attacker, EVENT_BATTLE_CONFIRM, 0, 0, 0, 0, 0);
		if(core.attack_target) {
			if(core.attack_target->temp.position & POS_FACEDOWN)
				core.pre_field[1] = core.attack_target->fieldid;
			raise_single_event(core.attack_target, EVENT_BATTLE_CONFIRM, 0, 0, 0, 0, 1);
		}
		raise_event((card*)0, EVENT_BATTLE_CONFIRM, 0, 0, 0, 0, 0);
		process_single_event();
		process_instant_event();
		if(core.new_fchain.size() || core.new_ochain.size()) {
			core.hint_timing[infos.turn_player] = TIMING_DAMAGE_STEP;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
			core.units.begin()->arg1 = TRUE;
		}
		if(!core.units.begin()->arg1) {
			pduel->write_buffer8(MSG_HINT);
			pduel->write_buffer8(HINT_EVENT);
			pduel->write_buffer8(0);
			pduel->write_buffer32(40);
			pduel->write_buffer8(MSG_HINT);
			pduel->write_buffer8(HINT_EVENT);
			pduel->write_buffer8(1);
			pduel->write_buffer32(40);
			core.hint_timing[infos.turn_player] = TIMING_DAMAGE_STEP;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		}
		return FALSE;
	}
	case 13: {
		if((core.sub_attack_target != (card*)0xffffffff) || core.sub_attacker) {
			core.attacker = core.sub_attacker;
			core.attack_target = core.sub_attack_target;
			core.units.begin()->step = 10;
			core.pre_field[0] = core.attacker->fieldid;
			if(core.attack_target)
				core.pre_field[1] = core.attack_target->fieldid;
			else
				core.pre_field[1] = 0;
			return FALSE;
		}
		if(core.attacker->current.location != LOCATION_MZONE || core.attacker->fieldid != core.pre_field[0]
		        || (core.attack_target && (core.attack_target->current.location != LOCATION_MZONE || core.attack_target->fieldid != core.pre_field[1]))) {
			core.units.begin()->arg1 = 0;
			core.damage_calculated = TRUE;
			core.units.begin()->step = 20;
			return FALSE;
		}
		return FALSE;
	}
	case 14: {
		infos.phase = PHASE_DAMAGE_CAL;
		raise_single_event(core.attacker, EVENT_PRE_DAMAGE_CALCULATE, 0, 0, 0, 0, 0);
		if(core.attack_target)
			raise_single_event(core.attack_target, EVENT_PRE_DAMAGE_CALCULATE, 0, 0, 0, 0, 1);
		raise_event((card*)0, EVENT_PRE_DAMAGE_CALCULATE, 0, 0, 0, 0, 0);
		process_single_event();
		process_instant_event();
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_EVENT);
		pduel->write_buffer8(0);
		pduel->write_buffer32(41);
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_EVENT);
		pduel->write_buffer8(1);
		pduel->write_buffer32(41);
		core.hint_timing[infos.turn_player] = TIMING_DAMAGE_CAL;
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, TRUE);
		return FALSE;
	}
	case 15: {
		if(core.attacker->current.location != LOCATION_MZONE || core.attacker->fieldid != core.pre_field[0]
		        || (core.attack_target && (core.attack_target->current.location != LOCATION_MZONE || core.attack_target->fieldid != core.pre_field[1]))) {
			core.units.begin()->arg1 = 0;
			core.damage_calculated = TRUE;
			core.units.begin()->step = 20;
			return FALSE;
		}
		raise_event((card*)0, EVENT_DAMAGE_CALCULATING, 0, 0, 0, 0, 0);
		process_instant_event();
		core.new_ochain.clear();
		core.new_fchain.clear();
		return FALSE;
	}
	case 16: {
		uint32 a = core.attacker->get_attack(), d;
		uint8 pa = core.attacker->current.controler, pd;
		uint8 damp = 0;
		core.battle_damage[0] = core.battle_damage[1] = 0;
		card* reason_card = 0;
		core.attacker->set_status(STATUS_BATTLE_DESTROYED, FALSE);
		if(core.attack_target) {
			core.attack_target->set_status(STATUS_BATTLE_DESTROYED, FALSE);
			pd = core.attack_target->current.controler;
			if(core.attack_target->is_position(POS_ATTACK)) {
				d = core.attack_target->get_attack();
				if(a > d) {
					if(!core.attacker->is_affected_by_effect(EFFECT_NO_BATTLE_DAMAGE)
					        && !core.attack_target->is_affected_by_effect(EFFECT_AVOID_BATTLE_DAMAGE, core.attacker)
					        && !is_player_affected_by_effect(pd, EFFECT_AVOID_BATTLE_DAMAGE)) {
						if(core.attack_target->is_affected_by_effect(EFFECT_REFLECT_BATTLE_DAMAGE, core.attacker))
							damp = 1 - pd;
						else damp = pd;
						if(is_player_affected_by_effect(damp, EFFECT_REFLECT_BATTLE_DAMAGE))
							damp = 1 - damp;
						core.battle_damage[damp] = a - d;
						reason_card = core.attacker;
					}
					if(core.attack_target->is_destructable_by_battle(core.attacker))
						core.attack_target->set_status(STATUS_BATTLE_DESTROYED, TRUE);
				} else if (a < d) {
					if(!core.attack_target->is_affected_by_effect(EFFECT_NO_BATTLE_DAMAGE)
					        && !core.attacker->is_affected_by_effect(EFFECT_AVOID_BATTLE_DAMAGE, core.attack_target)
					        && !is_player_affected_by_effect(pa, EFFECT_AVOID_BATTLE_DAMAGE)) {
						if(core.attacker->is_affected_by_effect(EFFECT_REFLECT_BATTLE_DAMAGE, core.attack_target))
							damp = 1 - pa;
						else damp = pa;
						if(is_player_affected_by_effect(damp, EFFECT_REFLECT_BATTLE_DAMAGE))
							damp = 1 - damp;
						core.battle_damage[damp] = d - a;
						reason_card = core.attack_target;
					}
					if(core.attacker->is_destructable_by_battle(core.attack_target))
						core.attacker->set_status(STATUS_BATTLE_DESTROYED, TRUE);
				} else {
					if(a != 0) {
						if(core.attack_target->is_destructable_by_battle(core.attacker))
							core.attack_target->set_status(STATUS_BATTLE_DESTROYED, TRUE);
						if(core.attacker->is_destructable_by_battle(core.attack_target))
							core.attacker->set_status(STATUS_BATTLE_DESTROYED, TRUE);
					}
				}
			} else {
				d = core.attack_target->get_defence();
				if(a > d) {
					effect_set eset;
					uint8 dp[2];
					dp[0] = dp[1] = 0;
					core.attacker->filter_effect(EFFECT_PIERCE, &eset);
					if(eset.count && !core.attacker->is_affected_by_effect(EFFECT_NO_BATTLE_DAMAGE)
					        && !core.attack_target->is_affected_by_effect(EFFECT_AVOID_BATTLE_DAMAGE, core.attacker)) {
						for(int32 i = 0; i < eset.count; ++i)
							dp[1 - eset[i]->get_handler_player()] = 1;
						if(dp[0] && is_player_affected_by_effect(0, EFFECT_AVOID_BATTLE_DAMAGE))
							dp[0] = 0;
						if(dp[1] && is_player_affected_by_effect(1, EFFECT_AVOID_BATTLE_DAMAGE))
							dp[1] = 0;
						if(dp[pd] && core.attack_target->is_affected_by_effect(EFFECT_REFLECT_BATTLE_DAMAGE, core.attacker)) {
							dp[pd] = 0;
							dp[1 - pd] = 1;
						}
						if(dp[pd] && is_player_affected_by_effect(pd, EFFECT_REFLECT_BATTLE_DAMAGE)) {
							dp[pd] = 0;
							dp[1 - pd] = 1;
						}
						if(dp[1 - pd] && is_player_affected_by_effect(1 - pd, EFFECT_REFLECT_BATTLE_DAMAGE)) {
							dp[pd] = 1;
							dp[1 - pd] = 0;
						}
						if(dp[0]) core.battle_damage[0] = a - d;
						if(dp[1]) core.battle_damage[1] = a - d;
						reason_card = core.attacker;
					}
					if(core.attack_target->is_destructable_by_battle(core.attacker))
						core.attack_target->set_status(STATUS_BATTLE_DESTROYED, TRUE);
				} else if (a < d) {
					if(!core.attack_target->is_affected_by_effect(EFFECT_NO_BATTLE_DAMAGE)
					        && !core.attacker->is_affected_by_effect(EFFECT_AVOID_BATTLE_DAMAGE, core.attack_target)
					        && !is_player_affected_by_effect(pa, EFFECT_AVOID_BATTLE_DAMAGE)) {
						if(core.attacker->is_affected_by_effect(EFFECT_REFLECT_BATTLE_DAMAGE, core.attack_target))
							damp = 1 - pa;
						else damp = pa;
						if(is_player_affected_by_effect(damp, EFFECT_REFLECT_BATTLE_DAMAGE))
							damp = 1 - damp;
						core.battle_damage[damp] = d - a;
						reason_card = core.attack_target;
					}
				}
			}
		} else {
			if(!core.attacker->is_affected_by_effect(EFFECT_NO_BATTLE_DAMAGE)
			        && !is_player_affected_by_effect(1 - pa, EFFECT_AVOID_BATTLE_DAMAGE)) {
				damp = 1 - pa;
				if(is_player_affected_by_effect(damp, EFFECT_REFLECT_BATTLE_DAMAGE))
					damp = 1 - damp;
				core.battle_damage[damp] = a;
				reason_card = core.attacker;
			}
		}
		pduel->write_buffer8(MSG_BATTLE);
		pduel->write_buffer32(core.attacker->get_info_location());
		pduel->write_buffer32(a);
		pduel->write_buffer8(core.attacker->is_status(STATUS_BATTLE_DESTROYED) ? 1 : 0);
		if(core.attack_target) {
			pduel->write_buffer32(core.attack_target->get_info_location());
			pduel->write_buffer32(d);
			pduel->write_buffer8(core.attack_target->is_status(STATUS_BATTLE_DESTROYED) ? 1 : 0);
		} else {
			pduel->write_buffer32(0);
			pduel->write_buffer32(0);
			pduel->write_buffer8(0);
		}
		core.units.begin()->peffect = (effect*)reason_card;
		if(reason_card)
			core.units.begin()->arg1 = reason_card->current.controler;
		if(core.battle_damage[0]) {
			raise_single_event(core.attacker, EVENT_PRE_BATTLE_DAMAGE, 0, 0, reason_card->current.controler, 0, core.battle_damage[0]);
			if(core.attack_target)
				raise_single_event(core.attack_target, EVENT_PRE_BATTLE_DAMAGE, 0, 0, reason_card->current.controler, 0, core.battle_damage[0]);
			raise_event((card*)reason_card, EVENT_PRE_BATTLE_DAMAGE, 0, 0, reason_card->current.controler, 0, core.battle_damage[0]);
		}
		if(core.battle_damage[1]) {
			raise_single_event(core.attacker, EVENT_PRE_BATTLE_DAMAGE, 0, 0, reason_card->current.controler, 1, core.battle_damage[1]);
			if(core.attack_target)
				raise_single_event(core.attack_target, EVENT_PRE_BATTLE_DAMAGE, 0, 0, reason_card->current.controler, 1, core.battle_damage[1]);
			raise_event((card*)reason_card, EVENT_PRE_BATTLE_DAMAGE, 0, 0, reason_card->current.controler, 1, core.battle_damage[1]);
		}
		process_single_event();
		process_instant_event();
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_EVENT);
		pduel->write_buffer8(0);
		pduel->write_buffer32(43);
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_EVENT);
		pduel->write_buffer8(1);
		pduel->write_buffer32(43);
		core.hint_timing[infos.turn_player] = TIMING_DAMAGE_CAL;
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		core.damage_calculated = TRUE;
		return FALSE;
	}
	case 17: {
		infos.phase = PHASE_DAMAGE;
		core.chain_attack = FALSE;
		if(core.attack_target) {
			core.attacker->battled_cards[core.attack_target->fieldid] = core.attack_target;
			core.attack_target->battled_cards[core.attacker->fieldid] = core.attacker;
		} else
			core.attacker->battled_cards[0] = 0;
		card* reason_card = (card*)core.units.begin()->peffect;
		uint8 reason_player = core.units.begin()->arg1;
		if(core.battle_damage[0]) {
			damage(0, REASON_BATTLE, reason_player, reason_card, 0, core.battle_damage[0]);
			raise_single_event(reason_card, EVENT_BATTLE_DAMAGE, 0, 0, reason_player, 0, core.battle_damage[0]);
		}
		if(core.battle_damage[1]) {
			damage(0, REASON_BATTLE, reason_player, reason_card, 1, core.battle_damage[1]);
			raise_single_event(reason_card, EVENT_BATTLE_DAMAGE, 0, 0, reason_player, 1, core.battle_damage[1]);
		}
		reset_phase(PHASE_DAMAGE_CAL);
		adjust_all();
		return FALSE;
	}
	case 18: {
		card_set des;
		effect* peffect;
		uint32 dest, seq;
		if(core.attacker->is_status(STATUS_BATTLE_DESTROYED)
		        && core.attacker->current.location == LOCATION_MZONE && core.attacker->fieldid == core.pre_field[0]) {
			des.insert(core.attacker);
			core.attacker->temp.reason = core.attacker->current.reason;
			core.attacker->temp.reason_card = core.attacker->current.reason_card;
			core.attacker->temp.reason_effect = core.attacker->current.reason_effect;
			core.attacker->temp.reason_player = core.attacker->current.reason_player;
			core.attacker->current.reason_effect = 0;
			core.attacker->current.reason = REASON_BATTLE;
			core.attacker->current.reason_card = core.attack_target;
			core.attacker->current.reason_player = core.attack_target->current.controler;
			dest = LOCATION_GRAVE;
			seq = 0;
			if((peffect = core.attack_target->is_affected_by_effect(EFFECT_BATTLE_DESTROY_REDIRECT))) {
				dest = peffect->get_value(core.attacker);
				seq = dest >> 16;
				dest &= 0xffff;
			}
			core.attacker->operation_param = (POS_FACEUP << 24) + (((uint32)core.attacker->owner) << 16) + (dest << 8) + seq;
		}
		if(core.attack_target && core.attack_target->is_status(STATUS_BATTLE_DESTROYED)
		        && core.attack_target->current.location == LOCATION_MZONE && core.attack_target->fieldid == core.pre_field[1]) {
			des.insert(core.attack_target);
			core.attack_target->temp.reason = core.attack_target->current.reason;
			core.attack_target->temp.reason_card = core.attack_target->current.reason_card;
			core.attack_target->temp.reason_effect = core.attack_target->current.reason_effect;
			core.attack_target->temp.reason_player = core.attack_target->current.reason_player;
			core.attack_target->current.reason_effect = 0;
			core.attack_target->current.reason = REASON_BATTLE;
			core.attack_target->current.reason_card = core.attacker;
			core.attack_target->current.reason_player = core.attacker->current.controler;
			dest = LOCATION_GRAVE;
			seq = 0;
			if((peffect = core.attacker->is_affected_by_effect(EFFECT_BATTLE_DESTROY_REDIRECT))) {
				dest = peffect->get_value(core.attack_target);
				seq = dest >> 16;
				dest &= 0xffff;
			}
			core.attack_target->operation_param = (POS_FACEUP << 24) + (((uint32)core.attack_target->owner) << 16) + (dest << 8) + seq;
		}
		core.attacker->set_status(STATUS_BATTLE_DESTROYED, FALSE);
		if(core.attack_target)
			core.attack_target->set_status(STATUS_BATTLE_DESTROYED, FALSE);
		core.battle_destroy_rep.clear();
		core.desrep_chain.clear();
		if(des.size()) {
			group* ng = pduel->new_group();
			ng->container = des;
			ng->is_readonly = TRUE;
			add_process(PROCESSOR_DESTROY, 10, 0, ng, REASON_BATTLE, PLAYER_NONE);
			core.units.begin()->ptarget = ng;
		}
		return FALSE;
	}
	case 19: {
		core.selfdes_disabled = FALSE;
		group* des = core.units.begin()->ptarget;
		if(!des || !des->container.size())
			return FALSE;
		card_set::iterator cit;
		for(cit = des->container.begin(); cit != des->container.end(); ++cit) {
			(*cit)->set_status(STATUS_BATTLE_DESTROYED, TRUE);
			(*cit)->filter_disable_related_cards();
		}
		adjust_all();
		return FALSE;
	}
	case 20: {
		raise_single_event(core.attacker, EVENT_BATTLE_END, 0, 0, PLAYER_NONE, 0, 0);
		if(core.attack_target)
			raise_single_event(core.attack_target, EVENT_BATTLE_END, 0, 0, PLAYER_NONE, 0, 1);
		raise_event((card*)0, EVENT_BATTLE_END, 0, 0, PLAYER_NONE, 0, 0);
		process_single_event();
		process_instant_event();
		if(core.new_fchain.size() || core.new_ochain.size())
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, TRUE);
		core.units.begin()->arg1 = 1;
		return FALSE;
	}
	case 21: {
		core.flip_delayed = FALSE;
		core.new_fchain.splice(core.new_fchain.begin(), core.new_fchain_b);
		core.new_ochain.splice(core.new_ochain.begin(), core.new_ochain_b);
		core.flip_chain.splice(core.flip_chain.begin(), core.flip_chain_b);
		if(core.units.begin()->arg1) {
			raise_single_event(core.attacker, EVENT_BATTLED, 0, 0, PLAYER_NONE, 0, 0);
			if(core.attack_target)
				raise_single_event(core.attack_target, EVENT_BATTLED, 0, 0, PLAYER_NONE, 0, 1);
			raise_event((card*)0, EVENT_BATTLED, 0, 0, PLAYER_NONE, 0, 0);
			process_single_event();
			process_instant_event();
		}
		if(core.flip_chain.size() || core.new_fchain.size() || core.new_ochain.size())
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, FALSE);
		return FALSE;
	}
	case 22: {
		group* des = core.units.begin()->ptarget;
		if(core.battle_destroy_rep.size())
			destroy(&core.battle_destroy_rep, 0, REASON_EFFECT, PLAYER_NONE);
		if(core.desrep_chain.size())
			add_process(PROCESSOR_OPERATION_REPLACE, 15, 0, 0, 0, 0);
		if(des) {
			card_set::iterator cit, rm;
			for(cit = des->container.begin(); cit != des->container.end();) {
				rm = cit++;
				if((*rm)->current.location != LOCATION_MZONE || ((*rm)->fieldid != core.pre_field[0] && (*rm)->fieldid != core.pre_field[1]))
					des->container.erase(rm);
			}
			add_process(PROCESSOR_DESTROY, 2, 0, des, REASON_BATTLE, PLAYER_NONE);
		}
		return FALSE;
	}
	case 23: {
		core.units.begin()->ptarget = 0;
		card_set ing;
		card_set ed;
		if(core.attacker->is_status(STATUS_BATTLE_DESTROYED)) {
			raise_single_event(core.attack_target, EVENT_BATTLE_DESTROYING, 0, core.attacker->current.reason, core.attack_target->current.controler, 0, 1);
			raise_single_event(core.attacker, EVENT_BATTLE_DESTROYED, 0, core.attacker->current.reason, core.attack_target->current.controler, 0, 0);
			ing.insert(core.attack_target);
			ed.insert(core.attacker);
		}
		if(core.attack_target && core.attack_target->is_status(STATUS_BATTLE_DESTROYED)) {
			raise_single_event(core.attacker, EVENT_BATTLE_DESTROYING, 0, core.attack_target->current.reason, core.attacker->current.controler, 0, 0);
			raise_single_event(core.attack_target, EVENT_BATTLE_DESTROYED, 0, core.attack_target->current.reason, core.attacker->current.controler, 0, 1);
			ing.insert(core.attacker);
			ed.insert(core.attack_target);
		}
		if(ing.size())
			raise_event(&ing, EVENT_BATTLE_DESTROYING, 0, 0, 0, 0, 0);
		if(ed.size())
			raise_event(&ed, EVENT_BATTLE_DESTROYED, 0, 0, 0, 0, 0);
		raise_single_event(core.attacker, EVENT_DAMAGE_STEP_END, 0, 0, 0, 0, 0);
		if(core.attack_target)
			raise_single_event(core.attack_target, EVENT_DAMAGE_STEP_END, 0, 0, 0, 0, 1);
		raise_event((card*)0, EVENT_DAMAGE_STEP_END, 0, 0, 0, 0, 0);
		process_single_event();
		process_instant_event();
		core.attacker->set_status(STATUS_BATTLE_DESTROYED, FALSE);
		if(core.attack_target)
			core.attack_target->set_status(STATUS_BATTLE_DESTROYED, FALSE);
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, FALSE, FALSE);
		core.units.begin()->step = 28;
		return FALSE;
	}
	case 29: {
		core.units.begin()->step = -1;
		infos.phase = PHASE_BATTLE;
		pduel->write_buffer8(MSG_DAMAGE_STEP_END);
		reset_phase(PHASE_DAMAGE);
		adjust_all();
		if(core.chain_attack) {
			if(core.attacker->is_status(STATUS_BATTLE_DESTROYED) || core.attacker->fieldid != core.pre_field[0]
			        || !core.attacker->is_capable_attack_announce(infos.turn_player)
			        || core.attacker->announce_count > 1)
				return FALSE;
			if(core.chain_attack_target) {
				if(!core.chain_attack_target->is_capable_be_battle_target(core.attacker)
				        || core.chain_attack_target->current.location != LOCATION_MZONE)
					return FALSE;
			} else {
				core.select_cards.clear();
				get_attack_target(core.attacker, &core.select_cards, TRUE);
				if(core.select_cards.size() == 0 && core.attacker->operation_param == 0)
					return FALSE;
			}
			effect_set eset;
			filter_player_effect(infos.turn_player, EFFECT_ATTACK_COST, &eset, FALSE);
			core.attacker->filter_effect(EFFECT_ATTACK_COST, &eset);
			for(int32 i = 0; i < eset.count; ++i) {
				pduel->lua->add_param(core.attacker, PARAM_TYPE_CARD);
				pduel->lua->add_param(infos.turn_player, PARAM_TYPE_INT);
				if(!eset[i]->check_value_condition(2))
					continue;
				if(eset[i]->cost) {
					core.sub_solving_event.push_back(nil_event);
					add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, infos.turn_player, 0);
				}
			}
			core.units.begin()->step = 2;
		}
		return FALSE;
	}
	case 30: {
		returns.ivalue[0] = core.units.begin()->arg1;
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::process_damage_phase(uint16 step) {
	return TRUE;
}
int32 field::process_turn(uint16 step, uint8 turn_player) {
	switch(step) {
	case 0: {
		//Pre Draw
		card* pcard;
		for(auto elit = core.used_event.begin(); elit != core.used_event.end(); ++elit) {
			if(elit->event_cards)
				pduel->delete_group((group*)elit->event_cards);
		}
		core.used_event.clear();
		for(auto eit = core.reseted_effects.begin(); eit != core.reseted_effects.end(); ++eit) {
			pduel->delete_effect(*eit);
		}
		core.reseted_effects.clear();
		for(uint8 p = 0; p < 2; ++p) {
			for(uint8 i = 0; i < 5; ++i) {
				pcard = player[p].list_mzone[i];
				if(!pcard)
					continue;
				pcard->set_status(STATUS_SUMMON_TURN, FALSE);
				pcard->set_status(STATUS_SET_TURN, FALSE);
				pcard->set_status(STATUS_FORM_CHANGED, FALSE);
				pcard->set_status(STATUS_ATTACKED, FALSE);
				pcard->announce_count = 0;
				pcard->attacked_count = 0;
				pcard->announced_cards.clear();
				pcard->attacked_cards.clear();
				pcard->battled_cards.clear();
			}
			for(uint8 i = 0; i < 6; ++i) {
				pcard = player[p].list_szone[i];
				if(!pcard)
					continue;
				pcard->set_status(STATUS_SET_TURN, FALSE);
			}
			core.summon_state[p] = 0;
			core.normalsummon_state[p] = 0;
			core.flipsummon_state[p] = 0;
			core.spsummon_state[p] = 0;
			core.attack_state[p] = 0;
		}
		for(auto rit = effects.rechargeable.begin(); rit != effects.rechargeable.end(); ++rit)
			if(!((*rit)->flag & EFFECT_FLAG_NO_TURN_RESET))
				(*rit)->recharge();
		infos.turn_id++;
		core.summon_count[0] = 0;
		core.summon_count[1] = 0;
		infos.turn_player = turn_player;
		pduel->write_buffer8(MSG_NEW_TURN);
		pduel->write_buffer8(turn_player);
		infos.phase = PHASE_DRAW;
		core.phase_action = FALSE;
		raise_event((card*)0, EVENT_PHASE_START + PHASE_DRAW, 0, 0, 0, turn_player, 0);
		process_instant_event();
		adjust_all();
		return FALSE;
	}
	case 1: {
		core.new_fchain.clear();
		core.new_ochain.clear();
		core.instant_f_chain.clear();
		if(is_player_affected_by_effect(infos.turn_player, EFFECT_SKIP_DP)) {
			core.units.begin()->step = 2;
			reset_phase(PHASE_DRAW);
			adjust_all();
			return FALSE;
		}
		pduel->write_buffer8(MSG_NEW_PHASE);
		pduel->write_buffer8(infos.phase);
		raise_event((card*)0, EVENT_PREDRAW, 0, 0, 0, turn_player, 0);
		process_instant_event();
		if(core.new_fchain.size() || core.new_ochain.size() || core.flip_chain.size())
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 2: {
		//Draw
		int32 count = get_draw_count(infos.turn_player);
		if(count > 0) {
			draw(0, REASON_RULE, turn_player, turn_player, count);
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		}
		add_process(PROCESSOR_PHASE_EVENT, 0, 0, 0, PHASE_DRAW, 0);
		return FALSE;
	}
	case 3: {
		//Standby Phase
		infos.phase = PHASE_STANDBY;
		core.phase_action = FALSE;
		raise_event((card*)0, EVENT_PHASE_START + PHASE_STANDBY, 0, 0, 0, turn_player, 0);
		process_instant_event();
		adjust_all();
		return FALSE;
	}
	case 4: {
		if(core.new_fchain.size() || core.new_ochain.size() || core.flip_chain.size())
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 5: {
		core.new_fchain.clear();
		core.new_ochain.clear();
		core.instant_f_chain.clear();
		if(is_player_affected_by_effect(infos.turn_player, EFFECT_SKIP_SP)) {
			reset_phase(PHASE_STANDBY);
			adjust_all();
			return FALSE;
		}
		pduel->write_buffer8(MSG_NEW_PHASE);
		pduel->write_buffer8(infos.phase);
		add_process(PROCESSOR_PHASE_EVENT, 0, 0, 0, PHASE_STANDBY, 0);
		return FALSE;
	}
	case 6: {
		//Main1
		infos.phase = PHASE_MAIN1;
		core.phase_action = FALSE;
		raise_event((card*)0, EVENT_PHASE_START + PHASE_MAIN1, 0, 0, 0, turn_player, 0);
		process_instant_event();
		adjust_all();
		return FALSE;
	}
	case 7: {
		if(core.new_fchain.size() || core.new_ochain.size() || core.flip_chain.size())
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 8: {
		core.new_fchain.clear();
		core.new_ochain.clear();
		core.instant_f_chain.clear();
		pduel->write_buffer8(MSG_NEW_PHASE);
		pduel->write_buffer8(infos.phase);
		add_process(PROCESSOR_IDLE_COMMAND, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 9: {
		if(returns.ivalue[0] == 7) { // End Phase
			core.units.begin()->step = 14;
			return FALSE;
		}
		infos.phase = PHASE_BATTLE;
		core.phase_action = FALSE;
		pduel->write_buffer8(MSG_NEW_PHASE);
		pduel->write_buffer8(infos.phase);
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_EVENT);
		pduel->write_buffer8(infos.turn_player);
		pduel->write_buffer32(28);
		pduel->write_buffer8(MSG_HINT);
		pduel->write_buffer8(HINT_EVENT);
		pduel->write_buffer8(1 - infos.turn_player);
		pduel->write_buffer32(28);
		raise_event((card*)0, EVENT_PHASE_START + PHASE_BATTLE, 0, 0, 0, turn_player, 0);
		process_instant_event();
		adjust_all();
		return FALSE;
	}
	case 10: {
		core.hint_timing[infos.turn_player] = TIMING_BATTLE_START;
		add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 11: {
		core.new_fchain.clear();
		core.new_ochain.clear();
		core.instant_f_chain.clear();
		add_process(PROCESSOR_BATTLE_COMMAND, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 12: {
		if(returns.ivalue[0] == 3) { // End Phase
			core.units.begin()->step = 14;
			return FALSE;
		}
		//Main2
		infos.phase = PHASE_MAIN2;
		core.phase_action = FALSE;
		raise_event((card*)0, EVENT_PHASE_START + PHASE_MAIN2, 0, 0, 0, turn_player, 0);
		process_instant_event();
		adjust_all();
		return FALSE;
	}
	case 13: {
		if(core.new_fchain.size() || core.new_ochain.size() || core.flip_chain.size())
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 14: {
		core.new_fchain.clear();
		core.new_ochain.clear();
		core.instant_f_chain.clear();
		pduel->write_buffer8(MSG_NEW_PHASE);
		pduel->write_buffer8(infos.phase);
		add_process(PROCESSOR_IDLE_COMMAND, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 15: {
		//End Phase
		infos.phase = PHASE_END;
		core.phase_action = FALSE;
		pduel->write_buffer8(MSG_NEW_PHASE);
		pduel->write_buffer8(infos.phase);
		raise_event((card*)0, EVENT_PHASE_START + PHASE_END, 0, 0, 0, turn_player, 0);
		process_instant_event();
		adjust_all();
		return FALSE;
	}
	case 16: {
		if(core.new_fchain.size() || core.new_ochain.size() || core.flip_chain.size())
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		return FALSE;
	}
	case 17: {
		core.new_fchain.clear();
		core.new_ochain.clear();
		core.instant_f_chain.clear();
		add_process(PROCESSOR_PHASE_EVENT, 0, 0, 0, PHASE_END, 0);
		return FALSE;
	}
	case 18: {
		core.units.begin()->step = -1;
		core.units.begin()->arg1 = 1 - core.units.begin()->arg1;
		return FALSE;
	}
	}
	return TRUE;
}
int32 field::add_chain(uint16 step) {
	switch (step) {
	case 0: {
		chain_list::iterator clit = core.new_chains.begin();
		effect* peffect = clit->triggering_effect;
		if (!core.new_chains.size())
			return TRUE;
		effect_set eset;
		filter_player_effect(clit->triggering_player, EFFECT_ACTIVATE_COST, &eset);
		for(int32 i = 0; i < eset.count; ++i) {
			pduel->lua->add_param(clit->triggering_effect, PARAM_TYPE_EFFECT);
			pduel->lua->add_param(clit->triggering_player, PARAM_TYPE_INT);
			if(!eset[i]->check_value_condition(2))
				continue;
			if(eset[i]->cost) {
				core.sub_solving_event.push_back(clit->evt);
				add_process(PROCESSOR_EXECUTE_OPERATION, 0, eset[i], 0, clit->triggering_player, 0);
			}
		}
		if (peffect->type & EFFECT_TYPE_ACTIVATE) {
			break_effect();
			if (peffect->handler->current.location == LOCATION_HAND) {
				peffect->handler->enable_field_effect(FALSE);
				move_to_field(peffect->handler, peffect->handler->current.controler, peffect->handler->current.controler, LOCATION_SZONE, POS_FACEUP);
			} else
				change_position(peffect->handler, 0, peffect->handler->current.controler, POS_FACEUP, 0);
		}
		if(peffect->handler->current.location & 0x30)
			move_card(peffect->handler->current.controler, peffect->handler, peffect->handler->current.location, 0);
		return FALSE;
	}
	case 1: {
		chain_list::iterator clit = core.new_chains.begin();
		effect* peffect = clit->triggering_effect;
		if(peffect->type & EFFECT_TYPE_ACTIVATE) {
			clit->triggering_controler = peffect->handler->current.controler;
			clit->triggering_location = peffect->handler->current.location;
			clit->triggering_sequence = peffect->handler->current.sequence;
		}
		pduel->write_buffer8(MSG_CHAINING);
		pduel->write_buffer32(peffect->handler->data.code);
		pduel->write_buffer32(peffect->handler->get_info_location());
		pduel->write_buffer8(clit->triggering_controler);
		pduel->write_buffer8(clit->triggering_location);
		pduel->write_buffer8(clit->triggering_sequence);
		pduel->write_buffer32(peffect->description);
		pduel->write_buffer8(core.current_chain.size() + 1);
		break_effect();
		core.chain_limit = 0;
		if(!(peffect->flag & EFFECT_FLAG_FIELD_ONLY) && peffect->handler->is_affected_by_effect(EFFECT_DISABLE_EFFECT))
			clit->flag |= CHAIN_DISABLE_EFFECT;
		clit->chain_type = peffect->handler->get_type();
		clit->chain_count = core.current_chain.size() + 1;
		clit->target_cards = 0;
		clit->target_player = PLAYER_NONE;
		clit->target_param = 0;
		clit->disable_reason = 0;
		clit->disable_player = PLAYER_NONE;
		clit->replace_op = 0;
		core.current_chain.push_back(*clit);
		peffect->handler->create_relation(peffect);
		if(peffect->cost) {
			core.sub_solving_event.push_back(clit->evt);
			add_process(PROCESSOR_EXECUTE_COST, 0, peffect, 0, clit->triggering_player, 0);
		}
		core.new_chains.pop_front();
		return FALSE;
	}
	case 2: {
		chain_array::iterator clit = --(core.current_chain.end());
		effect* peffect = clit->triggering_effect;
		if(peffect->target) {
			core.sub_solving_event.push_back(clit->evt);
			add_process(PROCESSOR_EXECUTE_TARGET, 0, peffect, 0, clit->triggering_player, 0);
		}
		return FALSE;
	}
	case 3: {
		break_effect();
		chain_array::iterator clit = --(core.current_chain.end());
		effect* peffect = clit->triggering_effect;
		card_set::iterator cit;
		if(clit->target_cards && clit->target_cards->container.size()) {
			for(cit = clit->target_cards->container.begin(); cit != clit->target_cards->container.end(); ++cit)
				(*cit)->create_relation(clit->triggering_effect);
			if(clit->triggering_effect->flag & EFFECT_FLAG_CARD_TARGET) {
				for(cit = clit->target_cards->container.begin(); cit != clit->target_cards->container.end(); ++cit) {
					raise_single_event(*cit, EVENT_BECOME_TARGET, clit->triggering_effect, 0, clit->triggering_player, 0, clit->chain_count);
				}
				process_single_event();
			}
		}
		if(peffect->type & EFFECT_TYPE_ACTIVATE) {
			core.leave_confirmed.insert(peffect->handler);
			if(!(peffect->handler->data.type & (TYPE_CONTINUOUS + TYPE_FIELD + TYPE_EQUIP))
			        && !peffect->handler->is_affected_by_effect(EFFECT_REMAIN_FIELD))
				peffect->handler->set_status(STATUS_LEAVE_CONFIRMED, TRUE);
		}
		clit->triggering_effect->dec_count();
		core.phase_action = TRUE;
		pduel->write_buffer8(MSG_CHAINED);
		pduel->write_buffer8(clit->chain_count);
		raise_event(clit->triggering_effect->handler, EVENT_CHAINING, clit->triggering_effect, 0, clit->triggering_player, clit->triggering_player, clit->chain_count);
		process_instant_event();
		if(core.new_chains.size())
			add_process(PROCESSOR_ADD_CHAIN, 0, 0, 0, 0, 0);
		adjust_all();
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::sort_chain(uint16 step, uint8 tp) {
	switch(step) {
	case 0: {
		chain_list::iterator clit;
		core.select_cards.clear();
		if(tp)
			for(clit = core.tpchain.begin(); clit != core.tpchain.end(); ++clit)
				core.select_cards.push_back(clit->triggering_effect->handler);
		else
			for(clit = core.ntpchain.begin(); clit != core.ntpchain.end(); ++clit)
				core.select_cards.push_back(clit->triggering_effect->handler);
		add_process(PROCESSOR_SORT_CARD, 0, 0, 0, tp ? infos.turn_player : (1 - infos.turn_player), 1);
		return FALSE;
	}
	case 1: {
		if(returns.bvalue[0] == -1)
			return TRUE;
		chain_list::iterator clit;
		int32 i = 0, count;
		chain_array ch(8);
		if(tp) {
			count = core.tpchain.size();
			for(i = 0, clit = core.tpchain.begin(); i < count; ++clit, ++i)
				ch[returns.bvalue[i]] = *clit;
			core.tpchain.clear();
			for(i = 0; i < count; ++i)
				core.tpchain.push_back(ch[i]);
		} else {
			count = core.ntpchain.size();
			for(i = 0, clit = core.ntpchain.begin(); i < count; ++clit, ++i)
				ch[returns.bvalue[i]] = *clit;
			core.ntpchain.clear();
			for(i = 0; i < count; ++i)
				core.ntpchain.push_back(ch[i]);
		}
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::solve_continuous(uint16 step, effect * peffect, uint8 triggering_player) {
	switch(step) {
	case 0: {
		chain newchain;
		core.solving_event.splice(core.solving_event.begin(), core.sub_solving_event);
		newchain.chain_id = 0;
		newchain.chain_count = 0;
		newchain.triggering_effect = peffect;
		newchain.triggering_player = triggering_player;
		newchain.evt = *core.solving_event.begin();
		newchain.target_cards = 0;
		newchain.target_player = PLAYER_NONE;
		newchain.target_param = 0;
		newchain.disable_player = PLAYER_NONE;
		newchain.disable_reason = 0;
		newchain.flag = 0;
		core.continuous_chain.push_back(newchain);
		if(!peffect->target)
			return FALSE;
		core.sub_solving_event.push_back(*core.solving_event.begin());
		add_process(PROCESSOR_EXECUTE_TARGET, 0, peffect, 0, triggering_player, 0);
		return FALSE;
	}
	case 1: {
		return FALSE;
	}
	case 2: {
		if(!peffect->operation)
			return FALSE;
		peffect->dec_count();
		core.sub_solving_event.push_back(*core.solving_event.begin());
		add_process(PROCESSOR_EXECUTE_OPERATION, 0, peffect, 0, triggering_player, 0);
		return FALSE;
	}
	case 3: {
		if(core.continuous_chain.rbegin()->target_cards)
			pduel->delete_group(core.continuous_chain.rbegin()->target_cards);
		chain::opmap::iterator oit;
		for(oit = core.continuous_chain.rbegin()->opinfos.begin(); oit != core.continuous_chain.rbegin()->opinfos.end(); ++oit) {
			if(oit->second.op_cards)
				pduel->delete_group(oit->second.op_cards);
		}
		core.continuous_chain.pop_back();
		core.solving_event.pop_front();
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::solve_chain(uint16 step, uint32 skip_new) {
	if(core.current_chain.size() == 0 && step == 0)
		return TRUE;
	chain_array::reverse_iterator cait = core.current_chain.rbegin();
	switch(step) {
	case 0: {
		pduel->write_buffer8(MSG_CHAIN_SOLVING);
		pduel->write_buffer8(cait->chain_count);
		add_to_disable_check_list(cait->triggering_effect->handler);
		adjust_instant();
		raise_event((card*)0, EVENT_CHAIN_ACTIVATING, cait->triggering_effect, 0, cait->triggering_player, cait->triggering_player, cait->chain_count);
		process_instant_event();
		return FALSE;
	}
	case 1: {
		if(cait->flag & CHAIN_DISABLE_ACTIVATE && is_chain_inactivatable(cait->chain_count)) {
			remove_oath_effect(cait->triggering_effect);
			if((cait->triggering_effect->flag & EFFECT_FLAG_COUNT_LIMIT) && (cait->triggering_effect->flag & EFFECT_FLAG_REPEAT))
				cait->triggering_effect->reset_count += 0x100;
			raise_event((card*)0, EVENT_CHAIN_INACTIVATED, cait->triggering_effect, 0, cait->triggering_player, cait->triggering_player, cait->chain_count);
			process_instant_event();
			core.units.begin()->step = 9;
			return FALSE;
		}
		oath_effects::iterator oeit;
		for(oeit = effects.oath.begin(); oeit != effects.oath.end(); ++oeit)
			if(oeit->second == cait->triggering_effect)
				oeit->second = 0;
		break_effect();
		core.chain_solving = TRUE;
		if(cait->opinfos.count(0x200))
			core.spsummon_state[cait->triggering_player] = TRUE;
		if((cait->triggering_effect->type & EFFECT_TYPE_ACTIVATE) && cait->triggering_effect->handler->is_has_relation(cait->triggering_effect)) {
			cait->triggering_effect->handler->enable_field_effect(TRUE);
			adjust_instant();
		}
		raise_event((card*)0, EVENT_CHAIN_SOLVING, cait->triggering_effect, 0, cait->triggering_player, cait->triggering_player, cait->chain_count);
		process_instant_event();
		return FALSE;
	}
	case 2: {
		card* pcard = cait->triggering_effect->handler;
		if(is_chain_disablable(cait->chain_count)) {
			if((cait->flag & CHAIN_DISABLE_EFFECT) || (pcard->is_status(STATUS_DISABLED) && pcard->is_has_relation(cait->triggering_effect))) {
				if(!(cait->flag & CHAIN_NEGATED)) {
					pduel->write_buffer8(MSG_CHAIN_DISABLED);
					pduel->write_buffer8(cait->chain_count);
				}
				raise_event((card*)0, EVENT_CHAIN_DISABLED, cait->triggering_effect, 0, cait->triggering_player, cait->triggering_player, cait->chain_count);
				process_instant_event();
				core.units.begin()->step = 3;
				return FALSE;
			}
		}
		core.units.begin()->peffect = (effect*)cait->triggering_effect->operation;
		if(cait->replace_op)
			cait->triggering_effect->operation = cait->replace_op;
		if(cait->triggering_effect->operation) {
			core.sub_solving_event.push_back(cait->evt);
			add_process(PROCESSOR_EXECUTE_OPERATION, 0, cait->triggering_effect, 0, cait->triggering_player, 0);
		}
		return FALSE;
	}
	case 3: {
		effect* peffect = cait->triggering_effect;
		if((peffect->type & EFFECT_TYPE_ACTIVATE) && (peffect->handler->current.location == LOCATION_SZONE))
			peffect->handler->set_status(STATUS_ACTIVATED, TRUE);
		peffect->operation = (ptr)core.units.begin()->peffect;
		if(core.special_summoning.size())
			special_summon_complete(peffect, cait->triggering_player);
		return FALSE;
	}
	case 4: {
		core.chain_solving = FALSE;
		effect_vector::iterator eit;
		event_list::iterator evit;
		for(eit = core.delayed_tp.begin(), evit = core.delayed_tev.begin(); eit != core.delayed_tp.end(); ++eit, ++evit) {
			core.sub_solving_event.push_back(*evit);
			add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, (*eit), 0, infos.turn_player, 0);
		}
		for(eit = core.delayed_ntp.begin(), evit = core.delayed_ntev.begin(); eit != core.delayed_ntp.end(); ++eit, ++evit) {
			core.sub_solving_event.push_back(*evit);
			add_process(PROCESSOR_SOLVE_CONTINUOUS, 0, (*eit), 0, 1 - infos.turn_player, 0);
		}
		core.delayed_tp.clear();
		core.delayed_ntp.clear();
		core.delayed_tev.clear();
		core.delayed_ntev.clear();
		pduel->write_buffer8(MSG_CHAIN_SOLVED);
		pduel->write_buffer8(cait->chain_count);
		raise_event((card*)0, EVENT_CHAIN_SOLVED, cait->triggering_effect, 0, cait->triggering_player, cait->triggering_player, cait->chain_count);
		process_instant_event();
		core.units.begin()->step = 9;
		return FALSE;
	}
	case 10: {
		card* pcard = cait->triggering_effect->handler;
		if(cait->target_cards && cait->target_cards->container.size()) {
			card_set::iterator cit;
			for(cit = cait->target_cards->container.begin(); cit != cait->target_cards->container.end(); ++cit)
				(*cit)->release_relation(cait->triggering_effect);
		}
		if((pcard->data.type & TYPE_EQUIP) && (cait->triggering_effect->type & EFFECT_TYPE_ACTIVATE) && !pcard->equiping_target)
			pcard->set_status(STATUS_LEAVE_CONFIRMED, TRUE);
		if((pcard->data.type & TYPE_FIELD) && (cait->triggering_effect->type & EFFECT_TYPE_ACTIVATE) && !pcard->is_status(STATUS_LEAVE_CONFIRMED)
		        && pcard->is_has_relation(cait->triggering_effect) && player[1 - pcard->current.controler].list_szone[5]
		        && player[1 - pcard->current.controler].list_szone[5]->is_position(POS_FACEUP))
			destroy(player[1 - pcard->current.controler].list_szone[5], 0, REASON_RULE, 0);
		pcard->release_relation(cait->triggering_effect);
		if(cait->target_cards)
			pduel->delete_group(cait->target_cards);
		chain::opmap::iterator oit;
		for(oit = cait->opinfos.begin(); oit != cait->opinfos.end(); ++oit) {
			if(oit->second.op_cards)
				pduel->delete_group(oit->second.op_cards);
		}
		adjust_all();
		core.current_chain.pop_back();
		if(!core.current_chain.size()) {
			core.chain_limit = 0;
			return FALSE;
		}
		core.units.begin()->step = -1;
		return FALSE;
	}
	case 11: {
		card_set::iterator cit, rm;
		for(cit = core.leave_confirmed.begin(); cit != core.leave_confirmed.end();) {
			rm = cit++;
			if(!(*rm)->is_status(STATUS_LEAVE_CONFIRMED))
				core.leave_confirmed.erase(rm);
		}
		if(core.leave_confirmed.size())
			send_to(&core.leave_confirmed, 0, REASON_RULE, PLAYER_NONE, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
		return FALSE;
	}
	case 12: {
		core.used_event.splice(core.used_event.end(), core.point_event);
		pduel->write_buffer8(MSG_CHAIN_END);
		core.chain_limit_p = 0;
		reset_chain();
		if(core.summoning_card || core.spsummoning_card) {
			core.subunits.push_back(core.reserved);
		} else if(!skip_new) {
			raise_event((card*)0, EVENT_CHAIN_END, 0, 0, 0, 0, 0);
			process_instant_event();
			core.hint_timing[0] |= TIMING_CHAIN_END;
			core.hint_timing[1] |= TIMING_CHAIN_END;
			add_process(PROCESSOR_POINT_EVENT, 0, 0, 0, 0, 0);
		}
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::break_effect() {
	core.hint_timing[0] = 0;
	core.hint_timing[1] = 0;
	for (auto chit = core.new_ochain.begin(); chit != core.new_ochain.end();) {
		auto rm = chit++;
		if (!(rm->triggering_effect->flag & EFFECT_FLAG_DELAY)) {
			pduel->write_buffer8(MSG_MISSED_EFFECT);
			pduel->write_buffer32(rm->triggering_effect->handler->get_info_location());
			pduel->write_buffer32(rm->triggering_effect->handler->data.code);
			core.new_ochain.erase(rm);
		}
	}
	core.used_event.splice(core.used_event.end(), core.instant_event);
	adjust_instant();
	return 0;
}
void field::adjust_instant() {
	adjust_disable_check_list();
}
void field::adjust_all() {
	add_process(PROCESSOR_ADJUST, 0, 0, 0, 0, 0);
}
void field::refresh_location_info_instant() {
	effect_set eset;
	uint32 value, p;
	int32 dis1 = player[0].disabled_location | (player[1].disabled_location << 16);
	player[0].disabled_location = 0;
	player[1].disabled_location = 0;
	filter_field_effect(EFFECT_DISABLE_FIELD, &eset);
	for (int32 i = 0; i < eset.count; ++i) {
		value = eset[i]->get_value();
		player[0].disabled_location |= value & 0x1f1f;
		player[1].disabled_location |= (value >> 16) & 0x1f1f;
	}
	eset.clear();
	filter_field_effect(EFFECT_USE_EXTRA_MZONE, &eset);
	for (int32 i = 0; i < eset.count; ++i) {
		p = eset[i]->get_handler_player();
		value = eset[i]->get_value();
		player[p].disabled_location |= (value >> 16) & 0x1f;
	}
	eset.clear();
	filter_field_effect(EFFECT_USE_EXTRA_SZONE, &eset);
	for (int32 i = 0; i < eset.count; ++i) {
		p = eset[i]->get_handler_player();
		value = eset[i]->get_value();
	}
	int32 dis2 = player[0].disabled_location | (player[1].disabled_location << 16);
	if(dis1 != dis2) {
		pduel->write_buffer8(MSG_FIELD_DISABLED);
		pduel->write_buffer32(dis2);
	}
}
int32 field::refresh_location_info(uint16 step) {
	switch(step) {
	case 0: {
		effect_set eset;
		uint32 value;
		int32 p;
		core.units.begin()->arg2 = player[0].disabled_location | (player[1].disabled_location << 16);
		player[0].disabled_location = 0;
		player[1].disabled_location = 0;
		core.disfield_effects.clear();
		core.extraz_effects.clear();
		core.extraz_effects_e.clear();
		filter_field_effect(EFFECT_DISABLE_FIELD, &eset);
		for (int32 i = 0; i < eset.count; ++i) {
			value = eset[i]->get_value();
			if(value) {
				player[0].disabled_location |= value & 0x1f1f;
				player[1].disabled_location |= (value >> 16) & 0x1f1f;
			} else
				core.disfield_effects.add_item(eset[i]);
		}
		eset.clear();
		filter_field_effect(EFFECT_USE_EXTRA_MZONE, &eset);
		for (int32 i = 0; i < eset.count; ++i) {
			p = eset[i]->get_handler_player();
			value = eset[i]->get_value();
			player[p].disabled_location |= (value >> 16) & 0x1f;
			if(field_used_count[(value >> 16) & 0x1f] == 0)
				core.extraz_effects_e.add_item(eset[i]);
			else if(field_used_count[(value >> 16) & 0x1f] < (value & 0xffff))
				core.extraz_effects.add_item(eset[i]);
		}
		eset.clear();
		filter_field_effect(EFFECT_USE_EXTRA_SZONE, &eset);
		for (int32 i = 0; i < eset.count; ++i) {
			p = eset[i]->get_handler_player();
			value = eset[i]->get_value();
			player[p].disabled_location |= (value >> 8) & 0x1f00;
			if(field_used_count[(value >> 16) & 0x1f] == 0)
				core.extraz_effects_e.add_item(eset[i]);
			else if(field_used_count[(value >> 16) & 0x1f] < (value & 0xffff))
				core.extraz_effects.add_item(eset[i]);
		}
		return FALSE;
	}
	case 1: {
		if(core.disfield_effects.count == 0) {
			core.units.begin()->step = 2;
			return FALSE;
		}
		effect* peffect = core.disfield_effects[0];
		core.units.begin()->peffect = peffect;
		core.disfield_effects.remove_item(0);
		if(!peffect->operation) {
			peffect->value = 0x80;
			core.units.begin()->step = 0;
			return FALSE;
		}
		core.sub_solving_event.push_back(nil_event);
		add_process(PROCESSOR_EXECUTE_OPERATION, 0, peffect, 0, peffect->get_handler_player(), 0);
		return FALSE;
	}
	case 2: {
		returns.ivalue[0] &= 0x1f1f1f1f;
		if(returns.ivalue[0] == 0)
			returns.ivalue[0] = 0x80;
		if(core.units.begin()->peffect->get_handler_player() == 0) {
			core.units.begin()->peffect->value = returns.ivalue[0];
			player[0].disabled_location |= returns.ivalue[0] & 0x1f1f;
			player[1].disabled_location |= (returns.ivalue[0] >> 16) & 0x1f1f;
		} else {
			core.units.begin()->peffect->value = ((returns.ivalue[0] << 16) | (returns.ivalue[0] >> 16));
			player[1].disabled_location |= returns.ivalue[0] & 0x1f1f;
			player[0].disabled_location |= (returns.ivalue[0] >> 16) & 0x1f1f;
		}
		core.units.begin()->step = 0;
		return FALSE;
	}
	case 3: {
		if(core.extraz_effects.count == 0) {
			core.units.begin()->step = 4;
			return FALSE;
		}
		effect* peffect = core.extraz_effects[0];
		core.units.begin()->peffect = peffect;
		core.extraz_effects.remove_item(0);
		int32 p = peffect->get_handler_player();
		int32 loc = (peffect->code == EFFECT_USE_EXTRA_MZONE) ? LOCATION_MZONE : LOCATION_SZONE;
		if((loc == LOCATION_MZONE && ((player[p].disabled_location & 0x1f) == 0x1f))
		        || (loc == LOCATION_SZONE && ((player[p].disabled_location & 0x1f00) == 0x1f00))) {
			core.units.begin()->step = 2;
			return FALSE;
		}
		int32 val = peffect->get_value();
		int32 count1 = (val & 0xffff) - field_used_count[(val >> 16) & 0x1f];
		uint32 flag = 0;
		int32 count2 = get_useable_count(p, loc, &flag);
		if(count1 > count2)
			count1 = count2;
		core.units.begin()->arg1 = count1;
		if(loc == LOCATION_SZONE)
			flag = ((flag << 8) & 0xff00) | 0xffff00ff;
		else
			flag = (flag & 0xff) | 0xffffff00;
		add_process(PROCESSOR_SELECT_DISFIELD, 0, 0, 0, p + (count1 << 16), flag);
		return FALSE;
	}
	case 4: {
		uint32 count1 = core.units.begin()->arg1;
		uint32 selflag = 0;
		uint8 s, pt = 0;
		for(uint32 i = 0; i < count1; ++i) {
			s = returns.bvalue[pt + 2];
			selflag |= 1 << s;
			pt += 3;
		}
		effect* peffect = core.units.begin()->peffect;
		if(peffect->code == EFFECT_USE_EXTRA_MZONE)
			player[peffect->get_handler_player()].disabled_location |= selflag;
		else
			player[peffect->get_handler_player()].disabled_location |= selflag << 8;
		peffect->value = ((int32)peffect->value & 0xffff) | (selflag << 16);
		core.units.begin()->step = 2;
		return FALSE;
	}
	case 5: {
		if(core.extraz_effects_e.count == 0) {
			core.units.begin()->step = 6;
			return FALSE;
		}
		effect* peffect = core.extraz_effects_e[0];
		core.units.begin()->peffect = peffect;
		core.extraz_effects_e.remove_item(0);
		int32 p = peffect->get_handler_player();
		int32 loc = (peffect->code == EFFECT_USE_EXTRA_MZONE) ? LOCATION_MZONE : LOCATION_SZONE;
		if(((loc == LOCATION_MZONE) && ((player[p].disabled_location & 0x1f) == 0x1f))
		        || ((loc == LOCATION_SZONE) && ((player[p].disabled_location & 0x1f00) == 0x1f00))) {
			core.units.begin()->step = 4;
			return FALSE;
		}
		int32 val = peffect->get_value();
		int32 count1 = (val & 0xffff) - field_used_count[(val >> 16) & 0x1f];
		uint32 flag = 0;
		int32 count2 = get_useable_count(p, loc, &flag);
		if(count1 > count2)
			count1 = count2;
		core.units.begin()->arg1 = count1;
		if(loc == LOCATION_SZONE)
			flag = ((flag << 8) & 0xff00) | 0xffff00ff;
		else
			flag = (flag & 0xff) | 0xffffff00;
		add_process(PROCESSOR_SELECT_DISFIELD, 0, 0, 0, p + (count1 << 16), flag);
		return FALSE;
	}
	case 6: {
		uint32 count1 = core.units.begin()->arg1;
		uint32 selflag = 0;
		uint8 s, pt = 0;
		for(uint32 i = 0; i < count1; ++i) {
			s = returns.bvalue[pt + 2];
			selflag |= 1 << s;
			pt += 3;
		}
		effect* peffect = core.units.begin()->peffect;
		if(peffect->code == EFFECT_USE_EXTRA_MZONE)
			player[peffect->get_handler_player()].disabled_location |= selflag;
		else
			player[peffect->get_handler_player()].disabled_location |= selflag << 8;
		peffect->value = ((int32)peffect->value & 0xffff) | (selflag << 16);
		core.units.begin()->step = 4;
		return FALSE;
	}
	case 7: {
		uint32 dis = player[0].disabled_location | (player[1].disabled_location << 16);
		if(dis != (uint32)core.units.begin()->arg2) {
			pduel->write_buffer8(MSG_FIELD_DISABLED);
			pduel->write_buffer32(dis);
		}
		return TRUE;
	}
	}
	return TRUE;
}
int32 field::adjust_step(uint16 step) {
	//win, isable, control, self_des, equip, position, trap_monster
	switch(step) {
	case 0: {
		core.units.begin()->arg1 = FALSE;
		core.re_adjust = FALSE;
		return FALSE;
	}
	case 1: {
		//win check
		if(core.win_player != 5) {
			pduel->write_buffer8(MSG_WIN);
			pduel->write_buffer8(core.win_player);
			pduel->write_buffer8(core.win_reason);
			core.win_player = 5;
			core.win_reason = 0;
			core.overdraw[0] = core.overdraw[1] = FALSE;
			return FALSE;
		}
		uint32 winp = 5, rea = 1;
		if((player[0].lp <= 0 && player[1].lp > 0)) {
			winp = 1;
			rea = 1;
		}
		if(core.overdraw[0] && !core.overdraw[1]) {
			winp = 1;
			rea = 2;
		}
		if(player[1].lp <= 0 && player[0].lp > 0) {
			winp = 0;
			rea = 1;
		}
		if(core.overdraw[1] && !core.overdraw[0]) {
			winp = 0;
			rea = 2;
		}
		if(player[1].lp <= 0 && player[0].lp <= 0) {
			winp = PLAYER_NONE;
			rea = 1;
		}
		if(core.overdraw[1] && core.overdraw[0]) {
			winp = PLAYER_NONE;
			rea = 2;
		}
		if(winp != 5) {
			pduel->write_buffer8(MSG_WIN);
			pduel->write_buffer8(winp);
			pduel->write_buffer8(rea);
		}
		return FALSE;
	}
	case 2: {
		//disable check
		uint8 tp = infos.turn_player;
		card* pcard;
		for(uint8 p = 0; p < 2; ++p) {
			for(uint8 i = 0; i < 5; ++i) {
				pcard = player[tp].list_mzone[i];
				if(pcard)
					add_to_disable_check_list(pcard);
			}
			for(uint8 i = 0; i < 6; ++i) {
				pcard = player[tp].list_szone[i];
				if(pcard)
					add_to_disable_check_list(pcard);
			}
			tp = 1 - tp;
		}
		adjust_disable_check_list();
		add_process(PROCESSOR_REFRESH_LOC, 0, 0, 0, 0, 0);
	}
	case 3: {
		//trap monster
		card* pcard;
		uint8 tp = infos.turn_player;
		for(uint8 p = 0; p < 2; ++p) {
			for(uint8 i = 0; i < 5; ++i) {
				pcard = player[tp].list_mzone[i];
				if(!pcard) continue;
				if((pcard->get_type()&TYPE_TRAPMONSTER) && pcard->is_affected_by_effect(EFFECT_DISABLE_TRAPMONSTER)) {
					pcard->reset(RESET_LEAVE, RESET_EVENT);
					refresh_location_info_instant();
					move_to_field(pcard, tp, tp, LOCATION_SZONE, pcard->current.position);
					core.units.begin()->arg1 = TRUE;
				}
			}
			tp = 1 - tp;
		}
		return FALSE;
	}
	case 4: {
		//1-4 control
		card* pcard;
		uint8 cur, ref, tp = infos.turn_player;
		card_list::iterator cit;
		core.adjust_list.clear();
		for(uint8 p = 0; p < 2; ++p) {
			for(uint8 i = 0; i < 5; ++i) {
				pcard = player[tp].list_mzone[i];
				if(!pcard) continue;
				cur = pcard->current.controler;
				ref = pcard->refresh_control_status();
				if(cur != ref && pcard->is_capable_change_control()) {
					core.adjust_list.push_back(pcard);
					pcard->operation_param = ref;
				}
			}
			tp = 1 - tp;
		}
		if(core.adjust_list.size())
			core.units.begin()->arg1 = TRUE;
		card* swap[2];
		card_list::iterator it[2];
		bool swaped;
		swap[0] = swap[1] = 0;
		do {
			swaped = false;
			for(cit = core.adjust_list.begin(); cit != core.adjust_list.end(); ++cit) {
				tp = (*cit)->current.controler;
				if(((tp == 0 && swap[0] == 0) || (tp == 1 && swap[1] == 0)) &&
				        (!((*cit)->get_type()&TYPE_TRAPMONSTER) || get_useable_count(1 - tp, LOCATION_SZONE) > 0)) {
					swap[tp] = *cit;
					it[tp] = cit;
					if(swap[0] && swap[1]) {
						swap_control(0, PLAYER_NONE, swap[0], swap[1], 0, 0);
						core.adjust_list.erase(it[0]);
						core.adjust_list.erase(it[1]);
						swap[0] = 0;
						swap[1] = 0;
						swaped = true;
						break;
					}
				}
			}
		} while(swaped);
		core.adjust_set.clear();
		return FALSE;
	}
	case 5: {
		if(!core.adjust_list.size()) {
			core.units.begin()->step = 6;
			return FALSE;
		}
		card_list::iterator cit = core.adjust_list.begin();
		get_control(0, PLAYER_NONE, *cit, 1 - (*cit)->current.controler, 0, 0);
		return FALSE;
	}
	case 6: {
		if(!returns.ivalue[0])
			core.adjust_set.insert(core.adjust_list.front());
		core.adjust_list.pop_front();
		core.units.begin()->step = 4;
		return FALSE;
	}
	case 7: {
		if(core.adjust_set.size())
			send_to(&core.adjust_set, 0, REASON_RULE, PLAYER_NONE, PLAYER_NONE, LOCATION_GRAVE, 0, POS_FACEUP);
		return FALSE;
	}
	case 8: {
		if(core.selfdes_disabled) {
			core.units.begin()->step = 9;
			return FALSE;
		}
		//self destroy
		uint8 tp = infos.turn_player;
		card* pcard;
		core.destroy_set.clear();
		for(uint8 p = 0; p < 2; ++p) {
			for(uint8 i = 0; i < 5; ++i) {
				pcard = player[tp].list_mzone[i];
				if(pcard && pcard->is_position(POS_FACEUP) && pcard->is_affected_by_effect(EFFECT_SELF_DESTROY))
					core.destroy_set.insert(pcard);
			}
			for(uint8 i = 0; i < 6; ++i) {
				pcard = player[tp].list_szone[i];
				if(pcard && pcard->is_position(POS_FACEUP) && pcard->is_affected_by_effect(EFFECT_SELF_DESTROY))
					core.destroy_set.insert(pcard);
			}
			tp = 1 - tp;
		}
		if(core.destroy_set.size()) {
			destroy(&core.destroy_set, 0, REASON_EFFECT, PLAYER_NONE);
			for(card_set::iterator cit = core.destroy_set.begin(); cit != core.destroy_set.end(); ++cit)
				(*cit)->current.reason_player = (*cit)->current.controler;
		} else {
			returns.ivalue[0] = 0;
		}
		return FALSE;
	}
	case 9: {
		if(returns.ivalue[0] > 0)
			core.units.begin()->arg1 = TRUE;
		//equip check
		uint8 tp = infos.turn_player;
		card* pcard;
		core.destroy_set.clear();
		for(uint8 p = 0; p < 2; ++p) {
			for(uint8 i = 0; i < 6; ++i) {
				pcard = player[tp].list_szone[i];
				if(pcard && pcard->equiping_target && !pcard->is_affected_by_effect(EFFECT_EQUIP_LIMIT, pcard->equiping_target))
					core.destroy_set.insert(pcard);
			}
			tp = 1 - tp;
		}
		if(core.destroy_set.size()) {
			core.units.begin()->arg1 = TRUE;
			destroy(&core.destroy_set, 0, REASON_RULE, PLAYER_NONE);
		}
		return FALSE;
	}
	case 10: {
		//position
		uint32 tp = infos.turn_player, pos;
		card* pcard;
		core.adjust_set.clear();
		effect_set eset;
		for(uint8 p = 0; p < 2; ++p) {
			for(uint8 i = 0; i < 5; ++i) {
				pcard = player[tp].list_mzone[i];
				if(!pcard)
					continue;
				eset.clear();
				pcard->filter_effect(EFFECT_SET_POSITION, &eset);
				if(eset.count) {
					pos = eset.get_last()->get_value();
					if((pos & 0xff) != pcard->current.position) {
						core.adjust_set.insert(pcard);
						pcard->operation_param = pos;
						if(pcard->is_status(STATUS_JUST_POS))
							pcard->set_status(STATUS_CONTINUOUS_POS, TRUE);
						else
							pcard->set_status(STATUS_CONTINUOUS_POS, FALSE);
					} else
						pcard->set_status(STATUS_CONTINUOUS_POS, FALSE);
					pcard->set_status(STATUS_JUST_POS, FALSE);
				}
			}
			tp = 1 - tp;
		}
		if(core.adjust_set.size()) {
			core.units.begin()->arg1 = TRUE;
			group* ng = pduel->new_group();
			ng->container = core.adjust_set;
			ng->is_readonly = TRUE;
			add_process(PROCESSOR_CHANGEPOS, 0, 0, ng, PLAYER_NONE, TRUE);
		}
		return FALSE;
	}
	case 11: {
		raise_event((card*)0, EVENT_ADJUST, 0, 0, PLAYER_NONE, PLAYER_NONE, 0);
		process_instant_event();
		return FALSE;
	}
	case 12: {
		if(core.re_adjust || core.units.begin()->arg1) {
			core.units.begin()->step = -1;
			return FALSE;
		}
		return TRUE;
	}
	}
	return TRUE;
}