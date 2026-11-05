#include <bits/stdc++.h>
using namespace std;
int main(){
    int n,m;cin >> n >> m;vector<long long>a(n+1),f(n+1),best(n+1);vector<set<int>>g(n+1);
    for(int i=1;i<=n;++i)cin >> a[i];for(int i=1,u,v;i<n;++i){cin >> u >> v;g[u].insert(v);g[v].insert(u);}
    vector<int>p(n+1),order;int root=1;
    auto rebuild=[&](){order.clear();order.push_back(root);p[root]=0;
        for(size_t i=0;i<order.size();++i){int u=order[i];for(int v:g[u])if(v!=p[u]){p[v]=u;order.push_back(v);}}
        for(int i=n-1;i>=0;--i){int u=order[i];f[u]=a[u];best[u]=LLONG_MIN;
            for(int v:g[u])if(p[v]==u){f[u]+=max(0LL,f[v]);best[u]=max(best[u],best[v]);}best[u]=max(best[u],f[u]);}
    };
    rebuild();cout << f[root] << '\n';
    while(m--){int op,u;cin >> op >> u;
        if(op==1){cout << best[u] << '\n';continue;}
        if(op==2)cin >> a[u];else if(op==3)root=u;else{int b,c,d;cin >> b >> c >> d;g[u].erase(b);g[b].erase(u);g[c].insert(d);g[d].insert(c);}
        rebuild();cout << f[root] << '\n';
    }
}
