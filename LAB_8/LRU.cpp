#include<iostream>
#include<queue>
#include<vector>
#include <unordered_map>
#include <fstream>
#include<list>
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

    ifstream file(filename);
    int page;
    int pageFault =0;
    int pageHit = 0;

    list<int> cache;
    unordered_map<int, list<int> :: iterator > m;
    unordered_map<int, int> swapSet;
    while (file >> page) {
        //Handle page faults
        if(m.find(page) == m.end()){  //not found in cache
            pageFault++;
            //cache size full
            if(cache.size() == frame_count){
                int victimPage = cache.back();
                cache.pop_back();
                m.erase(victimPage);
                if(swapSet.size() == block_count){
                    cout << "Error: Insufficient size of block swap space\n";
                    exit(0);
                }
                else swapSet[victimPage] = 1;
            }
        }
        else { //found in cache
            pageHit++;
            cache.erase(m[page]);
        }
        cache.push_front(page);
        m[page] = cache.begin();
    }
    cout << "\nPage Faults " << pageFault << "\nPage Hit " << pageHit << endl;
    // cout << pageFault << endl;
    return 0;

}