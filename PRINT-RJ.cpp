// PRINT-RJ.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "TD2_MINIMAL_API.h"

#define CURL_VERBOSE 0
#define CURL_CA "cacert.pem"
#define CURL_USERAGENT "PRINT-RJ/1.0.0"
#define CURL_USERPWD ""
//#define CURL_USERPWD "USER:PASSWORD"
#define apiEndpoint "https://api.td2.info.pl:9640/?"

/*
                            PRINT-RJ 1.0.0

    Program for printing timetables from the Train Driver 2 game.
    The program supports dot matrix printers with Epson FX and IBM PPR emulation.
    Required Character Pitch: >=12 cpi.
    
*/

// Enable Double Height function (only Epson FX emulation)
#define DH_ON

// SELECT ONE!
//#define IBM_PPR_EMULATION
// OR
#define EPSON_FX_EMULATION

// DON'T USE!
/*Retrieving information about sections between sceneries from the sceneryExtTrack.json.
The file must be updated on an ongoing basis so that the data is not disassembled.*/
//#define DETECTION_DUAL_TRACK






#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "json.hpp"
#include "curl.h"

using namespace std;
using json = nlohmann::json;

string portname = "LPT1";

// Not used
#ifdef DETECTION_DUAL_TRACK
    #define DDT True
#else
    #define DDT False
#endif

#ifdef IBM_PPR_EMULATION
    #define boldon wstring(L"\u001BE")
    #define boldoff wstring(L"\u001BF")
    #define DHon wstring(L"")
    #define DHoff wstring(L"")
    #define SubSon wstring(L"\u001BS1")
    #define SubSoff wstring(L"\u001BT")
#endif

#ifdef EPSON_FX_EMULATION
    #define boldon wstring(L"\u001BE")
    #define boldoff wstring(L"\u001BF")
    #ifdef DH_ON
        #define DHon wstring(L"\u001Bw1")
        #define DHoff wstring(L"\u001Bw0")
    #else
        #define DHon wstring(L"")
        #define DHoff wstring(L"")
    #endif
    #define SubSon wstring(L"\u001BS1")
    #define SubSoff wstring(L"\u001BT")
#endif


