_CLASSDEF(TDlgCondNote)

class TDlgCondNote  :public TDialog, TNoteEdit
{
public:
	BYTE from;
   BYTE octave;
	TDlgCondNote(PTWindowsObject pwParent, int nResourceID, BYTE From, BYTE oct )
		:TDialog(pwParent, nResourceID), TNoteEdit(this, From) { from = From; octave = oct; }
	~TDlgCondNote()
	{
		WinHelp(this->HWindow, "alcomp.hlp", HELP_QUIT, 0L);
   }

	virtual void SetupWindow();
	virtual BOOL CanClose() { return TDialog::CanClose() && TNoteEdit::CanClose(); }
	virtual void Help(RTMessage Msg) = [ID_FIRST + IDHELP] ;
	virtual void Ok(RTMessage Msg);
	BYTE tracval() { return 1+(from/12)+octave; }
};

void TDlgCondNote::SetupWindow()
{
	TDialog::SetupWindow();
	TNoteEdit::InitNotes();

	char title[45], ns[10];
	strcpy(title, "Set Conditional Probabilities from ");
	switch (from % 12)  {
		case  0 : sprintf(ns, "C%1d", tracval()); break;
		case  1 : sprintf(ns, "C#%1d / Db%1d", tracval(), tracval()); break;
		case  2 : sprintf(ns, "D%1d", tracval()); break;
		case  3 : sprintf(ns, "D#%1d / Eb%1d", tracval(), tracval()); break;
		case  4 : sprintf(ns, "E%1d", tracval()); break;
		case  5 : sprintf(ns, "F%1d", tracval()); break;
		case  6 : sprintf(ns, "F#%1d / Gb%1d", tracval(), tracval()); break;
		case  7 : sprintf(ns, "G%1d", tracval()); break;
		case  8 : sprintf(ns, "G#%1d / Ab%1d", tracval(), tracval()); break;
		case  9 : sprintf(ns, "A%1d", tracval()); break;
		case 10 : sprintf(ns, "A#%1d / Bb%1d", tracval(), tracval()); break;
		case 11 : sprintf(ns, "B%1d", tracval()); break;
	}
	strcat (title, ns);
	SetCaption( (LPSTR) title );
}


void TDlgCondNote::Ok(RTMessage Msg)
{
	if (Msg.LP.Hi==BN_CLICKED && CanClose())
	{
		// need to find the first ith position
		for (int i = 0; i<tempTrack.Note.count(); i++)
			if ( tempTrack.Note[i].From == from )  {
				// must have found start position, delete from here
				while ( i<tempTrack.Note.count() && tempTrack.Note[i].From == from )
					tempTrack.Note.detach(i, TRUE);
				break;
			}

		StrNote curval;
		curval.From = from;
      // now we can add from note array normally
		for (i=0; i<MAX_NOTENUM; i++)
			if ( note[i] )  {
				curval.To   = i;
				curval.Prob = note[i];
				tempTrack.Note.add( curval );
			}

		TDialog::Ok(Msg);
	}
}

void TDlgCondNote::Help(RTMessage Msg)
{
	if ( Msg.LP.Hi == BN_CLICKED )
		if (! WinHelp(this->HWindow, "alcomp.hlp", HELP_CONTEXT, EDIT_CONDITIONAL_PROB ))
			BWCCMessageBox(this->HWindow, "Help File Not Found", "Error!", MB_OK|MB_ICONEXCLAMATION);
}
