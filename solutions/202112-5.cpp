#include <bits/stdc++.h>
using namespace std;
struct State{int u,p,mn,mx;};
int main(){
    int n,k1,k2;cin >> n >> k1 >> k2;vector<vector<int>>g(n+1);
    for(int i=1,u,v;i<n;++i){cin >> u >> v;g[u].push_back(v);g[v].push_back(u);}
    long long ans=0;vector<State>stack;stack.reserve(n);
    for(int x=1;x<=n;++x){stack.clear();stack.push_back({x,0,x,x});
        while(!stack.empty()){auto s=stack.back();stack.pop_back();
            if(s.u>=x&&s.mn>=x-k1&&s.mx<=s.u+k2)++ans;
            for(int v:g[s.u])if(v!=s.p)stack.push_back({v,s.u,min(s.mn,v),max(s.mx,v)});
        }
    }cout << ans << '\n';
}
