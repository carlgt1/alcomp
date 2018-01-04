_CLASSDEF(TMIDIFile)
class TMIDIFile  {
	char tempname[100];  // temp file name
	long counter;   // counter for chunk length
   PTWindowsObject pwParent;
	struct TimeSigMap {
		UINT Measure;   // start measure for this time sig
		double Ratio;   // ratio of Numerator/Denominator (2/4 = .500)
		TimeSigMap() { Measure=0; Ratio=1.0000;};
		int operator ==(const TimeSigMap& tsm)  { return ( Measure == tsm.Measure ); }
		int operator  <(const TimeSigMap& tsm)  { return ( Measure <  tsm.Measure ); }
	};
	int lastnote, lastrhythm;  // for conditional probs
	fstream io;
	// set up array of structures for time sig map
	BI_SVectorImp<TimeSigMap> TSM;
public:
	TMIDIFile(PTWindowsObject pwTrackView)
	  :TSM(90, sizeof(TSM)*10) { pwParent = pwTrackView; lastnote = -1; lastrhythm = -1; }
	void MakeMIDIFile(); // creates the temp MIDI file
	void PlayMIDIFile(); // plays MIDI file
	void SaveMIDIFile(); // save MIDI file

	void WriteMIDITrack(UINT m);

	// overload some stuff for writing out
	void WriteOut(LPSTR value);
	//cool recursive int/long binary write function
	void WriteOut(ULONG value, UINT size);

	void WriteVarLen(long value);  // var length for delta times

	// return the delta time given the measure and time signature
	TimeSigMap AddTimeSig(ULONG delta, UINT num, UINT den, UINT position);
	void AddTempo(ULONG delta, ULONG tempo);

	BYTE GetNote();    // send back a note from 0 to 127
	UINT GetRhythm();  // send back a rhythm
	UINT SumNote();  // sum probs for normalized distribution
	UINT SumRhythm();  // sum probs for normalized distribution
};

void TMIDIFile::MakeMIDIFile()
{
	// create temp file name at root of current drive
	HCURSOR hcOld = SetCursor(hcBusy);
	BYTE disk;
	disk = getdisk() + 'A';
	sprintf(tempname, "%c:\\%d.mid", disk, rand());

	io.open( tempname, ios::binary | ios::in | ios::out | ios::trunc );
	if ( io.bad() )  {
		io.close();
		SetCursor(hcOld);
		return;
	}

	counter = 0L;   // setup counter for # of bytes written

	// setup header for MIDI file

	WriteOut("MThd");

	// write out number of tracks and the pulses per quarter note
	WriteOut(6, BLONG);
	WriteOut(1, BINT);

	WriteOut(dTrack.count() + 1, BINT);  // # OF TRACKS + 1 FOR SETTINGS
	WriteOut(120, BINT);  // PPQ

	// now write the header track 0 which is a tempo and time sig map
	// allocate mem (1K at first)
	// pad with ????, then at end go back and change
	WriteOut("MTrk");

	long chunklength = io.tellg();  // save absolute file position

	WriteOut(0L, BLONG);   // set to zero for now, need to update later

	counter = 0L;
	// start adding up counter here

	// add my title as a text event  (00 FF 01 len)
	WriteOut(0xFF0100L+20, BLONG);
	WriteOut("Algorithmic Composer");

	// copyright info (00 FF 02 len)
	WriteOut(0xFF0200L+28, BLONG);
	WriteOut("(c) 1993 Carl M. Christensen");  // final copyright stuff

     // here we're creating the time signature map

	int num = dTrack[0].TimeSigNum;   // initialize first time sig
	int den = dTrack[0].TimeSigDen;

	ULONG tempo = dTrack[0].Tempo;   // setup first tempo
	ULONG delta = 120 * (dTrack[0].StartMeasure - 1) *
					    4 * ( (float) num / (float) den ) ;
	BOOL usedDelta;

	AddTempo(0, tempo);  // do the first tempo, note delta is 0

	TSM.add( AddTimeSig(delta, num, den, 0) );   // do the 1st time signature

	for (UINT j=1; j<dTrack.count(); j++)  {
     	// DELTA can be used by both time sig & tempo
          usedDelta = FALSE;  // reset Delta flag
		delta = 120 * ( dTrack[j].StartMeasure -
					 dTrack[j-1].StartMeasure ) *
    				4 * ( (float) num / (float) den ) ;

		// different time signature encountered, add to map
		if ( delta && ( (num != dTrack[j].TimeSigNum) ||
			 (den != dTrack[j].TimeSigDen) ) )
		{
			num = dTrack[j].TimeSigNum;
			den = dTrack[j].TimeSigDen;
			TSM.add( AddTimeSig(delta, num, den, j) );   // do the 1st time signature
			usedDelta = TRUE;  // mark that we used delta
		}

		// now check tempo, but if delta was used above, don't need
		// to recreate it again, it'll be zero difference
		if ( delta && (tempo != dTrack[j].Tempo) )
		{// different time signature encountered, add to map
			tempo = dTrack[j].Tempo;
			// if delta was used above, just make it zero
			AddTempo( usedDelta ? 0 : delta, tempo);
		}
	}

	// all done with header, just add 00 FF 2F 00 for end of track
	WriteOut(0xFF2F00L, BLONG);

	// NOW UPDATE THE TRACK CHUNK LENGTH
	// first, we gotta save current position
   long whereami = io.tellg();

	io.seekg( chunklength );  // go back to chunk length

	// write chunk length out (32-bits / long)
	WriteOut(counter, BLONG);  // write out the long value for chunk length

	// now go back to regular position
	io.seekg( whereami );

	// all done writing track 0, now comes the dirty work -- the real tracks
	for (int i=0; i<dTrack.count(); i++)
		WriteMIDITrack(i);

	io.flush();
	io.close();

	SetCursor(hcOld);
	if ( io.good() )  { 
		PlayMIDIFile();
		SaveMIDIFile();
	}
	unlink(tempname);
}

