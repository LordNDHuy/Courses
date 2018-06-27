/*
 * =========================================================================================
 * Name        : processData.cpp
 * Description : student code for Assignment 1 - Data structures and Algorithms - Spring 2018
 * =========================================================================================
 */
#include "requestLib.h"
#include "dbLib.h"


#define     cmdList_N  27

//******************       General purpose resources
string                  *cmdList;
char                    rqCode[4];
char                    rqID[16];
char                    des[40];

struct DataNode{
    char*               id;

    double              x_idling, y_idling;
    double              x_idling_last, y_idling_last;
    double              x_first, y_first;
    double              x_last, y_last;


    double              journeyLength;          // averageSpeed = journeyLength / (timeTotal - timeIdling)
    double              journeyLength_AVE;
    double              speed_AVE;


    time_t              stamp_first, stamp_last;
    unsigned int        timeTotal;              // timeMoving = timeTotal - timeIdling
    unsigned int        timeIdlingTotal, timeIdlingMax;
    unsigned int        timeIdling_tmp;

    size_t              N;                      // nonstop devices have N = 1

    DataNode              (VRecord p){
        N = 1;
        id = new char;
        strcpy(id,p.id);
        
    	x_last = p.x; y_last = p.y;
        x_first = p.x; y_first = p.y;
        x_idling_last = y_idling_last = 0;
        

        stamp_last = p.timestamp;
        stamp_first = p.timestamp;

        journeyLength = journeyLength_AVE = 0;
        timeTotal  = 0;
        timeIdlingTotal = timeIdlingMax = timeIdling_tmp = 0;

        // if (strcmp(id, "65C03325") == 0)
        //     cout << " here : " << timeIdlingMax << "\n";

    }
    ~DataNode             (){
        x_first = y_first = x_last = y_last = N = journeyLength = 0;
        stamp_first = stamp_last =  0; delete id;
        timeTotal = 0;
        timeIdlingTotal = timeIdlingMax = timeIdling_tmp = 0;      
    }

    void Calculation      (double x_input, double y_input, time_t stamp_input){
        //  general calculations
        unsigned int time_tmp = stamp_input - stamp_last;        
        stamp_last = stamp_input;

        if (x_input != x_last || y_input != y_last)
            timeTotal += time_tmp;

        double journeyLength_tmp = distanceVR(y_last, x_last, y_input, x_input);
        journeyLength += journeyLength_tmp;        
        
        N++;

        
        journeyLength_AVE = journeyLength / (N - 1);
        // specific calculations
        
        // if length of 2 positions are less than 5m
        // then previous point is set to the center of the circle
        // and every next regconized positions of that device which are belong to
        // that circle would be considered as unmoved. 
         
        if ((journeyLength_tmp <= 0.005 && x_idling + y_idling == 0)
        || (distanceVR(y_idling, x_idling, y_input, x_input) <= 0.005 && x_idling + y_idling != 0)){
            // the following ensures that the stable point is set to the firstly regconzed point
            if (x_idling + y_idling == 0){ x_idling = x_last; y_idling = y_last; }

            timeIdling_tmp += time_tmp;
        }
        if (distanceVR(y_idling, x_idling, y_input, x_input) > 0.005 && x_idling + y_idling != 0){
            x_idling_last = x_idling; y_idling_last = y_idling;
            x_idling = y_idling = 0;
            timeIdlingTotal += timeIdling_tmp;
            
            if (timeIdlingMax < timeIdling_tmp){
                timeIdlingMax = timeIdling_tmp;          

            }   
            timeIdling_tmp = 0;
        }

        speed_AVE = journeyLength / (timeTotal - timeIdlingTotal);
        x_last = x_input; y_last = y_input;

        // if (strcmp(id, "51C31699") == 0)
        //     cout << "N = " << N << " and " << journeyLength << "\n";
    }
};

// *****************       Manipulating in large scale resources
            char*       id;
L1List<DataNode>        *pDeviceList ;
L1Item<DataNode>        *pItemTmp;
            bool        isDeviceListMade = false;

          double        MovingDevices_totalLength;
    unsigned int        MovingDevice_NRecord;

    unsigned int        N = 0;


// ******************      RVR command resource
L1List<size_t>     *pDeviceListRemove;
L1Item<size_t>     *pItemTmpRemove;



// ******************************** functions used in processing commands *******************************
// -------------------- haltFlag commands
bool    FindFirst(DataNode&  m, void* ID){
    if (strcmp(m.id, (char*)ID) == 0)
        return true;                        // find out, halt the subroutine
    else return false;                      // continue outer loop
}

