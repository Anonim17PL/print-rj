

#pragma once
        #include "json.hpp"
        using namespace std; using json = nlohmann::json;

        void loadTimetable(const json TTData, json& trainInfoVar, json& stopPointsVar) {
            trainInfoVar = TTData["trainInfo"];
            stopPointsVar = TTData["stopPoints"];
        }
        int buildArray(const json TInfo, const json RJ, const json localdb, string PointName[], bool pointNameisBold[], string ArrivalTimeWString[], string DepartureTimeWString[], int ArrivalDelay[], int DepartureDelay[], int DepartureLineTracks[], int &MaxStations, string &TrainString, string &TrainRouteString, double pointDistance[], string PointCodes[], bool skipSBL){  
            TrainString = TInfo.value("trainCategoryCode", "") + to_string(TInfo.value("trainNo", 0));
            TrainRouteString = TInfo.value("route", "");
            int hhprev=0, mmprev=0, offset=0, nextday=0;
            unsigned int i = 0;
            int breakloop = 0;
            while (breakloop == 0) {
                string NameRAW = RJ[i].value("pointNameRAW", "");
                if (skipSBL==true) {
                    while (NameRAW.substr(0, 3) == "SBL" || NameRAW.substr(0, 3) == "sbl") {
                        i++;
                        offset--;
                        NameRAW = RJ[i].value("pointNameRAW", "");
                    }
                }
                PointName[i+offset] = NameRAW;
                if (string::npos != RJ[i].value("pointName", "").find("<strong>")) { pointNameisBold[i + offset] = 1; }
                else { pointNameisBold[i + offset] = 0; }
                int hh, mm, ss, timeam=0, timedm=0;
                if (RJ[i]["arrivalTime"].is_null() == false) {
                    string Arrival = RJ[i].value("arrivalTime", "");
                    string ArrivalTime = Arrival.substr(Arrival.find("T") + 1, Arrival.find("Z") - 1 - Arrival.find("T"));
                    hh = stoi(ArrivalTime.substr(0, 2));
                    mm = stoi(ArrivalTime.substr(3, 2));
                    ss = stoi(ArrivalTime.substr(6, 2));
                    hh = hh + 2;
                    if (hh >= 24) hh = hh - 24;
                    timeam = hh * 60 + mm;
                    string smm;
                    string shh;

                    if (hh < 10) shh = ("0" + to_string(hh));
                    else shh = to_string(hh);

                    if (mm < 10) smm = ("0" + to_string(mm));
                    else smm = to_string(mm);

                    ArrivalTimeWString[i + offset] = (shh + ":" + smm); //Godznia przyjazdu
                    int arrdel = timeam - mmprev;
                    if (arrdel < 0) arrdel = arrdel + 1440;
                    ArrivalDelay[i + offset] = arrdel; //czas postoju
                }
                else {
                    ArrivalTimeWString[i + offset] = "-----";
                    ArrivalDelay[i + offset] = 0;
                    hh = 0;
                    mm = 0;
                    ss = 0;
                }

                int Dhh, Dmm, Dss;
                //MessageBox(NULL, (LPCWSTR)L"dep in", (LPCWSTR)L"DEBUG", 0x00000000L);
                if (RJ[i]["departureTime"].is_null() == false) {
                    string Dep = RJ[i].value("departureTime", "");
                    string DepTime = Dep.substr(Dep.find("T") + 1, Dep.find("Z") - 1 - Dep.find("T"));
                    Dhh = stoi(DepTime.substr(0, 2));
                    Dmm = stoi(DepTime.substr(3, 2));
                    Dss = stoi(DepTime.substr(6, 2));
                    Dhh = Dhh + 2;
                    string smm;
                    string shh;
                    if (Dhh >= 24) Dhh = Dhh - 24;
                    int timedm = Dhh * 60 + Dmm;
                    if (Dhh < 10) shh = ("0" + to_string(Dhh));
                    else shh = to_string(Dhh);

                    if (Dmm < 10) smm = ("0" + to_string(Dmm));
                    else smm = to_string(Dmm);

                    DepartureTimeWString[i + offset] = (shh + ":" + smm);
                    mmprev = timedm;
                    int depdel = timedm - timeam;
                    if (depdel < 0) depdel = depdel + 1440;
                    if (i!=0) {
                        DepartureDelay[i + offset] = depdel; //czas postoju
                    } else DepartureDelay[i + offset] = 0;
                }
                else {
                    DepartureTimeWString[i + offset] = "-----";
                    DepartureDelay[i + offset] = 0;
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
                DepartureLineTracks[i + offset] = 2;
                pointDistance[i + offset] = RJ[i].value("pointDistance", DBL_MAX);
                PointCodes[i + offset] = RJ[i].value("pointStopType", "");
                i = i + 1;
            }
            return i-1 + offset;
        }
