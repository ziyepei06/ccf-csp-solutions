#include <bits/stdc++.h>
using namespace std;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int n,m;cin >> n >> m;
    vector<int>a(n+1),p(n+1),dep(n+1),sz(n+1,1),heavy(n+1),top(n+1),in(n+1);vector<vector<int>>g(n+1);
    for(int i=1;i<=n;++i)cin >> a[i];for(int i=1,u,v;i<n;++i){cin >> u >> v;g[u].push_back(v);g[v].push_back(u);}
    vector<int>order{1};for(size_t i=0;i<order.size();++i){int u=order[i];for(int v:g[u])if(v!=p[u]){p[v]=u;dep[v]=dep[u]+1;order.push_back(v);}}
    for(int i=n-1;i>0;--i){int u=order[i];sz[p[u]]+=sz[u];if(sz[u]>sz[heavy[p[u]]])heavy[p[u]]=u;}
    int timer=0;vector<pair<int,int>>st{{1,1}};
    while(!st.empty()){auto [u,h]=st.back();st.pop_back();for(;u;u=heavy[u]){top[u]=h;in[u]=timer++;for(int v:g[u])if(v!=p[u]&&v!=heavy[u])st.push_back({v,v});}}
    int base=1;while(base<n)base*=2;vector<int>tr(base*2,n);
    for(int u=1;u<=n;++u)tr[base+in[u]]=a[u];for(int u=base-1;u;--u)tr[u]=min(tr[u*2],tr[u*2+1]);
    auto query=[&](int l,int r){int ans=n;for(l+=base,r+=base+1;l<r;l/=2,r/=2){if(l&1)ans=min(ans,tr[l++]);if(r&1)ans=min(ans,tr[--r]);}return ans;};
    while(m--){
        int u,v;cin >> u >> v;vector<pair<int,int>>segs;
        while(top[u]!=top[v]){if(dep[top[u]]<dep[top[v]])swap(u,v);segs.push_back({in[top[u]],in[u]});u=p[top[u]];}
        if(in[u]>in[v])swap(u,v);segs.push_back({in[u],in[v]});sort(segs.begin(),segs.end());
        int last=0,ans=n;for(auto [l,r]:segs){if(last<l)ans=min(ans,query(last,l-1));last=r+1;}
        if(last<n)ans=min(ans,query(last,n-1));cout << ans << '\n';
    }
}
