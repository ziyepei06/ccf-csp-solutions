#include <bits/stdc++.h>
using namespace std;
struct Node{int left=-1,right=-1;char c=0;};
vector<Node>tree;string encoded;int pos;
int build(){int u=tree.size();tree.push_back({});char t=encoded[pos++];if(t=='1')tree[u].c=encoded[pos++];else{int l=build(),r=build();tree[u].left=l;tree[u].right=r;}return u;}
string decode(string s){
    if(s[0]!='H')return s;if(s.size()>1&&s[1]=='H')return s.substr(1);
    vector<int>b;for(size_t i=1;i<s.size();i+=2)b.push_back(stoi(s.substr(i,2),nullptr,16));
    int count=(b.size()-1)*8-b.back(),u=0;string ans;
    for(int i=0;i<count;++i){int bit=(b[i/8]>>(7-i%8))&1;u=bit?tree[u].right:tree[u].left;if(tree[u].c){ans+=tree[u].c;u=0;}}
    return ans;
}
int main(){
    int S,D;cin >> S >> D;vector<pair<string,string>>fixed(S);deque<pair<string,string>>dynamic;
    for(auto &[k,v]:fixed)cin >> k >> v;cin >> encoded;pos=0;build();int N;cin >> N;
    auto entry=[&](int i){return i<=S?fixed[i-1]:dynamic[i-S-1];};
    while(N--){
        int op,i;cin >> op >> i;pair<string,string>e;
        if(op==1)e=entry(i);
        else{string k,v;if(i)e.first=entry(i).first;else{cin >> k;e.first=decode(k);}cin >> v;e.second=decode(v);}
        cout << e.first << ": " << e.second << '\n';
        if(op==2){dynamic.push_front(e);if((int)dynamic.size()>D)dynamic.pop_back();}
    }
}
