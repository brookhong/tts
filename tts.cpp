/*
 * Copyright (c) 2008-2013 Brook Hong (hzgmaxwell <at> hotmail.com)
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Build:
 *      cl tts.cpp lame.lib libmp3lame-static.lib mpglib-static.lib
 */
#include <conio.h>
#include <sapi.h>
#include <sphelper.h>
#include <vector>
#include <queue>
#include <string>
#include <exception>
extern "C"
{
    int wav2mp3(char*inPath,char*outPath);
}
using namespace std;

class SPFORMAT
{
    SPSTREAMFORMAT m_val;
    string m_str;
    DWORD m_bytePS;
    SPFORMAT(SPSTREAMFORMAT vl,const char * sz)
    {
        m_val = vl;
        m_str = sz;
        string sChannel = string(sz).substr(15);
        DWORD sampleRate,bitRate;
        sscanf(sz,"SPSF_%ukHz%uBit",&sampleRate,&bitRate);
        if(sampleRate == 22)
            sampleRate = 22050;
        else if(sampleRate == 44)
            sampleRate = 44100;
        else
            sampleRate *= 1000;
        m_bytePS = sampleRate*bitRate/8;
        if(sChannel == "Stereo")
            m_bytePS *= 2;
    }
    static vector<SPFORMAT> m_vctsFMT;
    public:
    static void init()
    {
        m_vctsFMT.push_back(SPFORMAT(SPSF_12kHz16BitStereo,"SPSF_12kHz16BitStereo"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_16kHz16BitMono,"SPSF_16kHz16BitMono"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_16kHz16BitStereo,"SPSF_16kHz16BitStereo"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_22kHz16BitMono,"SPSF_22kHz16BitMono"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_22kHz16BitStereo,"SPSF_22kHz16BitStereo"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_24kHz16BitStereo,"SPSF_24kHz16BitStereo"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_32kHz16BitStereo,"SPSF_32kHz16BitStereo"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_44kHz16BitMono,"SPSF_44kHz16BitMono"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_44kHz16BitStereo,"SPSF_44kHz16BitStereo"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_48kHz16BitMono,"SPSF_48kHz16BitMono"));
        m_vctsFMT.push_back(SPFORMAT(SPSF_48kHz16BitStereo,"SPSF_48kHz16BitStereo"));
    }
    static SPSTREAMFORMAT getValByIdx(int i)
    {
        return m_vctsFMT[i].m_val;
    }
    static void display()
    {
        int i;
        for(i=0;i<m_vctsFMT.size();i++)
        {
            printf("%d %s\n",i,m_vctsFMT[i].m_str.c_str());
        }
    }
    static SPSTREAMFORMAT fromStr(const string& str)
    {
        int i;
        for(i=0;i<m_vctsFMT.size();i++)
        {
            if(str == m_vctsFMT[i].m_str)
                return m_vctsFMT[i].m_val;
        }
        return SPSF_Default;
    }
    static string fromVal(SPSTREAMFORMAT m_val)
    {
        int i;
        for(i=0;i<m_vctsFMT.size();i++)
        {
            if(m_val == m_vctsFMT[i].m_val)
                return m_vctsFMT[i].m_str;
        }
        return "";
    }
    static DWORD rateFromVal(SPSTREAMFORMAT m_val)
    {
        int i;
        for(i=0;i<m_vctsFMT.size();i++)
        {
            if(m_val == m_vctsFMT[i].m_val)
                return m_vctsFMT[i].m_bytePS;
        }
        return 1;
    }
};
vector<SPFORMAT> SPFORMAT::m_vctsFMT;
//end of class SPFORMAT

class VoicesLib
{
    vector<ISpObjectToken*> m_vctsVoices;
    public:
    VoicesLib()
    {
        IEnumSpObjectTokens* cpEnum;
        HRESULT hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
        ULONG i = 0, ulCount = 0;
        hr = cpEnum->GetCount(&ulCount);

        ISpObjectToken* tok;
        while (SUCCEEDED(hr) && i<ulCount)
        {
            hr = cpEnum->Next( 1, &tok, NULL );
            m_vctsVoices.push_back(tok);
            i++;
        }
        cpEnum->Release();
    }
    ISpObjectToken* operator [] (size_t i)
    {
        if(i<m_vctsVoices.size())
            return m_vctsVoices[i];
        else
            throw out_of_range("out of range");
    }
    size_t size()
    {
        return m_vctsVoices.size();
    }
    void display()
    {
        WCHAR* szDesc;
        for(size_t i=0;i<m_vctsVoices.size();i++)
        {
            SpGetDescription(m_vctsVoices[i],&szDesc);
            wprintf(L"%d %s\n",i,szDesc);
        }
    }
    ~VoicesLib()
    {
        /*
           for(size_t i=0;i<m_vctsVoices.size();i++)
           {
           m_vctsVoices[i]->Release();
           }
           */
    }
};
//end of class VoicesLib