void TMIDIFile::WriteVarLen(long value)
{
	long buffer;
	buffer = value & 0x7F;
	while ((value >>= 7) > 0)  {
		buffer <<= 8;
		buffer |= 0x80;
		buffer += (value & 0x7F);
	}
	while (TRUE)  {
		io.put(buffer);
		counter++;
		if (buffer & 0x80)
			buffer >>= 8;
		else
			break;
	}
}

void TMIDIFile::WriteOut(ULONG value, UINT size)
{
	size--;
	if (value >> 8) {
		WriteOut(value >> 8, size);
		io.put(value & 0xFF);
          counter++;
	}
	else  {
     	// made it to bottom but ctr not 0, means we pad 0x00's
          for ( ; size>0; size--) {
			io.put(0x00);
			counter++;
		}
		io.put(value);
		counter++;
	}
}

void TMIDIFile::WriteOut(LPSTR value)
{
	counter += _fstrlen(value);
	io.write ( (BYTE*) value, _fstrlen(value ) );
}

UINT TMIDIFile::SumNote()
{
	// get the sum of notes
	register UINT sumprobs = 0;

	for (register int j=0; j < tempTrack.Note.count(); j++)
		if (nProbType == RANDOM || tempTrack.Note[j].From == lastnote)
			sumprobs+= tempTrack.Note[j].Prob;

	return sumprobs;
}

UINT TMIDIFile::SumRhythm()
{
	// get the sum of rhythms
	register UINT sumprobs = 0;
	for (register int j=0; j < tempTrack.Rhythm.count(); j++)
		if (nProbType == RANDOM || tempTrack.Rhythm[j].From == lastrhythm)
			sumprobs+= tempTrack.Rhythm[j].Prob;

	return sumprobs;
}