std::wstring ReplaceAll(std::wstring str, const std::wstring& from, const std::wstring& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::wstring PL(wstring text)
{
    text = ReplaceAll(text, L"ą", L"\xa5");
    text = ReplaceAll(text, L"ć", L"\x86");
    text = ReplaceAll(text, L"ę", L"\xa9");
    text = ReplaceAll(text, L"ł", L"\x88");
    text = ReplaceAll(text, L"ń", L"\xe4");
    text = ReplaceAll(text, L"ó", L"\xa2");
    text = ReplaceAll(text, L"ś", L"\x98");
    text = ReplaceAll(text, L"ż", L"\xbe");
    text = ReplaceAll(text, L"ź", L"\xab");
    text = ReplaceAll(text, L"Ą", L"\xa4");
    text = ReplaceAll(text, L"Ć", L"\x8f");
    text = ReplaceAll(text, L"Ę", L"\xa8");
    text = ReplaceAll(text, L"Ł", L"\x9d");
    text = ReplaceAll(text, L"Ń", L"\xe3");
    text = ReplaceAll(text, L"Ó", L"\xe0");
    text = ReplaceAll(text, L"Ś", L"\x97");
    text = ReplaceAll(text, L"Ż", L"\xbd");
    text = ReplaceAll(text, L"Ź", L"\x8d");

    text = ReplaceAll(text, L"|", L" - ");
    return text;
}



wstring header = (
    wstring(L"\xda\xc4\xc4\xc2\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xbf" + wstring(L"\n")) +
    wstring(L"\xb3  \xb3  \xb3                                \xb3                   \xb3 LOK1 \xb3        \xb3        \xb3" + wstring(L"\n")) +
    wstring(L"\xb3  \xb3  \xb3                                \xb3                   \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xb4 Brutto \xb3  Vmax  \xb3" + wstring(L"\n")) +
    wstring(L"\xb3V"+SubSon+L"P"+SubSoff+L"\xb3V" + SubSon + L"P" + SubSoff + L"\xb3             "+DHon+L"Stacja" + DHoff + L"             \xb3      " + DHon + L"Godzina" + DHoff + L"      \xb3 LOK2 \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4" + wstring(L"\n")) +
    wstring(L"\xb3  \xb3  \xb3                                \xb3                   \xc3\xc4\xc4\xc4\xc4\xc4\xc4\xb4        \xb3        \xb3" + wstring(L"\n")) +
    wstring(L"\xb3  \xb3  \xb3                                \xb3                   \xb3 LOK3 \xb3" + DHon + L"\x44\x88.Poc." + DHoff + L" \xb3   " + DHon + L"%" + DHoff + L"    \xb3" + wstring(L"\n\0"))
    );

wstring footer = L"\xc0\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc1\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xd9\x0c";

wstring bodygenerator(wstring Fstring[], int nonfirstbody) {
    wstring body;
    if (nonfirstbody != 0) {
        body = (
            wstring(L"\xc3\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n") +
            wstring(L"\xb3     \xb3                                \xb3         \xb3         \xb3" + wstring(Fstring[3]) + L"\xb3        \xb3        \xb3\n") +
            wstring(L"\xb3 ___ \xb3" + wstring(Fstring[0]) + L"\xb3" + wstring(Fstring[1]) + L"\xb3" + wstring(Fstring[2]) + L"\xc3\xc4\xc4\xc4\xc4\xc4\xc4\xb4" + wstring(Fstring[4]) + L"\xb3" + wstring(Fstring[5]) + L"\xb3\n") +
            wstring(L"\xb3     \xb3                                \xb3         \xb3         \xb3" + wstring(Fstring[7]) + L"\xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n") +
            wstring(L"\xb3     \xb3                                \xb3" + wstring(Fstring[9]) + L"\xb3" + Fstring[10] + L"\xc3\xc4\xc4\xc4\xc4\xc4\xc4\xb4        \xb3        \xb3\n") +
            wstring(L"\xb3     \xb3" + wstring(Fstring[8]) + L"\xb3         \xb3         \xb3" + wstring(Fstring[11]) + L"\xb3" + wstring(Fstring[12]) + L"\xb3        \xb3\n")
            );
    }
    else {
        body = (
            wstring(L"\xc3\xc4\xc4\xc1\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc2\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n") +
            wstring(L"\xb3     \xb3                                \xb3         \xb3         \xb3" + wstring(Fstring[3]) + L"\xb3        \xb3        \xb3\n") +
            wstring(L"\xb3 ___ \xb3" + wstring(Fstring[0]) + L"\xb3" + wstring(Fstring[1]) + L"\xb3" + wstring(Fstring[2]) + L"\xc3\xc4\xc4\xc4\xc4\xc4\xc4\xb4" + wstring(Fstring[4]) + L"\xb3" + wstring(Fstring[5]) + L"\xb3\n") +
            wstring(L"\xb3     \xb3                                \xb3         \xb3         \xb3" + wstring(Fstring[7]) + L"\xc3\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc5\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xc4\xb4\n") +
            wstring(L"\xb3     \xb3                                \xb3" + wstring(Fstring[9]) + L"\xb3" + Fstring[10] + L"\xc3\xc4\xc4\xc4\xc4\xc4\xc4\xb4        \xb3        \xb3\n") +
            wstring(L"\xb3     \xb3" + wstring(Fstring[8]) + L"\xb3         \xb3         \xb3" + wstring(Fstring[11]) + L"\xb3" + wstring(Fstring[12]) + L"\xb3        \xb3\n")
            );
    };

    return body;
}

wstring centerstring(wstring ws,int size)
{
    wstring output;
    const wchar_t* s = ws.c_str();
    int l = wcslen(s);
    int pos = (int)((size - l) / 2);
    int prepos=0;
    for (int i = 0; i < pos; i++) {
        output += L" ";
        prepos++;
    }
    output += s;
    for (int i = 0; i < (int)(size - l - prepos); i++)
        output += L" ";
    return output;
}

wstring toleftstring(wstring ws, int size)
{
    wstring output;
    const wchar_t* s = ws.c_str();
    int l = wcslen(s);
    int add = size - l;
    output += s;
    for (int i = 0; i < (int)add; i++)
        output += L" ";
    return output;
}

int bodyWStringArrayGenerator(const wstring stationName, const bool stationNameBolded, const wstring Number, const wstring Locomotive1, const wstring Locomotive2, const wstring Locomotive3, const wstring brutto, const wstring trainlength, const wstring Vmax, const wstring distance, const wstring TimeArr, const wstring TimeDep, const wstring ArrDel, const wstring DepDel, const wstring StationInfo, wstring returndata[]) {
    //wstring data[12];
    //string data[12];
    //wstring data[13];

    if (stationNameBolded) {
        
    }
    else {
        //data[0] << SubSon << stationName << SubSoff;
    }
    returndata[0] = toleftstring(stationName,32);
    returndata[1] = centerstring(TimeArr,9);
    returndata[2] = centerstring(ArrDel,9);
    returndata[3] = centerstring(Locomotive1,6);
    returndata[4] = centerstring(brutto,8);
    returndata[5] = centerstring(Vmax,8);
    returndata[6] = centerstring(Number,19);

    if (Locomotive2 == L"x") returndata[7] = centerstring(L" ", 6);
    else returndata[7] = centerstring(L" ", 6);
    returndata[9] = centerstring(TimeDep,9);
    returndata[10] = centerstring(DepDel,9);
    if (Locomotive3 == L"x") returndata[11] = centerstring(L" ", 6);
    else returndata[11] = centerstring(L" ", 6);
    returndata[12] = centerstring(trainlength,8);

    int spaces = 32 - (distance.length() + StationInfo.length());
    returndata[8] = distance;
    for (int i = 0; i < spaces; i++) {
        returndata[8] += L" ";
    }
    returndata[8] += StationInfo;

    for (int i = 0; i <= 12; i++) {
        returndata[i] = PL(returndata[i]);
    }
    if (stationNameBolded)
            returndata[0] = boldon + returndata[0] + boldoff;

    returndata[0] = DHon + returndata[0] + DHoff;
    returndata[4] = DHon + returndata[4] + DHoff;
    returndata[5] = DHon + returndata[5] + DHoff;
    returndata[8] = DHon + returndata[8] + DHoff;
    returndata[12] = DHon + returndata[12] + DHoff;
    returndata[1] = DHon + returndata[1] + DHoff;
    returndata[2] = DHon + returndata[2] + DHoff;
    returndata[9] = DHon + returndata[9] + DHoff;
    returndata[10] = DHon + returndata[10] + DHoff;

    return 0;


}



size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}


