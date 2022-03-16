#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <limits.h>
// #include "test.h"
#include "datastructure.h"
using namespace std;

/******************************/
map <string, Block*> block_map;
map <string, Terminal*> terminal_map;
vector<Block*> block_list;
vector<Terminal*> terminal_list;
vector<Net*> net_list;
int block_count = 0;
int pl_count = 0;
int net_count = 0;
int numnets = 0;
int numpins = 0;
int numblocks = 0;
int numterminals = 0;
int total_area;
double deadSpaceRatio = 0;
double fixedWidth = 0;
vector<int> contour;
int best_wirelength = 0;
int uphill = 0;
int reject = 0;
Block *root_block;
int block_remain = 0;
double init_wire = 0;
double init_area = 0;
double best_cost = 0;
bool in_flag = false;
vector<Block*> feasible_block_list;
vector<Net*> feasible_net_list;
Block *feasible_root;
/******************************/

void left_traverse();

//calculate feasible block for output
int calculate_wirelength2(){
    int total_wire_length = 0;
    for(int i=0;i<feasible_net_list.size();++i){
        int x_min=INT_MAX;
        int x_max=0;
        int y_min=INT_MAX;
        int y_max=0;
        int wirelength = 0;
        int count = 0;
        for(int j=0;j<feasible_net_list[i]->block_vec.size();++j){
            int x_center = feasible_net_list[i]->block_vec[j]->x +feasible_net_list[i]->block_vec[j]->width/2;
            int y_center = feasible_net_list[i]->block_vec[j]->y +feasible_net_list[i]->block_vec[j]->height/2;
            if(x_center<x_min){
                x_min = x_center;
            }
            if(x_center>x_max){
                x_max = x_center;
            }
            if(y_center<y_min){
                y_min = y_center;
            }
            if(y_center>y_max){
                y_max = y_center;
            }
            count++;
        }
        for(int j=0;j<feasible_net_list[i]->terminal_vec.size();++j){
            if(feasible_net_list[i]->terminal_vec[j]->x<x_min){
                x_min = feasible_net_list[i]->terminal_vec[j]->x;
            }
            if(feasible_net_list[i]->terminal_vec[j]->x>x_max){
                x_max = feasible_net_list[i]->terminal_vec[j]->x;
            }
            if(feasible_net_list[i]->terminal_vec[j]->y<y_min){
                y_min = feasible_net_list[i]->terminal_vec[j]->y;
            }
            if(feasible_net_list[i]->terminal_vec[j]->y>y_max){
                y_max = feasible_net_list[i]->terminal_vec[j]->y;
            }
        }
        total_wire_length += (x_max - x_min)+(y_max - y_min);
    }
    


    return total_wire_length;
}


//traverse the tree in post order
void traverse(Block* curr_block){
    if(curr_block->left!=NULL){
        traverse(curr_block->left);
    }
    if(curr_block->right!=NULL){
        traverse(curr_block->right);
    }
//     Block *tmp_ptr = curr_block;
//    cout<<tmp_ptr->block_name<<" ";
//         cout<<tmp_ptr<<" ";
//         if(tmp_ptr->left!=nullptr){
//             cout<<tmp_ptr->left->id<<" ";
//         }else{
//             cout<<"NULL"<<" ";
//         }
//         if(tmp_ptr->right!=nullptr){
//             cout<<tmp_ptr->right->id<<" ";
//         }else{
//             cout<<"NULL"<<" ";
//         }
//         if(tmp_ptr->parent!=nullptr){
//             cout<<tmp_ptr->parent->id<<" ";
//         }else{
//             cout<<"NULL"<<" ";
//         }
//         cout<<endl;
    block_remain++;
}

//update the coordinate
void update_coor(Block* curr_block, bool isLeft){
    Block *parent_block = curr_block->parent;
    
    curr_block->x = 0;
    curr_block->y = 0;
    if(!parent_block){
        // cout<<"enter root "<<curr_block->id<<endl;
        curr_block->x = 0;
        curr_block->y = 0;
    }
    else if(isLeft){
        curr_block->x = parent_block->x + parent_block->width;
    }
    else{
        curr_block->x = parent_block->x;
    }
    
    //find the max height in range and update new contour
    int tmp_left = curr_block->x;
    int tmp_right = curr_block->x+curr_block->width;
    int y_max = 0;
    for(int i=tmp_left;i<tmp_right;++i){
        if(contour[i]>y_max){
            y_max = contour[i];
        }
    }
    curr_block->y = y_max;
    y_max += curr_block->height;
    for(int i=tmp_left;i<tmp_right;++i){
        contour[i] = y_max;
    }
    // cout<<curr_block->id<<" "<<curr_block->left<<" "<<curr_block->right<<endl;
    if(curr_block->left !=NULL){
        update_coor(curr_block->left,true);
    }
    if(curr_block->right!=NULL){
        update_coor(curr_block->right,false);
    }
}

