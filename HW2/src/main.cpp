#include <iostream>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <map>
#include <algorithm>
#include <time.h>
#include "datastructure.h"
using namespace std;


/*--------------------------------------*/
map<string, int> cellMap;
vector<Cell*> cell_list;
vector<vector<int> > net_list;
map<int,Node*> bucket_list_left;
map<int,Node*> bucket_list_right; 
vector<int> move_history;
int cell_count = 0 ;
int left_area = 0;
int right_area = 0;
int total_area = 0;
int area_constraint = 0;
int net_count = 0;
int p_max = 0; 
bool leftright = 0;

int left_cell_count = 0;
int right_cell_count = 0;
int tmp_left_cell_count = 0;
int tmp_right_cell_count = 0;
int best_cut = 0;

int gain = 0;
int best_gain = 0;
int best_left_count = 0;
int best_right_count = 0;
int best_left_area = 0;
int best_right_area = 0;
int best_tt=0;

clock_t t_start, t_end;
double cpu_time_used;
/*--------------------------------------*/


int compute_cut(){
    int cut_count=0;
    for(int i=0;i<net_list.size();i++){
        int tmp=cell_list[net_list[i][0]]->group;
        for(int j = 1;j<net_list[i].size();j++){
            if(tmp!=cell_list[net_list[i][j]]->group){
                cut_count++;
                break;
            }
        }
    }
    return cut_count;
}

void build_bucket_list(){
    bucket_list_left.clear();
    bucket_list_right.clear();
    for(int i=-p_max;i<=p_max;i++){
        bucket_list_left[i] = new Node(-1);
        bucket_list_right[i] = new Node(-1);
    }

    for(int i=0;i<cell_count;i++){
        // cout<<i<<endl;
        int gain = cell_list[i]->gain;
        Node* curr_node = cell_list[i]->point;
        if(cell_list[i]->group == false){
            curr_node->prev = bucket_list_left[gain];
            curr_node->next = bucket_list_left[gain]->next;
            bucket_list_left[gain]->next = curr_node;
            if(curr_node->next!=nullptr){
                curr_node->next->prev = curr_node;
            }
        }
        else{
            curr_node->prev = bucket_list_right[gain];
            curr_node->next = bucket_list_right[gain]->next;
            bucket_list_right[gain]->next = curr_node;
            if(curr_node->next!=nullptr){
                curr_node->next->prev = curr_node;
            }
        }
    }

}

// display the leftright bucketlist
void display_bucket_list(){
    Node* test_node;
    Node* test_node2;
    for(int i=-p_max;i<=p_max;i++){
        cout<<i<<endl;
        cout<<"left: ";
        test_node = bucket_list_left[i];
        while(test_node->next!=nullptr){
            test_node = test_node->next;
            cout<<test_node->id<<" ";
            
        }
        cout<<endl<<"right: ";
        test_node = bucket_list_right[i];
        while(test_node->next!=nullptr){
            test_node = test_node->next;
            cout<<test_node->id<<" ";
           
        }
        cout<<endl<<"======"<<endl;
    }
}

void compute_pmax(){
    for(int i=0;i<cell_count;i++){
        if(cell_list[i]->pins>p_max){
            p_max = cell_list[i]->pins;
        }
    }
}

// update initial gain
void compute_init_gain(){
    for(int i=0;i<cell_count;i++){
        cell_list[i]->gain = 0;
        cell_list[i]->lock = 0;
    }
    tmp_left_cell_count = left_cell_count;
    tmp_right_cell_count = right_cell_count;
    for(int i=0;i<net_count;i++){
        vector<int> tmp_left;
        vector<int> tmp_right;
        for(int j=0;j<net_list[i].size();j++){
            if(cell_list[net_list[i][j]]->group==false){
                tmp_left.push_back(net_list[i][j]);
            }
            if(cell_list[net_list[i][j]]->group==true){
                tmp_right.push_back(net_list[i][j]);
            }
        }
        if(tmp_left.size()==0){
            for(int j=0;j<tmp_right.size();j++){
                cell_list[tmp_right[j]]->gain--;
                // gain_list[tmp_right[j]]--;
            }
        }
        if(tmp_right.size()==0){
            for(int j=0;j<tmp_left.size();j++){
                cell_list[tmp_left[j]]->gain--;
                // gain_list[tmp_left[j]]--;
            }
        }
        if(tmp_left.size()==1){
            cell_list[tmp_left[0]]->gain++;
            // gain_list[tmp_left[0]]++;
        }
        if(tmp_right.size()==1){
            cell_list[tmp_right[0]]->gain++;
            // gain_list[tmp_right[0]]++;
        }
    }
    best_gain = gain;
   
}

bool check_area_valid(int front, int  tale, Cell* move_cell){
    if(abs(front-tale-2*move_cell->size)<area_constraint){
        return true;
    }
    else{
        return false;
    }
}

