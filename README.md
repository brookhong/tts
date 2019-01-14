A command line tool to read text with Microsoft Speech API (SAPI), written by Brook Hong in 2008.

Pre-built binary at http://sharing-from-brook.16002.n6.nabble.com/file/n4967530/tts.zip

# Usage
    tts [options] [text_to_read]
        -h       show this message.
        -I       interactive mode.
        -F       list all formats supported.
        -V       list all voices installed.
        -f <format index>        use the indexed format.
        -v <voice index>         use the indexed voice.
        -i <filename>    read from the file.
        -o <filename>    output to the .wav file.
        -l       output .lrc files.
        -t       convert to .mp3 file.
        -s <minutes>     split the output every X minutes, default by 5.

# Example
1 `tts.exe -F` to list all formats available on your computer.

    0 SPSF_12kHz16BitStereo
    1 SPSF_16kHz16BitMono
    2 SPSF_16kHz16BitStereo
    3 SPSF_22kHz16BitMono
    4 SPSF_22kHz16BitStereo
    5 SPSF_24kHz16BitStereo
    6 SPSF_32kHz16BitStereo
    7 SPSF_44kHz16BitMono
    8 SPSF_44kHz16BitStereo
    9 SPSF_48kHz16BitMono
    10 SPSF_48kHz16BitStereo

2 `tts.exe -V` to list all voices available on your computer.

    0 Microsoft Anna - English (United States)
    1 ATT DTNV1.4 Crystal16
    2 ATT DTNV1.4 Mike16
    3 ATT DTNV1.4 Audrey16
    4 Microsoft Lili - Chinese (China)
    5 VW Paul

3 now read text with format SPSF_48kHz16BitStereo and voice ATT DTNV1.4 Crystal16:

`tts.exe -f 10 -v 1 "look, here is brook"`

`tts.exe -f 10 -v 1 "look, here is brook" -o look` to generate look.wav instead of reading it.

`tts.exe -f 10 -v 1 "look, here is brook" -o look -t` to generate look.mp3 instead of reading it.

# Build

    cl tts.cpp lame.lib libmp3lame-static.lib mpglib-static.lib legacy_stdio_definitions.lib