//calculate the wirelength with penalty
int calculate_wirelength(){
    int total_wire_length = 0;
    for(int i=0;i<net_list.size();++i){
        int x_min=INT_MAX;
        int x_max=0;
        int y_min=INT_MAX;
        int y_max=0;
        int wirelength = 0;
        int count = 0;
        for(int j=0;j<net_list[i]->block_vec.size();++j){
            int x_center = net_list[i]->block_vec[j]->x +net_list[i]->block_vec[j]->width/2;
            int y_center = net_list[i]->block_vec[j]->y +net_list[i]->block_vec[j]->height/2;
            if(x_center<x_min){
                x_min = x_center;
            }
            if(x_center>x_max){
                x_max = x_center;
            }
            if(y_center<y_min){
                y_min = y_center;
            }
            if(y_center>y_max){
                y_max = y_center;
            }
            count++;
        }
        for(int j=0;j<net_list[i]->terminal_vec.size();++j){
            if(net_list[i]->terminal_vec[j]->x<x_min){
                x_min = net_list[i]->terminal_vec[j]->x;
            }
            if(net_list[i]->terminal_vec[j]->x>x_max){
                x_max = net_list[i]->terminal_vec[j]->x;
            }
            if(net_list[i]->terminal_vec[j]->y<y_min){
                y_min = net_list[i]->terminal_vec[j]->y;
            }
            if(net_list[i]->terminal_vec[j]->y>y_max){
                y_max = net_list[i]->terminal_vec[j]->y;
            }
        }
        total_wire_length += (x_max - x_min)+(y_max - y_min);
    }
    
    // for(int i=0;i<block_count;i++){
    //     int x_right = block_list[i]->x + block_list[i]->width;
    //     int y_top = block_list[i]->y + block_list[i]->height;
    //     if(x_right>fixedWidth || y_top>fixedWidth){
    //         total_wire_length+=100*block_list[i]->area+5000;
    //     }
    // }

    // bool incheck = true;
    // for(int i=0;i<block_count;i++){
    //     int x_right = block_list[i]->x + block_list[i]->width;
    //     int y_top = block_list[i]->y + block_list[i]->height;
    //     if(x_right>fixedWidth || y_top>fixedWidth){
    //         incheck = false;
    //         break;
    //     }
    // }
    // if(incheck){
    //     total_wire_length-=50000;
    // }
    // double ratio = 0;
    // double now_x = 0;
    // double now_y = 0;
    // double now_area = 0;
    // for(int i=0;i<block_count;i++){
    //     int x_right = block_list[i]->x + block_list[i]->width;
    //     int y_top = block_list[i]->y + block_list[i]->height;
    //     if(x_right>now_x){
    //         now_x = x_right;
    //     }
    //     if(y_top>now_y){
    //         now_y = y_top;
    //     }
    // }
    // now_area = now_x*now_y;

    // int flag = 0;
    // for(int i=0;i<block_count;i++){
    //     int x_right = block_list[i]->x + block_list[i]->width;
    //     int y_top = block_list[i]->y + block_list[i]->height;
    //     if(x_right>fixedWidth || y_top>fixedWidth){
    //         flag=1;
    //         break;
    //     }
    // }
    // if(flag==0){
    //     total_wire_length -= 50000;
    // }
    // add penalty
    // for(int i=0;i<block_count;i++){
    //     int x_right = block_list[i]->x + block_list[i]->width;
    //     int y_top = block_list[i]->y + block_list[i]->height;
    //     total_wire_length+= sqrt(x_right^2+y_top^2)*block_list[i]->aspect_ratio;
    // }
    // for(int i=0;i<block_count;i++){
    //     total_wire_length+=sqrt(block_list[i]->x^2+block_list[i]->y^2)*block_list[i]->area;
    // }

    return total_wire_length;
}

//rotate the block
void rotate(Block* curr_block){
    int tmp = curr_block->height;
    curr_block->height = curr_block->width;
    curr_block->width = tmp;
    curr_block->isRotate = 1- curr_block->isRotate;
}

//copy a block to tmp class
Block* copy_class(Block* block1){
    Block *tmp = new Block(-1,-1,-1,block1->block_name,-1);
    tmp->area = block1->area;
    tmp->block_name = block1->block_name;
    tmp->height = block1->height;
    tmp->id = block1->id;
    tmp->isRotate = block1->isRotate;
    tmp->left = block1->left;
    tmp->parent = block1->parent;
    tmp->right = block1->right;
    tmp->width = block1->width;
    tmp->x = block1->x;
    tmp->y = block1->y;
    tmp->aspect_ratio = block1->aspect_ratio;
    return tmp;
}

