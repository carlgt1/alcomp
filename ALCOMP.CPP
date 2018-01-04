// Aleatoric Composer - (c) Copyright 1993 by Carl M. Christensen
// CIS 751
// MSCS Project for Temple University -- Advisor:  Paul Wolfgang

/*
	This program will use random or conditional probabilities to 
	produce a MIDI file which can be played using the Windows Media Player
	or imported into a variety of sequencer programs.

	The MIDI Mapper and MCI Sequencer must be installed from
	the Control Panel	in order for the program to play generated files.
	Otherwise, the user has to play the generated MIDI files `by hand'
*/
#define WIN31
#define STRICT

#include <windows.h>
#include <iostream.h>
#include <stdio.h>
#include <owl.h>
#include <owlrc.h>
#include <bwcc.h>
#include <fstream.h>
#include <iomanip.h>
#include <dir.h>
#include <filedial.h>
#include <math.h>
#include <mmsystem.h>
#include <applicat.h>

// more OWL libs ( Edit, dialog, window, button, group box, radio button
//                 static text, scrollbar, listbox, combo box, whew!!! )
#include <edit.h>
#include <dialog.h>
#include <bwindow.h>
#include <bbutton.h>
#include <bgrpbox.h>
#include <bradio.h>
#include <bchkbox.h>
#include <bstatic.h>
#include <scrollba.h>
#include <listbox.h>
#include <combobox.h>

// BIDS Template for dynamic, sorted, and normal arrays
#include <vectimp.h>

// my include files and resource dialog stuff
#include "alcomp.h"
#include "patches.h"
#include "probabil.cpp"
#include "dabout.cpp"
#include "drhythm.cpp"
#include "dsong.cpp"
#include "midifile.cpp"
#include "dtraced.cpp"
#include "notedit.cpp"
#include "drandom.cpp"
#include "drhycond.cpp"
#include "dsetcond.cpp"
#include "dcondit.cpp"
#include "dtracvw.cpp"

_CLASSDEF(TAlCompWindow)
class TAlCompWindow : public TBWindow
{
public:
	TAlCompWindow (PTWindowsObject pwParent, LPSTR lpszTitle);
	virtual LPSTR GetClassName () { return "TAlCompWindow"; }
	virtual void GetWindowClass (WNDCLASS& wc);
	// took out (RTMessage Msg)
	virtual void FileNew   () = [CM_FIRST + MN_FILENEW];
	virtual void FileOpen  () = [CM_FIRST + MN_FILEOPEN];
	virtual void FileSave()   = [CM_FIRST + MN_FILESAVE];
	virtual void FileSaveAs() = [CM_FIRST + MN_FILESAVEAS];
	virtual void FileExit  () = [CM_FIRST + MN_FILEEXIT];
	virtual void EditRandom () = [CM_FIRST + MN_PROBABILITY];
	virtual void EditConditional () = [CM_FIRST + MN_CONDITIONAL];
	virtual void HelpContents () = [CM_FIRST + MN_HELPINDEX];
	virtual void HelpAbout () = [CM_FIRST + MN_ABOUT];
	virtual void SaveFile();
	virtual void OpenFile();
	BOOL LetEmSave();  // let's user save a file
};

// Constructor Function for Window; set's up application
TAlCompWindow::TAlCompWindow (PTWindowsObject pwParent, LPSTR lpszTitle)
  : TBWindow (pwParent, lpszTitle)
{
	AssignMenu("MN_ALCOMP");
	Attr.X = 20;
	Attr.Y = 20;
	Attr.W = 500;
	Attr.H = 300;
	if (! WinHelp(this->HWindow, "alcomp.hlp", HELP_SETCONTENTS, CONTENTS) )
		BWCCMessageBox(this->HWindow, "Help File Not Found", "Error!", MB_OK|MB_ICONEXCLAMATION);
}

