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

#include "Common.h"

#include <wx/string.h>
#include <wx/event.h>
#include <wx/app.h>

#include "Nohttpd2App.h"
#include "Nohttpd2Dlg.h"

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_LOGMSG, wxID_HIGHEST+1)
    DECLARE_EVENT_TYPE(wxEVT_SHUTDOWN, wxID_HIGHEST+2)
    DECLARE_EVENT_TYPE(wxEVT_SHOWMAIN, wxID_HIGHEST+3)
    DECLARE_EVENT_TYPE(wxEVT_SETHITS, wxID_HIGHEST+4)
    DECLARE_EVENT_TYPE(wxEVT_LOGVERBOSE, wxID_HIGHEST+5)
END_DECLARE_EVENT_TYPES()

// Declaration of Nohttpd2App for ::wxGetApp()
DECLARE_APP(Nohttpd2App)

void nohttpd2::logMessage(const wxChar *msg){

	Nohttpd2Dlg *dlg;

	dlg=::wxGetApp().m_maindlg;
	if (dlg==NULL || ::wxGetApp().m_dontlog){
		return;
	}	

	wxCommandEvent event(wxEVT_LOGMSG);
	event.SetString(msg);
	::wxPostEvent(dlg,event);
}

void nohttpd2::logMessage(const wxString &str){
	
	nohttpd2::logMessage(str.c_str());
}

void nohttpd2::logMessageDbg(const wxChar *msg,const wxChar *file,int line){
	
	nohttpd2::logMessage(wxString::Format(_T("[%s:%i] %s"),file,line,msg));
}

void nohttpd2::logMessageDbg(const wxString &str,const wxChar *file,int line){
	
	nohttpd2::logMessage(wxString::Format(_T("[%s:%i] %s"),file,line,str.c_str()));
}

void nohttpd2::logVerbose(const wxChar *msg){

	Nohttpd2Dlg *dlg;

	dlg=::wxGetApp().m_maindlg;
	if (dlg==NULL || ::wxGetApp().m_dontlog){
		return;
	}	

	wxCommandEvent event(wxEVT_LOGVERBOSE);
	event.SetString(msg);
	::wxPostEvent(dlg,event);
}

void nohttpd2::logVerbose(const wxString &str){
	
	nohttpd2::logVerbose(str.c_str());
}

void nohttpd2::logVerboseDbg(const wxChar *msg,const wxChar *file,int line){
	
	nohttpd2::logVerbose(wxString::Format(_T("[%s:%i] %s"),file,line,msg));
}

void nohttpd2::logVerboseDbg(const wxString &str,const wxChar *file,int line){
	
	nohttpd2::logVerbose(wxString::Format(_T("[%s:%i] %s"),file,line,str.c_str()));
}
