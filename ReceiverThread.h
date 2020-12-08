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

#ifndef __RECEIVERTHREAD_HPP_
#define __RECEIVERTHREAD_HPP_

#include "Common.h"

#ifndef __WIN95__
	#include <stdio.h>
	#include <unistd.h>	// gethostname()
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/wait.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <memory.h>
	#include <arpa/inet.h>
#endif

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

#include <wx/hashmap.h>
#include <wx/datetime.h>

class StatDetailItem{

public:

	wxString m_url,m_referrer,m_useragent;
	wxDateTime m_accesstime;
};

WX_DECLARE_LIST(StatDetailItem,sdilist);

class StatItem{

public:

	StatItem();
	~StatItem();

	static long s_histlength;

	void add(const wxString &url,const wxString &referrer,const wxString &useragent);

	unsigned long m_accesses;
	wxDateTime m_lastaccess;
	sdilist m_history;	// a wxList of StatDetailItem objects (=the history)

};

WX_DECLARE_STRING_HASH_MAP(StatItem*,StatItemHashMap);

class ReceiverThread:public wxThread{

public:
	#ifdef __WIN95__
	bool m_wsainited;
	#endif
	bool m_closing;

	/** These variables cache the respective configuration settings to avoid
	reading the registry/config file each time the receiver thread receives a query.
	*/
	static bool s_logqueries,s_statistics,s_fullqueries;

	ReceiverThread(class Nohttpd2Dlg *maindlg);
	virtual ~ReceiverThread();
	virtual void *Entry();
	virtual void OnExit();

	bool initNetwork();
	void killNetwork();

	static long getSessionHits();
	static void setSessionHits(long value);
	static void waitOnSem(class wxSemaphore &sem);

	static StatItemHashMap *s_sihm;

private:
	sockaddr_in m_addr_srv;
	sockaddr_in m_addr_cli;
	socklen_t m_addr_cli_size;
	class Nohttpd2Dlg *m_dlg;
	_UNISOCKET m_sock_serv;
	bool m_runreceiver;
	static long s_sessionhits;

	void receiverLoop();
};

#endif
