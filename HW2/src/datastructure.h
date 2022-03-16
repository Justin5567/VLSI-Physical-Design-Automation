#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Node{
    public:
        int id;
        Node *prev,*next;
        Node (int);
};

class Cell{
public:
    vector <int> cellToNet;
    string name;
    int size, pins, gain;
    bool group, lock;
    Node *point;

    Cell(string,int,bool,int);

    
};




#endif