void TAlCompWindow::FileNew ()
{
	if ( LetEmSave() )  {
		_fmemset( lpszFile, 0, 100);
		dTrack.flush(TRUE);  // flush out (erase) all the tracks
		bModified = FALSE;  // reset modified flag
		nProbType = UNSELECTED;
		// Don't forget to disable save now!!
		EnableMenuItem( GetMenu(HWindow) , MN_FILESAVEAS, MF_GRAYED );
		EnableMenuItem( GetMenu(HWindow) , MN_FILESAVE, MF_GRAYED );
		EnableMenuItem( GetMenu(HWindow) , MN_CONDITIONAL, MF_ENABLED );
		EnableMenuItem( GetMenu(HWindow) , MN_PROBABILITY, MF_ENABLED );
		// change the title
		SetCaption("Aleatoric Composer");
	}
}

void TAlCompWindow::FileOpen ()
{
	if ( LetEmSave() )  {
		bModified = FALSE;  // reset modified flag
		char tempFile[100];
		strcpy(tempFile, "*.CMC");
		if ( GetApplication()->ExecDialog(new TFileDialog(this, SD_FILEOPEN, tempFile )) == IDOK )  {
			if ( CanClose() ) {
				dTrack.flush(TRUE);  // flush out (erase) all the tracks
				strcpy( CCH lpszFile, CheckExtension( tempFile, ".cmc" ));
				OpenFile();
				// change the title
				sprintf(tempFile, "Aleatoric Composer - [%s]", CCH lpszFile);
				SetCaption(tempFile);
				EnableMenuItem( GetMenu(HWindow) , MN_FILESAVEAS, MF_ENABLED);
				EnableMenuItem( GetMenu(HWindow) , MN_FILESAVE, MF_ENABLED);

				EnableMenuItem( GetMenu(HWindow) , MN_CONDITIONAL,   nProbType==CONDITIONAL ? MF_ENABLED : MF_GRAYED );
				EnableMenuItem( GetMenu(HWindow) , MN_PROBABILITY,   nProbType==RANDOM      ? MF_ENABLED : MF_GRAYED );
			}
		}
	}
}

void TAlCompWindow::FileSave ()
{
	char temptitle[100];
	bModified = FALSE;  // reset modified flag
	SaveFile();
	sprintf(temptitle, "Aleatoric Composer - [%s]", CCH lpszFile);
	SetCaption(temptitle);
}

void TAlCompWindow::FileSaveAs ()
{
	char tempFile[100];
	strcpy(tempFile, "*.CMC");   // didn't save yet, no name to plug in
	if ( GetApplication()->ExecDialog(new TFileDialog(this, SD_FILESAVE, tempFile)) == IDOK )  {
		if (CanClose()) {
			strcpy( CCH lpszFile, CheckExtension( tempFile, ".cmc" ));
			SaveFile();
			// change the title
			sprintf(tempFile, "Aleatoric Composer - [%s]", CCH lpszFile);
			SetCaption(tempFile);
			bModified = FALSE;  // reset modified flag
		}
	}
}

BOOL TAlCompWindow::LetEmSave()
{
	BOOL wellwhat = TRUE;
	if (bModified) {  // was modified
		switch( BWCCMessageBox
            (HWindow, "You have modified tracks.\n\nDo you want to save?", "File Save", MB_YESNOCANCEL | MB_ICONQUESTION) )  {
			case (IDYES):  {
				if ( strlen(CCH lpszFile) )   // then we have a file name
					FileSave();
				else
					FileSaveAs();
				break;
			}
			case (IDCANCEL):  wellwhat = FALSE;
		}
	}
	return wellwhat;
}

