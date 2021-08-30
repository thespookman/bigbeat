# bigbeat
A commandline drum machine

## Build
Requires Bison and Flex, both available in the standard repos for (I think) most Linux repos.  
Run `make all`. Only uses standard C++17 libraries, so should just work _fingers crossed_.

## Usage
`bigbeat [options] -i beat_file`

### Options
```
        --help           - Display this help and exit
	--deps           - Also create a dependency file for make	
        -i beat_file     - File to read for beat data (mandatory)
        -o out_file      - File to output WAV to (default is output.wav)
        -s sample_rate   - Sets sample rate of output (default is 44100)
        -t bpm           - sets default tempo of output (default is 80)
        --24bit          - Output 24-bit WAV (default is 16)
```

## Beat file syntax

Beat files are made up of instrument definitions and modules. Whitespace is ignored, so feel free to lay out the files how you like. Definitions all end with a semicolon (`;`) or a closing brace (`}`).

Instrument definitions are used to import a sample from a wav file like so: `identifier volume URL;` where `identifier` is a name used to access the instrument in other parts of the file. It can contain letters, numbers or underscores, but the first character may not be a number. `volume` is a number, allowing the instrument's volume to be adjusted. Higher is louder, but only relative to other instruments, i.e. if your drum beat uses two samples, both with volume `2`, it will sound exactly the same as if both had volume `200`. An instrument with twice the volume of another will sound twice as loud. The maximum volume is `255`, any volumes greater than this will be treated as if they were `255`. `URL` is the path to the sample WAV file. This must begin with `./` or `../`, but otherwise may contain any characters.

Modules come in three forms: 

### Tempo
`identifier { tempo; }`, where `identifier` is a name used to call upon the module later, and `tempo` is an integer number of beats per minute.

### Pattern 
```
identifier granularity {
instrument1 velocity [velocity [...]];
instrument2 velocity [velocity [...]];
}
```
The `granularity` is the integer number of notes that are defined per beat, i.e. crotchet beats are `1`, quaver beats are `2` etc. Fractional granularities are not allowed. `instrument1` and `instrument2` should be replaced with identifiers set in instrument definitions. `velocity` is a number from `0` to `9`, signifying the strength of the beat, with `9` being full strength and `0` being the complete absense of a beat. `.` can be used in place of `0` for readability. A pattern can contain as many beats as needed, increasing the length of the pattern, i.e. assuming 4:4 time `beat 1 { drum 9 . 9 . 9 . 9 .;}` defines a pattern called `beat` that involves the instrument `drum` being played every two crotchet beats. In the (usual) case that there are multiple instruments in the pattern, they are played simultaneously, rather than one after the other.

### Compound
```
identifier {
module1;
module2 [repeats];
}
```
Compound modules allow modules to be combined into more complex structures. Each line is played one after the other. `repeat` is an integer and if supplied, the module will be repeated that many times. Compound modules can contain other compound modules.

### Include
Other beat files can be included with `include URL`, which must be followed by a new line. Any beat file syntax in the file referred to by the URL will be read as if it is part of the file. This allows the user to create libraries of beats or samples, to be reused in various songs or projects.

### Order
One feature of bigbeat is that the entire file is read before work is started building the final WAV file. This means that instruments and modules can be used in the file before they are defined. If multiple instruments or modules are defined using the same identifier, the latest will take precedence. This is useful, since a user who defines a file containing a set of drum samples for use in multiple projects can include it at the top of each file. If they decide that for one song they want to define one of the samples slightly differently, they can just redefine that instrument in the song file, rather than writing out a whole new set of instrument definitions.

### Song module
The beat file must contain a module called `Song`. This is where the track will start playing.

### Example
```
bass_drum 40 ./drum samples/bass.wav;
highhat 60 ./drum samples/high hat.wav;

tempo1 { 80; }
tempo2 { 120; }

intro 1 {
highhat 9 9 9 9;
}

verse 2 {
bass_drum 9 . 9 . 9 . 9 .;
highhat   . 9 . 9 . 9 . 9;
}

fill 2 {
bass_drum 9 . . 9 9 . . 9;
}

outro 3 {
highhat 9 9 9 9 8 7 6 5 4 3 2 1;
}

Song {
tempo1;
intro;
verse;
fill 2;
verse;
tempo2;
fill 2;
verse;
outro;
}
```
This example defines two drum samples, includes a change of tempo, and a combination of crotchets, quavers and triplets, with the highhat fading out at the end.

