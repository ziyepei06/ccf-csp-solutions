#include <bits/stdc++.h>
using namespace std;
using ll=long long;
struct Range{
    int base,neutral;bool isMin;vector<int>t;
    Range(int n,int inf,bool mini):base(1),neutral(inf),isMin(mini){while(base<n)base*=2;t.assign(base*2,neutral);}
    int merge(int a,int b){return isMin?min(a,b):max(a,b);}
    void put(int p,int v){p+=base;t[p]=v;while(p/=2)t[p]=merge(t[p*2],t[p*2+1]);}
    int get(int l,int r){int ans=neutral;if(l>r)return ans;for(l+=base,r+=base+1;l<r;l/=2,r/=2){if(l&1)ans=merge(ans,t[l++]);if(r&1)ans=merge(ans,t[--r]);}return ans;}
};
ll solve(int lo,int hi,vector<pair<int,int>>edges){
    if(lo>=hi||edges.empty())return 0;int mid=(lo+hi)/2,len=hi-lo+1,INF=hi+1;
    vector<int>f(mid-lo+1,INF),g(hi-mid,lo-1),far(len,-1),near(len,INF);
    vector<pair<int,int>>left,right;
    for(auto [l,r]:edges){
        if(r<=mid)left.push_back({l,r});else if(l>mid)right.push_back({l,r});
        if(l<=mid){if(r>=mid)f[l-lo]=min(f[l-lo],r);else far[l-lo]=max(far[l-lo],r);}
        if(r>=mid+1){if(l<=mid+1)g[r-mid-1]=max(g[r-mid-1],l);else near[r-lo]=min(near[r-lo],l);}
    }
    Range low(mid-lo+2,INF,true);low.put(mid+1-lo,mid);
    vector<pair<int,int>>events;
    for(int x=mid;x>=lo;--x){
        if(far[x-lo]>=x+1)f[x-lo]=min(f[x-lo],low.get(x+1-lo,far[x-lo]+1-lo));
        low.put(x-lo,f[x-lo]);if(f[x-lo]<=hi)events.push_back({f[x-lo],x});
    }
    Range high(hi-mid+1,lo-1,false);high.put(0,mid+1);
    for(int x=mid+1;x<=hi;++x){
        if(near[x-lo]<=x-1)g[x-mid-1]=max(g[x-mid-1],high.get(near[x-lo]-1-mid,x-1-mid));
        high.put(x-mid,g[x-mid-1]);
    }
    sort(events.begin(),events.end());vector<int>bit(mid-lo+2);size_t ptr=0;ll ans=0;
    for(int R=mid+1;R<=hi;++R){
        while(ptr<events.size()&&events[ptr].first<=R){int x=events[ptr++].second-lo+1;for(;x<(int)bit.size();x+=x&-x)++bit[x];}
        int bound=min(mid,g[R-mid-1]);if(bound>=lo)for(int x=bound-lo+1;x;x-=x&-x)ans+=bit[x];
    }
    // Release level arrays before recursion.
    edges.clear();edges.shrink_to_fit();
    return ans+solve(lo,mid,move(left))+solve(mid+1,hi,move(right));
}
int main(){int n,m;cin >> n >> m;vector<pair<int,int>>e(m);for(auto &[l,r]:e)cin >> l >> r;cout << solve(1,n,move(e)) << '\n';}
