#include "datastructure.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

Node::Node(int id){
    this->id = id;
    this->prev = nullptr;
    this->next = nullptr;
}

Cell::Cell(string newname, int newsize, bool newgroup, int id){
    this->name = newname;
    this->size = newsize;
    this->pins = 0;
    this->gain = 0;
    this->group = newgroup;
    this->lock = false;
    this->point = new Node(id);
}

