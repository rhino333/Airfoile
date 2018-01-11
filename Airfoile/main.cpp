/*
 --- 기초 ---
 Array로 하면 편하겠지만, 그럴 경우 Memory 초과가 난다는 박사님의 말에 따라
 Class로 구현함.
 
 정사각형을 Cell, 사각형을 이루는 변을 Face, 각 꼭지점을 Node라 정의함.
 
 --- 전체 흐름 설명 ---
 각 네모를 Cell이라 부르며
 네모를 이루는 상/하/좌/우의 벽을 Face라 부른다.
 네모의 각 꼭지점을 Node라 부른다.
 
 편의를 위해, 전체 네모가 Cell들을 참조할 수 있다.( Rectangle->initCell[i][j] )
 Cell에서 Face 밑 Node를 접근할 수 있다.
 ( Cell-> LFace,  DFace,  RFace,  UFace );
 ( Cell->LUNode, LDNode, RDNode, RUNode );
 
 Face에서 인접해있는 Cell과 Node를 접근할 수 있다.
 ( Face-> sNode, eNode [startNode, endNode의 줄임말] )
 
 --- 출력 순서와 관련된 부분 ---
 각 부분에 대해, 파일로 출력할 때, 먼저 출력되는 부분에 대한 설정이 필요하다.
 특히 Face를 출력할 때 sNode, eNode, sCell, eCell 순서로 출력하는 부분이 있는데,
 기역자(?)로 항상 그리면서 출력하는 내용은 박사님께 직접 전해듣기를 바란다.
 
 판을 그릴 때, (0, 0)에 가까운 부분을 start로 하고, (width, height)에 가까울수록 end로 설정한다.
 출력할 때, 끝 부분이 NULL인 경우에만 순서를 거꾸로 출력하도록 설정하였다.
 예를 들어, Face(변)에 대해 생각해보면
 (0, 0) ~ (100, 0)을 잇는 Face가 있다고 생각할 때,
 그 Face의 sNode는 (0, 0)이고, eNode는 (100, 0)이다.
 
 본인은, 고려하기 쉽게 좌표평면상의 점으로 생각한 후 작성하였다.
 
 
 전체 판을 만들기 위해
 1. 가로, 세로 크기에 대한 입력을 받는다.
 2. 가로/세로 GCD를 구해서, 그 GCD에 맞도록 판을 쪼갠다.
 3. 각 Cell에 대한 정보를 입력한다.
 */
#include "Airfoile.hpp"
//extern vector<Node*> nodeVector;
//extern vector<Cell*> cellVector;
//extern vector<Face*> faceVector;
//extern vector<Face*> wallVector; //0208
//extern vector<fi> wall; // < <삼각형인지 사각형인지 , 도형의 id > < snode, enode> >
//extern Rectangle *initRectangle;
//extern vector<CellShape*> minimalCellVector_tri; // 재 : 이 곳에 CellRectangle과 CellTriangle 삽입 2017-01-12
//extern map< pdd, map < pdd, CellShape* > > mininodeCell; // 재 : map 2017-01-12
//extern vector<detface> detfaceVector;
//extern vector<detcell> detcellVector;
//extern vector< pair<pdd, pdd> > inputLine; // 재 : 입력받는 선들을 저장 2017_02_08
//
//extern int initCellMaxID; // 재 : initCell에 대한 접근 방지용, 어디까지 initCell인지 저장 2017_02_19
//extern double S_N;
vector<pdd> mininode[2];
map< pdd, pair< int, void* > > mininodePlace;
FILE *fp; // 선언만 하고 메인에서 인스턴스화