void TAlCompWindow::SaveFile()
{
	ofstream out(CCH lpszFile, ios::out | ios::binary | ios::trunc );

	if ( out.bad() )
		BWCCMessageBox( HWindow, "Cannot write to file", "Error!", MB_OK|MB_ICONEXCLAMATION);

	HCURSOR hcOld = SetCursor(hcBusy);
	// first say type of track saved
	out << (nProbType==CONDITIONAL ? CONDITIONAL_FLAG : RANDOM_FLAG );

	int i, j;

	// write out track settings
	for (j = 0; j < dTrack.count(); j++)  {

		out.put(TRACK_FLAG);
		out.write( (unsigned char*) dTrack[j].Description,      sizeof(dTrack[j].Description) );
		out.write( (unsigned char*) &(dTrack[j].StartMeasure),  sizeof( dTrack[j].StartMeasure ));
		out.write( (unsigned char*) &(dTrack[j].EndMeasure),    sizeof( dTrack[j].EndMeasure ));
		out.write( (unsigned char*) &(dTrack[j].Tempo     ),    sizeof( dTrack[j].Tempo ));
		out.put(dTrack[j].Patch);
		out.put(dTrack[j].Volume);
		out.put(dTrack[j].Channel);
		out.put(dTrack[j].TimeSigNum);
		out.put(dTrack[j].TimeSigDen);
		out.put(dTrack[j].Octave);
		out.put(NOTES_FLAG) ;

		for (i = 0; i < dTrack[j].Note.count(); i++)  {
			out.put(dTrack[j].Note[i].From);
			out.put(dTrack[j].Note[i].To);
			out.put(dTrack[j].Note[i].Prob);
          }

		out.put(RHYTHM_FLAG) ;

		for (i = 0; i < dTrack[j].Rhythm.count(); i++)  {
			out.put(dTrack[j].Rhythm[i].From);
			out.put(dTrack[j].Rhythm[i].To);
			out.put(dTrack[j].Rhythm[i].Prob);
		}
	}

	out.flush();
	out.close();

	EnableMenuItem( GetMenu(HWindow) , MN_FILESAVE, MF_ENABLED);
	SetCursor(hcOld);
}

void TAlCompWindow::OpenFile()
{
	HCURSOR hcOld = SetCursor(hcBusy);
	BYTE dummy;
	ifstream in( CCH lpszFile, ios::in | ios::binary );

	if ( in.bad() )  {
		BWCCMessageBox( HWindow, "File cannot be opened", "Error!", MB_OK|MB_ICONEXCLAMATION);
		SetCursor(hcOld);
		return;
   }

	// first byte is if Conditional or Random
	in.get(dummy);
	nProbType = (dummy == CONDITIONAL_FLAG) ? CONDITIONAL : RANDOM;

	StrNote tNR;  // temporary note and rhythm structure

	in.get(dummy);  // get next byte, should be TRACK ID

	// now read whole file in
	while ( ! in.eof() && in.good() && dummy == TRACK_FLAG )  {
		tempTrack = nullTrack;  // reset tempTrack to 0's

		// now read in all of the track info
		in.read( (unsigned char*) tempTrack.Description,   sizeof(tempTrack.Description) );
		in.read( (unsigned char*) &tempTrack.StartMeasure, sizeof(tempTrack.StartMeasure) );
		in.read( (unsigned char*) &tempTrack.EndMeasure,   sizeof(tempTrack.EndMeasure) );
		in.read( (unsigned char*) &tempTrack.Tempo,        sizeof(tempTrack.Tempo) );

		in.get( tempTrack.Patch );
		in.get( tempTrack.Volume );
		in.get( tempTrack.Channel);
		in.get( tempTrack.TimeSigNum );
		in.get( tempTrack.TimeSigDen );
		in.get( tempTrack.Octave );

		in.get(  dummy );  // next byte better be NOTE_FLAG
		if ( dummy == NOTES_FLAG)  { // check that it was
			in.get( dummy );  // now this dummy should be From, but need to read ahead

         // Get all of the Note Probabilities
			while (! in.eof() && dummy != RHYTHM_FLAG)  { // so far so good
				tNR.From = dummy;  // dummy was a legit value of From if here
				in.get(tNR.To);
				in.get(tNR.Prob);
				in.get(dummy);
				tempTrack.Note.add(tNR);  // add to sorted array structure
			}

			in.get( dummy );  // now this dummy should be From, but need to read ahead
			// Get all of the Rhythm probabilities
			while (! in.eof() && dummy != TRACK_FLAG)  { // so far so good
				tNR.From = dummy;  // dummy was a legit value of From if here
				in.get(tNR.To);
				in.get(tNR.Prob);
				in.get(dummy);
				tempTrack.Rhythm.add(tNR);  // add to sorted array structure
			}  // when this while ends, we hit NOTE_FLAG, so loop
		}
		// now add our tempTrack values to the real dTrack
          dTrack.add( tempTrack );
	}
	in.close();

	if ( in.bad() )
		BWCCMessageBox( HWindow, "Problem encountered while reading file", "Error!", MB_OK|MB_ICONEXCLAMATION);

	SetCursor(hcOld);
}