//swap two block
void swap(Block* block1, Block* block2){
    //swap two block
    Block *block1_parent,*block1_left_child,*block1_right_child;
    Block *block2_parent,*block2_left_child,*block2_right_child;
    block1_parent = block1->parent;
    block1_left_child = block1->left;
    block1_right_child = block1->right;

    block2_parent = block2->parent;
    block2_left_child = block2->left;
    block2_right_child = block2->right;

    int block1_check = -1;
    int block2_check = -1;

    //check root and parent's leftright child
    if(block1_parent!=nullptr){ 
        if(block1_parent->left==block1){
            block1_check = 0;
        }
        else{
            block1_check = 1;
        }
    }
    if(block2_parent!=nullptr){
        if(block2_parent->left==block2){
            block2_check = 0;
        } 
        else{
            block2_check = 1;
        }
        
    }

    if(block1_parent == block2){
        //block1 swap to block2
        if(block2_check==0){
            
            block2_parent->left = block1;
        } 
        else if(block2_check==1){
            block2_parent->right = block1;
        }
        block1->parent = block2_parent; 
        if(block2->left == block1){
            block1->left = block2;
            block1->right = block2_right_child;
            if(block2_right_child!=NULL){
                block2_right_child->parent = block1;
            }
            
        }
        else if(block2->right == block1){
            block1->left = block2_left_child;
            if(block2_left_child!=NULL){
                block2_left_child->parent = block1;
            }
            
            block1->right = block2;
        }
        else{
            cout<<"error"<<endl;
        }

        block2->parent = block1;//*
        block2->left = block1_left_child;
        if(block1_left_child!=NULL){
            block1_left_child->parent = block2;
        }
    
        block2->right = block1_right_child;
        if(block1_right_child!=NULL){
            block1_right_child->parent = block2;
        }
        
    }
    
    else if(block2_parent == block1){
        // block2 swap to block1
        if(block1_check==0){
            block1_parent->left = block2;
        } 
        else if(block1_check==1){
            block1_parent->right = block2;
        }
        block2->parent = block1_parent;
        if(block1->left==block2){//*
            block2->left = block1;
            
            block2->right = block1_right_child;
            if(block1_right_child!=nullptr){
                block1_right_child->parent = block2;
            }
            
        }
        else if(block1->right==block2){//*
            block2->left = block1_left_child;
            if(block1_left_child!=nullptr){
                block1_left_child->parent = block2;
            }
            block2->right = block1;
        }
        else{
            cout<<"error"<<endl;
        }
        
        block1->parent = block2; //*
        block1->left = block2_left_child;
        if(block2_left_child!=nullptr){
            block2_left_child->parent = block1;
        }
        
        block1->right = block2_right_child;
        if(block2_right_child!=nullptr){
            block2_right_child->parent = block1;
        }
        

    }
    else{
        //block2 swap to block1
        if(block1_check==0){
            block1_parent->left = block2;
        } 
        else if(block1_check==1){
            block1_parent->right = block2;
        }
        block2->parent = block1_parent;
        block2->left = block1_left_child;
        if(block1_left_child!=nullptr){
            block1_left_child->parent = block2;
        }
        block2->right = block1_right_child;
        if(block1_right_child!=nullptr){
            block1_right_child->parent = block2;
        }
        //block1 swap to block2
        if(block2_check==0){
            block2_parent->left = block1;
        } 
        else if(block2_check==1){
            block2_parent->right = block1;
        }
        block1->parent = block2_parent;
        block1->left = block2_left_child;
        if(block2_left_child!=nullptr){
            block2_left_child->parent = block1;
        }
        block1->right = block2_right_child;
        if(block2_right_child!=nullptr){
            block2_right_child->parent = block1;
        }
        
    }
}