int main() {
    initCellMaxID = 0;
    S_N = 0.000000001;
    double LeftDownX = -3, LeftDownY = -3;
    int layer = 3, depth = 7; //진
    double mincellsize = 0; //진
    int Xcount = 8, Ycount = 8;//진_initcellsize설정
    double initRCellsize = 1;//진_initcellsize설정
    FILE * inputfp;
    int n, uppern, lowern, tail; 
    double x, y;
    vector<pdd> airpoile;
    bool vertical_horizontal = false;// true = vertical , false= horizontal
    bool y_increase;
    
    // 입력파일 읽어서 airpoile에 저장
    if ((inputfp = fopen("/Users/rhino/Documents/Airfoile/Airfoile/input.txt", "r")) == NULL) {
        cout << "input file을 찾을수 없습니다 " << endl;
        return 0;
        
    }
    fscanf(inputfp, "%d %d %d %d", &n, &uppern, &lowern, &tail);
    for (int i = 0; i < n; i++) {
        fscanf(inputfp, "%lf %lf", &x, &y);
        airpoile.push_back(make_pair(x, y));
    }
    
    //초기화
    initRectangle = new Rectangle(LeftDownX, LeftDownY, initRCellsize, Xcount, Ycount);//진_initcellsize설정
    initCellMaxID = initRectangle->initCell[initRectangle->widthCount - 1][initRectangle->heightCount - 1].getID();
    mincellsize = initRectangle->initCellSize / pow((double)2, (double)depth); // 재 : 잠정 중지 2017_02_20
    
    //splitcell
    //initRectangle->splitCell(-2, -2, 3, 3, 1, 2, depth);
    //    cout << "PHASE : splitcell 완료" << endl;
    // mininode 찾기
    int ud = 0;
    
    double length_prev = 0;
    int index = 0;
    for (int i = 0; i < n - 1; i++) {
        if (i == uppern - 1) {
            index = 0;
            ud = 1;
        }
        findminnode(airpoile[i].first, airpoile[i].second, airpoile[i + 1].first, airpoile[i + 1].second, mincellsize, ud);
        inputLine.push_back(make_pair(airpoile[i], airpoile[i + 1]));
        
    }
    
    
    cout << "PHASE : findmininode 완료" << endl;
    
    // vertical 인지 horizontal 인지 확인
    if (vertical_horizontal) {//세로형이면
        sort(mininode[0].begin(), mininode[0].end(), cmp_vertical);
        sort(mininode[1].begin(), mininode[1].end(), cmp_vertical);
        y_increase = airpoile[0].second < airpoile[uppern - 1].second;
    }
    
    //dotpoint 및 mininodecell 찾기
    for (int i = 0; i < 2; i++) {
        if (!mininode[i].empty()) {
            for (int k = 0; k < mininode[i].size(); k++) {
                initRectangle->dotPoint(mininode[i][k].first, mininode[i][k].second, layer, depth);
                if (k != 0) {
                    setupmininodeCell(mininode[i][k - 1], mininode[i][k]);
                }
            }
        }
    }
    cout << "PHASE : setupmininodeCell 완료" << endl;
    // isshape 하면서 delete를 해버려서 seupmininodecell이 참조못하는 경우가 생겨서 분리함
    if (!vertical_horizontal) {
        for (int i = 0; i < 2; i++) {
            if (!mininode[i].empty()) {
                for (int k = 1; k < mininode[i].size(); k++) {
                    if (i == 0) {
                        if (!isshape(mininode[i][k - 1], mininode[i][k], 1, mincellsize)) return 0;
                    }
                    else {
                        if (!isshape(mininode[i][k - 1], mininode[i][k], -1, mincellsize)) return 0;
                    }
                }
            }
        }
    }
    else {
        for (int i = 0; i < 2; i++) {
            if (!mininode[i].empty()) {
                for (int k = 1; k < mininode[i].size(); k++) {
                    if ((y_increase && i == 1) || (!y_increase && i == 0)) {
                        if (!isshape(mininode[i][k - 1], mininode[i][k], 1, mincellsize)) return 0;
                    }
                    else {
                        if (!isshape(mininode[i][k - 1], mininode[i][k], -1, mincellsize)) return 0;
                    }
                }
            }
        }
    }
    cout << "PHASE : Isshape 완료" << endl;
    
    if (tail == 1) {// 진 : tail 설정  2017-03-07
        tailmanager(airpoile[0], airpoile[n - 1], mincellsize, layer, depth);
        
        cout << "tailcut 완료" << endl;
    }
    
    //cutInnerCell();
    bool flag = false;
    cout << "PHASE : manageEdge 완료" << endl;
    
    // 재: 2017_03_10 cutInnerCell ccw알고리즘 제거 및 vertical check 작업 추가
    // 재 : cutInnerCell 제거, ccw알고리즘 제거, checkInner 변경,
    checkInner(LeftDownX, LeftDownY, initRCellsize, Xcount, Ycount, flag);
    
    sort(nodeVector.begin(), nodeVector.end(), nodeCompare); // 2017_03_16 재
    deleteInnerFace();
    cout << "PHASE : cutinnetcell 완료" << endl;
    
    sort(detfaceVector.begin(), detfaceVector.end());
    
    vector<detface>::iterator pos = unique(detfaceVector.begin(), detfaceVector.end()); // 재 : detfaceVector에 face가 중복되서 들어가는 문제가 발생하여 코드 추가
    detfaceVector.erase(pos, detfaceVector.end());                                        // 재 : 중복되는 값을 제거 // 2017_02_13
    
    for (int i = detfaceVector.size() - 1; i >= 0; i--) {
        delete(detfaceVector[i].second);
    }
    
    for (int i = 0; i < wall.size(); i++) {
        Face* wf = new Face(nodeVector[wall[i].second.first], nodeVector[wall[i].second.second]);
        if (wall[i].first.first == 0) wf->sCell = cellVector[wall[i].first.second];
        if (wall[i].first.first == 1) wf->sCell = minimalCellVector_tri[wall[i].first.second];
        wallVector.push_back(wf);
    }
    
    sort(detcellVector.begin(), detcellVector.end());
    
    vector<detcell>::iterator posC = unique(detcellVector.begin(), detcellVector.end()); // 재 : detcellVector에 face가 중복되서 들어가는 문제가 발생하여 코드 추가
    detcellVector.erase(posC, detcellVector.end());                                        // 재 : 중복되는 값을 제거 // 2017_02_13
    
    for (int i = detcellVector.size() - 1; i >= 0; i--) {
        delete(detcellVector[i].second);
        detcellVector[i].second->setID(0);
    }
    
    
    
    sort(faceVector.begin(), faceVector.end(), faceCompare);
    sort(cellVector.begin(), cellVector.end(), cellCompare);
    
    for (unsigned int i = 0; i<faceVector.size(); i++) {
        if (faceVector[i]->id == 365)
            cout << endl;
        faceVector[i]->setID(i + 1);
    }
    
    for (unsigned int i = 0; i<cellVector.size(); i++) {
        cellVector[i]->setID(i + 1);
    }
    
    for (unsigned int i = 0; i < minimalCellVector_tri.size(); i++) {//0208
        minimalCellVector_tri[i]->setID(i + 1 + cellVector.size());
    }
    
    
    fp = fopen("Test_07_31_jae_.msh", "w");
    
    DescribeMesh();
    
    fclose(inputfp);
    fclose(fp); //진_cutcell수정
    
    //initRectangle->printAll();
    //initRectangle->confirmAll();
    
    
    return 0;
}