Cell* find_move_cell(bool group){
    int p = p_max;
    map<int,Node*> tmp_bucket_list = group ? bucket_list_right : bucket_list_left;
    while(tmp_bucket_list[p]->next == nullptr && p>-p_max ){
        p--;
    }

    Cell* now_cell = cell_list[tmp_bucket_list[p]->next->id];


    return now_cell;

}

void remove_cell(Cell *rm_cell){
    Node *rm_node = rm_cell->point;
    rm_node->prev->next = rm_node->next;
    if(rm_node->next!=nullptr){
        rm_node->next->prev = rm_node->prev;
    }
}

void update_gain(Cell* curr_cell){
    remove_cell(curr_cell);
    int i = curr_cell->point->id;
    int gain = cell_list[i]->gain;
    Node* curr_node = curr_cell->point;
    if(cell_list[i]->group == false){
        curr_node->prev = bucket_list_left[gain];
        curr_node->next = bucket_list_left[gain]->next;
        bucket_list_left[gain]->next = curr_node;
        if(curr_node->next!=nullptr){
            curr_node->next->prev = curr_node;
        }
    }
    else{
        curr_node->prev = bucket_list_right[gain];
        curr_node->next = bucket_list_right[gain]->next;
        bucket_list_right[gain]->next = curr_node;
        if(curr_node->next!=nullptr){
            curr_node->next->prev = curr_node;
        }
    }
    
}


void best_backup(){
    left_cell_count = best_left_count;
    right_cell_count = best_right_count;
    left_area = best_left_area;
    right_area = best_right_area;
    for(int i=move_history.size()-1;i>best_tt;i--){
        cell_list[move_history[i]]->group =1-cell_list[move_history[i]]->group;
    }

}

// move the cell and update gain
void FM_partition(){
    compute_init_gain();
    build_bucket_list();
    int t = 0;
    best_tt=0;
    move_history.clear();
    cout<<left_cell_count<<" "<<right_cell_count<<endl;
    while(t<cell_count){
        Cell *move_cell;
        move_cell = find_move_cell(leftright);
        move_cell->group = 1-move_cell->group;
        
        int F_area = leftright ? right_area : left_area;
        int T_area = leftright ? left_area : right_area;
        if(!check_area_valid(F_area,T_area,move_cell)){
            leftright = 1-leftright;
            move_cell = find_move_cell(leftright);
            F_area = leftright ? right_area : left_area;
            T_area = leftright ? left_area : right_area;
            if(!check_area_valid(F_area,T_area,move_cell)){
                cout<<"no cell can be move"<<endl;
                return;
            }
        }
        
        move_cell->lock = 1;
        if(leftright){
            right_area -= move_cell->size;
            left_area += move_cell->size;
            tmp_right_cell_count--;
            left_cell_count++;
            right_cell_count--;
        }
        else{
            right_area += move_cell->size;
            left_area -= move_cell->size;
            tmp_left_cell_count--;
            left_cell_count--;
            right_cell_count++;
        }
        gain += move_cell->gain;
        if(gain>best_gain){
            best_gain = gain;
            best_left_count = left_cell_count;
            best_right_count = right_cell_count;
            best_left_area = left_area;
            best_right_area = right_area;
            best_tt = t;
        }
        move_history.push_back(move_cell->point->id);
        remove_cell(move_cell);
        // cout<<"now cell "<<move_cell->point->id<<endl;
        int now_cell_id = move_cell->point->id;
        for(int t=0;t<move_cell->cellToNet.size();t++){
            int i=move_cell->cellToNet[t];
            // cout<<i<<endl;
            vector<int>::iterator it = find(net_list[i].begin(),net_list[i].end(),now_cell_id);
            if(it==net_list[i].end()){
                continue;
            }
            vector<int> tmp_front;
            vector<int> tmp_tale;
            // cout<<"check";
            for(int j=0;j<net_list[i].size();j++){
                if(j==distance(net_list[i].begin(),it)){
                    continue;
                }
                if(cell_list[net_list[i][j]]->group==0 && leftright==0){
                    tmp_front.push_back(net_list[i][j]);
                }
                if(cell_list[net_list[i][j]]->group==1 && leftright==0){
                    tmp_tale.push_back(net_list[i][j]);
                }
                if(cell_list[net_list[i][j]]->group==0 && leftright==1){
                    tmp_tale.push_back(net_list[i][j]);
                }
                if(cell_list[net_list[i][j]]->group==1 && leftright==1){
                    tmp_front.push_back(net_list[i][j]);
                }
            }
            // cout<<"1";
            if(tmp_tale.size()==0){
                for(int j=0;j<net_list[i].size();j++){
                    if(net_list[i][j]==now_cell_id){
                        continue;
                    }
                    if(!cell_list[net_list[i][j]]->lock){
                        cell_list[net_list[i][j]]->gain++;
                        update_gain(cell_list[net_list[i][j]]);
                    }
                }
            }
            // cout<<"2";
            if(tmp_tale.size()==1){
                if(!cell_list[tmp_tale[0]]->lock){
                    cell_list[tmp_tale[0]]->gain--;
                    update_gain(cell_list[tmp_tale[0]]);
                }
            }
            // cout<<"3";
            if(tmp_front.size()==0){
                for(int j=0;j<net_list[i].size();j++){
                    if(net_list[i][j]==now_cell_id){
                        continue;
                    }
                    if(!cell_list[net_list[i][j]]->lock){
                        cell_list[net_list[i][j]]->gain--;
                        update_gain(cell_list[net_list[i][j]]);
                    }
                }
            }
            // cout<<"4";
            if(tmp_front.size()==1){
                if(!cell_list[tmp_front[0]]->lock){
                    cell_list[tmp_front[0]]->gain++;
                    update_gain(cell_list[tmp_front[0]]);
                }
            }
        }
        // cout<<"5"<<endl;
        if(tmp_left_cell_count==0 && tmp_right_cell_count!=0){
            leftright=1;
        }
        else if(tmp_right_cell_count==0 && tmp_left_cell_count!=0){
            leftright=0;
        }
        else{
            leftright = 1-leftright;
        }
        t++;
    }
    t_end = clock();
    cpu_time_used = ((double) (t_end - t_start)) / CLOCKS_PER_SEC;
    if(cpu_time_used>270){
        cout<<"time is out"<<endl;
        return;
    }
    else if(best_gain>0){
        cout<<best_gain<<endl;
        best_backup();
        FM_partition();
    }
    else{
        cout<<"finish partitioning"<<endl;
        return;
    }
    
   
    

}

