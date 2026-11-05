#include <bits/stdc++.h>
using namespace std;
struct Info {int mn=0,mx=0,nz=1000000000;};
Info mergeInfo(Info a,Info b) {return {min(a.mn,b.mn),max(a.mx,b.mx),min(a.nz,b.nz)};}
struct Seg {
    vector<Info>t;vector<int>lazy;int n;
    Seg(int n):t(4*n),lazy(4*n,-1),n(n){}
    void apply(int u,int v){t[u]={v,v,v?v:1000000000};lazy[u]=v;}
    void down(int u){if(lazy[u]!=-1){apply(u*2,lazy[u]);apply(u*2+1,lazy[u]);lazy[u]=-1;}}
    void put(int u,int l,int r,int L,int R,int v){
        if(L<=l&&r<=R){apply(u,v);return;}down(u);int m=(l+r)/2;
        if(L<=m)put(u*2,l,m,L,R,v);if(R>m)put(u*2+1,m+1,r,L,R,v);
        t[u]=mergeInfo(t[u*2],t[u*2+1]);
    }
    Info get(int u,int l,int r,int L,int R){
        if(L<=l&&r<=R)return t[u];down(u);int m=(l+r)/2;
        if(R<=m)return get(u*2,l,m,L,R);if(L>m)return get(u*2+1,m+1,r,L,R);
        return mergeInfo(get(u*2,l,m,L,R),get(u*2+1,m+1,r,L,R));
    }
};
string readAddr(){string s,r="0";cin >> s;for(char c:s)if(c!=':')r+=c;return r;}
string inc(string s){string hex="0123456789abcdef";for(int i=s.size()-1;i>=0;--i){int x=hex.find(s[i]);s[i]=hex[(x+1)%16];if(x<15)break;}return s;}
struct Op{int type,id=0;string l,r;};
int main(){
    int B,q;cin >> B >> q;vector<Op>ops(q);vector<string>xs;
    for(auto &o:ops){
        cin >> o.type;if(o.type==1)cin >> o.id;o.l=readAddr();o.r=o.type==2?o.l:readAddr();o.r=inc(o.r);
        xs.push_back(o.l);xs.push_back(o.r);
    }
    sort(xs.begin(),xs.end());xs.erase(unique(xs.begin(),xs.end()),xs.end());int n=xs.size()-1;Seg seg(n);
    for(auto &o:ops){
        int l=lower_bound(xs.begin(),xs.end(),o.l)-xs.begin(),r=lower_bound(xs.begin(),xs.end(),o.r)-xs.begin()-1;
        Info v=seg.get(1,0,n-1,l,r);
        if(o.type==1){
            bool ok=v.mn==0&&(v.mx==0||(v.nz==o.id&&v.mx==o.id));cout << (ok?"YES":"NO") << '\n';
            if(ok)seg.put(1,0,n-1,l,r,o.id);
        }else cout << (v.mn==v.mx?v.mn:0) << '\n';
    }
}
