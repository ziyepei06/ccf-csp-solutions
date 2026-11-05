#include <bits/stdc++.h>
using namespace std;
using ll=long long;
struct Buffer{ll begin,length,offset;};
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n,q;cin>>n>>q;
    vector<vector<Buffer>> buffers(n+1);
    // 只保存有限空洞；tail 之后始终空闲，等价于无限尾区间。
    map<ll,ll> holes; // 起点 -> 长度
    set<pair<ll,ll>> bySize;
    ll tail=0;
    auto remove=[&](ll x){bySize.erase({holes.at(x),x});holes.erase(x);};
    auto insert=[&](ll x,ll len){if(len){holes[x]=len;bySize.insert({len,x});}};
    while(q--){
        string op;int p;cin>>op>>p;
        if(op=="new"){
            ll len,start;cin>>len;
            auto it=bySize.lower_bound({len,-1});
            if(it==bySize.end()){start=tail;tail+=len;}
            else{
                auto [size,pos]=*it;start=pos;remove(pos);
                insert(pos+len,size-len);
            }
            buffers[p].push_back({start,len,0});cout<<start<<'\n';
        }else if(op=="send"){
            ll sum=0;
            for(auto &b:buffers[p]){
                sum+=b.begin+b.offset;b.offset=(b.offset+1)%b.length;
            }
            cout<<sum<<'\n';
        }else{
            int i;cin>>i;--i;auto b=buffers[p][i];
            buffers[p].erase(buffers[p].begin()+i);
            ll l=b.begin,r=l+b.length;
            auto it=holes.lower_bound(l);
            if(it!=holes.begin()){
                auto prevIt=prev(it);
                if(prevIt->first+prevIt->second==l){l=prevIt->first;remove(l);}
            }
            it=holes.lower_bound(l);
            if(it!=holes.end()&&it->first==r){
                ll pos=it->first;r+=it->second;remove(pos);
            }
            if(r==tail)tail=l;else insert(l,r-l);
        }
    }
}
