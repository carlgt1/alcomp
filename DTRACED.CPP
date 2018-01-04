// Here's the big mess!!!  The damned track edit window.
_CLASSDEF(TDlgTrackEdit)
// Control dialog box for editing Song Settings
class TDlgTrackEdit
{
public:
	TDlgTrackEdit(PTWindowsObject pwParent);

	PTEdit         pTEDescription;  // text edit region for description
	PTBRadioButton pRBOctaveMinus2; // octave -2 disp radio-buttons
	PTBRadioButton pRBOctaveMinus1; // octave -1 disp
	PTBRadioButton pRBOctaveZero  ; // octave 0 disp
	PTBRadioButton pRBOctavePlus1 ; // octave +1 disp
	PTBRadioButton pRBOctavePlus2 ; // octave +1 disp
	PTScrollBar    pSBTempo;        // Tempo Scroll bar
	PTBStatic      pTXTempo;        // Tempo static text for output
	PTScrollBar    pSBVolume;       // Volume Scroll bar
	PTBStatic      pTXVolume;       // Volume static text for output
	PTComboBox     pCBMIDIChannel;  // MIDI channel combo box
	PTEdit         pTEStartMeasure; // start measure text edit
	PTEdit         pTEEndMeasure;   // end measure text edit
	PTBGroupBox    pGBOctaves;      // Group Box for Octaves Radio Buttons
	PTComboBox     pCBPatch;        // General MIDI patch combo box
	PTComboBox     pCBTimeSigNum;   // Time sig numerator combo box
	PTComboBox     pCBTimeSigDen;   // Time sig denominator combo box

	virtual void SetupWindow();
	virtual BOOL CanClose();

	virtual void Tempo(/*RTMessage Msg*/);  // messages dispatched in derived classes
	virtual void Volume(/*RTMessage Msg*/);
	virtual void Channel(RTMessage Msg);
	virtual void Patch(RTMessage Msg);

	void SetupComboStrings();
	void SaveIt();
};

// setup combo boxes and scroll bars for Time Signatures, tempo, & measures
TDlgTrackEdit::TDlgTrackEdit(PTWindowsObject pwParent)
{
	//	Now time to initialize all these lovely controls
	pTXTempo         = new TBStatic(pwParent, TX_TEMPO, 4);
	pTXVolume        = new TBStatic(pwParent, TX_VOLUME, 4);

	pTEDescription   = new TEdit(pwParent, TX_DESCRIPTION, 51);
	pTEStartMeasure  = new TEdit(pwParent, TX_STARTMEASURE, 4);
	pTEEndMeasure    = new TEdit(pwParent, TX_ENDMEASURE, 4);

	pGBOctaves	     = new TBGroupBox(pwParent, GB_OCTAVES);

	pRBOctaveMinus2  = new TBRadioButton(pwParent, RB_MINUS2OCT, pGBOctaves);
	pRBOctaveMinus1  = new TBRadioButton(pwParent, RB_MINUS1OCT, pGBOctaves);
	pRBOctaveZero    = new TBRadioButton(pwParent, RB_ZEROOCT,   pGBOctaves);
	pRBOctavePlus1   = new TBRadioButton(pwParent, RB_PLUS1OCT,  pGBOctaves);
	pRBOctavePlus2   = new TBRadioButton(pwParent, RB_PLUS2OCT,  pGBOctaves);

	pSBTempo         = new TScrollBar(pwParent, SB_TEMPO);
	pSBVolume        = new TScrollBar(pwParent, SB_VOLUME);

	pCBMIDIChannel   = new TComboBox(pwParent, CB_MIDICHANNEL, 3);
	pCBPatch         = new TComboBox(pwParent, CB_PATCH, 26);
	pCBTimeSigNum    = new TComboBox(pwParent, CB_TIMESIGNUM, 3);
	pCBTimeSigDen    = new TComboBox(pwParent, CB_TIMESIGDEN, 3);
}

