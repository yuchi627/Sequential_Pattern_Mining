#include<iostream>
#include<map>
#include<set>
#include<sstream> 
#include<fstream>
#include<bits/stdc++.h>
#include <ctime>
#include <string.h>

using namespace std;


template<typename T>
void pretty_print(const T& input_seq, int combo, set<set<string>> &permutation_set)
{
    set<string> tmp1;
    set<string> tmp2;
    int n = input_seq.size();
    bool flag = true;
    for (int i = 0; i < n; ++i) {
        if ((combo >> i) & 1){
            tmp1.insert(input_seq[i]);
        }
    }
    permutation_set.insert(tmp1);
}

template<typename T>
void combo(const T& input_seq, set<set<string>> &permutation_set)
{
    // ------- combination and permutation ---------
    permutation_set.clear();
    int n = input_seq.size();
    for(int k=1; k<=n; k++){
        int combo = (1 << k) - 1;       // k bit sets
        while (combo < 1<<n) {
            pretty_print(input_seq, combo, permutation_set);
            int x = combo & -combo;
            int y = combo + x;
            int z = (combo & ~y);
            combo = z / x;
            combo >>= 1;
            combo |= y;
        }
    }
}


float preprocess(map<vector<int>, int> &freq_seq, string filename, map<string, vector<set<int>>> &SID_sequence, map<set<string>, int> &itemset_newID, map<int, set<string>> &newID_itemset, float min_supp){
    
    // =========== Sort Phase ===========
    ifstream file(filename);
    string str;
    const char delim = ' ';
    float transaction_index = 0;
    map<string, vector<vector<string>>> tmp_SID_sequence;
    map<set<string>, int> tmp_itemset_num;
    string trash = " ";
    regex r("\r\n|\r|\n");
    // ---------------- split input data -----------------
    while (getline(file, str)) {
        
        stringstream ss(str); 
        string s, SID;
        string tmp_time; 
        bool SID_flag = true;
        bool time_flag = true;
        map<string, vector<string>> tmp_sequences;
        
        while (getline(ss, s, delim)) { 
            s = regex_replace(s, r, "");
            if(s.compare(trash) == 0 || s.length() ==0){
                continue;
            }
            if(SID_flag){ // ------- first element is sequence ID -------
                SID = s;
                SID_flag = false;
            }
            else if(time_flag){ // ------- The pair of number is the transaction time and the item ID -------
                tmp_time = s;
                time_flag = false;
            }
            else{ // ------- save item ID -------
                tmp_sequences[tmp_time].push_back(s);
                time_flag = true;
            }
        }
        for (auto it = tmp_sequences.begin(); it != tmp_sequences.end(); ++it){
            tmp_SID_sequence[SID].push_back(it->second);
            set<set<string>> permutation_set;
            combo(it->second, permutation_set);
            for (auto it_set = permutation_set.begin(); it_set != permutation_set.end(); ++it_set) {
                tmp_itemset_num[*it_set] ++;
            }
        }
        transaction_index++;
    
    }
    // =========== Litemset Phase ===========
    set<set<string>> tmp;
    // ------ remove itemset which number less than min_supp ------
    int newID_index = 1;
    map<set<string>, int> freq_seq_tmp;
    for (auto it = tmp_itemset_num.begin(); it != tmp_itemset_num.end(); ++it){
        if(it->second/transaction_index >= min_supp){
            tmp.insert(it->first);
            freq_seq_tmp[it->first] = it->second;
        }
    }
    for (auto it = tmp.begin(); it != tmp.end(); ++it){
        itemset_newID[*it] = newID_index;
        newID_itemset[newID_index] = *it;
        newID_index ++;
    }
    // =========== Transformation Phase ===========
    for (auto it = tmp_SID_sequence.begin(); it != tmp_SID_sequence.end(); ++it){
        vector<set<int>> tmp_setset;
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            set<set<string>> permutation_set;
            combo(*it2, permutation_set);
            set<int> tmp_set;
            for (auto it_set = permutation_set.begin(); it_set != permutation_set.end(); ++it_set) {
                auto iter = itemset_newID.find(*it_set);
                if(iter != itemset_newID.end()){
                    tmp_set.insert(iter->second);
                }
            }
            if(tmp_set.size()!=0)
                tmp_setset.push_back(tmp_set);
        }
        if(tmp_setset.size()!=0)
            SID_sequence[it-> first] = tmp_setset;
    }
    return transaction_index;
}