//move from_block to to_block
void move(Block* from_block, Block* to_Block){
    //delete origin node
    //check child status
    //0 child
    // cout<<"DELETING"<<endl;
    if(from_block->left == nullptr && from_block->right == nullptr){
        
        if(from_block->parent!=nullptr){
            if(from_block->parent->left == from_block){
                from_block->parent->left = nullptr;
            }
            else{
                from_block->parent->right = nullptr;
            }
        } 
        // else{
        //     cout<<"WHY UR PARENT IS NULL???"<<endl;
        // }
    }
    // 1 right child
    else if(from_block->left == nullptr && from_block->right != nullptr){
        // cout<<"ONE RIGHT CHILD CASE"<<endl;
        if(from_block->parent != nullptr){
            if(from_block->parent->left == from_block){
                from_block->parent->left = from_block->right;
            }
            else{
                from_block->parent->right = from_block->right;
            }
        } 
        // else{
        //     cout<<"WHY UR PARENT IS NULL???"<<endl;
        // }
        if(from_block->right != nullptr){
            from_block->right->parent = from_block->parent;
        } 
        // else{
        //     cout<<"WHY UR CHILD IS NULL???"<<endl;
        // }
        if(from_block==root_block){
            // cout<<"ORIGIN ROOT "<<root_block->id<<endl;
            // cout<<"NEW ROOT "<<from_block->right->id<<endl;
            root_block = from_block->right;
            // cout<<"CHECK CHANGE "<<root_block->id<<endl;
        }
    }
    // 1 left child
    else if(from_block->left != nullptr && from_block->right == nullptr){
        // cout<<"ONE LEFT CHILD CASE"<<endl;
        if(from_block->parent != nullptr){
            if(from_block->parent->left == from_block){
                from_block->parent->left = from_block->left;
            }
            else{
                from_block->parent->right = from_block->left;
            }
        } 
        // else{
        //     cout<<"WHY UR PARENT IS NULL???"<<endl;
        // }
        if(from_block->left!=nullptr){
            from_block->left->parent = from_block->parent;
        }
        // else{
        //     cout<<"WHY UR CHILD IS NULL???"<<endl;
        // }
        if(from_block==root_block){
            // cout<<"ORIGIN ROOT "<<root_block->id<<endl;
            // cout<<"NEW ROOT "<<from_block->left->id<<endl;
            root_block = from_block->left;
            // cout<<"CHECK CHANGE "<<root_block->id<<endl;
        }
    }
    //2 child
    else{
        // cout<<"TWO CHILD CASE"<<endl;
        bool leftright;
        while(from_block->left != nullptr && from_block->right != nullptr){
            leftright = (int)rand()%2;
            // leftright = 0;
            if(leftright==0){
                if(from_block == root_block){
                    root_block = from_block->left;
                }
                // if(from_block->left!=nullptr){
                //     cout<<"NOW "<<from_block->id<<endl;
                //     cout<<"SWAP "<<from_block->left->id<<endl;
                // } 
                swap(from_block,from_block->left);
            }
            else{
                if(from_block == root_block){
                    root_block = from_block->right;
                }
                // if(from_block->right!=nullptr){
                //     cout<<"NOW "<<from_block->id<<endl;
                //     cout<<"SWAP "<<from_block->right->id<<endl;
                // }
                swap(from_block,from_block->right);
            }
        }
        // if(from_block->left!=nullptr){
        //     cout<<from_block->left->id<<" ";
        // } else{
        //     cout<<"NULL ";
        // }
        // if(from_block->right!=nullptr){
        //     cout<<from_block->right->id<<" ";
        // }else{
        //     cout<<"NULL ";
        // }

        // cout<<"SWAP DONE"<<endl;
        // cout<<root_block->id<<endl;
        if(from_block->left == nullptr && from_block->right != nullptr){
            if(from_block->parent->left == from_block){
                from_block->parent->left = from_block->right;
            }
            else{
                from_block->parent->right = from_block->right;
            }
            // check if child is nullptr
            if(from_block->right!=nullptr){
                from_block->right->parent = from_block->parent;
            }
            
        }
        else{
            if(from_block->parent->left == from_block){
                from_block->parent->left = from_block->left;
            }
            else{
                from_block->parent->right = from_block->left;
            }
            // check if child is nullptr
            if(from_block->left!=nullptr){
                from_block->left->parent = from_block->parent;
            }
            
        }
    }

    from_block->parent = nullptr;
    from_block->left = nullptr;
    from_block->right = nullptr;

    // cout<<"INSERT"<<endl;
    //insert to the to block's child

    bool leftright2 = (int)rand()%2;
    // leftright2=0;
    if(leftright2==0){
        Block *tmp_child = to_Block->left;
        from_block->left = tmp_child;
        if(tmp_child!=nullptr){
            tmp_child->parent = from_block;
        }
        to_Block->left = from_block;
        from_block->parent = to_Block;
    }
    else{
        Block *tmp_child = to_Block->right;
        from_block->right = tmp_child;
        if(tmp_child!=nullptr){
            tmp_child->parent = from_block;
        }
        to_Block->right = from_block;
        from_block->parent = to_Block;
    }
    // cout<<"END INSERT"<<endl;
}

//write the feasible output file
void output_write_feasible(const char* filename){
    ofstream output_file;
    output_file.open(filename);
    output_file<<"Wirelength "<<best_wirelength<<"\n";
    output_file<<"Blocks\n";
    for(int i=0;i<numblocks;i++){
        output_file<<feasible_block_list[i]->block_name<<" "<<feasible_block_list[i]->x<<" "<<feasible_block_list[i]->y<<" "<<feasible_block_list[i]->isRotate<<"\n";
    }
}

//write not feasible file
void output_write(const char* filename){
    ofstream output_file;
    output_file.open(filename);
    output_file<<"Wirelength "<<best_wirelength<<"\n";
    output_file<<"Blocks\n";
    for(int i=0;i<numblocks;i++){
        output_file<<block_list[i]->block_name<<" "<<block_list[i]->x<<" "<<block_list[i]->y<<" "<<block_list[i]->isRotate<<"\n";
    }
}

//traverse left child
void left_traverse(){
    Block *tmp_ptr = root_block;
    while(tmp_ptr!=nullptr){
        cout<<tmp_ptr->block_name<<" ";
        cout<<tmp_ptr<<" ";
        if(tmp_ptr->left!=nullptr){
            cout<<tmp_ptr->left->id<<" ";
        }else{
            cout<<"NULL"<<" ";
        }
        if(tmp_ptr->right!=nullptr){
            cout<<tmp_ptr->right->id<<" ";
        }else{
            cout<<"NULL"<<" ";
        }
        if(tmp_ptr->parent!=nullptr){
            cout<<tmp_ptr->parent->id<<" ";
        }else{
            cout<<"NULL"<<" ";
        }
        cout<<endl;
        // cout<<tmp_ptr->x<<" ";
        // cout<<tmp_ptr->y<<" ";
        // cout<<tmp_ptr->width<<" ";
        // cout<<tmp_ptr->height<<" ";
        // if(tmp_ptr->parent!=nullptr){
        //     cout<<tmp_ptr->parent->id<<" ";
        //     cout<<tmp_ptr->parent<<endl;

        // }
        // else{
        //     cout<<"NULL NULL"<<endl;
        // }
        tmp_ptr = tmp_ptr->left;
    }
    
}

