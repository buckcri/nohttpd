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

#include "ReceiverThread.h"
#include "Nohttpd2Dlg.h"
#include "Nohttpd2App.h"
#include <wx/config.h>
#include <wx/tokenzr.h>

#ifndef __WIN95__
#include <errno.h>
#endif

// Declaration of custom events caught by class Nohttpd2Dlg
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_LOGMSG, wxID_HIGHEST+1)
    DECLARE_EVENT_TYPE(wxEVT_SHUTDOWN, wxID_HIGHEST+2)
    DECLARE_EVENT_TYPE(wxEVT_SHOWMAIN, wxID_HIGHEST+3)
    DECLARE_EVENT_TYPE(wxEVT_SETHITS, wxID_HIGHEST+4)
    DECLARE_EVENT_TYPE(wxEVT_LOGVERBOSE, wxID_HIGHEST+5)
    DECLARE_EVENT_TYPE(wxEVT_HIDEDLG, wxID_HIGHEST+6)
    DECLARE_EVENT_TYPE(wxEVT_ADDTRAY, wxID_HIGHEST+7)
    DECLARE_EVENT_TYPE(wxEVT_REMTRAY, wxID_HIGHEST+8)
END_DECLARE_EVENT_TYPES()

// Declaration of Nohttpd2App for ::wxGetApp()
DECLARE_APP(Nohttpd2App)

// Declaration of static members.
long ReceiverThread::s_sessionhits;
StatItemHashMap *ReceiverThread::s_sihm;
bool ReceiverThread::s_logqueries,ReceiverThread::s_statistics,ReceiverThread::s_fullqueries;

long StatItem::s_histlength;

// Implementation of StatItem follows
#include <wx/listimpl.cpp>

WX_DEFINE_LIST(sdilist);

StatItem::StatItem(){

	m_accesses=0;

	// Delete StatDetailItem objects upon removal from the wxList
	m_history.DeleteContents(true);
}

StatItem::~StatItem(){

	m_history.Clear();
}

void StatItem::add(const wxString &url,const wxString &referrer,const wxString &useragent){

	StatDetailItem *sdi=new StatDetailItem();

	m_lastaccess=wxDateTime::Now();
	m_accesses++;

	sdi->m_accesstime=m_lastaccess;
	sdi->m_referrer=referrer;
	sdi->m_url=url;
	sdi->m_useragent=useragent;

	if (s_histlength==0){
		m_history.Clear();
	} else {
		while ((long)m_history.GetCount()>=s_histlength){
			m_history.DeleteNode(m_history.GetFirst());
		}
		m_history.Append(sdi);
	}
}

// Implementation of ReceiverThread follows
ReceiverThread::ReceiverThread(Nohttpd2Dlg *maindlg){

	setSessionHits(0);
	m_dlg=maindlg;
	m_runreceiver=true;
	#ifdef __WIN95__
	m_wsainited=false;
	#endif
	m_sock_serv=0;
	m_closing=false;
	s_sihm=new StatItemHashMap();
}

ReceiverThread::~ReceiverThread(){}

void *ReceiverThread::Entry(){

	LOGVERBOSE(_T("ReceiverThread::Entry()"));

	/**
	Starting of the receiver was successful if we ended up here, so the
	main dialog can now be hidden.
	*/
	wxCommandEvent event(wxEVT_HIDEDLG);
	::wxPostEvent(m_dlg,event);

	receiverLoop();
	return NULL;
}

void ReceiverThread::OnExit(){

	LOGVERBOSE(_T("ReceiverThread::OnExit()"));

	#ifdef __WIN95__
	if (m_wsainited){
		WSACleanup();
	}
	#endif
	if (m_sock_serv!=0){
		_UNICLOSE(m_sock_serv);
	}

	StatItemHashMap::iterator it;
	waitOnSem(::wxGetApp().semhashmap);

	for (it=s_sihm->begin();it!=s_sihm->end();it++){
		delete it->second;
	}
	delete s_sihm;
	// Don't release the lock on the hashmap, just to be safe.

	if (m_closing){
		/**
		If nohttpd is being closed and not shut down by the OS,
		post exit event to main dialog.
		*/
		wxCommandEvent event(wxEVT_SHUTDOWN);
		::wxPostEvent(m_dlg,event);
	}
}

