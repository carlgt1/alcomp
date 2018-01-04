// Definitions for Aleatoric Composer Functions
#define MAX_TEXTLEN     51  // max length for text descriptions
#define MAX_TRACKS     100  // MAX NUMBER OF TRACKS; TESTED UP TO 300!!!
#define MAX_NOTEPROBS   400 // max number of note probs per track
#define MAX_RHYTHMPROBS 36  // max # of rhythm probs per track
#define MAX_NOTENUM     49  // four octaves plus a C at the end
#define MAX_RHYTHM       6  // whole, half, quarter, eighth, sixteenth, thirty-second
#define NOTE_OFFSET    840  // what's offset between index and Note ID #
#define MAX_CONDITIONAL 25
#define CC_SELCHANGE 31666  // a message I pass to see if conditional note probs
#define DRUM_PATCH 128      // patch # of drums
#define DRUM_CHANNEL 9	   // channel # of drums (-1)
#define UNSELECTED 0
#define CONDITIONAL 1
#define RANDOM 2

// since I use (char*) all over the place to cast
// LPSTR to good old-fashioned char*
#define CCH (char*)

#define BBYTE 1
#define BINT 2               // for my slick recursive function
#define BITS24 3
#define BLONG 4

#define NOTE_WIDTH 18   // the note edit height for check boxes and text edit
#define NOTE_HEIGHT 17

// HELP System defines
#define CONTENTS 868
#define EDIT_RANDOM_PROB 866
#define EDIT_CONDITIONAL_PROB 867
#define RHYTHM_RANDOM 854
#define RHYTHM_CONDITIONAL 855
#define TRACK_VIEW_RANDOM 850
#define TRACK_VIEW_CONDITIONAL 851

typedef unsigned long ULONG;
// flags for input and output of *.CMC files
const BYTE CONDITIONAL_FLAG = 0xFA ;
const BYTE RANDOM_FLAG      = 0xFB ;
const BYTE TRACK_FLAG       = 0xFC ;
const BYTE NOTES_FLAG       = 0xFD ;
const BYTE RHYTHM_FLAG      = 0xFE ;

// CURSOR STUFF HERE
#define CU_BUSY 1234
HCURSOR hcBusy;

// global structure for Track Settings, Notes, and Rhythms
struct StrNote
{
	BYTE   From;         // int for the note [ 0 to MAX_NOTENUM ]
	BYTE   To;           // int for the note to [ 0 to MAX_NOTENUM]
	BYTE   Prob;         // int for each note probability
	StrNote();
	int operator ==(const StrNote& n);
	int operator <(const StrNote& n);
};

StrNote::StrNote()
{
	From =  0;
	To   =  0;
	Prob =  0;
}

inline int StrNote::operator ==(const StrNote& n)
{
	return ( From == n.From );
}

inline int StrNote::operator <(const StrNote& n)
{
	return ( From < n.From );
} 

struct Track
{
	char  Description[MAX_TEXTLEN]; // short description for track
	UINT  StartMeasure;       // starting measure of track
	UINT  EndMeasure;         // ending measure of track
	BYTE  Patch;              // General MIDI patch number, index into PatchTable
	BYTE  Channel;		        // MIDI Channel, from 1 to 16
	UINT  Tempo;              // Tempo of this Track
	BYTE  TimeSigNum;         // variable to hold time signature numerator
	BYTE  TimeSigDen;         // variable to hold time signature denominator
	BYTE  Octave;             // Octave displacement (-2, -1, 0, 1, 2)
	BYTE  Volume;             // relative volume ( from 0 to 127);
	BI_SVectorImp<StrNote> Note;     // setup 100 notes
	BI_SVectorImp<StrNote> Rhythm;   // setup 100 rhythms

	int operator ==(const Track& t);
	int operator <(const Track& t);
	Track();
};

Track::Track()	:Note(MAX_NOTEPROBS, sizeof(Track)*MAX_NOTEPROBS),
		 Rhythm(MAX_RHYTHMPROBS, sizeof(Track)*MAX_RHYTHMPROBS)
{
	strcpy(Description, "Empty Track");
	StartMeasure = 0;
	EndMeasure   = 0;
	Patch        = 0;
	Channel      = 1;
	Tempo        = 120;
	TimeSigNum   = 4;
	TimeSigDen   = 4;
	Octave       = 2;
	Volume       = 80;
}

inline int Track::operator ==(const Track& t)
{
	return (     StartMeasure == t.StartMeasure
			&& EndMeasure   == t.EndMeasure
			&& Channel      == t.Channel
			&& Patch        == t.Patch        );
}