void TDlgTrackEdit::SetupWindow()
{
	char buf2[3], buf3[4];

	SetupComboStrings();  //setup gen'l MIDI patches and other strings

	// Tempo Static Text
	sprintf(buf3, "%03d", tempTrack.Tempo);
	pTXTempo->SetText(buf3);

	// Tempo Scroll Bar
	pSBTempo->SetRange(10,400);
	pSBTempo->SetPosition(tempTrack.Tempo);

	// Volume Static Text
	sprintf(buf3, "%03d", tempTrack.Volume);
	pTXVolume->SetText(buf3);

	// Volume Scroll Bar
	pSBVolume->SetRange(0, 127);
	pSBVolume->SetPosition(127 - tempTrack.Volume);

	// Description Text
	pTEDescription->SetText(tempTrack.Description);

	// StartMeasure Text
	sprintf(buf3, "%03d", tempTrack.StartMeasure);
	pTEStartMeasure->SetText(buf3);
	pTEStartMeasure->SetSelection(0, 4);

	// EndMeasure Text
	sprintf(buf3, "%03d", tempTrack.EndMeasure);
	pTEEndMeasure->SetText(buf3);
	pTEEndMeasure->SetSelection(0, 4);

	// setup Octave Radio buttons
	if (tempTrack.Octave == 0)
		pRBOctaveMinus2->SetCheck(BF_CHECKED);
	else
		pRBOctaveMinus2->SetCheck(BF_UNCHECKED);

	if (tempTrack.Octave == 1)
		pRBOctaveMinus1->SetCheck(BF_CHECKED);
	else
		pRBOctaveMinus1->SetCheck(BF_UNCHECKED);

	if (tempTrack.Octave == 2)
		pRBOctaveZero->SetCheck(BF_CHECKED);
	else
		pRBOctaveZero->SetCheck(BF_UNCHECKED);

	if (tempTrack.Octave == 3)
		pRBOctavePlus1->SetCheck(BF_CHECKED);
	else
		pRBOctavePlus1->SetCheck(BF_UNCHECKED);

	if (tempTrack.Octave == 4)
		pRBOctavePlus2->SetCheck(BF_CHECKED);
	else
		pRBOctavePlus2->SetCheck(BF_UNCHECKED);

	// setup time signature numerator
	sprintf(buf2, "%-2d", tempTrack.TimeSigNum);
	pCBTimeSigNum->SetText(buf2);

	// setup time signature denominator
	sprintf(buf2, "%-2d", tempTrack.TimeSigDen);
	pCBTimeSigDen->SetText(buf2);

	// setup MIDI CHANNEL
	sprintf(buf2, "%-2d", tempTrack.Channel);
	pCBMIDIChannel->SetText(buf2);

	// setup MIDI Patch
	pCBPatch->SetText(CCH pszPatch[tempTrack.Patch]);
}

void TDlgTrackEdit::Tempo(/*RTMessage Msg*/)
{
	char buf3[4];
	// Tempo Static Text
	sprintf(buf3, "%03d", pSBTempo->GetPosition() );
	pTXTempo->SetText(buf3);
}

void TDlgTrackEdit::Volume(/*RTMessage Msg*/)
{
	char buf3[4];
	// Volume Static Text
	sprintf(buf3, "%03d", 127 - pSBVolume->GetPosition() );
	pTXVolume->SetText(buf3);
}

void TDlgTrackEdit::Patch(RTMessage Msg)
{
	// see if drums is picked
	if (Msg.LP.Hi == CBN_SELCHANGE) 
		if ( pCBPatch->GetSelIndex() == DRUM_PATCH )
			pCBMIDIChannel->SetSelIndex(DRUM_CHANNEL);
}

void TDlgTrackEdit::Channel(RTMessage Msg)
{
     // see if drums is picked
	if (Msg.LP.Hi == CBN_SELCHANGE)
		if ( pCBMIDIChannel->GetSelIndex() == DRUM_CHANNEL )
			pCBPatch->SetSelIndex(DRUM_PATCH);
}

BOOL TDlgTrackEdit::CanClose()
{
	char buf2[3], buf3[4], errstr[256], tempstr[80];
	int  start = 0, end = 0;
	BOOL check = TRUE;

	memset(errstr, 0, 256);

 	// check for bad start and end measure (end < start, not from 1 to 999)
	pTEStartMeasure->GetText(buf3, 4);
	start = atoi(buf3);
	if (start < 1 || start > 999)  {
		check = FALSE;
		sprintf(tempstr, "* Start Measure Is Invalid.\n  Must be between 1 and 999.\n");
		strcat(errstr, tempstr);
	}

	pTEEndMeasure->GetText(buf3, 4);
	end = atoi(buf3);
	if (end < 1 || end > 999)  {
		check = FALSE;
		sprintf(tempstr, "* End Measure Is Invalid.\n  Must be between 1 and 999.\n");
		strcat(errstr, tempstr);
	}

	// since made a combo box for TimeSigNum, better check it
	pCBTimeSigNum->GetText(buf2, 3);
	if ( atoi(buf2) < 1 || atoi(buf2) >99 )  {  //must invalid
		check = FALSE;
		sprintf(tempstr, "* Time Sig Num Is Invalid.\n  Must be between 1 and 99.\n");
		strcat(errstr, tempstr);
     }

	if (end < start)  { // can't have end less than start (but can be equal?)
		check = FALSE;
		sprintf(tempstr, "* End Measure < Start Measure.\n");
		strcat(errstr, tempstr);
	}

	if (! check)
		BWCCMessageBox(NULL, errstr, "Error!", MB_OK | MB_ICONEXCLAMATION);

	return check;
}