void TAlCompWindow::FileExit ()
{  // bye bye !!  ya'll come back now, y'hear!
	if ( LetEmSave() )  {
		dTrack.flush(TRUE);
		WinHelp(this->HWindow, "alcomp.hlp", HELP_QUIT, 0L);
		CloseWindow ();
		delete lpszFile;
		delete pszPatch;
	}
}

void TAlCompWindow::EditRandom ()
{
	nProbType = RANDOM;
	GetModule()->ExecDialog(new TDlgTrackView(this, DG_TRACKVIEW));
}

void TAlCompWindow::EditConditional ()
{
	nProbType = CONDITIONAL;
	GetModule()->ExecDialog(new TDlgTrackView(this, DG_TRACKVIEW));
}

void TAlCompWindow::HelpContents ()
{
	if (! WinHelp(this->HWindow, "alcomp.hlp", HELP_CONTENTS, 0L) )
		BWCCMessageBox(this->HWindow, "Help File Not Found", "Error!", MB_OK|MB_ICONEXCLAMATION);
}

void TAlCompWindow::HelpAbout ()
{
	GetModule()->ExecDialog(new TDlgAbout(this, DG_ABOUT));
}

// GetWindowClass member  --  set's up menus and icon
void TAlCompWindow::GetWindowClass (WNDCLASS &wc)
{
	TBWindow::GetWindowClass(wc);
	wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(ICON_1) );
	if (wc.hIcon == NULL) {
		BWCCMessageBox(HWindow, "Bad Icon Load", "Error!", MB_ICONSTOP | MB_OK);
		wc.hIcon = LoadIcon(0, "IDI_APPLICATION");
	}
	hcBusy = LoadCursor( wc.hInstance, MAKEINTRESOURCE(CU_BUSY) );
}

// Define a class derived from TApplication
_CLASSDEF(TAlCompApp)
class TAlCompApp :public TApplication
{
public:
	TAlCompApp(LPSTR lpszName, HINSTANCE hInstance, HINSTANCE hPrevInstance,
				 LPSTR lpszCmdLine, int nCmdShow)
		: TApplication(lpszName, hInstance, hPrevInstance, lpszCmdLine, nCmdShow) {}
	virtual void InitInstance();
	virtual void InitMainWindow ();
};

// set up the main window
void TAlCompApp::InitMainWindow()
{
	BWCCGetVersion();
	MainWindow = new TAlCompWindow(NULL, "Aleatoric Composer");
}

void TAlCompApp::InitInstance()
{
	TApplication::InitInstance();
	HAccTable = LoadAccelerators(hInstance, "AC_ALCOMP");
}

// Main Program
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	TAlCompApp AlCompApp ("Aleatoric Composer", hInstance, hPrevInstance, lpszCmdLine,
							 nCmdShow);

	_fmemset( lpszFile, 0, 100);
	AlCompApp.HAccTable = LoadAccelerators (hInstance, "ACCELERATORS_1");
	AlCompApp.Run();
	return AlCompApp.Status;
}