bool find_candidate(vector<vector<int>> &freq_seq_candidate, vector<vector<int>> &itemset, int itemset_len){
    freq_seq_candidate.clear();
    bool find_flag = false;
    for (auto it = itemset.begin(); it != itemset.end(); ++it){
        for (auto it2 = it; it2 != itemset.end(); ++it2){
            vector<int> inter_vec;
            set_intersection((*it).begin(), (*it).end(), (*it2).begin(), (*it2).end(), back_inserter(inter_vec));
            if(inter_vec.size() >= itemset_len-1){
                vector<int> uni_vec;
                set_union((*it).begin(), (*it).end(), (*it2).begin(), (*it2).end(), back_inserter(uni_vec));
                if(uni_vec.size() == itemset_len+1){
                    if(equal(uni_vec.begin()+1, uni_vec.end()-1, inter_vec.begin())){
                        freq_seq_candidate.push_back(uni_vec);
                    }
                }
                else if(uni_vec.size() == itemset_len){
                    bool same_flag = true;
                    for (auto uni = uni_vec.begin(); uni != uni_vec.end(); ++uni){
                        if(*uni != *(uni_vec.begin())){
                            same_flag=false;
                            break;
                        }
                    }
                    if (same_flag){
                        uni_vec.push_back((*uni_vec.begin()));
                        freq_seq_candidate.push_back(uni_vec);
                    }
                }
            }
        }
    }
    if(freq_seq_candidate.size() > 0)
        find_flag = true;
    return find_flag;
}

void filter_itemset(map<vector<int>, int> &freq_seq, vector<vector<int>> &itemset, map<vector<int>, int> &itemset_num, float min_supp, float trans_amount){
    for (auto it = itemset_num.begin(); it != itemset_num.end(); ++it){
        if(it->second/trans_amount >= min_supp){
            itemset.push_back(it->first);
            freq_seq[it->first] = it->second;
        }
    }
}

int main(){
    ofstream output_file;
    output_file.open("out.txt");
    // ------- user input min_supp -------
    string filename = "seqdata.dat.txt";
    cout << "Enter min_supp(%): ";
    float min_supp;
    cin >> min_supp;
    cout << "Read " << filename << endl;
    cout << "min_supp: " << min_supp << "%" << endl;
    output_file << "min_supp: " << min_supp << "%" << endl;
    min_supp /= 100;
    
    map<vector<int>, int> itemset_num;
    map<string, vector<set<int>>> SID_sequence;
    map<set<string>, set<set<string>>> permutation_set;
    map<set<string>, int> itemset_newID;
    map<int, set<string>> newID_itemset;
    map<vector<int>, int> freq_seq;
    
    float trans_amount = preprocess(freq_seq, filename, SID_sequence, itemset_newID, newID_itemset, min_supp);

    vector<vector<int>> freq_seq_candidate;
    int itemset_len = 1;
    for (auto it = newID_itemset.begin(); it != newID_itemset.end(); ++it){
        for (auto it2 = newID_itemset.begin(); it2 != newID_itemset.end(); ++it2){
            vector<int> tmp;
            tmp.push_back(it->first);
            tmp.push_back(it2->first);
            freq_seq_candidate.push_back(tmp);
        }
    }

    bool continue_flag = true;
    // =========== Mining Phase ===========
    while(continue_flag){
        vector<vector<int>> itemset;
        itemset_num.clear();
        itemset_len ++;
        for (auto cand = freq_seq_candidate.begin(); cand != freq_seq_candidate.end(); ++cand){
            // ------- scan SID_sequence --------
            for (auto it = SID_sequence.begin(); it != SID_sequence.end(); ++it){
                bool find_cand_flag = false;
                auto iter = (*cand).begin();
                for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2){
                    for (const auto &s : *it2) {
                        if(*iter == s){
                            iter++;
                            if(iter == (*cand).end()){
                                // ----- candidate match -----
                                itemset_num[(*cand)] ++;
                                find_cand_flag = true;
                            }
                            break;
                        }
                    }
                    if(find_cand_flag)
                        break;
                }
            }
        }
        filter_itemset(freq_seq, itemset, itemset_num, min_supp, trans_amount);
        continue_flag= find_candidate(freq_seq_candidate, itemset, itemset_len);
    }

    // ============ backprojection ============
    // ----- print freq_seq -------
    for (auto its = freq_seq.begin(); its != freq_seq.end(); ++its){
        for (auto its2 = (its->first).begin(); its2 != (its->first).end(); ++its2){
            if(newID_itemset[*its2].size() == 1){
                for (const auto &s : newID_itemset[*its2]) {
                    output_file << s << " ";
                }
            }
            else{
                output_file << "(";
                for (const auto &s : newID_itemset[*its2]) {
                    output_file << s << " ";
                }
                output_file << ") ";
            }
        }
        output_file << " SUP: " << its->second << endl;
    }
    return 0;
}
