#pragma once
#ifndef Airfoile_hpp
#define Airfoile_hpp

#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <cstdio>
#include <queue>
#include <set>
#include <map>
#include <cmath>

#pragma warning(disable:4996)

#define EPSILON 0.00001 // 재: double 비교

using namespace std;

class CellShape; // 재
class Cell; // 재
class CellRectangle; // 재
class CellTriangle; // 재
class Face;
class Node;
class Rectangle;

typedef pair<double, double> pdd;//진
typedef pair<double, int> pdi;
typedef pair<pair<int, int>, pair<int, int> > fi;
typedef pair<int, Face*> detface;//진 delete
typedef pair<int, Cell*> detcell;//진 delete
typedef pair<int, CellShape*> node_of_cell;//진:tail

extern vector<Node*> nodeVector;
extern vector<Cell*> cellVector;
extern vector<Face*> faceVector;
extern vector<Face*> wallVector; //0208
extern vector<fi> wall; // < <삼각형인지 사각형인지 , 도형의 id > < snode, enode> >

extern Rectangle *initRectangle;
extern vector<pdd> mininode[2];//진 //vector<CellShape*> minimalCellVector_rec;
extern vector<CellShape*> minimalCellVector_tri; // 재 : 이 곳에 CellRectangle과 CellTriangle 삽입 2017-01-12
extern map< pdd, pair< int, void* > > mininodePlace; // 재 : mininode를 포함한 face나 node의 주소를 저장한다.( 0 : Node, 1 : Face) 2017-01-12
extern map< pdd, map < pdd, CellShape* > > mininodeCell; // 재 : map 2017-01-12
extern vector<detface> detfaceVector;
extern vector<detcell> detcellVector;
//vector<int> detinitcellVector; // 재 : 2017_03_17

extern vector< pair<pdd, pdd> > inputLine; // 재 : 입력받는 선들을 저장 2017_02_08
extern double S_N;
extern int initCellMaxID; // 재 : initCell에 대한 접근 방지용, 어디까지 initCell인지 저장 2017_02_19
extern FILE *fp; // 선언만 하고 메인에서 인스턴스화

bool double_Compare(double d1, double d2);

class Node {
public:
    double x, y;
    int id;
    static int nodeCount; // node 개수를 count
    /* attribute
     -2: line (도형위)
     -1: inner (도형의 안쪽)
     0 : outflow  (맨 오른쪽)
     1 : inlet    (맨 왼쪽)
     2 : top      (맨 위)
     3 : bottom   (맨 아래)
     4 : interrior(중간 Node들)
     5 : unknown (미확인)
     */
    int inout;
    
public:
    Node();
    Node(double x, double y);
    ~Node();
    void setPoint(double x, double y);
    double getX();
    double getY();
    int getID();
    int getCount();
    int getInout();
    bool operator==(Node* n);
    node_of_cell findtailCell(pdd n); // 진 : tail 설정  2017-03-07
    CellShape* findCell(Node* n); // 재 : Node와 Node일때 mininodeCell 설정 2017-01-12
    CellShape* findCell(Face* f); // 재 : Node와 face일때 mininodeCell 설정 2017-01-12
};

class Face {
public:
    Node *sNode, *eNode, *centerNode;
    CellShape *sCell, *eCell; // 재 : Cell => CellShape
    Face *Child1, *Child2, *curParent;
    int id, attribute;
    int depth;
    int printID, myPrintID;
    bool childOrNot;
    /* attribute
     0 : outflow  (맨 오른쪽)
     1 : inlet    (맨 왼쪽)
     2 : top      (맨 위)
     3 : bottom   (맨 아래)
     4 : interrior(중간 Face들) */
    static int faceCount;
    static int maxDepth;
public:
    Face();
    Face(Node *sNode, Node *eNode);
    ~Face();
    void setAttribute(int attribute);
    void setStartCell(CellShape *cell);
    void setEndCell(CellShape *cell);
    void setStartNode(Node *node);
    void setEndNode(Node *node);
    void setCenterNode(Node *node);
    void setChilds(Face *c1, Face *c2);
    void setID(int newID);
    void setDepth(int depth);
    void setPrintID(int newID);
    void setMyPrintID(int newID);
    void setParent(Face *parent);
    int getMyPrintID();
    int getMaxDepth();
    int getPrintID();
    int getID();
    int getAttribute();
    int getCount();
    Node* getStartNode();
    Node* getEndNode();
    Node* getCenterNode();
    CellShape* getStartCell();
    CellShape* getEndCell();
    Face* getChild1();
    Face* getChild2();
    int getDepth();
    bool isParent();
    bool isChild();
    bool operator==(Face& f);
    CellShape* findCell(Node* n); // 재 : face와 Node일때 mininodeCell 설정 2017-01-12
    CellShape* findCell(Face* f); // 재 : face와 face일때 mininodeCell 설정 2017-01-12
};


