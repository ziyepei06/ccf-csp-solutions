#include <bits/stdc++.h>
using namespace std;
struct State{int ask=0,yes=-1,no=-1;};
int n,timer;vector<vector<int>>ch;vector<int>tin,tout,order,parent;vector<long long>w;vector<State>states;
void dfs(int u){tin[u]=++timer;order.push_back(u);for(int v:ch[u])dfs(v);tout[u]=timer;}
bool inside(int a,int b){return tin[a]<=tin[b]&&tin[b]<=tout[a];}
int build(vector<int> active){
    int id=states.size();states.push_back({});if(active.size()==1)return id;
    vector<long long>s(n+1);long long total=0;
    for(int u:active){s[u]=w[u];total+=w[u];}
    for(int i=n-1;i>0;--i){int u=order[i];s[parent[u]]+=s[u];}
    int best=0;long long diff=LLONG_MAX;
    for(int u:active){long long d=llabs(total-2*s[u]);if(d<diff||(d==diff&&u<best)){diff=d;best=u;}}
    vector<int>yes,no;for(int u:active)(inside(best,u)?yes:no).push_back(u);
    int a=build(move(yes)),b=build(move(no));states[id]={best,a,b};return id;
}
int main(){
    int m;cin >> n >> m;ch.resize(n+1);tin.resize(n+1);tout.resize(n+1);parent.resize(n+1);w.resize(n+1);
    for(int i=1;i<=n;++i)cin >> w[i];for(int i=2;i<=n;++i){cin >> parent[i];ch[parent[i]].push_back(i);}dfs(1);
    vector<int>a(n);iota(a.begin(),a.end(),1);build(move(a));
    while(m--){int target;cin >> target;int cur=0;bool first=true;
        while(states[cur].ask){int u=states[cur].ask;cout << (first?"":" ") << u;first=false;cur=inside(u,target)?states[cur].yes:states[cur].no;}
        cout << '\n';
    }
}
