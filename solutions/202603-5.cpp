#include <bits/stdc++.h>
using namespace std;
struct Hash{
    static uint64_t mix(uint64_t x){
        x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);
    }
    size_t operator()(uint64_t x)const{
        static const uint64_t salt=chrono::steady_clock::now().time_since_epoch().count();
        return mix(x+salt);
    }
};
using Set=unordered_set<int,Hash>;
struct DynamicForest{
    vector<int> parent,weight,bad;
    vector<Set> adjacent,pending;
    unordered_map<uint64_t,vector<int>,Hash> bucket;
    int answer=0;
    DynamicForest(int n,int m):parent(n),weight(n,1),bad(m),adjacent(n),pending(n){
        iota(parent.begin(),parent.end(),0);
    }
    int find(int x){return parent[x]==x?x:parent[x]=find(parent[x]);}
    uint64_t key(int a,int b){if(a>b)swap(a,b);return (uint64_t)(unsigned)a<<32|(unsigned)b;}
    void edge(int a,int b){adjacent[a].insert(b);adjacent[b].insert(a);++weight[a];++weight[b];}
    void condition(int a,int b,int plan){
        if(a==b||adjacent[a].count(b))return;
        ++bad[plan];++weight[a];++weight[b];
        bucket[key(a,b)].push_back(plan);pending[a].insert(b);pending[b].insert(a);
    }
    void finish(int a,int b){
        auto it=bucket.find(key(a,b));if(it==bucket.end())return;
        for(int plan:it->second)if(--bad[plan]==0)++answer;
        bucket.erase(it);pending[a].erase(b);pending[b].erase(a);
    }
    void merge(int a,int b){
        a=find(a);b=find(b);if(a==b)return;
        if(weight[a]<weight[b])swap(a,b);
        // B 的邻居将与 A 相邻，原 A-C 查询因此首次有效。
        for(int c:adjacent[b])if(c!=a)finish(a,c);
        finish(a,b);
        adjacent[a].erase(b);
        for(int c:adjacent[b])if(c!=a){
            adjacent[c].erase(b);adjacent[c].insert(a);adjacent[a].insert(c);
        }
        Set().swap(adjacent[b]);
        // 将 B 端的所有未满足条件改挂到 A；重复端点对合桶。
        while(!pending[b].empty()){
            int c=*pending[b].begin();
            if(c==a||adjacent[a].count(c)){finish(b,c);continue;}
            uint64_t oldKey=key(b,c),newKey=key(a,c);
            auto moved=move(bucket.at(oldKey));bucket.erase(oldKey);
            pending[b].erase(c);pending[c].erase(b);
            pending[a].insert(c);pending[c].insert(a);
            auto &dest=bucket[newKey];dest.insert(dest.end(),moved.begin(),moved.end());
        }
        Set().swap(pending[b]);parent[b]=a;weight[a]+=weight[b];
    }
};
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n,X;cin>>n>>X;vector<vector<pair<int,int>>> g(n);
    vector<pair<int,int>> edges(n-1);
    for(int i=0;i<n-1;i++){
        int u,v;cin>>u>>v;--u;--v;edges[i]={u,v};g[u].push_back({v,i});g[v].push_back({u,i});
    }
    const int LOG=18;vector<array<int,LOG>> up(n);vector<int> depth(n),pe(n,-1),order{0};
    up[0].fill(0);
    for(size_t i=0;i<order.size();i++){
        int u=order[i];for(auto [v,e]:g[u])if(v!=up[u][0]){
            up[v][0]=u;pe[v]=e;depth[v]=depth[u]+1;
            for(int j=1;j<LOG;j++)up[v][j]=up[up[v][j-1]][j-1];order.push_back(v);
        }
    }
    auto lift=[&](int u,int d){for(int j=0;j<LOG;j++)if(d>>j&1)u=up[u][j];return u;};
    auto lca=[&](int u,int v){
        if(depth[u]<depth[v])swap(u,v);u=lift(u,depth[u]-depth[v]);
        if(u==v)return u;
        for(int j=LOG-1;j>=0;j--)if(up[u][j]!=up[v][j]){u=up[u][j];v=up[v][j];}
        return up[u][0];
    };
    auto distance=[&](int u,int v){return depth[u]+depth[v]-2*depth[lca(u,v)];};
    int k;cin>>k;vector<int> station(k);vector<bool> isStation(n);
    for(int &u:station){cin>>u;--u;isStation[u]=true;}
    // 在每条站点-普通点边上复制一个站点边界；站点之间的边天然可行。
    int nodes=n;vector<int> boundary(n-1,-1);vector<pair<int,int>> local(n-1,{-1,-1});
    for(int i=0;i<n-1;i++){
        auto [u,v]=edges[i];
        if(isStation[u]&&isStation[v])continue;
        if(isStation[u]||isStation[v]){
            boundary[i]=nodes++;if(isStation[u])u=boundary[i];else v=boundary[i];
        }
        local[i]={u,v};
    }
    int m;cin>>m;DynamicForest forest(nodes,m);
    for(auto [u,v]:local)if(u!=-1)forest.edge(u,v);
    auto endpoint=[&](int u,int toward){
        if(!isStation[u])return u;
        int v=lca(u,toward)==u?lift(toward,depth[toward]-depth[u]-1):up[u][0];
        int e=depth[u]>depth[v]?pe[u]:pe[v];return boundary[e];
    };
    for(int i=0;i<m;i++){
        int s,t;cin>>s>>t;--s;--t;int d=distance(s,t);
        vector<pair<int,int>> cuts{{0,s},{d,t}};
        for(int r:station)if(r!=s&&r!=t){
            int ds=distance(s,r);if(ds+distance(r,t)==d)cuts.push_back({ds,r});
        }
        sort(cuts.begin(),cuts.end());
        for(size_t j=1;j<cuts.size();j++){
            int u=cuts[j-1].second,v=cuts[j].second;
            if(distance(u,v)<=1)continue;
            forest.condition(endpoint(u,v),endpoint(v,u),i);
        }
        if(!forest.bad[i])++forest.answer;
    }
    int q,last=0;cin>>q;
    while(q--){
        int op;cin>>op;
        if(op==2){last=forest.answer;cout<<last<<'\n';}
        else{
            int u,v;cin>>u>>v;u^=X*last;v^=X*last;--u;--v;
            int e=depth[u]>depth[v]?pe[u]:pe[v];auto [a,b]=local[e];
            if(a!=-1)forest.merge(a,b);
        }
    }
}
