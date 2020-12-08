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

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/snglinst.h>
#include <wx/config.h>

#include "Nohttpd2App.h"
#include "Nohttpd2Dlg.h"
#include "ReceiverThread.h"

IMPLEMENT_APP(Nohttpd2App)

BEGIN_EVENT_TABLE(Nohttpd2App,wxApp)
    EVT_QUERY_END_SESSION(Nohttpd2App::OnQueryEndSession)
    EVT_END_SESSION(Nohttpd2App::OnEndSession)
END_EVENT_TABLE()

Nohttpd2App::Nohttpd2App():semhits(1),semhashmap(1){

	m_instchecker=NULL;
	m_maindlg=NULL;
	m_dontlog=false;
}

Nohttpd2App::~Nohttpd2App(){
}

bool Nohttpd2App::OnInit(){

	wxConfigBase *cfg;	// deleted in OnExit()
	long dummylong;
	bool dummyboolean;

	SetVendorName(_T("nohttpd2"));
	SetAppName(_T("nohttpd2"));
	cfg=wxConfigBase::Get();

	// todo: experiment with wxLogNull and Linux to make it work.
	/**
	I'm having troubles w/ wxSingleInstanceChecker on Linux;
	when logging off the lockfile won't get deleted.
	Therefore single instance checking is Windows only right now.
	*/
	#ifdef __WIN95__
	m_instchecker=new wxSingleInstanceChecker(_T("nohttpd2global"));
	if (m_instchecker->IsAnotherRunning()){
	    wxMessageDialog(NULL,_T("There's already an instance of nohttpd running."),_T("nohttpd: Error"),wxOK|wxICON_ERROR).ShowModal();
		return false;
	}
	#endif

	/**
	Initialize the config. If a key isn't present in the config, force writing a default
 	value. This way the defaults must not be supplied later when reading a key.
	*/
	cfg->SetRecordDefaults(true);
	cfg->Read(_T("notrayhide"),&dummyboolean,false);
	cfg->Read(_T("closetotray"),&dummyboolean,true);
	cfg->Read(_T("savehits"),&dummyboolean,true);
	cfg->Read(_T("logqueries"),&dummyboolean,false);
	cfg->Read(_T("verboselog"),&dummyboolean,false);
	cfg->Read(_T("importedconf"),&dummyboolean,false);
	cfg->Read(_T("hits"),&dummylong,0l);
	// New in 2.1.0
	cfg->Read(_T("fullqueries"),&dummyboolean,false);
	cfg->Read(_T("statistics"),&dummyboolean,false);
	cfg->Read(_T("historylen"),&dummylong,10l);

	// Initialize cached settings.
	cfg->Read(_T("logqueries"),&ReceiverThread::s_logqueries);
	cfg->Read(_T("statistics"),&ReceiverThread::s_statistics);
	cfg->Read(_T("fullqueries"),&ReceiverThread::s_fullqueries);

	m_maindlg=new Nohttpd2Dlg(NULL);
	SetTopWindow(m_maindlg);
	m_maindlg->Nohttpd2DlgInitDialog();
	m_maindlg->Show(true);

	return true;
}

int Nohttpd2App::OnExit(){

	delete wxConfigBase::Set((wxConfigBase *)NULL);

	if (this->m_instchecker!=NULL){
		delete m_instchecker;
		m_instchecker=NULL;
	}

	return wxApp::OnExit();
}

void Nohttpd2App::OnQueryEndSession(wxCloseEvent &event){

	bool dummyboolean;

	if (m_maindlg!=NULL){
		m_maindlg->m_shutdown=true;				// Set shutdown mode.
		m_maindlg->m_recvthread->killNetwork();			// Close socket and thread.
		wxConfigBase::Get()->Read(_T("savehits"),&dummyboolean);
		if (dummyboolean){
			wxConfigBase::Get()->Write(_T("hits"),m_maindlg->m_allhits);
			wxConfigBase::Get()->Flush();
		}
		m_maindlg->Destroy();
		m_maindlg=NULL;
	}
	event.Skip();
}

void Nohttpd2App::OnEndSession(wxCloseEvent &event){

	this->OnQueryEndSession(event);
}
