#include <bits/stdc++.h>
using namespace std;
using ll=long long;const ll INF=4000000000000000000LL;
struct Model{
    int K;vector<vector<ll>>unary;vector<map<int,vector<ll>>>edges;vector<char>alive;
    Model(int n,int k):K(k),unary(n,vector<ll>(k)),edges(n),alive(n,1){}
    void add(int u,int v,const vector<ll>&cost){
        auto &a=edges[u][v];if(a.empty())a.assign(K*K,0);for(int i=0;i<K*K;++i)a[i]+=cost[i];
        vector<ll>b(K*K);for(int i=0;i<K;++i)for(int j=0;j<K;++j)b[j*K+i]=a[i*K+j];edges[v][u]=move(b);
    }
};
ll solve(Model model){
    int n=model.alive.size(),K=model.K;ll constant=0;
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>>q;
    for(int u=0;u<n;++u)if(model.alive[u])q.push({model.edges[u].size(),u});
    while(!q.empty()){
        auto [deg,u]=q.top();q.pop();if(!model.alive[u]||deg!=(int)model.edges[u].size())continue;if(deg>2)break;
        vector<int>nb;for(auto &[v,a]:model.edges[u])nb.push_back(v);
        if(!deg)constant+=*min_element(model.unary[u].begin(),model.unary[u].end());
        if(deg==1){int v=nb[0];auto &A=model.edges[u][v];for(int j=0;j<K;++j){ll z=INF;for(int i=0;i<K;++i)z=min(z,model.unary[u][i]+A[i*K+j]);model.unary[v][j]+=z;}}
        if(deg==2){int v=nb[0],w=nb[1];auto &A=model.edges[u][v],&B=model.edges[u][w];vector<ll>C(K*K,INF);
            for(int j=0;j<K;++j)for(int k=0;k<K;++k)for(int i=0;i<K;++i)C[j*K+k]=min(C[j*K+k],model.unary[u][i]+A[i*K+j]+B[i*K+k]);model.add(v,w,C);
        }
        model.alive[u]=0;for(int v:nb){model.edges[v].erase(u);q.push({model.edges[v].size(),v});}model.edges[u].clear();
    }
    vector<int>core;for(int u=0;u<n;++u)if(model.alive[u])core.push_back(u);if(core.empty())return constant;
    if(core.size()<=6){
        vector<int>chosen(n,-1);ll best=INF;
        function<void(int,ll)>dfs=[&](int pos,ll cost){if(cost>=best)return;if(pos==(int)core.size()){best=cost;return;}int u=core[pos];
            for(int c=0;c<K;++c){ll next=cost+model.unary[u][c];for(auto &[v,A]:model.edges[u])if(chosen[v]>=0)next+=A[c*K+chosen[v]];chosen[u]=c;dfs(pos+1,next);chosen[u]=-1;}
        };dfs(0,0);return constant+best;
    }
    int u=*max_element(core.begin(),core.end(),[&](int x,int y){return model.edges[x].size()<model.edges[y].size();});ll best=INF;
    for(int c=0;c<K;++c){Model next=model;next.alive[u]=0;for(auto &[v,A]:model.edges[u]){for(int j=0;j<K;++j)next.unary[v][j]+=A[c*K+j];next.edges[v].erase(u);}next.edges[u].clear();best=min(best,model.unary[u][c]+solve(move(next)));}
    return constant+best;
}
int main(){int n,m,K;cin >> n >> m >> K;Model model(n,K);for(auto &row:model.unary)for(ll &x:row)cin >> x;
    while(m--){int u,v;cin >> u >> v;vector<ll>A(K*K);for(ll &x:A)cin >> x;model.add(u,v,A);}cout << solve(move(model)) << '\n';
}