bool ReceiverThread::initNetwork(){

	#ifndef __WIN95__
	int errnocp;
	#endif

	// Initialize Winsock
	#ifdef __WIN95__
	WORD wsaversion;
	WSADATA wsadata;
	int ret;

	wsaversion=MAKEWORD(2,2);
	ret=WSAStartup(wsaversion,&wsadata);
	if (ret!=0){
    	LOGMESSAGE(wxString::Format(_T("Error: Unable to initialize Winsock2.\r\nErrorcode: %d"),ret));
	    return false;
	}

	if (LOBYTE(wsadata.wVersion )!= 2||HIBYTE(wsadata.wVersion)!=2){
    	LOGMESSAGE(_T("Error: Unable to initialize Winsock2 (Version 2.2 required)."));
		WSACleanup();
	    return false;
	}

	m_wsainited=true;

	LOGVERBOSE(_T("Winsock2 initialized."));
	#endif

	m_sock_serv=socket(AF_INET,SOCK_STREAM,0);
	#ifdef __WIN95__
	if (m_sock_serv==INVALID_SOCKET){
	#else
	if (m_sock_serv<0){
	#endif
		LOGMESSAGE(_T("Error: Unable to create server socket."));
		#ifdef __WIN95__
		LOGMESSAGE(wxString::Format(_T("Errorcode: %d"),WSAGetLastError()));
		#endif
		return false;
	}

	LOGMESSAGE(_T("Server socket created."));

	// Try to reuse socket, circumventing TIME_WAIT states.
	#ifdef __WIN95__
	// What's it with Windows and weird data types?
	BOOL activate=TRUE;
	if (setsockopt(m_sock_serv,SOL_SOCKET,SO_REUSEADDR,(char*)&activate,sizeof(activate)!=0)){
	#else
	int activate=1;
	if (setsockopt(m_sock_serv,SOL_SOCKET,SO_REUSEADDR,&activate,sizeof(activate)!=0)){
	#endif
		LOGVERBOSE(_T("Warning: setsockopt() failed."));
		#ifdef __WIN95__
		LOGVERBOSE(wxString::Format(_T("Errorcode: %d"),WSAGetLastError()));
		#else
		errnocp=errno;
		LOGVERBOSE(_T("Reason: ")+wxString(strerror(errnocp),*wxConvCurrent));
		LOGVERBOSE(wxString::Format(_T("(Errno: %d)"),errnocp));
		#endif
	}

	memset((char*)&m_addr_srv,0,sizeof(m_addr_srv));
	m_addr_cli_size=sizeof(m_addr_cli);
	memset((char*)&m_addr_cli,0,m_addr_cli_size);
	m_addr_srv.sin_family=AF_INET;
	m_addr_srv.sin_addr.s_addr=htonl(INADDR_ANY);
	m_addr_srv.sin_port=htons(80);

	if (bind(m_sock_serv,(sockaddr*)&m_addr_srv,sizeof(m_addr_srv))==-1){
		LOGMESSAGE(_T("Error: Unable to bind to port 80. Is a webserver already running?\nYou also need Computer Administrator/root privileges to run nohttpd."));
		#ifdef __WIN95__
		LOGMESSAGE(wxString::Format(_T("Errorcode: %d"),WSAGetLastError()));
		#endif

		_UNICLOSE(m_sock_serv);
		return false;
	}

	LOGMESSAGE(_T("Server socket bound to port 80."));

	if (listen(m_sock_serv,SOMAXCONN)==-1){
		LOGMESSAGE(_T("Error: Unable to listen on socket."));
		#ifdef __WIN95__
		LOGMESSAGE(wxString::Format(_T("Errorcode: %d"),WSAGetLastError()));
		#endif

		_UNICLOSE(m_sock_serv);
		return false;
	}

	return true;
}

void ReceiverThread::receiverLoop(){

	const char retstr[]="HTTP/1.1 404 Not found\n";	// nothing to see here :)
	_UNISOCKET sock_cli;
	const int BUFSIZE=65536;
	char *recvbuf;
	int recvsize;	// # of bytes received in request
	wxStringTokenizer tokenizer;

	recvbuf=new char[BUFSIZE+2];

	while(m_runreceiver){

		wxCommandEvent event(wxEVT_SETHITS);
		::wxPostEvent(m_dlg,event);

		sock_cli=accept(m_sock_serv,(sockaddr*)&m_addr_cli,&m_addr_cli_size);

		if (!m_runreceiver){						// are we shutting down?
			break;
		}

		#ifdef __WXMSW__
		if (sock_cli==INVALID_SOCKET){
		#else
		if (sock_cli<0){
		#endif
			LOGMESSAGE(_T("Warning: Invalid client or server socket.\r\n"));
			#ifdef __WXMSW__
			LOGMESSAGE(wxString::Format(_T("Errorcode: %d\r\n"),WSAGetLastError()));
			#endif
			break;
		}

		recvsize=recvfrom(sock_cli,recvbuf,BUFSIZE,0,(sockaddr*)&m_addr_cli,&m_addr_cli_size);
		if (recvsize==-1){
			LOGMESSAGE(_T("Warning: recvfrom() failed.\r\n"));
			#ifdef __WXMSW__
			LOGMESSAGE(wxString::Format(_T("Errorcode: %d\r\n"),WSAGetLastError()));
			#endif
		} else {
			if (sendto(sock_cli,retstr,sizeof(retstr),0,(sockaddr*)&m_addr_cli,sizeof(m_addr_cli))==-1){
				LOGMESSAGE(_T("Warning: Unable to send 404.\r\n"));
				#ifdef __WXMSW__
				LOGMESSAGE(wxString::Format(_T("Errorcode: %d\r\n"),WSAGetLastError()));
				#endif
			}
		}

		_UNICLOSE(sock_cli);

		if (recvsize!=-1 && (s_logqueries || s_statistics)){

			// Terminate buffer to avoid old query segments when parsing.
			// Please note this won't overflow, because recvbuf is BUFSIZE+2 chars large.
			recvbuf[recvsize]=0;
			recvbuf[recvsize+1]=0;

			wxString convquery(recvbuf,*wxConvCurrent);
			wxString output;

			if (s_logqueries && s_fullqueries){
				output << _T("Full query:\n") << convquery;
				LOGMESSAGE(output);
			}
			if ((s_logqueries && !s_fullqueries) || s_statistics){

				wxString token,host,url,referrer,useragent;

				tokenizer.SetString(convquery,_T("\r\n"));

				// Todo: This could be speeded up with a bitmask to terminate
				// when all 4 tokens were found.
				while (tokenizer.HasMoreTokens()){
					token=tokenizer.GetNextToken();
					if (token.StartsWith(_T("Host: "),&host)){
						continue;
					}
					if (token.StartsWith(_T("GET "),&url)){
						continue;
					}
					if (token.StartsWith(_T("Referer: "),&referrer)){
						continue;
					}
					if (token.StartsWith(_T("User-Agent: "),&useragent)){
						continue;
					}
				}
				// Strip " HTTP/..." from URL.
				url=url.BeforeFirst(_T(' '));

				if (s_logqueries && !s_fullqueries){
					output	<< _T("Query summary:\n")
						<< _T("Host: ") << host << _T("\n")
						<< _T("URL: ") << url << _T("\n")
						<< _T("User-Agent: ") << useragent << _T("\n")
						<< _T("Referrer: ") << referrer;
					LOGMESSAGE(output);
				}

				if (s_statistics && !host.IsEmpty()){

					waitOnSem(::wxGetApp().semhashmap);

					if (s_sihm->find(host)==s_sihm->end()){
						(*s_sihm)[host]=new StatItem();
						LOGVERBOSE(_T("New host added to statistics:"));
					} else {
						LOGVERBOSE(_T("Updated statistics. Host was:"));
					}
					(*s_sihm)[host]->add(url,referrer,useragent);
					::wxGetApp().semhashmap.Post();

					LOGVERBOSE(host);
				}
			}
		}

		// Don't use setSessionHits(getSessionHits()+1) here,
		// because the user might have cleared the session hits
		// after getSessionHits(), but still before
		// setSessionHits() gets called.
		waitOnSem(::wxGetApp().semhits);
		s_sessionhits++;
		::wxGetApp().semhits.Post();
	}

	delete[] recvbuf;
}

void ReceiverThread::killNetwork(){

	LOGVERBOSE(_T("killNetwork() called."));
	m_runreceiver=false;
	shutdown(m_sock_serv,2);
	_UNICLOSE(m_sock_serv);
	m_sock_serv=0;
}

long ReceiverThread::getSessionHits(){

	return s_sessionhits;
}

void ReceiverThread::setSessionHits(long value){

	waitOnSem(::wxGetApp().semhits);
	s_sessionhits=value;
	::wxGetApp().semhits.Post();
}

void ReceiverThread::waitOnSem(wxSemaphore &sem){

	wxSemaError se;

 	do {
		LOGVERBOSE(_T("Waiting on semaphore."));
		se=sem.WaitTimeout(100);
		if (se!=wxSEMA_TIMEOUT && se!=wxSEMA_NO_ERROR){
			if (se==wxSEMA_INVALID){
				LOGVERBOSE(_T("wxSEMA_INVALID!"));
			}
			if (se==wxSEMA_MISC_ERROR){
				LOGVERBOSE(_T("wxSEMA_MISC_ERROR!"));
			}
			break;
		}
	} while (se!=wxSEMA_NO_ERROR);
	LOGVERBOSE(_T("Done waiting on semaphore."));
}
