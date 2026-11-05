#include <bits/stdc++.h>
using namespace std;
using ll=long long;using i128=__int128_t;
struct Operation{int type,l,r;ll v=0;};
struct Tree{
    struct Node{int x=0,y=0,xy=0,lx=0,ly=0;};
    int n;ll k;vector<Node> t;
    Tree(int n,ll k):n(n),k(k),t(4*n+4){}
    void pull(int p){
        t[p].x=(t[p*2].x+(ll)t[p*2+1].x)%k;
        t[p].y=(t[p*2].y+(ll)t[p*2+1].y)%k;
        t[p].xy=(t[p*2].xy+(ll)t[p*2+1].xy)%k;
    }
    void build(int p,int l,int r,const vector<ll>&a,ll power){
        if(l==r){
            ll x=a[l]%k,y=a[l]/power%k;
            t[p].x=x;t[p].y=y;t[p].xy=x*y%k;return;
        }
        int mid=(l+r)/2;build(p*2,l,mid,a,power);build(p*2+1,mid+1,r,a,power);pull(p);
    }
    void apply(int p,int len,ll dx,ll dy){
        auto &s=t[p];ll oldX=s.x,oldY=s.y;
        s.xy=(s.xy+dx*oldY+dy*oldX+(len*dx%k)*dy)%k;
        s.x=(oldX+len*dx)%k;s.y=(oldY+len*dy)%k;
        s.lx=(s.lx+dx)%k;s.ly=(s.ly+dy)%k;
    }
    void push(int p,int l,int r){
        if(l==r||(!t[p].lx&&!t[p].ly))return;
        int mid=(l+r)/2;apply(p*2,mid-l+1,t[p].lx,t[p].ly);
        apply(p*2+1,r-mid,t[p].lx,t[p].ly);t[p].lx=t[p].ly=0;
    }
    void update(int p,int l,int r,int ql,int qr,ll dx,ll dy){
        if(ql<=l&&r<=qr){apply(p,r-l+1,dx,dy);return;}
        push(p,l,r);int mid=(l+r)/2;
        if(ql<=mid)update(p*2,l,mid,ql,qr,dx,dy);
        if(qr>mid)update(p*2+1,mid+1,r,ql,qr,dx,dy);pull(p);
    }
    pair<ll,ll> query(int p,int l,int r,int ql,int qr){
        if(ql<=l&&r<=qr)return {t[p].xy,t[p].y};
        push(p,l,r);int mid=(l+r)/2;pair<ll,ll> ans{0,0};
        auto add=[&](pair<ll,ll> z){ans.first=(ans.first+z.first)%k;ans.second=(ans.second+z.second)%k;};
        if(ql<=mid)add(query(p*2,l,mid,ql,qr));
        if(qr>mid)add(query(p*2+1,mid+1,r,ql,qr));return ans;
    }
};
void print(i128 x){
    if(!x){cout<<0<<'\n';return;}string s;
    while(x){s+=char('0'+x%10);x/=10;}reverse(s.begin(),s.end());cout<<s<<'\n';
}
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n,m;ll k,maxValue=0;cin>>n>>m>>k;vector<ll>a(n+1);
    for(int i=1;i<=n;i++){cin>>a[i];maxValue=max(maxValue,a[i]);}
    vector<Operation> op(m);for(auto &o:op){
        cin>>o.type>>o.l>>o.r;if(o.type==1){cin>>o.v;maxValue=max(maxValue,o.v);}
    }
    vector<i128> answer(m);ll power=1;
    // 各数位独立重放操作，复用同一棵树的内存。
    for(int digit=0;;digit++){
        Tree tree(n,k);tree.build(1,1,n,a,power);
        for(int i=0;i<m;i++){
            auto o=op[i];
            if(o.type==1)tree.update(1,1,n,o.l,o.r,o.v%k,o.v/power%k);
            else{
                auto [xy,y]=tree.query(1,1,n,o.l,o.r);
                ll v=(xy+y)%k;
                if(!digit)v=v*((k+1)/2)%k;
                answer[i]+=(i128)v*power;
            }
        }
        if(power>maxValue/k)break;power*=k;
    }
    for(int i=0;i<m;i++)if(op[i].type==2)print(answer[i]);
}
