/*
 * =========================================================================================
 * Name        : processData.cpp
 * Description : Student code for Assignment 2 - Data structures and Algorithms - Spring 2018
 * =========================================================================================
 */

#include <sstream>

#include "requestLib.h"
#include "dbLib.h"
#include "dsaLib.h"
#include <chrono>

#define     cmdList_N   9
#define     EPSILON     0.0000000001
//******************       General purpose resources       ********************
string          *cmdList;

// ************ request processing


char            rqCode[16];
char            rqID[16];


double          input1, input2, input3, input4;
double          Ry, Rx, dy, dx;
char            *ID;
size_t          rqNumber = 0;

void    requestProcessing(VRequest& request, void* pGData){
    stringstream ss;

    
    cmdList = (string*)pGData;      


    // retrieve requestCode (3 first letters), ID (perharps), Ry, dy, Rx, dx    
    for (size_t i = 0; i < strlen(request.code); i++)
        if (request.code[i] == '_')
            request.code[i] = ' ';
    
    ss << request.code;
    ss >> rqCode;

    strncpy(rqID, rqCode + 3, 16);    // export 3 first capitalized letters  
    strncpy(rqCode, rqCode, 3);    // export 3 first capitalized letters
    rqCode[3] = '\0';
    
    ss >> input1; 
    ss >> input2;
    ss >> input3;
    ss >> input4;   

    for (rqNumber = 0; rqNumber < cmdList_N && !(rqCode == cmdList[rqNumber]); rqNumber++);
}


// AVLTree_ID will be used mostly in request processing, so it is created once when
// processing fist request
AVLTree<char*> *AVLTree_ID = nullptr;
bool            isAVLTree_IDCreated = false;

// LinkedList result stores address of records resulted of findDuplication()
L1List<VRecord*>   *result = new L1List<VRecord*>();



bool    lessorequal(double a, double b){
    return (a-b < 0 || fabs(a-b) < EPSILON);
}
AVLTree<time_t> *AVLTree_time = nullptr;

bool    isPassbyX(double Rx, double deltax, VRecord* record){
    if (lessorequal(Rx - deltax, record->x) && lessorequal(record->x, Rx + deltax))
        return true;
    return false;
}

bool    isPassbyY(double Ry, double deltay, VRecord* record){
    if (lessorequal(Ry - deltay, record->y) && lessorequal(record->y, Ry + deltay))
        return true;
    return false;
}

bool    isPassbyXY(double Rx, double deltax, double Ry, double deltay, VRecord* record){
    if (isPassbyX(Rx, deltax, record) && isPassbyY(Ry, deltay, record))
        return true;
    return false;
}


//************************* requests dealing with all records

/*************************************************
 *                     NVR                       *
 * **********************************************/

AVLTree<IDInfo*>* IDTree = nullptr;

// inout will keep the result of isPassbyXY, 
// so NVR_Core2 will decide whether p->N is increased or not
bool    inout;

// this function will be passed to traverse() to check pass-by condition
void    NVR_Core1(VRecord &pInput){
    inout = isPassbyXY(Rx, dx, Ry, dy, &pInput);

    IDInfo *ret = nullptr, *find = new IDInfo(pInput.id, 0, false);
    IDTree->find(find, ret);
    
    if (ret == nullptr){
        if (inout == true)                
            IDTree->insert(new IDInfo(pInput.id, 1, inout), nullptr);
        else
            IDTree->insert(new IDInfo(pInput.id, 0, inout), nullptr);
    }
    else{
        if (inout == true)      // record is in R area, increase N if satisfaction            
            ret->N++;
    }
    inout = false;
}
void    NVR(L1List<VRecord> &recList){
    recList.traverse(NVR_Core1);
}


// below section takes the role as counting eligible devices
unsigned int  deviceNear;
void    NVR_count(IDInfo* pInput){
    if (pInput->N > 0)
        deviceNear++;
    
}

/*************************************************
 *                     NRR                       *
 ************************************************/
size_t  recordNear = 0;
void    NRR_Core(VRecord &pInput){
    if (isPassbyXY(Rx, dx, Ry, dy, &pInput)){
        recordNear++;        
    }
}
void    NRR(L1List<VRecord> &recList){
    recordNear = 0;
    recList.traverse(NRR_Core);
}