// this would create a list whose elements are devices with informations needing to print out
void    DeviceListMaking(L1Item<VRecord>   *&m){
    pItemTmp = pDeviceList->traverse1(&FindFirst, m->data.id);
    
    if (pItemTmp == NULL)  // if not found, add new one to pDeviceList list
        pDeviceList->insertHead(*(new DataNode(m->data)));
    else{
        pItemTmp->data.Calculation(m->data.x, m->data.y, m->data.timestamp);
    }
}



// ------------------- traverses in large scale commands

void    MST(L1Item<DataNode> *&m){
    if (pItemTmp == NULL || pItemTmp->data.timeIdlingMax < m->data.timeIdlingMax)
        pItemTmp = m;
}
void    MRV(L1Item<DataNode> *&m){
    if (pItemTmp == NULL || pItemTmp->data.N < m->data.N)
        pItemTmp = m;
}
void    LRV(L1Item<DataNode> *&m){
    if (pItemTmp == NULL || pItemTmp->data.N > m->data.N)
        pItemTmp = m;
}

void    LPV(L1Item<DataNode> *&m){
    if (pItemTmp == NULL || pItemTmp->data.journeyLength < m->data.journeyLength)
        pItemTmp = m;
}
void    SPV(L1Item<DataNode> *&m){
    if (pItemTmp == NULL || pItemTmp->data.journeyLength > m->data.journeyLength)
        pItemTmp = m;
}
void    CAS(L1Item<DataNode> *&m){
    if (m->data.journeyLength == 0) return; 
    MovingDevices_totalLength += m->data.journeyLength;    
    MovingDevice_NRecord      += m->data.N - 1;

}
void    MTV(L1Item<DataNode> *&m){
    if (pItemTmp == NULL || pItemTmp->data.timeTotal - pItemTmp->data.timeIdlingTotal < m->data.timeTotal - m->data.timeIdlingTotal)
        pItemTmp = m;
}
void    MVV(L1Item<DataNode> *&m){
    if (pItemTmp == NULL || pItemTmp->data.speed_AVE < m->data.speed_AVE)
        pItemTmp = m;
}
void    CNS(L1Item<DataNode> *&m){
    if (m->data.N == 1)
       N++;
}

// ------------------- RVR command
void    DeviceListMakingRemove(VRecord &m, void* param, size_t &pos){
    if (strcmp(m.id, (char*)param) == 0)
        pDeviceListRemove->insertHead(pos);
}



/*
        main functions that communicate with functions and data in other files
 */
bool initVGlobalData(void** pGData) {
    *pGData = new string[27];
    ((string*)*pGData)[0] = "CNR";              
    ((string*)*pGData)[1] = "VFF";
    ((string*)*pGData)[2] = "VFL";

    ((string*)*pGData)[3] = "VFY";              // haltFlag
    ((string*)*pGData)[4] = "VFX";              // haltFlag 
    ((string*)*pGData)[5] = "VFT";              // haltFlag
    ((string*)*pGData)[6] = "VFS";

    ((string*)*pGData)[7] = "VLX";              // 1-time saving
    ((string*)*pGData)[8] = "VLY";              // 1-time saving
    ((string*)*pGData)[9] = "VLT";              // 1-time saving
    ((string*)*pGData)[13] = "VLS";             // 1-time saving
    ((string*)*pGData)[10] = "VCL";             // 1-time saving
    ((string*)*pGData)[11] = "VMT";             // 1-time saving
    ((string*)*pGData)[12] = "VCR";             // 1-time saving
    ((string*)*pGData)[14] = "VMS";             // 1-time saving
    ((string*)*pGData)[15] = "VAS";             // 1-time saving

    ((string*)*pGData)[18] = "MRV";             // additional Linkedlist
    ((string*)*pGData)[19] = "LRV";             // additional Linkedlist
    
    ((string*)*pGData)[22] = "CNS";             // additional Linkedlist
    ((string*)*pGData)[16] = "MST";             // additional Linkedlist
    ((string*)*pGData)[17] = "CNV";             // additional Linkedlist

                // additional Linkedlist
    ((string*)*pGData)[21] = "MVV";             // additional Linkedlist
    ((string*)*pGData)[23] = "CAS";             // additional Linkedlist

    ((string*)*pGData)[24] = "LPV";             // additional Linkedlist
    ((string*)*pGData)[25] = "SPV";             // additional Linkedlist
    ((string*)*pGData)[20] = "MTV"; 

    ((string*)*pGData)[26] = "RVR";             // additional Linkedlist but use another type of Item

    return true;
}

void releaseVGlobalData(void* pGData) {
	cmdList = (string*)pGData;
    delete[] cmdList;
}