std::wstring utf8_decode(const std::string& str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string utf16_encode(const std::wstring& wstr)
{
    int size_needed = WideCharToMultiByte(1200, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(1200, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

json getTimetable(string trainNo,string svr = "eu") {
    string response_string;
    auto curl = curl_easy_init();
    if (curl) {
        string StationsURL = apiEndpoint + (string)"method=readFromSWDR&value=getTimetable;" + (string)trainNo + (string)";" + svr;
        curl_easy_setopt(curl, CURLOPT_URL, StationsURL);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, CURL_USERPWD);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, CURL_USERAGENT);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, CURL_VERBOSE);
        curl_easy_setopt(curl, CURLOPT_CAINFO, CURL_CA);

        CURLcode result = curl_easy_perform(curl);
        json j = json::parse(response_string);

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        curl = NULL;
        return j["message"];
    }
    return NULL;
}

void getTrainInfo(string trainNo, string datareturn[]) {
    string response_string;
    auto curl = curl_easy_init();
    if (curl) {
        string TrainURL = apiEndpoint + (string)"method=getTrainsOnline";
        curl_easy_setopt(curl, CURLOPT_URL, TrainURL);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, CURL_USERPWD);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, CURL_USERAGENT);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, CURL_VERBOSE);
        curl_easy_setopt(curl, CURLOPT_CAINFO, CURL_CA);

        CURLcode result = curl_easy_perform(curl);
        json j = json::parse(response_string);

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        curl = NULL;
        
        for (int i = 0; i < (int)9999; i++) {
            if (j["message"][i].is_null() == true) break;
            if (j["message"][i].value("trainNo", 111111) == stoi(trainNo)) {
                int dataMass = j["message"][i].value<int>("dataMass", INT_MAX);
                int dataLength  = j["message"][i].value<int>("dataLength", INT_MAX);
                datareturn[0] = "";
                datareturn[1] = to_string(int(dataMass / 1000));
                datareturn[2] = to_string(dataLength);
                break;
            }
        }
    }
}