inline int Track::operator <(const Track& t)
{  // crazy way to overload less than operator
	if ( StartMeasure < t.StartMeasure )
		return TRUE;
	else
		if ( StartMeasure == t.StartMeasure )
			if ( EndMeasure < t.EndMeasure )
				return TRUE;
			else
				if ( EndMeasure == t.EndMeasure )
					if ( Channel < t.Channel )
						return TRUE;
					else
						if ( Channel == t.Channel )
							if ( Patch < t.Patch )
								return TRUE;
							else
								if ( Patch == t.Patch )
									if ( strcmp(Description, t.Description) < 0 )
										return TRUE;
									else
										return FALSE;
								else
									return FALSE;
						else
							return FALSE;
				else
					return FALSE;
		else
			return FALSE;
}

BI_SVectorImp<Track> dTrack(300, sizeof(Track)*50);   // setup 300 tracks

// need temp buffers for edit screens
Track tempTrack = *(new Track);
const Track nullTrack = *(new Track);

LPSTR lpszFile = new char[100];  // filename of current save/open file

BOOL  bModified = FALSE;         // boolean to see if modified or not
BYTE  nProbType = UNSELECTED;      // default to random probabilities

// Handy little routine to tack on proper extensions
char* CheckExtension( char* infile, char* extension )
{
	char temp[100];
	memset(temp, 0, 100);
	for (int i=0; infile[i] && infile[i]!='.' ; i++);

	strncat(temp, infile, i);
	strcat(temp, extension);
     return temp;
}

// DEFINES FOR THE RESOURCES
// Menu Defines
#define MN_MENU	     100
#define MN_SONGSETTINGS	101
#define MN_TRACKVIEW	102
#define MN_PROBABILITY	103
#define MN_PLAY	      104
#define MN_FILENEW	   105
#define MN_FILEOPEN	   106
#define MN_FILESAVE	   107
#define MN_FILESAVEAS	108
#define MN_FILEEXIT     109
#define MN_HELPINDEX	   110
#define MN_ABOUT	      111
#define MN_STOP	      112
#define MN_CONDITIONAL 113

// Button Defines
#define ID_NOTES	250
#define ID_RHYTHM	251
#define ID_ADD	252
#define ID_EDIT	253
#define ID_DELETE	254
#define ID_WHOLE 255
#define ID_HALF 256
#define ID_QUARTER 257
#define ID_EIGHTH 258
#define ID_SIXTEENTH 259
#define ID_THIRTYSECOND 260
#define ID_MORE	261
#define ID_KEYBOARD 262
#define ID_GENESIS 263
#define ID_ELP	264
#define ID_YES 265
#define ID_FLOYD 266
#define ID_TOGGLENOTES	267
#define ID_USER      268  
#define ID_RANDOM    269  
#define ID_LINEAR    270  
#define ID_GAUSSIAN  271  
#define ID_UNIFORM   272  
#define ID_EXPONENT  273  
#define ID_POISSON   274 
#define ID_CAUCHY    275  
#define ID_GENERATE	276
#define ID_SONG 277
#define ID_JUNK 278
#define ID_NOTEGROUP 279
#define ID_SHUTUP 280

// BitMap Defines
#define BM_RHYTHM	1251
#define BM_ADD 1252
#define BM_EDIT 1253
#define BM_DELETE 1254

#define BM_RHYTHM_PUSHED	3251
#define BM_ADD_PUSHED 3252
#define BM_EDIT_PUSHED 3253
#define BM_DELETE_PUSHED 3254

#define BM_WHOLE	1255
#define BM_HALF	1256
#define BM_QUARTER	1257
#define BM_EIGHTH	1258
#define BM_SIXTEENTH	1259
#define BM_THIRTYSECOND	1260

#define BM_MORE	1261
#define BM_MORE_PUSHED	3261

#define BM_KEYBOARD	1262
#define BM_GENESIS	1263
#define BM_ELP	1264
#define BM_YES	1265
#define BM_FLOYD	1266

#define BM_TOGGLE	1267
#define BM_RANDOM    1269  
#define BM_LINEAR    1270  
#define BM_GAUSSIAN  1271  
#define BM_UNIFORM   1272  
#define BM_EXPONENT  1273  
#define BM_POISSON   1274 
#define BM_CAUCHY    1275
#define BM_GENERATE	1276
#define BM_SONG 1277
#define BM_JUNK 1278
#define BM_SHUTUP 1280

