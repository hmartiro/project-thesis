// Demo_WinDLL.h : Haupt-Header-Datei f�r die Anwendung DEMO_WINDLL
//

#if !defined(AFX_DEMO_WINDLL_H__F2A0E868_5C76_47F9_8376_8A8778213ACC__INCLUDED_)
#define AFX_DEMO_WINDLL_H__F2A0E868_5C76_47F9_8376_8A8778213ACC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CDemo_WinDLLApp:
// Siehe Demo_WinDLL.cpp f�r die Implementierung dieser Klasse
//

class CDemo_WinDLLApp : public CWinApp
{
public:
	CDemo_WinDLLApp();

// �berladungen
	// Vom Klassenassistenten generierte �berladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CDemo_WinDLLApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CDemo_WinDLLApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingef�gt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VER�NDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_DEMO_WINDLL_H__F2A0E868_5C76_47F9_8376_8A8778213ACC__INCLUDED_)
