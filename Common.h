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

#ifndef COMMON_H
#define COMMON_H

/**
winsock2.h *must* be included before windows.h when compiling under Windows, or
you'll get lots of redefinition errors.
And because wxWidgets includes windows.h, better include it right here.

Also, sockets possess different signedness on Windows, define a generic type
here.
While at it, provide a general function call for closing sockets. Windows
once again does it different to all others.
*/
#ifdef __WIN95__
	#include <winsock2.h>
	#include <ws2tcpip.h> 		// for socklen_t
	#define _UNISOCKET SOCKET	// unsigned int
	#define _UNICLOSE(s) closesocket(s)
#else
	#define _UNISOCKET int
	#define _UNICLOSE(s) close(s)
#endif

#include <wx/string.h>

class nohttpd2{

public:

	static void logMessage(const wxChar *msg);
 	static void logMessageDbg(const wxChar *msg,const wxChar *file,int line);
	static void logMessage(const wxString &str);
 	static void logMessageDbg(const wxString &str,const wxChar *file,int line);

	static void logVerbose(const wxChar *msg);
	static void logVerboseDbg(const wxChar *msg,const wxChar* file,int line);
	static void logVerbose(const wxString &str);
 	static void logVerboseDbg(const wxString &str,const wxChar* file,int line);
};

#ifdef __DEBUG__
	#define LOGMESSAGE(msg)\
	nohttpd2::logMessageDbg(msg,__TFILE__,__LINE__);

	#define LOGVERBOSE(msg)\
	nohttpd2::logVerboseDbg(msg,__TFILE__,__LINE__);
#else
	#define LOGMESSAGE(msg)\
	nohttpd2::logMessage(msg);

	#define LOGVERBOSE(msg)\
	nohttpd2::logVerbose(msg);
#endif

#endif