#define BM_TOGGLE_PUSHED    3267
#define BM_RANDOM_PUSHED    3269  
#define BM_LINEAR_PUSHED    3270  
#define BM_GAUSSIAN_PUSHED  3271  
#define BM_UNIFORM_PUSHED   3272  
#define BM_EXPONENT_PUSHED  3273  
#define BM_POISSON_PUSHED   3274 
#define BM_CAUCHY_PUSHED    3275  
#define BM_GENERATE_PUSHED  3276
#define BM_SHUTUP_PUSHED	 3280

// Edit Region Defines
#define TX_STARTMEASURE	300
#define TX_ENDMEASURE	301
#define TX_TEMPO	302
#define TX_MEASURES	303
#define TX_WHOLE	305
#define TX_HALF	306
#define TX_QUARTER	307
#define TX_EIGHTH	308
#define TX_SIXTEENTH	309
#define TX_THIRTYSECOND	310
#define TX_DESCRIPTION  312
#define TX_VOLUME 313


// Dialog Defines
#define DG_ABOUT	400
#define DG_TRACKVIEW	401
#define DG_TRACKEDIT	402
#define DG_RHYTHM 403
#define DG_ABOUTMORE	404
#define DG_SONG	405
#define DG_CONDEDIT 406
#define DG_RHYTHM_COND 407
#define DG_SETCOND 408

// Group Box Defines
#define GB_OCTAVES	665
#define GB_CONDEDIT	666

// Combo Box Defines
#define CB_MIDICHANNEL	600
#define CB_PATCH	601
#define CB_TRACKVIEW	602
#define CB_TIMESIGNUM	603
#define CB_TIMESIGDEN	604

// List Box Defines
#define LB_TRACKVIEW	700

// Define Accelerators
#define AC_OPEN	901
#define AC_GENERATE	902
#define AC_HELPINDEX 903

// Icon Define
#define ICON_1	1000

// Radio Buttons on track edit screen
#define RB_MINUS2OCT 500   // radio buttons for octaves
#define RB_MINUS1OCT 501
#define RB_ZEROOCT   502
#define RB_PLUS1OCT  503
#define RB_PLUS2OCT  504

// radio buttons for probability types

// Scroll Bar Defines
#define SB_MEASURES	800
#define SB_TEMPO	801
#define SB_WHOLE 802
#define SB_HALF 803
#define SB_QUARTER 804
#define SB_EIGHTH 805
#define SB_SIXTEENTH 806
#define SB_THIRTYSECOND 807
#define SB_VOLUME 808


// defines for all the edit boxes on the keyboard display
#define TX_1C  840
#define TX_1CS 841
#define TX_1D  842
#define TX_1DS 843
#define TX_1E  844
#define TX_1F  845
#define TX_1FS 846
#define TX_1G  847
#define TX_1GS 848
#define TX_1A  849
#define TX_1AS 850
#define TX_1B  851

#define TX_2C  852
#define TX_2CS 853
#define TX_2D  854
#define TX_2DS 855
#define TX_2E  856
#define TX_2F  857
#define TX_2FS 858
#define TX_2G  859
#define TX_2GS 860
#define TX_2A  861
#define TX_2AS 862
#define TX_2B  863

#define TX_3C  864
#define TX_3CS 865
#define TX_3D  866
#define TX_3DS 867
#define TX_3E  868
#define TX_3F  869
#define TX_3FS 870
#define TX_3G  871
#define TX_3GS 872
#define TX_3A  873
#define TX_3AS 874
#define TX_3B  875

#define TX_4C  876
#define TX_4CS 877
#define TX_4D  878
#define TX_4DS 879
#define TX_4E  880
#define TX_4F  881
#define TX_4FS 882
#define TX_4G  883
#define TX_4GS 884
#define TX_4A  885
#define TX_4AS 886
#define TX_4B  887

#define TX_5C  888

// conditional RHYTHM probabilities
#define TX_WW  1000
#define TX_WH  1001
#define TX_WQ  1002
#define TX_WE  1003
#define TX_WS  1004
#define TX_WT  1005

#define TX_HW  1006
#define TX_HH  1007
#define TX_HQ  1008
#define TX_HE  1009
#define TX_HS  1010
#define TX_HT  1011

#define TX_QW  1012
#define TX_QH  1013
#define TX_QQ  1014
#define TX_QE  1015
#define TX_QS  1016
#define TX_QT  1017

#define TX_EW  1018
#define TX_EH  1019
#define TX_EQ  1020
#define TX_EE  1021
#define TX_ES  1022
#define TX_ET  1023

#define TX_SW  1024
#define TX_SH  1025
#define TX_SQ  1026
#define TX_SE  1027
#define TX_SS  1028
#define TX_ST  1029

#define TX_TW  1030
#define TX_TH  1031
#define TX_TQ  1032
#define TX_TE  1033
#define TX_TS  1034
#define TX_TT  1035