SPSTREAMFORMAT getCurrentFormat(ISpVoice* pVoice)
{
    SPSTREAMFORMAT fmt = SPSF_Default;
    CComPtr<ISpStreamFormat> cpStream;
    HRESULT hr = pVoice->GetOutputStream(&cpStream);

    CSpStreamFormat Fmt;
    if (hr == S_OK)
    {
        hr = Fmt.AssignFormat(cpStream);
        if (SUCCEEDED(hr))
        {
            fmt = Fmt.ComputeFormatEnum();
        }
    }
    return fmt;
}
void displayUsage(char* exeName)
{
    printf("%s [options] [text_to_read]\n"
            "\t-h\t show this message.\n"
            "\t-I\t interactive mode.\n"
            "\t-F\t list all formats supported.\n"
            "\t-V\t list all voices installed.\n"
            "\t-f <format index>\t use the indexed format.\n"
            "\t-v <voice index>\t use the indexed voice.\n"
            "\t-i <filename>\t read from the file.\n"
            "\t-o <filename>\t output to the .wav file.\n"
            "\t-l \t output .lrc files.\n"
            "\t-t \t convert to .mp3 file.\n"
            "\t-s <minutes>\t split the output every X minutes, default by 5.\n"
            ,exeName);
}
class TimeOffSet
{
    protected:
        FILE *m_fLRC;
        int m_nLimit;
        TimeOffSet(int limit)
        {
            m_nLimit = limit;
            m_fLRC = stdout;
        }
        bool fromMillSecond(DWORD dwMillSecond,const string& text)
        {
            int second = (dwMillSecond/100)%60;
            int min = (dwMillSecond/100/60)%60;
            int hour = dwMillSecond/100/60/60;
            char buf[12];
            sprintf(buf,"[%.2d:%.2d.%.2d]",min,second,dwMillSecond%100);
            string s = buf;
            fprintf(m_fLRC,"%s%s\n",s.c_str(),text.c_str());
            if(m_nLimit == -1)
                return false;
            return (min >= m_nLimit);
        }
    public:
        virtual bool printLRC(const string& textReading) = 0;
        virtual ~TimeOffSet()
        {
            fflush(m_fLRC);
            fclose(m_fLRC);
        }
};
class TimeOffSetByTick : public TimeOffSet
{
    DWORD m_dwStart;
    public:
    TimeOffSetByTick(int limit) : TimeOffSet(limit)
    {
        m_dwStart = GetTickCount();
    }
    bool printLRC(const string& textReading)
    {
        DWORD dw = GetTickCount();
        return fromMillSecond((dw-m_dwStart)/10,textReading);
    }
};
class TimeOffSetByStat : public TimeOffSet
{
    DWORD m_dwBytePS;
    ISpStream*  m_wavStream;
    STATSTG m_stat;
    CSpStreamFormat m_format;
    string m_sFileName;
    DWORD m_dwSuffix;
    ISpVoice* m_pVoice;
    bool m_toLRC;
    bool m_bMP3;
    public:
    TimeOffSetByStat(const string& fileName,ISpVoice* pVoice,
            DWORD rate, int limit, bool toLRC, bool bMP3) : TimeOffSet(limit)
    {
        m_pVoice = pVoice;
        m_toLRC = toLRC;
        m_bMP3 = bMP3;
        m_dwBytePS = rate;
        m_dwSuffix = 0;
        m_sFileName = fileName;
        size_t base = m_sFileName.find(".wav");
        if(base == string::npos)
            base = m_sFileName.find(".WAV");
        if(base != string::npos)
            m_sFileName = m_sFileName.substr(0,base);
        char sz[10];
        sprintf(sz,"%d",m_dwSuffix++);
        string s = m_sFileName + sz;
        s += ".wav";

        USES_CONVERSION;
        WCHAR szWFileName[255];
        wcscpy( szWFileName, T2W(s.c_str()));

        CComPtr<ISpStreamFormat> cpOldStream;
        HRESULT hr = m_pVoice->GetOutputStream( &cpOldStream );
        hr = m_format.AssignFormat(cpOldStream);
        hr = SPBindToFile( szWFileName, 
                SPFM_CREATE_ALWAYS, 
                &m_wavStream, 
                &m_format.FormatId(),
                m_format.WaveFormatExPtr() ); 
        if(FAILED(hr))
            printf("SPBindToFile failed\n");
        else
            m_pVoice->SetOutput( m_wavStream, TRUE );

        if(m_toLRC)
        {
            s = m_sFileName + sz;
            s += ".lrc";
            m_fLRC = fopen(s.c_str(),"w");
        }
    }
    ~TimeOffSetByStat()
    {
        m_pVoice->WaitUntilDone( INFINITE );
        m_wavStream->Release();

        if(m_bMP3)
        {
            m_pVoice->SetOutput( NULL, TRUE );

            char sz[10];
            sprintf(sz,"%d",m_dwSuffix-1);
            string s = m_sFileName + sz;
            s += ".wav";
            string sMp3 = m_sFileName+sz+".mp3";
            printf("%s-->%s\n",s.c_str(),sMp3.c_str());
            int ret = wav2mp3((char*)s.c_str(),(char*)sMp3.c_str());
            printf("%s-->%s: %d\n",s.c_str(),sMp3.c_str(),ret);
            DeleteFile((char*)s.c_str());
        }
    }
    bool printLRC(const string& textReading)
    {
        m_wavStream->Stat(&m_stat,STATFLAG_NONAME);
        float f = (float)(m_stat.cbSize.LowPart)/(float)m_dwBytePS;

        bool splitHit = fromMillSecond(f*100,textReading);
        if(splitHit)
        {
            m_pVoice->WaitUntilDone( INFINITE );
            m_wavStream->Release();

            char sz[10];

            sprintf(sz,"%d",m_dwSuffix++);
            string s = m_sFileName + sz;
            s += ".wav";

            USES_CONVERSION;
            WCHAR szWFileName[255];
            wcscpy( szWFileName, T2W(s.c_str()));
            SPBindToFile( szWFileName, 
                    SPFM_CREATE_ALWAYS, 
                    &m_wavStream, 
                    &m_format.FormatId(),
                    m_format.WaveFormatExPtr() ); 
            m_pVoice->SetOutput( m_wavStream, TRUE );

            if(m_toLRC)
            {
                s = m_sFileName + sz;
                s += ".lrc";
                m_fLRC = fopen(s.c_str(),"w");
            }

            if(m_bMP3)
            {
                sprintf(sz,"%d",m_dwSuffix-2);
                s = m_sFileName + sz;
                s += ".wav";
                string sMp3 = m_sFileName+sz+".mp3";
                int ret = wav2mp3((char*)s.c_str(),(char*)sMp3.c_str());
                printf("%s-->%s: %d\n",s.c_str(),sMp3.c_str(),ret);
                DeleteFile((char*)s.c_str());
            }
        }
        return splitHit;
    }
};