// ************************requests dealing with specific ID
/************************************************
 *                     CXR                      *
 * *********************************************/

// CXR1 used in finding in AVLTree_ID
void    CXR1(VRecord *pInput){
    AVLTree_time->insert(pInput->timestamp, pInput);
}

short    isPassbyXChecked;
// CRX2 used in traversing AVLTree_time
void    CXR2(VRecord *pInput){
    if (isPassbyX(Rx, dx, pInput))
        if (isPassbyXChecked % 2 == 0)
            isPassbyXChecked++;
    else
        if (isPassbyXChecked % 2 == 1)
            isPassbyXChecked++;
}

/************************************************
 *                     CYR                      *
 * *********************************************/
// CXR1 used in finding in AVLTree_ID
void    CYR1(VRecord *pInput){
    AVLTree_time->insert(pInput->timestamp, pInput);
}

short    isPassbyYChecked;
// CRX2 used in traversing AVLTree_time
void    CYR2(VRecord *pInput){
    if (isPassbyY(Ry, dy, pInput))
        if (isPassbyYChecked % 2 == 0)
            isPassbyYChecked++;
    else
        if (isPassbyYChecked % 2 == 1)
            isPassbyYChecked++;
}

/************************************************
 *                     CVP                      *
 * *********************************************/
// CVP1 used in finding in AVLTree_ID
void    CVP1(VRecord *pInput){
    AVLTree_time->insert(pInput->timestamp, pInput);
}

short    isPassbyRChecked;
// CVP2 used in traversing AVLTree_time
void    CVP2(VRecord *pInput){
    if (isPassbyXY(Rx, dx, Ry, dy, pInput))
        if (isPassbyRChecked % 2 == 0)
            isPassbyRChecked++;
    else
        if (isPassbyRChecked % 2 == 1)
            isPassbyRChecked++;
}

/************************************************
 *                     NXR                      *
 * *********************************************/
size_t  recPassbyX;
void    NXR(VRecord *pInput){
    if (isPassbyX(Rx, dx, pInput))
        recPassbyX++;
}

/************************************************
 *                     NYR                      *
 * *********************************************/
size_t  recPassbyY;
void    NYR(VRecord *pInput){
    if (isPassbyY(Ry, dy, pInput))
        recPassbyY++;
}

/************************************************
 *                     NRP                      *
 * *********************************************/
size_t  recPassbyR_ID;
void    NRP(VRecord *pInput){
    if (isPassbyXY(Rx, dx, Ry,dy, pInput))
        recPassbyR_ID++;
}

/************************************************
 *                     NPR                      *
 * *********************************************/
// NPR1 used in finding in AVLTree_ID
void    NPR1(VRecord *pInput){
    AVLTree_time->insert(pInput->timestamp, pInput);
}

unsigned int recPassbyR;
// NPR2 used in traversing AVLTree_time
void    NPR2(VRecord *pInput){
    if (isPassbyXY(Rx, dx, Ry, dy, pInput))
        recPassbyR++;
}


// ********************************************************************************************
// ********************************************************************************************
// ********************************************************************************************



bool initVGlobalData(void** pGData) {
    *pGData = new string[9];

    // Checking group and specific ID
    ((string*)*pGData)[0] = "CXR";
    ((string*)*pGData)[1] = "CYR";
    ((string*)*pGData)[2] = "CVP";

    // Finding the quantity and specific ID
    ((string*)*pGData)[3] = "NXR";
    ((string*)*pGData)[4] = "NYR";
    ((string*)*pGData)[6] = "NRP";    

    // Finding the quantity of the whole
    ((string*)*pGData)[5] = "NRR";
    ((string*)*pGData)[8] = "NVR";
    ((string*)*pGData)[7] = "NPR";


    return true;
}
void releaseVGlobalData(void* pGData) {
    cmdList = (string*)pGData;
    delete[] cmdList;
}

void AVLTree_IDCreate(VRecord &input){
    AVLTree_ID->insert((char*)(input.id), &(input));        
}

