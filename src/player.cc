/*
 * Copyright (C) 2002 by the Widelands Development Team
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "widelands.h"
#include "pic.h"
#include "bob.h"
#include "myfile.h"
#include "descr_maintainer.h"
#include "tribe.h"
#include "player.h"

//
//
// class Player
//
//
Player::Player(Game* g, int type, int plnum) {
   m_type = type; 
	m_plnum = plnum;
   game=g;
   seen_fields=0;
}

Player::~Player(void) {
   if(seen_fields) 
      delete seen_fields;
}

/** Player::setup()
 *
 * Prepare the player for in-game action
 */
void Player::setup()
{
	seen_fields = new std::bit_vector(game->get_map()->get_w()*game->get_map()->get_h(), false);

	// place the HQ
	const Cords *c = game->get_map()->get_starting_pos(m_plnum);
	game->warp_building(c->x, c->y, m_plnum, 0);
}

/** Player::set_area_seen(int x, int y, uint area, bool on)
 *
 * Mark the given area as (un)seen
 */
void Player::set_area_seen(int x, int y, uint area, bool on)
{
	Map_Region_Cords r(x, y, area, game->get_map());
  
	while(r.next(&x, &y)) {
      set_field_seen(x, y, on);
   }
}