class ReadingTask
{
    bool m_bStop;

    size_t prepareText(string& s)
    {
        static string specialChars = ".;!,";
        size_t specialPos = string::npos;
        size_t start = s.find_first_not_of(" \n\r\".;!,");
        if(start == string::npos)
            s = "";
        else
        {
            size_t end = s.find_last_not_of(" \n\r");
            s = s.substr(start,end-start+1);
            size_t i, len = s.length();
            for(i=0;i<len;i++)
            {
                if(s[i] == '\r' || s[i] == '\n')
                    s[i] = ' ';
                //else if(i>20 && specialPos == string::npos
                else if(specialPos == string::npos
                        && specialChars.find(s[i]) != string::npos)
                    specialPos = i;
            }
            s += " ";
        }
        return specialPos;
    }
    protected:
    ISpVoice* m_pVoice;
    TimeOffSet* m_pTimeOffSet;
    void speakText(const string& textToRead)
    {
        wchar_t* wbuf;
        size_t nu = textToRead.length();
        size_t n = ::MultiByteToWideChar(CP_ACP,0,(const char *)textToRead.c_str(),int(nu),NULL,0);
        wbuf= new wchar_t[n+1];
        ::MultiByteToWideChar(CP_ACP,0,(const char *)textToRead.c_str(),int(nu),wbuf,int(n));
        wbuf[n] = 0;

        m_pVoice->Speak(wbuf, SPF_ASYNC, NULL);
    }
    virtual string getTextToRead()
    {
        return "";
    }
    virtual void onStop()
    {
    }
    virtual void onEOF(string& textToRead)
    {
        m_pTimeOffSet->printLRC(textToRead);
        speakText(textToRead);
        m_pVoice->WaitUntilDone( INFINITE );
        m_bStop = true;
    }
    static void __stdcall NotifyCallbackFunction(WPARAM wParam, LPARAM lParam)
    {
        printf("NotifyCallbackFunction\n");
        ((ReadingTask*)lParam)->callBack();
    }
    void callBack()
    {
        CSpEvent event;
        while( event.GetFrom(m_pVoice) == S_OK )
        {
            printf("%d\n",event.eEventId);
        }
    }
    public:
    ReadingTask(ISpVoice* pVoice, TimeOffSet* pTimeOffSet)
    {
        m_pVoice = pVoice;
        m_pTimeOffSet = pTimeOffSet;
        m_bStop = false;
        HRESULT hr = m_pVoice->SetNotifyCallbackFunction(NotifyCallbackFunction,0,(LPARAM)this);
        if(FAILED(hr))
            printf("%x\n",hr);
        hr = m_pVoice->SetInterest( SPFEI_ALL_TTS_EVENTS, SPFEI_ALL_TTS_EVENTS );
        if(FAILED(hr))
            printf("%x\n",hr);
    }
    virtual void run()
    {
        int ch;
        string newIn = "";
        size_t period = string::npos;
        string textToRead;

        while(!m_bStop)
        {
            //check if the previous reading has finished 
            if(m_pVoice->WaitUntilDone(800) == S_OK)
            {
                period = prepareText(newIn);
                if(period != string::npos)
                {
                    textToRead = newIn.substr(0,period+1);
                    newIn = newIn.substr(period+1);

                    m_pTimeOffSet->printLRC(textToRead);
                    speakText(textToRead);
                }
                else
                {
                    string s = getTextToRead();
                    if(s != "") 
                    {
                        prepareText(s);
                        newIn = newIn+s;
                    }
                    else
                        onEOF(newIn);
                }
            }
            if(_kbhit())
            {
                ch = _getch();
                switch(ch)
                {
                    case 'x':
                        m_bStop = true;
                        onStop();
                        break;
                    case 'p':
                        m_pVoice->Pause();
                        break;
                    case 'r':
                        m_pVoice->Resume();
                        break;
                }
            }//end kbhit
        }//end while
    }
    virtual ~ReadingTask()
    {
        m_pVoice->WaitUntilDone( INFINITE );
    }
};
class ReadingText: public ReadingTask
{
    string m_str;
    public:
    ReadingText(const string& str,ISpVoice* pVoice, TimeOffSet* pTimeOffSet) : ReadingTask(pVoice,pTimeOffSet)
    {
        m_str = str;
    }
    virtual void run()
    {
        speakText(m_str);
        m_pVoice->WaitUntilDone( INFINITE );
    }
};
class ReadingFile: public ReadingTask
{
    FILE* m_fIn;
    protected:
    string getTextToRead()
    {
        char buf[512];
        if(fgets(buf,sizeof(buf),m_fIn) != NULL)
            return string(buf);
        else
            return "";
    }
    public:
    ReadingFile(const string& fileName,ISpVoice* pVoice, TimeOffSet* pTimeOffSet) : ReadingTask(pVoice,pTimeOffSet)
    {
        m_fIn = fopen(fileName.c_str(), "r");
        if(m_fIn == NULL)
            throw runtime_error("bad file!");
    }
    ~ReadingFile()
    {
        fclose(m_fIn);
        printf("~ReadingFile\n");
    }
};
class ReadingClipBoard : public ReadingTask
{
    HANDLE m_hRunEvent;
    HANDLE m_hThread;
    HANDLE m_hMutex; 
    queue<string> m_queTextToRead;
    string m_strLast;
    static DWORD WINAPI monitorClipBoard(LPVOID lpParam)
    {
        ReadingClipBoard* pThis = (ReadingClipBoard*)lpParam;
        bool b;
        do
        {
            b = pThis->readClipBoard();
        }while(b);
        return 0;
    }
    bool readClipBoard()
    {
        string s;
        if (IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(NULL) ) 
        {
            HANDLE hData = GetClipboardData( CF_TEXT );
            s = (char*)GlobalLock( hData );
            GlobalUnlock( hData );
            CloseClipboard();
        }
        if(m_strLast != s)
        {
            WaitForSingleObject(m_hMutex,INFINITE);
            m_queTextToRead.push(s);
            m_strLast = s;
            ReleaseMutex(m_hMutex);
        }
        return (WaitForSingleObject(m_hRunEvent,100) == WAIT_TIMEOUT);
    }
    protected:
    string getTextToRead()
    {
        string s = "";
        WaitForSingleObject(m_hMutex,INFINITE);
        if(!m_queTextToRead.empty())
        {
            s = m_queTextToRead.front();
            m_queTextToRead.pop();
        }
        ReleaseMutex(m_hMutex);
        return s;
    }
    virtual void onStop()
    {
        SetEvent(m_hRunEvent);
    }
    virtual void onEOF(string& textToRead)
    {
        if(textToRead != "")
        {
            m_pTimeOffSet->printLRC(textToRead);
            speakText(textToRead);
            m_pVoice->WaitUntilDone( INFINITE );
            textToRead = "";
        }
        WaitForSingleObject(m_hRunEvent,100);
    }
    public:
    ReadingClipBoard(ISpVoice* pVoice, TimeOffSet* pTimeOffSet) : ReadingTask(pVoice,pTimeOffSet)
    {
        m_hMutex = CreateMutex( 
                NULL,                       // no security attributes
                FALSE,                      // initially not owned
                "MutexToProtectDatabase");  // name of mutex

        m_hRunEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
        DWORD dwThreadId; 
        m_hThread = CreateThread( 
                NULL,                        // no security attributes 
                0,                           // use default stack size  
                monitorClipBoard,            // thread function 
                this,                        // argument to thread function 
                0,                           // use default creation flags 
                &dwThreadId);                // returns the thread identifier 
        m_strLast = "";
    }
    ~ReadingClipBoard()
    {
        WaitForSingleObject(m_hThread,300);
        CloseHandle(m_hRunEvent);
        CloseHandle(m_hMutex);
        printf("~ReadingClipBoard\n");
    }
};
class MessageThread
{
    HANDLE m_hRunEvent;
    HANDLE m_hThread;
    bool isDone()
    {
        return (WaitForSingleObject(m_hRunEvent,100) != WAIT_TIMEOUT);
    }
    static DWORD WINAPI takingMessage(LPVOID lpParam)
    {
        printf("in takingMessage\n");
        MessageThread* pThis = (MessageThread*)lpParam;
        BOOL bRet;

        MSG msg;
        while(!pThis->isDone()
               && (bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
        { 
            printf("%d\n",msg.message);
            if (bRet == -1)
            {
                // handle the error and possibly exit
            }
            else
            {
                TranslateMessage(&msg); 
                DispatchMessage(&msg); 
            }
        }
        return 0;
    }
    DWORD m_dwThreadId; 
    public:
    MessageThread()
    {
        m_hRunEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
        DWORD dwThreadId; 
        m_hThread = CreateThread( 
                NULL,                        // no security attributes 
                0,                           // use default stack size  
                takingMessage,               // thread function 
                this,                        // argument to thread function 
                0,                           // use default creation flags 
                &m_dwThreadId);                // returns the thread identifier 

    }
    ~MessageThread()
    {
        SetEvent(m_hRunEvent);
        WaitForSingleObject(m_hThread,300);
        CloseHandle(m_hRunEvent);
        printf("~MessageThread\n");
    }
    DWORD getThreadID()
    {
        return m_dwThreadId;
    }
};

void interactiveRead(VoicesLib& vl, ISpVoice* pVoice)
{
    int choice = 0;
    char buf[1024];
    wchar_t* wbuf;
    do
    {
        vl.display();
        printf("Enter your choice: ");
        scanf("%d",&choice);
        if(choice >= vl.size())
            break;
        printf("Enter text you wish spoken here: ");
        fflush(stdin);
        fgets(buf,1024,stdin);
        size_t nu = strlen(buf);
        size_t n = ::MultiByteToWideChar(CP_ACP,0,(const char *)buf,int(nu),NULL,0);
        wbuf= new wchar_t[n+1];
        ::MultiByteToWideChar(CP_ACP,0,(const char *)buf,int(nu),wbuf,int(n));
        wbuf[n] = 0;

        HRESULT hr = pVoice->SetVoice(vl[choice]);
        if( SUCCEEDED( hr ) )
        {  
            hr = pVoice->Speak(wbuf, 0, NULL);
            if(FAILED(hr))
                printf("Speak failed\n");
        }
        delete [] wbuf;
    }while(1);
}

int main(int argc,char**argv)
{
    if(argc < 2)
    {
        displayUsage(argv[0]);
        return 0;
    }
    int i = 1;
    int voiceIndex = 0;
    string textToRead;
    SPFORMAT::init();
    if (FAILED(CoInitializeEx(NULL,COINIT_MULTITHREADED)))
    {
        printf("Error to intiliaze COM\n");
        return FALSE;
    }

    VoicesLib voices;
    ISpVoice * pVoiceSelected = NULL;
    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL,CLSCTX_ALL, IID_ISpVoice, (void **)&pVoiceSelected);

    ISpAudio* cpAudio;
    hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOOUT, &cpAudio);
    if(FAILED(hr))
        printf("SpCreateDefaultObjectFromCategoryId failed\n");

