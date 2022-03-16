#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <unordered_map>
#include <algorithm>
#include <limits>
#include <stdlib.h>
#include <math.h> 
#include <iostream>
#include <vector>
#include <queue>
#include <time.h>
using namespace std;




class Block {
    private:
        
    public:
        string block_name;
        int id;
        int width,height;
        int area;
        int x;
        int y;
        double aspect_ratio;
        bool isRotate;
        Block* parent;
        Block* left;
        Block* right;
        Block(int, int, int, string, double);
        void block_copy(Block*);
        int output_area();
        void display_family();
        friend class Termianl;
        friend class Net;
        ~Block();
};

class Terminal{
    private:
        
    public:
        string pl_name;
        int pl_id;
        int x;
        int y;
        Terminal(int,int,int,string);
        friend class Block;
        friend class Net;
};

class Net {
    private:
        
    public:
        // string net_name;
        vector<Block*> block_vec;
        vector<Terminal*> terminal_vec;
        int degree;
        Net(int);
        void add_terminal(Terminal*);
        void add_block(Block*);
        void display_net();
        friend class Block;
        friend class Terminal;
        ~Net();
};



#endif