void TDlgTrackEdit::SaveIt()
{
	char buf2[3], buf3[4];

	// Description Text
	pTEDescription->GetText( tempTrack.Description, 50);

	pTEStartMeasure->GetText(buf3, 4);
	tempTrack.StartMeasure = atoi(buf3);

	pTEEndMeasure->GetText(buf3, 4);
	tempTrack.EndMeasure   = atoi(buf3);

	tempTrack.Patch = pCBPatch->GetSelIndex();

	pCBMIDIChannel->GetText(buf2, 3);
	tempTrack.Channel = atoi(buf2);

	tempTrack.Tempo = pSBTempo->GetPosition();

	tempTrack.Volume = 127 - pSBVolume->GetPosition();

	pCBTimeSigNum->GetText(buf2, 3);
	tempTrack.TimeSigNum = atoi(buf2);

	pCBTimeSigDen->GetText(buf2, 3);
	tempTrack.TimeSigDen = atoi(buf2);

	if ( pRBOctaveMinus2->GetCheck() == BF_CHECKED )
		tempTrack.Octave = 0;

	if ( pRBOctaveMinus1->GetCheck() == BF_CHECKED )
		tempTrack.Octave = 1;

	if ( pRBOctaveZero->GetCheck() == BF_CHECKED )
		tempTrack.Octave = 2;

	if ( pRBOctavePlus1->GetCheck() == BF_CHECKED )
		tempTrack.Octave = 3;

	if ( pRBOctavePlus2->GetCheck() == BF_CHECKED )
		tempTrack.Octave = 4;
}

 //setup gen'l MIDI patches and other strings
void TDlgTrackEdit::SetupComboStrings()
{
	pCBMIDIChannel->Clear();
	pCBMIDIChannel->AddString("1 ");
	pCBMIDIChannel->AddString("2 ");
	pCBMIDIChannel->AddString("3 ");
	pCBMIDIChannel->AddString("4 ");
	pCBMIDIChannel->AddString("5 ");
	pCBMIDIChannel->AddString("6 ");
	pCBMIDIChannel->AddString("7 ");
	pCBMIDIChannel->AddString("8 ");
	pCBMIDIChannel->AddString("9 ");
	pCBMIDIChannel->AddString("10");
	pCBMIDIChannel->AddString("11");
	pCBMIDIChannel->AddString("12");
	pCBMIDIChannel->AddString("13");
	pCBMIDIChannel->AddString("14");
	pCBMIDIChannel->AddString("15");
	pCBMIDIChannel->AddString("16");

	pCBTimeSigNum->Clear();
	pCBTimeSigNum->AddString("1 ");
	pCBTimeSigNum->AddString("2 ");
	pCBTimeSigNum->AddString("3 ");
	pCBTimeSigNum->AddString("4 ");
	pCBTimeSigNum->AddString("5 ");
	pCBTimeSigNum->AddString("6 ");
	pCBTimeSigNum->AddString("7 ");
	pCBTimeSigNum->AddString("8 ");
	pCBTimeSigNum->AddString("9 ");
	pCBTimeSigNum->AddString("10");
	pCBTimeSigNum->AddString("11");
	pCBTimeSigNum->AddString("12");
	pCBTimeSigNum->AddString("13");
	pCBTimeSigNum->AddString("14");
	pCBTimeSigNum->AddString("15");
	pCBTimeSigNum->AddString("16");
	pCBTimeSigNum->AddString("17");
	pCBTimeSigNum->AddString("18");
	pCBTimeSigNum->AddString("19");
	pCBTimeSigNum->AddString("20");
	pCBTimeSigNum->AddString("21");
	pCBTimeSigNum->AddString("22");
	pCBTimeSigNum->AddString("23");
	pCBTimeSigNum->AddString("24");
	pCBTimeSigNum->AddString("25");
	pCBTimeSigNum->AddString("26");
	pCBTimeSigNum->AddString("27");
	pCBTimeSigNum->AddString("28");
	pCBTimeSigNum->AddString("29");
	pCBTimeSigNum->AddString("30");
	pCBTimeSigNum->AddString("31");
	pCBTimeSigNum->AddString("32");

	pCBTimeSigDen->Clear();
	pCBTimeSigDen->AddString("1 ");
	pCBTimeSigDen->AddString("2 ");
	pCBTimeSigDen->AddString("4 ");
	pCBTimeSigDen->AddString("8 ");
	pCBTimeSigDen->AddString("16");
	pCBTimeSigDen->AddString("32");

	pCBPatch->Clear();

	for (int i = 0; i<129; i++)
		pCBPatch->AddString( CCH pszPatch[i] );
}