bool processRequest(VRequest& request, L1List<VRecord> &recList, void* pGData) {
    // retrieve information from   VRequest& request
    requestProcessing(request, pGData);
    
    if (!isAVLTree_IDCreated){        
        isAVLTree_IDCreated = true;       
        
        AVLTree_ID = new AVLTree<char*>();
        recList.traverse(AVLTree_IDCreate);        
    }
   


    switch (rqNumber){
        // requests deal with the whole data        
        case 8: // NVR
            Ry = input1; Rx = input2; dy = input3; dx = input4;
            deviceNear = 0;
            IDTree = new AVLTree<IDInfo*>();
            NVR(recList);

            IDTree->traverseLNR(NVR_count);
            cout << "NVR_" << Ry << "_" << Rx << "_" << dy << "_" << dx << ": " << deviceNear << "\n";
            IDTree->~AVLTree();
            break;
        
        case 5: // NRR
            Ry = input1; Rx = input2; dy = input3; dx = input4;
            NRR(recList);
            cout << "NRR_" << Ry << "_" << Rx << "_" << dy << "_" << dx << ": " << recordNear << "\n";
            break;
        
        case 7: // NPR
            Ry = input1; Rx = input2; dy = input3; dx = input4;
            recPassbyR = 0;

            AVLTree_time = new AVLTree<time_t>();

            AVLTree_ID->findDuplication(rqID, NPR1);

            AVLTree_time->traverseLNR(NPR2);

            cout << "NPR" << rqID << "_" 
                 << Ry << "_" << Rx << "_"
                 << dy << "_" << dx << ": " << (recPassbyR/2) << "\n";

            AVLTree_time->~AVLTree();
            break;

        case 0: // CXR
            Rx = input1; dx = input2;
            isPassbyXChecked = 0;
            AVLTree_time = new AVLTree<time_t>();

            AVLTree_ID->findDuplication(rqID, CXR1);

            AVLTree_time->traverseLNR(CXR2);

            cout << "CXR" << rqID << "_" << Rx << "_" << dx << ": ";
            if (isPassbyXChecked)
                cout << "Succeed\n";
            else
                cout << "Failed\n";

            AVLTree_time->~AVLTree();
            break;            

        case 1: // CYR
            Ry = input1; dy = input2;
            isPassbyYChecked = 0;
            AVLTree_time = new AVLTree<time_t>();

            AVLTree_ID->findDuplication(rqID, CYR1);

            AVLTree_time->traverseLNR(CYR2);
            cout << "CYR" << rqID << "_" << Ry << "_" << dy << ": ";
            if (isPassbyYChecked)
                cout << "Succeed\n";
            else
                cout << "Failed\n";
            AVLTree_time->~AVLTree();
            break;

        case 2: // CVP
            Ry = input1; Rx = input2; dy = input3; dx = input4;
            isPassbyRChecked = 0;

            AVLTree_time = new AVLTree<time_t>();

            AVLTree_ID->findDuplication(rqID, CVP1);

            AVLTree_time->traverseLNR(CVP2);

            cout << "CVP" << rqID << "_" 
                 << Ry << "_" << dy << "_"
                 << Rx << "_" << dx << ": ";
            if (isPassbyRChecked)
                cout << "Succeed\n";
            else
                cout << "Failed\n";

            AVLTree_time->~AVLTree();
            break;
        case 3: // NXR
            Rx = input1; dx = input2;
            recPassbyX = 0;

            AVLTree_ID->findDuplication(rqID, NXR);

            cout << "NXR" << rqID << "_" << Rx << "_" << dx << ": " << recPassbyX << "\n";

            break;

        case 4: // NYR
            Ry = input1; dy = input2;
            recPassbyY = 0;

            AVLTree_ID->findDuplication(rqID, NYR);

            cout << "NYR" << rqID << "_" << Ry << "_" << dy << ": " << recPassbyY << "\n";
            break;
        
        case 6: // NRP
            Ry = input1; Rx = input2; dy = input3; dx = input4;
            recPassbyR_ID = 0;

            AVLTree_ID->findDuplication(rqID, NRP);

            cout << "NRP" << rqID << "_" 
                 << Ry << "_" << dy << "_"
                 << Rx << "_" << dx << ": " << recPassbyR_ID << "\n";
            break;

    }




    return true;
}