void TMIDIFile::WriteMIDITrack(UINT m)
{
	// setup tempTrack for the right Track
	tempTrack = dTrack[m];

	// seed last note values if we're starting (need a start note, right?)
	lastnote = tempTrack.Note[rand() % tempTrack.Note.count()].From;

	lastrhythm = tempTrack.Rhythm[rand() % tempTrack.Rhythm.count()].From;

	// setup header for the selected track
	WriteOut("MTrk");

	long trackchunk = io.tellg();  // save position for later update

	WriteOut(0, BLONG);

	counter = 0;  // initialize byte counter

	// write track name/description  ( 00 FF 03 name )
	WriteOut(0xFF0300L + strlen( tempTrack.Description ), BLONG);
	WriteOut( tempTrack.Description );

	UINT rhythm;
	int change_start = tempTrack.StartMeasure;
	int change_end   = tempTrack.EndMeasure + 1;

   // initialize values for the while loop counter
	ULONG start, end, elapsed;
	start = end = elapsed = 0L;

	// figure out the start offset
	for (int t=TSM.count()-1; t>=0; t--)  { // go through time sig map
		if ( TSM[t].Measure < change_start)  {
			// need to add the # of measures at this time sig
			start += (change_start - TSM[t].Measure)
				* (ULONG) ( TSM[t].Ratio * 480.00 );

			end += (change_end - change_start)
				* (ULONG) ( TSM[t].Ratio * 480.00 );

			change_start = TSM[t].Measure;  // mark the last change
		}
		else {
			// must be >= start, check if less than end
			if ( TSM[t].Measure < change_end)  {
				// time sig change is between start & end
				end += (change_end - TSM[t].Measure) *
				   (ULONG) ( TSM[t].Ratio * 480.00 );
				change_end = TSM[t].Measure;
			}
      }
	}

	WriteVarLen(start);   // delta time to get to start delta time

	// write patch change #
	// ( C0+channel  Patch ) 
	WriteOut( 0xC000 + (0x100 * (tempTrack.Channel - 1)) + tempTrack.Patch, BINT);

	WriteOut( 0x0090 + (tempTrack.Channel - 1), BINT );  // write note on for channel
	BYTE noteon;
	while (elapsed < end)  {  // go until we hit the end delta time
		noteon = GetNote();
		rhythm = GetRhythm();

		// check for bad values (dead end conditional probs most likely)
		if ( noteon == UCHAR_MAX || rhythm == UINT_MAX )
			break;  // we're done since it's the end of this track
      else  {
			WriteOut( noteon, BBYTE );
			WriteOut( tempTrack.Volume, BBYTE );

			WriteVarLen(rhythm);   //delta offset, the note length

			elapsed+=rhythm;    // add note time differential to elapsed counter
          
			WriteOut( noteon, BBYTE ); // what note did we turn on above?
			WriteOut(0, BINT);          // use running status for note off - volume = 0
		}
	}

	// all done, just add FF 2F 00 for end of track
	WriteOut(0xFF2F00L, BITS24);

	long whereami = io.tellg();

	io.seekg( trackchunk );  // go back to chunk length

     // write chunk length out (32-bits / long)
	WriteOut(counter, BLONG);  // write out the long value for chunk length

	io.seekg(whereami);  // get back to regular position
}

void TMIDIFile::PlayMIDIFile()
{
	pwParent->GetApplication()->ExecDialog(new TDlgSong(pwParent, DG_SONG, tempname));
}


void TMIDIFile::SaveMIDIFile()
{
	if (BWCCMessageBox(NULL, "Would you like to save this\nmasterpiece as a MIDI file?", "Save MIDI File", MB_YESNO|MB_ICONQUESTION) != IDYES)
		return;

	// always go to file save box, they can hit cancel if don't want it
	char newname[100] = "*.MID";
	PTFileDialog pTFSaveMIDI = new TFileDialog(pwParent, SD_FILESAVE, newname);
	if ( pwParent->GetApplication()->ExecDialog(pTFSaveMIDI) == IDOK )  {
		// copy temp file (tempname) to file saved
		if ( pTFSaveMIDI->CanClose() )  {
			HCURSOR hcOld = SetCursor(hcBusy);
			strcpy(newname, CheckExtension( newname, ".mid") );

			ifstream oldfile(  tempname , ios::binary | ios::in );
			ofstream newfile(  newname  , ios::binary | ios::out | ios::trunc );

			// check if can't open
			if (newfile.bad() || oldfile.bad() )  {
				newfile.close();
				oldfile.close();
				BWCCMessageBox(NULL, "Cannot create MIDI File.", "Error!", MB_OK|MB_ICONEXCLAMATION);
				SetCursor(hcOld);
				return;
         }
			BYTE ch;   // buffered output so OK?
			while ( oldfile.get(ch) )
				newfile.put(ch);

			newfile.flush();
			newfile.close();
			oldfile.close();
			SetCursor(hcOld);
		}
	}
}

