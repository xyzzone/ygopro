/*
 * libdebug.cpp
 *
 *  Created on: 2012-2-8
 *      Author: Argon
 */

#include "scriptlib.h"
#include "duel.h"
#include "field.h"
#include "card.h"
#include "effect.h"
#include "ocgapi.h"

int32 scriptlib::debug_message(lua_State *L) {
	duel* pduel = interpreter::get_duel_info(L);
	lua_getglobal(L, "tostring");
	lua_pushvalue(L, -2);
	lua_pcall(L, 1, 1, 0);
	sprintf(pduel->strbuffer, "%s", lua_tostring(L, -1));
	handle_message(pduel, 2);
	return 0;
}
int32 scriptlib::debug_add_card(lua_State *L) {
	check_param_count(L, 6);
	duel* pduel = interpreter::get_duel_info(L);
	int32 code = lua_tointeger(L, 1);
	int32 owner = lua_tointeger(L, 2);
	int32 playerid = lua_tointeger(L, 3);
	int32 location = lua_tointeger(L, 4);
	int32 sequence = lua_tointeger(L, 5);
	int32 position = lua_tointeger(L, 6);
	int32 proc = lua_toboolean(L, 7);
	if(owner != 0 && owner != 1)
		return 0;
	if(playerid != 0 && playerid != 1)
		return 0;
	if(pduel->game_field->is_location_useable(playerid, location, sequence)) {
		card* pcard = pduel->new_card(code);
		pcard->owner = owner;
		pduel->game_field->add_card(playerid, pcard, location, sequence);
		pcard->current.position = position;
		if(!(location & LOCATION_ONFIELD) || (position & POS_FACEUP)) {
			pcard->enable_field_effect(TRUE);
			pduel->game_field->adjust_instant();
		}
		if(proc)
			pcard->set_status(STATUS_PROC_COMPLETE, TRUE);
		interpreter::card2value(L, pcard);
		return 1;
	}
	return 0;
}
int32 scriptlib::debug_set_player_info(lua_State *L) {
	check_param_count(L, 4);
	duel* pduel = interpreter::get_duel_info(L);
	uint32 playerid = lua_tointeger(L, 1);
	uint32 lp = lua_tointeger(L, 2);
	uint32 startcount = lua_tointeger(L, 3);
	uint32 drawcount = lua_tointeger(L, 4);
	duel* pd = (duel*)pduel;
	if(playerid != 0 && playerid != 1)
		return 0;
	pd->game_field->player[playerid].lp = lp;
	pd->game_field->player[playerid].start_count = startcount;
	pd->game_field->player[playerid].draw_count = drawcount;
	return 0;
}