//find local min
void local_best(){
    int tmp=0;
    while(tmp<500000){
        /*--- make a backup ---*/
        vector<int> tmp_contour;
        vector<Block*> tmp_block_list;
        Block* tmp_root =new Block(-1,-1,-1,"test",-1);
        vector<Net*> tmp_net_list;
        for(int i=0;i<contour.size();i++){
            tmp_contour.push_back(contour[i]);
        }
        for(int i=0;i<block_count;i++){
            // Block* tmp_block = copy_class(block_list[i]);   
            Block*tmp_block=block_list[i];
            tmp_block_list.push_back(tmp_block);
        }
        for(int i=0;i<block_count;i++){
            int parent_id = (tmp_block_list[i]->parent==nullptr)?-1:tmp_block_list[i]->parent->id;
            int left_id = (tmp_block_list[i]->left==nullptr)?-1:tmp_block_list[i]->left->id;
            int right_id = (tmp_block_list[i]->right==nullptr)?-1:tmp_block_list[i]->right->id;
            if(parent_id!=-1){
                tmp_block_list[i]->parent = tmp_block_list[parent_id];
            }
            if(left_id!=-1){
                tmp_block_list[i]->left = tmp_block_list[left_id];
            }
            if(right_id!=-1){
                tmp_block_list[i]->right = tmp_block_list[right_id];
            }
        }
        tmp_root = tmp_block_list[root_block->id];

        for(int i=0;i<net_list.size();i++){
            Net *tmp_net = new Net(net_list[i]->degree);
            for(int j=0;j<net_list[i]->terminal_vec.size();j++){
                tmp_net->terminal_vec.push_back(terminal_list[net_list[i]->terminal_vec[j]->pl_id]);
            }
            for(int j=0;j<net_list[i]->block_vec.size();j++){
                tmp_net->block_vec.push_back(tmp_block_list[net_list[i]->block_vec[j]->id]);
            }
            tmp_net_list.push_back(tmp_net);
        }




        /*--- op ---*/
        int x1 = rand() % numblocks;
        int x2 = rand() % numblocks;
        int y2 = rand() % numblocks;
        int op = rand() % 10;
        if(x2==y2){
            continue;
        }
        if(op>=7){
            rotate(block_list[x1]);
        }
        else if(op>=4){
            if(block_list[x2]->id == root_block->id){
                root_block = block_list[y2];
            }
            else if(block_list[y2]->id == root_block->id){
                root_block = block_list[x2];
            }
            swap(block_list[x2],block_list[y2]);
        }
        else{

            move(block_list[x2],block_list[y2]);
        }
        /*--- caculate new coordinate ---*/
        for(int i=0;i<contour.size();i++){
            contour[i]=0;
        }
        if(root_block->parent!=nullptr){
            cout<<"there is an error"<<endl;
        }
        update_coor(root_block,true);
        /*--- cost ---*/
        int new_wirelength = calculate_wirelength();
        int cost = new_wirelength - best_wirelength;
        double mutation_rate = (double)rand()/RAND_MAX;


        // cout<<cost<<endl;
        if(cost<=0){
            best_wirelength = new_wirelength;
            cout<<"BETTER WIRELENGTH "<<best_wirelength<<endl;
        }
        else{
            root_block = tmp_root;
            block_list = tmp_block_list;
            net_list = tmp_net_list;
            for(int i=0;i<contour.size();i++){
                contour[i]=0;
            }
            update_coor(root_block,true);
        }
        tmp++;
    }
}

//make a backup
void backup_list(){
    
}