void TMIDIFile::AddTempo(ULONG delta, ULONG tempo)
{
	WriteVarLen(delta);

	// FF 51 03 is the flag for Tempo
	WriteOut(0xFF5103L, BITS24);

	// tempo is a 24-bit # that is in microseconds per MIDI quarter note
	WriteOut( 60000000L / (long) tempo, BITS24);
}

TMIDIFile::TimeSigMap TMIDIFile::AddTimeSig(ULONG delta, UINT num, UINT den, UINT position)
{
	// standard function to add time signatures
	WriteVarLen(delta);
	// FF 58 is the flag for Time Signature
	WriteOut( 0xFF5804L, BITS24);
	WriteVarLen(num);  // write the numerator straight out
	// denominator is x where x is 2 to the negative x power
	WriteVarLen((UINT) ( (double) log(den) / (double) log(2) ) );
	WriteVarLen(96/den);  // ticks per quarter note
	WriteVarLen(8);    // 32nd notes per quarter note

	// add to time signature map
	TimeSigMap temp;
	temp.Measure = dTrack[position].StartMeasure;
	temp.Ratio   = (double) num / (double) den;
	return temp;
}

BYTE TMIDIFile::GetNote()
{
	UINT nsum = SumNote();

	if ( nsum == 0 )
		return UCHAR_MAX;  // dead end

	UINT sum = 0, test = 1 + (rand() % nsum), q = 0;

	int max = tempTrack.Note.count();

	if (nProbType == CONDITIONAL)  {  // need to find start of last note
		// first find the start of the lastnote in the temp array
		for (register int i = 0; i<tempTrack.Note.count() && tempTrack.Note[i].From < lastnote; i++);
		q = max = i;
		// CMC -- bug fix (PRECONDITION VIOLATED) -- 8/11/93 -- added max<tempTrack
		while ( max < tempTrack.Note.count()  &&  tempTrack.Note[max].From == lastnote )
			max++;   // find upper limit of how many notes there are FROM
	}

	while (sum<test && q < max )  {
		sum += tempTrack.Note[q].Prob;
		q++;
	}
	--q;

	lastnote = tempTrack.Note[q].To;

	return (BYTE) ( (36 + lastnote) + ( 12 * (tempTrack.Octave - 2)));
}

UINT TMIDIFile::GetRhythm()
{
	UINT rsum = SumRhythm();

	if ( rsum == 0 ) 
		return UINT_MAX;  // dead end, nothing to choose

	UINT sum = 0, test = 1 + (rand() % rsum), q = 0;

	int max = tempTrack.Rhythm.count();

	if (nProbType == CONDITIONAL)  {  // need to find start of last note
		// first find the start of the lastnote in the temp array
		for (register int i = 0; i<tempTrack.Rhythm.count() && tempTrack.Rhythm[i].From < lastrhythm; i++);
		q = max = i;
		// CMC -- bug fix (PRECONDITION VIOLATED) -- 8/11/93 -- added max<tempTrack
		while ( max < tempTrack.Rhythm.count() && tempTrack.Rhythm[max].From == lastrhythm )
			max++;   // find upper limit of how many notes there are FROM
	}

	while (sum<test && q < max )  {
		sum += tempTrack.Rhythm[q].Prob;
		q++;
	}
   --q;

	lastrhythm = tempTrack.Rhythm[q].To;  // set for conditional

	switch(lastrhythm)  {
		case 0:  return 480;   // whole note
		case 1:  return 240;   // half  note
		case 2:  return 120;   // quarter note
		case 3:  return  60;   // eighth note
		case 4:  return  30;   // sixteenth note
		case 5:  return  15;   // thirty second note
		default: return 120;   // quarter note is the default
	}
}