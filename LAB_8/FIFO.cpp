#include<iostream>
#include<queue>
#include<vector>
#include <unordered_map>
#include <fstream>
using namespace std;

int main(int argc, char* argv[]){
    int page_count = atoi(argv[1]);
    int frame_count = atoi(argv[2]);
    int block_count = atoi(argv[3]);
    if(page_count > frame_count + block_count){
        cout << "Error: Memory overflow or allocation errors.";
        exit(0);
    }
    
    string filename = argv[4];

    unordered_map<int, int> pageTable;
    queue<int> q;
    unordered_map<int, int> swapSet;

    ifstream file(filename);
    int page;
    int pageFault =0;
    int pageHit = 0;
    while (file >> page) {
        //Handle page faults
        if(pageTable.find(page) == pageTable.end()) {
            pageFault++;
            if(pageTable.size() >= frame_count){
                int victimPage = q.front();
                q.pop();
                pageTable.erase(victimPage);
                if(swapSet.size() >= block_count){
                    cout << "Error: Insufficient size of block swap space\n";
                    exit(0);
                }
                else swapSet[victimPage] = 1;
            }
            pageTable[page] = 1;
            q.push(page);
        }
        else pageHit++;
    }    
    cout << "\nPage Faults " << pageFault << "\nPage Hit " << pageHit << endl;
    // cout << pageFault << endl;
    return 0;
}