bool processRequest(VRequest& request, L1List<VRecord>& recList, void* pGData) {
    cmdList = (string*)pGData;
    strncpy(rqCode, request.code, 3);    // export 3 first capitalized letters  
    
    if (request.code[3] != '\0')
        strncpy(rqID, request.code + 3, 16);            
    

    unsigned int i = 0;
    for (; i < cmdList_N && !(rqCode == cmdList[i]); i++);

    if (!isDeviceListMade){
        pDeviceList = new L1List<DataNode>();
        recList.traverse(&DeviceListMaking);
        isDeviceListMade = true;
    }



    switch (i){
        case 0:
            cout << "CNR: " << recList.getSize() << "\n";
            break;
        case 1:
            cout << "VFF: " ;
            printVRecord(recList.at(0));
            break;
        case 2:
            cout << "VFL: " << pDeviceList->at(0).id << "\n";
            break;
        case 3:                         // haltFlag
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VFY" << rqID << ": " << pItemTmp->data.y_first << "\n";
            break;
        case 4:                         // haltFlag
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VFX" << rqID << ": " << pItemTmp->data.x_first << "\n";
            break;
        case 5:                         // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            strPrintTime(des, pItemTmp->data.stamp_first);
            cout << "VFT" << rqID << ": " << des << "\n";
            break;
        case 6:                         // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VFS" << rqID << ": (" << pItemTmp->data.x_first << " " << pItemTmp->data.y_first << ")\n";
            break;           
        case 7:                         // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VLX" << rqID << ": " << pItemTmp->data.x_last << "\n";
            break; 
        case 8:                         // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VLY" << rqID << ": " << pItemTmp->data.y_last << "\n";
            break; 
        case 9:                         // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            strPrintTime(des, pItemTmp->data.stamp_last);
            cout << "VLT" << rqID << ": " << des << "\n";
            break;
        case 13:                        // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VLS" << rqID << ": (" << pItemTmp->data.x_idling_last << " " << pItemTmp->data.y_idling_last << ")\n";
            break;
        case 11:                        // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VMT" << rqID << ": " << (pItemTmp->data.timeTotal - pItemTmp->data.timeIdlingTotal) << "\n";
            break;
        case 12:                        // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VCR" << rqID << ": " << pItemTmp->data.N << "\n";
            break;
        case 10:                        // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VCL" << rqID << ": " << pItemTmp->data.journeyLength << "\n";
            break;
        case 14:                        // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID);
            cout << "VMS" << rqID << ": " << pItemTmp->data.timeIdlingMax << "\n";
            break;
        case 15:                        // 1-time saving
            pItemTmp = pDeviceList->traverse1(&FindFirst, rqID); //journeyLength_AVE
            cout << "VAS" << rqID << ": " << pItemTmp->data.journeyLength_AVE << "\n";
            break;


        //  commands dealing on a large scace
        case 22:
            N = 0;
            pDeviceList->traverse(&CNS);            
            cout << "CNS: " << N << "\n";
            break;
        case 17:
            cout << "CNV: " << pDeviceList->getSize() << "\n";
            break;


        case 16:
            pDeviceList->traverse(&MST);            
            cout << "MST: " << pItemTmp->data.timeIdlingMax << "\n";
            break;
        case 18:
            pDeviceList->traverse(&MRV);            
            cout << "MRV: " << pItemTmp->data.id << "\n";
            break;
        case 19:
            pDeviceList->traverse(&LRV);            
            cout << "LRV: " << pItemTmp->data.id << "\n";
            break;
        case 24:
            pDeviceList->traverse(&LPV);            
            cout << "LPV: " << pItemTmp->data.id << "\n";
            break;
        case 25:
            pDeviceList->traverse(&SPV);            
            cout << "SPV: " << pItemTmp->data.id << "\n";
            break;
        case 20: 
            pDeviceList->traverse(&MTV);            
            cout << "MTV: " << pItemTmp->data.id << "\n";
            break;


        case 23:                    
            pDeviceList->traverse(&CAS);
            cout << "CAS: " << MovingDevices_totalLength / MovingDevice_NRecord << "\n";            
            break;


        case 21:
            pDeviceList->traverse(&MVV);
            cout << "MVV: " << pItemTmp->data.id << "\n";
            break;



        case 26:
            pDeviceListRemove = new L1List<size_t>();
            recList.traverse(&DeviceListMakingRemove, rqID);

            cout << "RVR: ";
            if (pDeviceListRemove->isEmpty()){
                cout << "Failed\n";
                break;
            }
            while (!pDeviceListRemove->isEmpty()){
                recList.remove((int)pDeviceListRemove->at(0));             
                pDeviceListRemove->removeHead();
            }
            cout << "Succeed\n";
            break;        
    }
    return true;
}
