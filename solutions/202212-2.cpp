#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,m;cin >> n >> m;vector<int>p(m+1),t(m+1),e(m+1),l(m+1);
    for(int i=1;i<=m;++i)cin >> p[i];
    for(int i=1;i<=m;++i)cin >> t[i];
    bool ok=true;
    for(int i=1;i<=m;++i) {
        e[i]=p[i]?e[p[i]]+t[p[i]]:1;l[i]=n-t[i]+1;
        ok&=e[i]+t[i]-1<=n;cout << e[i] << (i==m?'\n':' ');
    }
    if(!ok)return 0;
    for(int i=m;i>=1;--i)if(p[i])l[p[i]]=min(l[p[i]],l[i]-t[p[i]]);
    for(int i=1;i<=m;++i)cout << l[i] << (i==m?'\n':' ');
}