//new cost function
double cost_func(){
    int total_wire_length = 0;
    for(int i=0;i<net_list.size();++i){
        int x_min=INT_MAX, x_max=0;
        int y_min=INT_MAX, y_max=0;
        int wirelength = 0;
        int count = 0;
        for(int j=0;j<net_list[i]->block_vec.size();++j){
            int x_center = net_list[i]->block_vec[j]->x +net_list[i]->block_vec[j]->width/2;
            int y_center = net_list[i]->block_vec[j]->y +net_list[i]->block_vec[j]->height/2;
            if(x_center<x_min){
                x_min = x_center;
            }
            if(x_center>x_max){
                x_max = x_center;
            }
            if(y_center<y_min){
                y_min = y_center;
            }
            if(y_center>y_max){
                y_max = y_center;
            }
            count++;
        }
        for(int j=0;j<net_list[i]->terminal_vec.size();++j){
            if(net_list[i]->terminal_vec[j]->x<x_min){
                x_min = net_list[i]->terminal_vec[j]->x;
            }
            if(net_list[i]->terminal_vec[j]->x>x_max){
                x_max = net_list[i]->terminal_vec[j]->x;
            }
            if(net_list[i]->terminal_vec[j]->y<y_min){
                y_min = net_list[i]->terminal_vec[j]->y;
            }
            if(net_list[i]->terminal_vec[j]->y>y_max){
                y_max = net_list[i]->terminal_vec[j]->y;
            }
        }
        total_wire_length += (x_max - x_min)+(y_max - y_min);
    }
    // for(int i=0;i<block_count;i++){
    //     int x_right = block_list[i]->x + block_list[i]->width;
    //     int y_top = block_list[i]->y + block_list[i]->height;
    //     if(x_right>fixedWidth || y_top>fixedWidth){
    //         total_wire_length+=100*block_list[i]->area+5000;
    //     }
    // }
    int now_x = 0;
    int now_y = 0;
    int now_area = 0;
    for(int i=0;i<block_count;i++){
        int x_right = block_list[i]->x + block_list[i]->width;
        int y_top = block_list[i]->y + block_list[i]->height;
        if(x_right>now_x){
            now_x = x_right;
        }
        if(y_top>now_y){
            now_y = y_top;
        }
    }
    now_area = now_x*now_y;
    if(init_wire==0){
        init_wire = total_wire_length;
    }
    if(init_area==0){
        init_area = now_area;
    }
    
    double x_pen = 0;
    double y_pen = 0;
    if(now_x>fixedWidth){
        x_pen = (double) now_x/fixedWidth;
    }
    if(now_y>fixedWidth){
        y_pen = (double) now_y/fixedWidth;
    }


    double now_R = (double)now_x/now_y;
    double a = 0;
    double b = 0;
    double c = 0;
    double dx = 0;
    double dy = 0;
    a = 1;
    b = 1;
    c = 0.6;
    dx = 10;
    dy = 10;


    
    double now_cost = a*(now_area/init_area) + b*(total_wire_length/init_wire) + c*(1-now_R)*(1-now_R)+dx*x_pen+dy*y_pen;
    // double now_cost = a*(now_area/init_area) + b*(total_wire_length/init_wire) + c*(1-now_R)*(1-now_R);
    return now_cost;
}
//SA algorithm
void SA(){
    int MT = 0;
    int uphill = 0;
    int reject = 0;
    int k=5 ;
    long long int T=1000;
    int N = k*block_count;
    double reduce_rate = 0.95;
    clock_t t_start, t_end;
    double cpu_time_used;
    t_start = clock();
    int rr = 2;
    double cool_down = 1;
    do{
        MT=0;
        uphill=0;
        reject=0;
        int tmp_count = 0;
        do{
            /*--- make a backup ---*/
            vector<int> tmp_contour;
            vector<Block*> tmp_block_list;
            Block* tmp_root =new Block(-1,-1,-1,"test",-1);
            vector<Net*> tmp_net_list;
            for(int i=0;i<contour.size();i++){
                tmp_contour.push_back(contour[i]);
            }
            for(int i=0;i<block_count;i++){
                Block* tmp_block = copy_class(block_list[i]);   
                // Block* tmp_block = block_list[i];
                tmp_block_list.push_back(tmp_block);
            }
            for(int i=0;i<block_count;i++){
                int parent_id = (tmp_block_list[i]->parent==nullptr)?-1:tmp_block_list[i]->parent->id;
                int left_id = (tmp_block_list[i]->left==nullptr)?-1:tmp_block_list[i]->left->id;
                int right_id = (tmp_block_list[i]->right==nullptr)?-1:tmp_block_list[i]->right->id;
                if(parent_id!=-1){
                    tmp_block_list[i]->parent = tmp_block_list[parent_id];
                }
                if(left_id!=-1){
                    tmp_block_list[i]->left = tmp_block_list[left_id];
                }
                if(right_id!=-1){
                    tmp_block_list[i]->right = tmp_block_list[right_id];
                }
            }
            tmp_root = tmp_block_list[root_block->id];

            for(int i=0;i<net_list.size();i++){
                Net *tmp_net = new Net(net_list[i]->degree);
                for(int j=0;j<net_list[i]->terminal_vec.size();j++){
                    tmp_net->terminal_vec.push_back(terminal_list[net_list[i]->terminal_vec[j]->pl_id]);
                }
                for(int j=0;j<net_list[i]->block_vec.size();j++){
                    tmp_net->block_vec.push_back(tmp_block_list[net_list[i]->block_vec[j]->id]);
                }
                tmp_net_list.push_back(tmp_net);
            }
            /*--- op ---*/
            int x1 = rand() % numblocks;
            int x2 = rand() % numblocks;
            int y2 = rand() % numblocks;
            int op = rand() % 10;
            if(x2==y2){
                continue;
            }
            if(op>=7){
                rotate(block_list[x1]);
            }
            else if(op>=4){
                if(block_list[x2]->id == root_block->id){
                    root_block = block_list[y2];
                }
                else if(block_list[y2]->id == root_block->id){
                    root_block = block_list[x2];
                }
                swap(block_list[x2],block_list[y2]);
            }
            else{
                move(block_list[x2],block_list[y2]);
            }
            /*--- caculate new coordinate ---*/
            for(int i=0;i<contour.size();i++){
                contour[i]=0;
            }
            if(root_block->parent!=nullptr){
                cout<<"there is an error"<<endl;
            }
            update_coor(root_block,true);
            /*--- cost ---*/
            MT++;
            // int new_wirelength = calculate_wirelength();
            // int cost = new_wirelength - best_wirelength;
            double new_cost = cost_func();
            double cost = (new_cost-best_cost)*250000*cool_down;
            double mutation_rate = (double)rand()/RAND_MAX;
            /*--- update or backup status ----*/
            int h_tmp = 0;
            int w_tmp = 0;
            for(int i=0;i<block_list.size();i++){
                int x_right = block_list[i]->x + block_list[i]->width;
                int y_top = block_list[i]->y + block_list[i]->height;
                if(x_right>w_tmp){
                    w_tmp=x_right;
                }
                if(y_top>h_tmp){
                    h_tmp=y_top;
                }
            }
            if(h_tmp<=fixedWidth && w_tmp<=fixedWidth){
                int tmp_wire = calculate_wirelength();
                if(tmp_wire<best_wirelength){
                    cout<<"NEW WIRE "<<tmp_wire<<endl;
                    best_wirelength = tmp_wire;
                    vector<Block*> tmp_feasibl_block_list;
                    Block* tmp_feasible_root =new Block(-1,-1,-1,"test",-1);
                    vector<Net*> tmp_feasible_net_list;
                    for(int i=0;i<block_count;i++){
                        Block* tmp_block = copy_class(block_list[i]);   
                        tmp_feasibl_block_list.push_back(tmp_block);
                    }
                    for(int i=0;i<block_count;i++){
                        int parent_id = (tmp_feasibl_block_list[i]->parent==nullptr)?-1:tmp_feasibl_block_list[i]->parent->id;
                        int left_id = (tmp_feasibl_block_list[i]->left==nullptr)?-1:tmp_feasibl_block_list[i]->left->id;
                        int right_id = (tmp_feasibl_block_list[i]->right==nullptr)?-1:tmp_feasibl_block_list[i]->right->id;
                        if(parent_id!=-1){
                            tmp_feasibl_block_list[i]->parent = tmp_feasibl_block_list[parent_id];
                        }
                        if(left_id!=-1){
                            tmp_feasibl_block_list[i]->left = tmp_feasibl_block_list[left_id];
                        }
                        if(right_id!=-1){
                            tmp_feasibl_block_list[i]->right = tmp_feasibl_block_list[right_id];
                        }
                    }
                    tmp_feasible_root = tmp_feasibl_block_list[root_block->id];
                    for(int i=0;i<net_list.size();i++){
                        Net *tmp_net = new Net(net_list[i]->degree);
                        for(int j=0;j<net_list[i]->terminal_vec.size();j++){
                            tmp_net->terminal_vec.push_back(terminal_list[net_list[i]->terminal_vec[j]->pl_id]);
                        }
                        for(int j=0;j<net_list[i]->block_vec.size();j++){
                            tmp_net->block_vec.push_back(tmp_feasibl_block_list[net_list[i]->block_vec[j]->id]);
                        }
                        tmp_feasible_net_list.push_back(tmp_net);
                    }
                    feasible_block_list = tmp_feasibl_block_list;
                    feasible_root = tmp_feasible_root;
                    feasible_net_list = tmp_feasible_net_list;
                }

                
            }



            if(cost<=0 || mutation_rate< exp(-cost/T)){
                if(cost>0){
                    uphill++;
                }
                // best_wirelength = new_wirelength;
                best_cost = new_cost;
                delete tmp_root;
                for(int i=0;i<tmp_net_list.size();i++){
                    delete tmp_net_list[i];
                }
            }
            else{
                reject++;
                delete root_block;
                for(int i=0;i<net_list.size();i++){
                    delete net_list[i];
                }
                root_block = tmp_root;
                block_list = tmp_block_list;
                net_list = tmp_net_list;
                for(int i=0;i<contour.size();i++){
                    contour[i]=0;
                }
                update_coor(root_block,true);
            }
            tmp_count++;

        }while(uphill<=N && MT<=rr*N);
        cout<<"MUTATE RATE "<<(double)uphill/tmp_count<<endl;
        cout<<"T: "<<T<<endl;
        int h_tmp = 0;
        int w_tmp = 0;
        for(int i=0;i<block_list.size();i++){
            int x_right = block_list[i]->x + block_list[i]->width;
            int y_top = block_list[i]->y + block_list[i]->height;
            if(x_right>w_tmp){
                w_tmp=x_right;
            }
            if(y_top>h_tmp){
                h_tmp=y_top;
            }
        }
        cout<<fixedWidth<<" "<<w_tmp<<" "<<h_tmp<<endl;
        cout<<"better cost "<<best_cost<<endl;
        if(T>10000){
            T = T * 0.75;
            rr=1;
            cool_down = 0.3;
        }
        else if(T>500){
            T = T * 0.9;
            rr=2;
            cool_down = 0.5;
        }
        else if(T>200) {
            T = T * 0.975;
            rr=2;
            cool_down = 0.65;
        }
        else if(T>100){
            T = T * 0.98;
            cool_down = 2;
            rr=5;
        }
        else{
            T = T * 0.99;
            cool_down = 2;
            rr=7;
        }
        t_end = clock();
        cpu_time_used = ((double) (t_end - t_start)) / CLOCKS_PER_SEC;
    }while(T>5 && cpu_time_used<1150);
    cout<<"[ TIME ] "<<cpu_time_used<<endl;
    // cout<<"[ WIRELENGTH ]"<<best_wirelength<<endl;
}


