#include <bits/stdc++.h>
using namespace std;
const int NONE=INT_MIN,INF=INT_MAX;
struct Info{int mn=0,mx=0,pm=INF,px=0;};
Info combine(Info a,Info b){return {min(a.mn,b.mn),max(a.mx,b.mx),min(a.pm,b.pm),max(a.px,b.px)};}
struct Seg{
    vector<Info>t;vector<int>owner,value;int n;
    Seg(int n):t(n*4),owner(n*4,0),value(n*4,0),n(n){}
    void setOwner(int u,int v){t[u]={v,v,v>0?v:INF,max(v,0)};owner[u]=v;}
    void down(int u){if(owner[u]!=NONE){setOwner(u*2,owner[u]);setOwner(u*2+1,owner[u]);owner[u]=NONE;}if(value[u]!=NONE){value[u*2]=value[u*2+1]=value[u];value[u]=NONE;}}
    void put(int u,int l,int r,int L,int R,int who,int val=NONE){
        if(L<=l&&r<=R){setOwner(u,who);if(val!=NONE)value[u]=val;return;}down(u);int m=(l+r)/2;
        if(L<=m)put(u*2,l,m,L,R,who,val);if(R>m)put(u*2+1,m+1,r,L,R,who,val);t[u]=combine(t[u*2],t[u*2+1]);
    }
    Info get(int u,int l,int r,int L,int R){
        if(L<=l&&r<=R)return t[u];down(u);int m=(l+r)/2;
        if(R<=m)return get(u*2,l,m,L,R);if(L>m)return get(u*2+1,m+1,r,L,R);return combine(get(u*2,l,m,L,R),get(u*2+1,m+1,r,L,R));
    }
    int bad(int u,int l,int r,int L,int R,int id){
        if(r<L||l>R||t[u].px==0||(t[u].pm==id&&t[u].px==id))return n;
        if(l==r)return l;down(u);int m=(l+r)/2;int x=bad(u*2,l,m,L,R,id);return x<n?x:bad(u*2+1,m+1,r,L,R,id);
    }
    pair<int,int>point(int u,int l,int r,int p){
        if(l==r)return {max(0,t[u].mn),t[u].mn>0?value[u]:0};down(u);int m=(l+r)/2;return p<=m?point(u*2,l,m,p):point(u*2+1,m+1,r,p);
    }
};
struct Op{int type,id=0,l=0,r=0,x=0;};
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int programs,disk,k;cin >> programs >> disk >> k;vector<Op>ops(k);vector<int>xs{1,disk+1};
    for(auto &o:ops){cin >> o.type;if(o.type==3){cin >> o.l;o.r=o.l;}else{cin >> o.id >> o.l >> o.r;if(o.type==0)cin >> o.x;}xs.push_back(o.l);xs.push_back(o.r+1);}
    sort(xs.begin(),xs.end());xs.erase(unique(xs.begin(),xs.end()),xs.end());int n=xs.size()-1;Seg s(n);
    for(auto o:ops){int l=lower_bound(xs.begin(),xs.end(),o.l)-xs.begin(),r=lower_bound(xs.begin(),xs.end(),o.r+1)-xs.begin()-1;
        if(o.type==0){int b=s.bad(1,0,n-1,l,r,o.id);r=min(r,b-1);if(r<l)cout << -1 << '\n';else{s.put(1,0,n-1,l,r,o.id,o.x);cout << xs[r+1]-1 << '\n';}}
        else if(o.type==1||o.type==2){auto info=s.get(1,0,n-1,l,r);int want=o.type==1?o.id:-o.id;bool ok=info.mn==want&&info.mx==want;cout << (ok?"OK":"FAIL") << '\n';if(ok)s.put(1,0,n-1,l,r,-want);}
        else{auto [id,val]=s.point(1,0,n-1,l);cout << id << ' ' << val << '\n';}
    }
}