// void output_file(ofstream myfile){
    

// }

int main(int  argc,  char*  argv[]){
    
    t_start = clock();
    string var,var2;

    // read cell
    ifstream inFile(argv[2]);
    int size;
    string cell;
    while(inFile>>cell>>size){
        cellMap[cell] = cell_count;
        // cout<<size<<" "<<left_area<<" "<<right_area<<endl;
        if(left_area<right_area){
            Cell* tmp_cell = new Cell(cell,size,false,cell_count);
            cell_list.push_back(tmp_cell);
            left_area = left_area + size;
        }
        else{
            Cell* tmp_cell = new Cell(cell,size,true,cell_count);
            cell_list.push_back(tmp_cell);
            right_area = right_area + size;
        }
        cell_count ++;
    }
    total_area = left_area + right_area;
    area_constraint = total_area/10;


    // read net
    vector<int> tmp_list;
    ifstream inFile2(argv[1]);
    while(inFile2>>var){
        if(var=="{"){
            // vector <int> tmp_netcell;
            while(inFile2>>var2){
                if(var2=="}"){
                    break;
                }
                cell_list[cellMap[var2]]->pins++;
                cell_list[cellMap[var2]]->cellToNet.push_back(net_count);
                tmp_list.push_back(cellMap[var2]);
            }
            net_list.push_back(tmp_list);
            tmp_list.clear();
            net_count++;
        }
    }

    left_cell_count = 0;
    right_cell_count = 0;
    for(int i=0;i<cell_count;i++){
        cell_list[i]->gain = 0;
        cell_list[i]->lock = 0;
        if(cell_list[i]->group==0){
            left_cell_count++;
        }
        else if(cell_list[i]->group==1){
            right_cell_count++;
        }
        else{
            cout<<"There is an error"<<endl;
        }
    }
    compute_pmax();
    
    cout<<p_max<<endl;
    

 
    FM_partition();

    best_cut = compute_cut();
    cout<<best_gain<<endl;
    
    ofstream myfile;
    myfile.open(argv[3]);
    myfile<<"cut_size "<<best_cut<<"\n";
    int A_count = 0;
    int B_count = 0;
    for(int i=0;i<cell_list.size();i++){
        if(cell_list[i]->group == 0){
            A_count++;
        }
        else{
            B_count++;
        }
    }
    myfile<<"A "<<A_count<<"\n";
    for(int i=0;i<cell_list.size();i++){
        if(cell_list[i]->group==0){
            myfile<<cell_list[i]->name<<"\n";
        }
    }
    myfile<<"B "<<B_count<<"\n";
    for(int i=0;i<cell_list.size();i++){
        if(cell_list[i]->group==1){
            myfile<<cell_list[i]->name<<"\n";
        }
    }

    t_end = clock();
    cpu_time_used = ((double) (t_end - t_start)) / CLOCKS_PER_SEC;
    cout<<"TIME: "<<cpu_time_used<<endl;


    return 0;
}