int main(int  argc,  char*  argv[]){
    srand(time(NULL));
    string var,var2,var3,var4;
    string tmp_s = argv[5];
    const char*var_tmp = tmp_s.c_str();
    double tmp = atof(var_tmp);
    deadSpaceRatio = tmp;
    // ifstream inFile("./testcases//n100.pl");
    ifstream inFile(argv[3]);
    while(inFile>>var>>var2>>var3){
        string name = var;
        const char*var_tmp2 = var2.c_str();
        int x_tmp = atoi(var_tmp2);
        const char*var_tmp3 = var3.c_str();
        int y_tmp = atoi(var_tmp3);
        Terminal *tmp_PL = new Terminal(pl_count,x_tmp,y_tmp,name);
        terminal_map.insert(pair<string,Terminal*>(name,tmp_PL));
        terminal_list.push_back(tmp_PL);
        pl_count++;
    }
    
    // ifstream inFile2("./testcases//n100.hardblocks");
    ifstream inFile2(argv[1]);
    while(getline(inFile2,var)){
        string var2;
        istringstream ss(var);
        getline(ss,var2,' ');
        if(var2=="NumHardRectilinearBlocks")
        {
            getline(ss,var2,' ');
            getline(ss,var2,' ');
            numblocks = stoi(var2);
        }
        if(var2=="NumTerminals")
        {
            getline(ss,var2,' ');
            getline(ss,var2,' ');
            numterminals = stoi(var2);
        }
        if(var2[0]=='s'){
            stringstream ss(var);
            int i=0;
            string tok;
            string b_name;
            int id, width, height;
            while(getline(ss,tok,' ')){
                if(i==0){   //ID
                    b_name = tok;
                }
                if(i==7){   //weight
                    const char*var_tmp = tok.c_str()+1;
                    int tmp1 = atoi(var_tmp);
                    width = tmp1;
                }
                if(i==8){   //height
                    const char*var_tmp = tok.c_str();
                    int tmp1 = atoi(var_tmp);
                    height = tmp1;
                }
                i++;
            }
            double ratio = (width<height)?(double)width/height : (double)height/width;
            // cout<<ratio<<endl;
            Block *tmp_block = new Block(block_count, width, height, b_name,ratio);
            total_area += tmp_block->output_area();
            block_map.insert(pair<string,Block*>(b_name,tmp_block));
            block_list.push_back(tmp_block);
            block_count++;
            // block_list.push_back(tmp);
        }
    }

    // ifstream inFile3("./testcases//n100.nets");
    ifstream inFile3(argv[2]);
    while(inFile3>>var){ //netlist
        if(var=="NumNets"){
            inFile3>>var2>>var3;
            numnets = stoi(var3);
        }
        if(var=="NumPins"){
            inFile3>>var2>>var3;
            numpins = stoi(var3);
        }
        if(var=="NetDegree"){
            int i=0;
            inFile3>>var2>>var3;
            const char*var3_tmp = var3.c_str();
            int degree = atoi(var3_tmp);
            Net* tmp_net = new Net(degree);
            Block* tmp_block;
            Terminal* tmp_terminal;
            while(i!=degree){
                inFile3>>var4;
                if(var4[0]=='p'){
                    tmp_terminal = terminal_map.find(var4)->second;
                    tmp_net->add_terminal(tmp_terminal);
                    // cout<<"p "<<tmp4<<endl;
                    
                }else if(var4[0]=='s'){
                    tmp_block = block_map.find(var4)->second;
                    tmp_net->add_block(tmp_block);
                    // cout<<"sb "<<tmp4<<endl;
                }
                i++;
            }
            net_list.push_back(tmp_net);
        }
    }
    
        
    fixedWidth = sqrt(total_area*(1+deadSpaceRatio));
    cout<<"block size "<<block_map.size()<<endl;
    cout<<"total area "<<total_area<<endl;
    cout<<"fixedWidth "<< fixedWidth<<endl;
    /*------------- Build initial BTree---------------------------*/
    while(1){
        queue<Block*> q_order;
        block_list[0]->parent = NULL;
        q_order.push(block_list[0]);
        int i=1;
        while(i!=numblocks){
            Block* tmp = q_order.front();
            q_order.pop();
            tmp->left = block_list[i];
            block_list[i]->parent = tmp;
            q_order.push(block_list[i]);
            ++i;
            if(i==numblocks){
                break;
            }
            tmp->right = block_list[i];
            block_list[i]->parent = tmp;
            q_order.push(block_list[i]);
            ++i;
        }
        break;
    }



    

    /*--------------------- update btree x and y  -----------*/
    contour.resize(10*fixedWidth,0);
    update_coor(block_list[0],true);
    /*--------------------------SA--------------------------*/
    best_wirelength = calculate_wirelength();
    cout<<"initial "<<best_wirelength<<endl;
    best_cost = cost_func();
    cout<<"[ COST Initial ]" <<best_cost<<endl;
    root_block = block_list[0];
    // local_best();
    SA();

    
    if(feasible_root==NULL){
        best_wirelength = calculate_wirelength();
        output_write(argv[4]);
    }
    else{
        cout<<"There is a feasible sol"<<endl;
        best_wirelength = calculate_wirelength2();
        output_write_feasible(argv[4]);
    }
    
    
    cout<<"finish all program"<<endl;
    return 0;
}


/*

*/