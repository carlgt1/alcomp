_CLASSDEF(TNoteEdit)
class TNoteEdit
{
public:
	PTEdit      pTENote[MAX_NOTENUM];     // Edit region for notes
	BYTE			note[MAX_NOTENUM];        // temp array for probs
	BYTE			FromNote;
	TNoteEdit(PTWindowsObject pwThisObj, BYTE nFrom);
	virtual void ShowNotes();
	virtual void InitNotes();
	virtual void SetTempNotes();  // set this up before pass rhythms
	virtual BOOL CanClose();
};

TNoteEdit::TNoteEdit(PTWindowsObject pwThisObj, BYTE nFrom = 0)
{
	FromNote = nFrom;
	// ptr window, ID #, Init txt, X, Y, dx, dy, text len, multi-line
	for (int i = 0; i < MAX_NOTENUM; i++) 
			pTENote[i]   = new TEdit(pwThisObj, NOTE_OFFSET + i, 3 );
}

void TNoteEdit::InitNotes()
{
	for (int i = 0; i<MAX_NOTENUM; i++)
		note[i] = 0;

	// fill the array with values from tempTrack (non-zero values only)
	// need to find the first ith position

	for (i = 0; i<tempTrack.Note.count(); i++)
		if (nProbType == CONDITIONAL)  {
			if ( tempTrack.Note[i].From == FromNote )  {
				for (int j = i; j < tempTrack.Note.count() && tempTrack.Note[j].From == FromNote; j++ )
					note[ tempTrack.Note[j].To ] = tempTrack.Note[j].Prob;
				break;
			}
      }
		else
			note[ tempTrack.Note[i].To ] = tempTrack.Note[i].Prob;

	ShowNotes();
}

void TNoteEdit::ShowNotes()
{
	char buf2[3];
	for (int i = 0; i < MAX_NOTENUM; i++)  {
		sprintf(buf2, "%2d", note[i] );
		pTENote[i]->SetText(buf2);
	}
}

void TNoteEdit::SetTempNotes()
{
	char buf2[3];
	for (int i=0; i<MAX_NOTENUM; i++)  {
		pTENote[i]->GetText(buf2, 3);
		note[i] = atoi(buf2);
	}
}

BOOL TNoteEdit::CanClose()
{
	int sumnotes = 0;
	BOOL check = TRUE;
	char tempstr[80], errstr[80];

	SetTempNotes();

	for (int i=0; i< MAX_NOTENUM; i++)
		if ( note[i] > 99 )  {
			check = FALSE;
			sprintf(tempstr, "* Invalid Note Probability.\n  Please Check Values.\n");
			strcat(errstr, tempstr);
			break;
		}
	   else
			sumnotes+=note[i];   // then add up notes, if 0 no probs assigned

	if (! sumnotes)  {
		check = FALSE;
		sprintf(tempstr, "* No Note Probs Assigned.\n");
		strcat(errstr, tempstr);
	}

	if (! check)
		BWCCMessageBox(NULL, tempstr, "Note Edit", MB_OK|MB_ICONEXCLAMATION);

	return check;
}