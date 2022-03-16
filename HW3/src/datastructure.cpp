#include "datastructure.h"
#include <iostream>
#include <vector>
using namespace std;

Block::Block(int id, int width, int height,string name, double ratio){
    this->block_name = name;
    this->id = id;
	this->area = width * height;
	this->width = width;
	this->height = height;
	this->x = 0.0;
	this->y = 0.0;
	this->isRotate = false;
    this->parent = NULL;
    this->left = NULL;
    this->right = NULL;
    this->aspect_ratio = ratio;
}

void Block::block_copy(Block* curr_block){
    this->block_name = curr_block->block_name;
    this->id = curr_block->id;
	this->area = curr_block->area;
	this->width = curr_block->width;
	this->height = curr_block->height;
	this->x = curr_block->x;
	this->y = curr_block->y;
	this->isRotate = curr_block->isRotate;
    this->parent = curr_block->parent;
    this->left = curr_block->left;
    this->right = curr_block->right;
    this->aspect_ratio = curr_block->aspect_ratio;
}


int Block::output_area(){
    return this->area;
}

Block::~Block(){
    // cout<<"deleting "<<this->id<<endl;
    delete left;
    delete right;
    
}

void Block::display_family(){
    cout<<"ID "<<this->id;
    if(this->parent){
        cout<<" parent : "<<this->parent->id;
    } else{
        cout<<" parent : NULL";
    }
    if(this->left){
        cout<<" left : "<<this->left->id;
    } else{
        cout<<" left : NULL";
    }
    if(this->right){
        cout<<" right : "<<this->right->id;
    } else{
        cout<<" right : NULL";
    }
    cout<<endl;
}



Terminal::Terminal(int pl_id, int x, int y,string name){
    this->pl_name = name;
    this->pl_id = pl_id;
    this->x = x;
    this->y = y;
}


Net::Net(int degree){
    this->degree = degree;
}


void Net::add_terminal(Terminal* tmp_terminal){
    this->terminal_vec.push_back(tmp_terminal);
}

void Net::add_block(Block* tmp_block){
    this->block_vec.push_back(tmp_block);
}



void Net::display_net(){
    cout<<"degree: "<<this->degree<<endl;
    for(int i=0;i<this->terminal_vec.size();i++){
        cout<<terminal_vec[i]->pl_name<<" ";
    }
    cout<<endl;
    for(int i=0;i<this->block_vec.size();i++){
        cout<<block_vec[i]->block_name<<" ";
    }
    cout<<endl<<"======="<<endl;
}

Net::~Net(){
    int b_count = this->block_vec.size();
    int t_count = this->terminal_vec.size();
    // for(int i=0;i<b_count;i++){
    //     delete block_vec[i];
    // }
    block_vec.clear();
    // for(int i=0;i<t_count;i++){
    //     delete terminal_vec[i];
    // }
    terminal_vec.clear();
}