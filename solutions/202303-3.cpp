#include <bits/stdc++.h>
using namespace std;
const int N=2500;using Bits=bitset<N>;
unordered_map<int,vector<pair<int,int>>> idx;
string expr;int pos;
int number() {int x=0;while(pos<(int)expr.size()&&isdigit(expr[pos]))x=x*10+expr[pos++]-'0';return x;}
Bits parse() {
    if(expr[pos]=='&'||expr[pos]=='|') {
        char op=expr[pos++];++pos;Bits a=parse();++pos;++pos;Bits b=parse();++pos;
        return op=='&'?(a&b):(a|b);
    }
    int attr=number();char op=expr[pos++];int value=number();Bits a;
    auto it=idx.find(attr);if(it==idx.end())return a;
    auto &v=it->second;
    if(op==':') {
        auto l=lower_bound(v.begin(),v.end(),make_pair(value,-1));
        for(;l!=v.end()&&l->first==value;++l)a.set(l->second);
    } else for(auto [x,u]:v)if(x!=value)a.set(u);
    return a;
}
int main() {
    int n;cin >> n;vector<int>dn(n);
    for(int i=0;i<n;++i) {
        int k;cin >> dn[i] >> k;
        while(k--){int a,v;cin >> a >> v;idx[a].push_back({v,i});}
    }
    for(auto &[a,v]:idx)sort(v.begin(),v.end());
    int m;cin >> m;
    while(m--) {
        cin >> expr;pos=0;Bits ans=parse();vector<int>users;
        for(int i=0;i<n;++i)if(ans[i])users.push_back(dn[i]);sort(users.begin(),users.end());
        for(size_t i=0;i<users.size();++i)cout << (i?" ":"") << users[i];cout << '\n';
    }
}