json load_db() {
    json j;
    string scn_db;
    std::ifstream i("sceneryExtTrack.json", ifstream::binary);
    cout << "Loading updateStationsLinks.database ...";
    cout << " OK" << endl;
    return j;
}





void PrintHeader(wstring HeaderInfo, wstring TrainString) {
    wofstream printer(portname);
    printer << TrainString << L"\r\n";
    printer << HeaderInfo << L"\r\n" << L"\00";


};

void PrintBody(wstring BodyInfo) {
    wofstream printer(portname);
    printer << BodyInfo << L"\r\n" << L"\00";
};

void PrintFooter() {
    wofstream printer(portname);
    printer << footer << L"\r\n" << L"\00";
};

int main(int pn, char** ps)
{
    if (pn >= 2) portname = ps[1];
    cout << "Print using port:" << portname << endl;
    cin;
    json RJ;
    json TInfo;
    //json localdb = load_db();
    json localdb;
    json test;
    //ifstream json_file("testRJ.json", std::ifstream::binary);
    //json json_filevar;
    //json_file >> json_filevar;
    string TrainNo;
    cout << "TRAIN NUMBER: ";
    cin >> TrainNo;
    cout << endl;
    string TrainData[3];
    bool pointnameBold[99];
    double pointDistance[99];
    string pointname[99], pointcodes[99], ArrTime[99], DepTime[99], Train, TRS;
    int ArrDel[99], DepDel[99], DepLT[99], MaxSt;
    loadTimetable(getTimetable(TrainNo), TInfo, RJ);
    //loadTimetable(json_filevar["message"], TInfo, RJ);
    getTrainInfo(TrainNo,TrainData);

    wstring Locomotive1 = L"";
    wstring Locomotive2 = L"";
    wstring Locomotive3 = L"";
    wstring Vmax = L"";
    int maxdata = buildArray(TInfo, RJ, localdb, pointname, pointnameBold, ArrTime, DepTime, ArrDel, DepDel, DepLT, MaxSt, Train, TRS, pointDistance, pointcodes, true);
    cout << endl << "LOK1:";
    wcin >> Locomotive1;
    cout << endl << "LOK2:";
    wcin >> Locomotive2;
    cout << endl << "LOK3:";
    wcin >> Locomotive3;
    cout << endl << "Vmax(km/h)=";
    wcin >> Vmax;
    cout << maxdata << endl;

    wstring outputdataF;
    wofstream printer(portname);
    //wofstream printer("test1.txt");
    printer << L"\n \n \n" << boldon << PL(utf8_decode(Train + " Relacja " + TRS)) << L"\n" << boldoff << header;
    for (int i=0; i<=maxdata; i++){
        cout << i;
        wstring returndata[99];
        bodyWStringArrayGenerator(utf8_decode(pointname[i]), pointnameBold[i], utf8_decode(Train), (Locomotive1), Locomotive2, (Locomotive3), utf8_decode(TrainData[1]), utf8_decode(TrainData[2]), (Vmax), to_wstring(pointDistance[i]), utf8_decode(ArrTime[i]), utf8_decode(DepTime[i]), to_wstring(ArrDel[i]), to_wstring(DepDel[i]), utf8_decode(pointcodes[i]), returndata);
        wstring outputdata = bodygenerator(returndata,i);
        printer << outputdata;
        cout << " OK" << endl;
    }
    
    printer << footer;

}

/* The Bin ;)

    Required dependencies difficult to access: JSON file with station data (StationsLinks).
    Current "sceneryExtTrack.json" may be outdated.

*/