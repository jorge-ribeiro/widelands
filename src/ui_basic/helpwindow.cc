/*
 * Copyright (C) 2002-2004, 2006-2010 by the Widelands Development Team
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "ui_basic/helpwindow.h"

#include <memory>
#include <string>

#include <boost/format.hpp>

#include "base/i18n.h"
#include "graphic/font.h"
#include "graphic/font_handler.h"
#include "graphic/font_handler1.h"
#include "graphic/graphic.h"
#include "graphic/text/font_set.h"
#include "graphic/text_constants.h"
#include "io/filesystem/layered_filesystem.h"
#include "logic/building.h"
#include "scripting/lua_interface.h"
#include "scripting/lua_table.h"
#include "ui_basic/button.h"
#include "ui_basic/window.h"

namespace UI {

HelpWindow::HelpWindow
	(Panel * const parent,
	 const std::string & caption,
	 const std::string & helptext,
	 uint32_t width, uint32_t height)
	:
	Window(parent, "help_window", 0, 0, width, height, (boost::format(_("Help: %s")) % caption).str()),
	textarea_(new MultilineTextarea(this, 5, 5, width - 10, height - 30, std::string(), Align_Left))
{
	int margin = 5;

	// Calculate sizes
	width  = (width  == 0) ? g_gr->get_xres() * 3 / 5 : width;
	height = (height == 0) ? g_gr->get_yres() * 4 / 5 : height;

	Button* btn = new Button(this, "ok", width / 3, 0, width / 3, 0,
									 g_gr->images().get("pics/but5.png"),
									 _("OK"), "", true, false);

	btn->sigclicked.connect(boost::bind(&HelpWindow::clicked_ok, boost::ref(*this)));
	btn->set_pos(Point(btn->get_x(), height - margin - btn->get_h()));

	textarea_->set_size(width - 2 * margin, height - btn->get_h() - 3 * margin);
	textarea_->set_text(helptext);

	set_inner_size(width, height);
	center_to_parent();
	focus();
}


/**
 * Handle mouseclick.
 *
 * Clicking the right mouse button inside the window acts like pressing Ok.
 */
bool HelpWindow::handle_mousepress(const uint8_t btn, int32_t, int32_t)
{
	if (btn == SDL_BUTTON_RIGHT) {
		play_click();
		clicked_ok();
	}
	return true;
}

bool HelpWindow::handle_mouserelease(const uint8_t, int32_t, int32_t)
{
	return true;
}

bool HelpWindow::handle_key(bool down, SDL_Keysym code)
{
	if (down) {
		switch (code.sym) {
			case SDLK_KP_ENTER:
			case SDLK_RETURN:
				clicked_ok();
				return true;
			default:
				return true; // handled
		}
	}
	return true;
}


void HelpWindow::clicked_ok()
{
	if (is_modal())
		end_modal<UI::Panel::Returncodes>(UI::Panel::Returncodes::kBack);
	else {
		// do not call die() here - could lead to broken pointers.
		// the window should get deleted with the parent anyways.
		set_visible(false);
	}
}

/*
===================
LuaTextHelpWindow
===================
*/
LuaTextHelpWindow::LuaTextHelpWindow
	(Panel * const parent,
	 UI::UniqueWindow::Registry & reg,
	 const Widelands::BuildingDescr& building_description,
	 LuaInterface * const lua,
	 uint32_t width, uint32_t height)
	:
	UI::UniqueWindow(parent, "help_window", &reg, width, height,
			(boost::format(_("Help: %s")) % building_description.descname()).str()),
	textarea_(new MultilineTextarea(this, 5, 5, width - 10, height -10, std::string(), Align_Left))
{
	try {
		std::unique_ptr<LuaTable> t(
		   lua->run_script(building_description.helptext_script()));
		std::unique_ptr<LuaCoroutine> cr(t->get_coroutine("func"));
		cr->push_arg(&building_description);
		cr->resume();
		const std::string help_text = cr->pop_string();
		textarea_->set_text(help_text);
	} catch (LuaError& err) {
		textarea_->set_text(err.what());
	}
}
LuaTextHelpWindow::~LuaTextHelpWindow()
{
}

}
