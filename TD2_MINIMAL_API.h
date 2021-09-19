

#ifndef TD2_MINIMAL_API_h
    #define TD2_MINIMAL_API_h
        #include "json.hpp"
        using namespace std; using json = nlohmann::json;
        void loadTimetable(const json TTData, json& trainInfoVar, json& stopPointsVar) {
            trainInfoVar = TTData["trainInfo"];
            stopPointsVar = TTData["stopPoints"];
        }
        int buildArray(const json TInfo, const json RJ, const json localdb, string PointName[], bool pointNameisBold[], string ArrivalTimeWString[], string DepartureTimeWString[], int ArrivalDelay[], int DepartureDelay[], int DepartureLineTracks[], int &MaxStations, string &TrainString, string &TrainRouteString, double pointDistance[], string PointCodes[]){
            /* Out of date
            [0][x]:
                    0: Rodzaj i numer poci¹gu
                    1: Relacja
            [?][x]:
                    0: Punkt stacyjny
                    1: Czy nazwa punktu stacyjnego powinna byæ pogrubiona? (0/1) (wykrywanie ci¹gu '<strong>')
                    2: Godznia przyjazdu
                    3: Godznia odjazdu
                    4: Czas postoju
                    5: Godznia przyjazdu w minutach, u¿ywana do obliczenia czasu przejazdu
                    6: Czas przejazdu
                    7: Iloœæ torów na szlaku wyjazdowym (1/2)
            */

            
            TrainString = TInfo.value("trainCategoryCode", "") + to_string(TInfo.value("trainNo", 0));
            
            TrainRouteString = TInfo.value("route", "");
            int hhprev=0, mmprev=0;
            unsigned int i = 0;
            int breakloop = 0;
            while (breakloop == 0) {
                string NameRAW = RJ[i].value("pointNameRAW", "");
                PointName[i] = NameRAW;
                if (string::npos != RJ[i].value("pointName", "").find("<strong>")) { pointNameisBold[i] = 1; }
                else { pointNameisBold[i] = 0; }
                int hh, mm, ss;
                if (RJ[i]["arrivalTime"].is_null() == false) {
                    string Arrival = RJ[i].value("arrivalTime", "");
                    string ArrivalTime = Arrival.substr(Arrival.find("T") + 1, Arrival.find("Z") - 1 - Arrival.find("T"));
                    hh = stoi(ArrivalTime.substr(0, 2));
                    mm = stoi(ArrivalTime.substr(3, 2));
                    ss = stoi(ArrivalTime.substr(6, 2));
                    hh = hh + 2;

                    string smm;
                    string shh;

                    if (hh < 10) shh = ("0" + to_string(hh));
                    else shh = to_string(hh);

                    if (mm < 10) smm = ("0" + to_string(mm));
                    else smm = to_string(mm);

                    ArrivalTimeWString[i] = (shh + ":" + smm); //Godznia przyjazdu
                    ArrivalDelay[i] = (hh * 60 + mm) - (hhprev * 60 + mmprev); //czas postoju
                }
                else {
                    ArrivalTimeWString[i] = "-----";
                    ArrivalDelay[i] = 0;
                    hh = 0;
                    mm = 0;
                    ss = 0;
                }

                int Dhh, Dmm, Dss;
                
                if (RJ[i]["departureTime"].is_null() == false) {
                    string Dep = RJ[i].value("departureTime", "");
                    string DepTime = Dep.substr(Dep.find("T") + 1, Dep.find("Z") - 1 - Dep.find("T"));
                    Dhh = stoi(DepTime.substr(0, 2));
                    Dmm = stoi(DepTime.substr(3, 2));
                    Dss = stoi(DepTime.substr(6, 2));
                    Dhh = Dhh + 2;

                    string smm;
                    string shh;

                    if (Dhh < 10) shh = ("0" + to_string(Dhh));
                    else shh = to_string(Dhh);

                    if (Dmm < 10) smm = ("0" + to_string(Dmm));
                    else smm = to_string(Dmm);

                    DepartureTimeWString[i] = (shh + ":" + smm);
                    hhprev = Dhh;
                    mmprev = Dmm;
                    if (i!=0) {
                        DepartureDelay[i] = (Dhh * 60 + Dmm) - (hh * 60 + mm); //czas postoju
                    } else DepartureDelay[i] = 0;
                }
                else {
                    DepartureTimeWString[i] = "-----";
                    DepartureDelay[i] = 0;
                    Dhh = 0;
                    Dmm = 0;
                    Dss = 0;
                    breakloop = 1;
                }
                //Out of date:
                //DelayArray[i] = (hh * 60 + mm); // Godznia przyjazdu w minutach, u¿ywana do obliczenia czasu przejazdu
                //if (i > 0) { ArrivalDelay[i] = DelayArray[i] - (hh * 60 + mm); } else { ArrivalDelay[i] = -1; } // Czas przejazdu
                //RJArray[j][7] = RJ[i]["arrivalLine"].get <wstring>();
                //DepartureLineTracks[i] = localdb[NameRAW].value("departureLine", "");
                DepartureLineTracks[i] = 2;
                pointDistance[i] = RJ[i].value("pointDistance", DBL_MAX);
                PointCodes[i] = RJ[i].value("pointStopType", "");
                i = i + 1;
            }
            return i-1;
        }
#endif