    string inFile = "";
    string outFile = "";
    TimeOffSet* pTimeOffSet = NULL;
    bool bLRCfile = false, bMP3 = false;
    //default splited every 5 minutes
    int splitMins = 5;
    ReadingTask* pTask = NULL;
    bool clipBoard = false;
    while(i<argc)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'V':
                    voices.display();
                    return 0;
                case 'F':
                    SPFORMAT::display();
                    return 0;
                case 'v':
                    if(++i < argc)
                        hr = pVoiceSelected->SetVoice(voices[atoi(argv[i])]);
                    else
                    {
                        displayUsage(argv[0]);
                        return 0;
                    }
                    break;
                case 'f':
                    if(++i < argc)
                    {
                        CSpStreamFormat Fmt;
                        Fmt.AssignFormat(SPFORMAT::getValByIdx(atoi(argv[i])));
                        hr = cpAudio->SetFormat( Fmt.FormatId(), Fmt.WaveFormatExPtr() );
                        if(FAILED(hr))
                            printf("SetFormat failed\n");
                        hr = pVoiceSelected->SetOutput( cpAudio, FALSE );
                        if(FAILED(hr))
                            printf("SetOutput failed\n");

                    }
                    else
                    {
                        displayUsage(argv[0]);
                        return 0;
                    }
                    break;
                case 'l':
                    bLRCfile = true;
                    break;
                case 't':
                    bMP3 = true;
                    break;
                case 'o':
                    if(++i < argc)
                        outFile = argv[i];
                    else
                    {
                        displayUsage(argv[0]);
                        return 0;
                    }
                    break;
                case 's':
                    if(++i < argc)
                        splitMins = atoi(argv[i]);
                    else
                    {
                        displayUsage(argv[0]);
                        return 0;
                    }
                    break;
                case 'I':
                    interactiveRead(voices,pVoiceSelected);
                    break;
                case 'i':
                    if(++i < argc)
                        inFile = argv[i];
                    else
                    {
                        displayUsage(argv[0]);
                        return 0;
                    }
                    break;
                case 'd':
                    clipBoard = true;
                    break;
                default:
                    displayUsage(argv[0]);
                    return 0;
            }
        }
        else
            textToRead = argv[i];
        i++;
    }
    //MessageThread m;
    try{
        if(outFile != "")
        {
            pTimeOffSet = new TimeOffSetByStat(outFile,pVoiceSelected,
                    SPFORMAT::rateFromVal(getCurrentFormat(pVoiceSelected)),splitMins,bLRCfile,bMP3);
        }
        else
            pTimeOffSet = new TimeOffSetByTick(splitMins);

        if(inFile != "")
            pTask = new ReadingFile(inFile,pVoiceSelected,pTimeOffSet);
        else if(clipBoard)
            pTask = new ReadingClipBoard(pVoiceSelected,pTimeOffSet);
        else
            pTask = new ReadingText(textToRead,pVoiceSelected,NULL);
        pTask->run();

        delete pTask;
        delete pTimeOffSet;
    }
    catch(runtime_error& e) {
        printf("%s\n",e.what());
    }

    pVoiceSelected->Release();
    pVoiceSelected = NULL;
    cpAudio->Release();

    CoUninitialize();
}
