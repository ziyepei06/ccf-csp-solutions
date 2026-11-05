#include <bits/stdc++.h>
using namespace std;
int main(){int n;cin >> n;unordered_map<string,int>count;while(n--){string board,row;for(int i=0;i<8;++i){cin >> row;board+=row;}cout << ++count[board] << '\n';}}
