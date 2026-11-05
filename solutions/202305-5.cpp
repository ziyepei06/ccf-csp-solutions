#include <bits/stdc++.h>
using namespace std;
using ll=long long;const ll INF=4000000000000000000LL;
struct Edge{int to,mask,len;};
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int N;string t;cin >> N >> t;int K=1<<t.size(),all=K-1;int tag[26]={};for(size_t i=0;i<t.size();++i)tag[t[i]-'a']=1<<i;
    vector<tuple<int,int,int,int>>routes;map<tuple<int,int,int>,int>shortest;
    for(int i=0;i<N;++i){string s;cin >> s;int u=s.front()-'a',v=s.back()-'a',mask=0,len=s.size()-1;for(char c:s)mask|=tag[c-'a'];routes.push_back({u,v,mask,len});auto key=make_tuple(u,v,mask);auto it=shortest.find(key);if(it==shortest.end()||len<it->second)shortest[key]=len;}
    vector<vector<Edge>>g(26);for(auto [key,len]:shortest){auto [u,v,mask]=key;g[u].push_back({v,mask,len});}
    vector<vector<ll>>dist(26,vector<ll>(26*K,INF));
    for(int source=0;source<26;++source){auto &d=dist[source];priority_queue<pair<ll,int>,vector<pair<ll,int>>,greater<pair<ll,int>>>q;d[source*K]=0;q.push({0,source*K});
        while(!q.empty()){auto [du,state]=q.top();q.pop();if(du!=d[state])continue;int u=state/K,mask=state%K;
            for(auto e:g[u]){int v=e.to*K+(mask|e.mask);ll nd=du+e.len;if(nd<d[v]){d[v]=nd;q.push({nd,v});}}
        }
        for(int u=0;u<26;++u)for(int bit=1;bit<K;bit*=2)for(int mask=0;mask<K;++mask)if(!(mask&bit))d[u*K+mask]=min(d[u*K+mask],d[u*K+(mask|bit)]);
    }
    for(auto [u,v,mask,len]:routes){ll answer=dist[v][u*K+(all^mask)];cout << (answer==INF?-1:answer+len) << '\n';}
}
