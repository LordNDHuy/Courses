#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <bits/stdc++.h>
#include <iomanip>

#define ID_MAX_LENGTH   16

using namespace std;

char header[] = "VRecord data";
size_t nRec = 10000000;

struct VRecord {
    char    id[ID_MAX_LENGTH];
    time_t  timestamp;
    double  x, y;

    VRecord():timestamp(0), x(0), y(0) {// default constructor
        id[0] = 0;
    }
    VRecord(const char* vID):timestamp(0), x(0), y(0){
        strcpy(id, vID);
    }
    VRecord(VRecord& vR) : timestamp(vR.timestamp),
                           x(vR.x), y(vR.y) {
        strcpy(id, vR.id);
    }
    VRecord(VRecord&& vR): timestamp(vR.timestamp),
                           x(vR.x), y(vR.y) {
        strcpy(id, vR.id);
    }

    bool operator==(VRecord& b) {
        return  strcmp(id, b.id) == 0 &&
                timestamp == b.timestamp;
    }
};

// data for ID random section

char* randomID(){   // pattern : ccAxxxxx
    int tmp;
    string s = "";

    // generate 2 first letters
    int twofirstletters = 0;
    tmp = rand()% 7;
    if (tmp == 0)
        twofirstletters = 51;
    else if (tmp == 1)
        twofirstletters = 53;
    else if (tmp == 2)
        twofirstletters = 54;
    else if (tmp == 3)
        twofirstletters = 57;
    else if (tmp == 4)
        twofirstletters = 61;
    else
        twofirstletters = 64;

    s = to_string(twofirstletters);


    //generate third letter
    tmp = rand() % 6 + 65;
    s = s + string(1, char(tmp));


    // generate 4 (5) last letters
    tmp = rand() % (10240 - 7890) + 7890;
        s += to_string(tmp);

    char* ret = new char[16];
    strcpy(ret, s.c_str());
    return ret;
}


struct tm timeInfo = {0};

time_t randomTimestamp(bool newStamp){
    if (!newStamp)
        timeInfo.tm_sec += 4;

    return mktime(&timeInfo);
}

double randomx(){
    int tmp = rand()% 101;
    double ret = 0;

    if (tmp <= 6)
        ret = 105;
    else if (tmp <= 15)
        ret = 107;
    else ret = 106;

    ret += (double)(rand() % 1000000) / (double)1000000 + (double)(rand()% 1000000) / (double)1000000000000;
    return ret;
}

double randomy(){
    int tmp = rand() % 101;
    double ret = 0;

    if (tmp <= 5)
        ret = 9;
    else if (tmp <= 25)
        ret = 11;
    else
        ret = 10;

    ret += (double)(rand() % 1000000) / (double)1000000 + (double)(rand()% 1000000) / (double)1000000000000;
    return ret;
}
int main(){
    // init basic data
    srand (time(NULL));
    struct VRecord *rec = new VRecord();

    timeInfo.tm_sec = 20;
    timeInfo.tm_min = 45;
    timeInfo.tm_hour = 11;

    timeInfo.tm_year = 2018 - 1900;
    timeInfo.tm_mon = 5;
    timeInfo.tm_mday = 18;


    FILE * dataFile = fopen("sampleData.dat", "wb");

    // write header and nRec
    fwrite(header, 1, 12, dataFile);
    fwrite(&nRec, 4, 1, dataFile);


    cout << setprecision(15);

    for (size_t i = 1; i <= nRec; i++){
        memcpy(rec->id, randomID(), 16);

        if (i%4 == 0)
            rec->timestamp = randomTimestamp(false);
        else
            rec->timestamp = randomTimestamp(true);
        rec->x = randomx();
        rec->y = randomy();

        if (i < 20)
            cout << rec->id << "  " << rec->x << "  " << rec->y << "\n";
        fwrite(rec, sizeof(VRecord), 1, dataFile);
    }

    fclose(dataFile);
    delete rec;
    return 0;

}