class CellShape { // 재 : Cell 기본형
public:
    double cellSize;
    int id, depth;
    static int cellCount;
    static int triCount; //0208
    bool iscut;
    
    int getID() {
        if (id>0)return id;
        return NULL;
    }
    void setID(int newID) {
        this->id = newID;
    }
};
class Cell : public CellShape { // 재 : 수정하였음 2017-01-12
public:
    Face *LFace, *DFace, *RFace, *UFace;
    Node *LUNode, *LDNode, *RDNode, *RUNode;
    CellShape *LUCell, *LDCell, *RDCell, *RUCell;
    
public:
    Cell();
    ~Cell();
    void setChildNodes(Node *L, Node *D, Node *R, Node *U, Node *C, int partialtype);
    void setNewFaces(Face *from, Face *child1, Face *child2, Node *Start, Node *Center, Node *End);
    void setupMininodePlace(double x, double y);
    bool getiscut();
    void setiscut(bool cut);
    int getNodeCount();
    int getCellCount();
    int getFaceCount();
    int getID();
    Node* getLUNode();
    Node* getLDNode();
    Node* getRDNode();
    Node* getRUNode();
    bool isParent();
    double getSize();
    int getDepth();
    void setDepth(int newDepth);
    void setID(int newID);
    void setLUNode(Node *node);
    void setLDNode(Node *node);
    void setRDNode(Node *node);
    void setRUNode(Node *node);
    void setSize(double size);
    void dotPoint(double x, double y, int layer, int depth);
    void printCell();
    void confirmAll();
    bool checkNode(Node* n);
    bool checkFace(Face* f);
    void splitCell(int layer, int depth);
};


class CellRectangle : public CellShape { // 재 : child를 가지지 않는다.
public:
    Node *LUNode, *LDNode, *RDNode, *RUNode;
    CellRectangle() {
        LUNode = LDNode = RDNode = RUNode = NULL;
        iscut = false;
    }
};

class CellTriangle : public CellShape { // 재 : child를 가지지 않는다.
public: //0208
    Node *HONode, *VONode, *WONode; // opposite angle, 각 페이스의 대각에 위치한 점
    CellTriangle();
    CellTriangle(Node *HO, Node *VO, Node *WO);
    ~CellTriangle();
    int getTriCount();
};


class Rectangle {
public:
    double LDx, LDy, RUx, RUy;
    Cell **initCell;
    double initCellSize;
    int widthCount, heightCount;
    
public:
    Rectangle(double LDX, double LDY, double Cellsize, int WidthCount, int HeightCount);
    int getNodeCount();
    int getCellCount();
    int getFaceCount();
    void printAll();
    void confirmAll();
    void dotPoint(double x, double y, int layer , int depth);
    void splitCell(double, double, double, double, int, int, int); // 재 : Cell split 함수 입니다. 점 두 개와 layer, depth를 입력받습니다. (x1, y1, x2, y2, layer, depth) 2017-01-22
};

int Node::nodeCount = 0;
int Face::faceCount = 0;
int Face::maxDepth = 0;
int CellShape::cellCount = 0;
//int CellRectangle::rectCount = 0;
int CellShape::triCount = 0;//0208

void DescribeMesh();
bool faceCompare(Face *f1, Face *f2);
bool cellCompare(Cell *c1, Cell *c2);
bool nodeCompare(Node *n1, Node *n2);
bool nodeCompareXY(Node *n1, Node *n2);
int findn(double point, double size, int SorL);
void findminnode(double x0, double y0, double x1, double y1, double size, int updown);
bool innerpoint(Cell* c, pdd n);
bool setupmininodeCell(pdd p1, pdd p2);
double ccw(pdd a, pdd b);
double ccw(pdd a, pdd b, pdd p);
double distancePTP(pdd a, pdd b);
double ccwAddition(pdd a, pdd b, pdd p);
void cuting(Cell* c, pdd fnode, pdd snode, double mincellsize, int updown, double gradient);
int cutface(pdd CenterPoint, Face* face, int child1or2);
void eraseRect(Cell* c, pdd fnode, pdd snode, int wopoint);

void eraseTri(Cell* c, pdd fnode, pdd snode, int wopoint);

bool isshape(pdd fnode, pdd snode, int updown, double mincellsize);

void checkInner(double LeftDownX, double LeftDownY, double initRCellsize, int Xcount, int Ycount, bool flag);

void deleteInnerFace();

void tailcut_rect(Cell* c, pdd inode, pdd cnode, bool updown, int type);

void tailcut_tri(Cell* c, pdd inode, pdd cnode, bool updown, int type);
void tailmanager(pdd startnode, pdd endnode, double mincellsize, int layer, int depth);
bool cmp_vertical(const pdd &a, const pdd &b);
#endif /* Airfoile_hpp */
