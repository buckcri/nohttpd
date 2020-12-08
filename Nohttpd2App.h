/*
    This file is part of nohttpd version 2, see http://www.cdhk.de/buck/nohttpd/
    Copyright (C) 2006 Christian Buck <c.buck@gmx.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __NOHTTPD2APP_H
#define __NOHTTPD2APP_H

#include <wx/event.h>

class Nohttpd2App:public wxApp{

	public:
		class Nohttpd2Dlg *m_maindlg;
		bool m_dontlog;
		wxSemaphore semhits,semhashmap;

		Nohttpd2App();
		~Nohttpd2App();
		
		bool OnInit();
		int OnExit();
		void OnQueryEndSession(wxCloseEvent &event);
		void OnEndSession(wxCloseEvent &event);

	private:
		DECLARE_EVENT_TABLE()
		class wxSingleInstanceChecker *m_instchecker;
};

#endif
