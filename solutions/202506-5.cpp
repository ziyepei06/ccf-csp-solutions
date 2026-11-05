#include <bits/stdc++.h>
using namespace std;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int n,m,q;cin >> n >> m >> q;vector<vector<int>>g(n+1);
    for(int i=0,u,v;i<m;++i){cin >> u >> v;g[u].push_back(v);g[v].push_back(u);}
    vector<vector<int>>T(2*n+1);vector<int>dfn(n+1),low(n+1),parent(n+1),ptr(n+1),vs,stack{1};int timer=0,N=n;
    dfn[1]=low[1]=++timer;vs.push_back(1);
    while(!stack.empty()){
        int u=stack.back();if(ptr[u]<(int)g[u].size()){
            int v=g[u][ptr[u]++];if(v==parent[u])continue;
            if(!dfn[v]){parent[v]=u;dfn[v]=low[v]=++timer;vs.push_back(v);stack.push_back(v);}else low[u]=min(low[u],dfn[v]);
        }else{
            stack.pop_back();int p=parent[u];if(!p)continue;low[p]=min(low[p],low[u]);
            if(low[u]>=dfn[p]){int b=++N;T[b].push_back(p);T[p].push_back(b);while(true){int v=vs.back();vs.pop_back();T[b].push_back(v);T[v].push_back(b);if(v==u)break;}}
        }
    }
    int LOG=1;while((1<<LOG)<=N)++LOG;vector<vector<int>>up(LOG,vector<int>(N+1));vector<int>dep(N+1),in(N+1),out(N+1),pref(N+1);timer=0;
    vector<pair<int,bool>>st{{1,false}};
    while(!st.empty()){
        auto [u,exit]=st.back();st.pop_back();if(exit){out[u]=timer;continue;}in[u]=++timer;pref[u]=pref[up[0][u]]+(u<=n);st.push_back({u,true});
        for(int j=1;j<LOG;++j)up[j][u]=up[j-1][up[j-1][u]];
        for(int v:T[u])if(v!=up[0][u]){up[0][v]=u;dep[v]=dep[u]+1;st.push_back({v,false});}
    }
    auto anc=[&](int u,int v){return in[u]<=in[v]&&in[v]<=out[u];};
    auto lca=[&](int u,int v){if(anc(u,v))return u;if(anc(v,u))return v;for(int j=LOG-1;j>=0;--j)if(up[j][u]&&!anc(up[j][u],v))u=up[j][u];return up[0][u];};
    vector<int>mark(N+1);
    while(q--){
        int c;cin >> c;vector<int>v(c);for(int &u:v){cin >> u;mark[u]=1;}
        auto cmp=[&](int a,int b){return in[a]<in[b];};sort(v.begin(),v.end(),cmp);
        for(int i=1;i<c;++i)v.push_back(lca(v[i-1],v[i]));sort(v.begin(),v.end(),cmp);v.erase(unique(v.begin(),v.end()),v.end());
        int z=v.size();vector<int>par(z,-1),cnt(z),best(z),path;
        for(int i=0;i<z;++i){while(!path.empty()&&!anc(v[path.back()],v[i]))path.pop_back();if(!path.empty())par[i]=path.back();path.push_back(i);cnt[i]=mark[v[i]];}
        long long ans=1LL*n*c;
        for(int i=z-1;i>=0;--i){
            if(v[i]<=n)ans+=max(best[i],c-cnt[i])-c;
            int p=par[i];if(p>=0){int w=pref[v[i]]-pref[v[p]]-(v[i]<=n);ans-=1LL*w*min(cnt[i],c-cnt[i]);cnt[p]+=cnt[i];best[p]=max(best[p],cnt[i]);}
        }
        cout << ans << '\n';for(int u:v)mark[u]=0;
    }
}
