#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,a,b;cin >> n >> a >> b;long long ans=0;
    while(n--) {
        int x1,y1,x2,y2;cin >> x1 >> y1 >> x2 >> y2;
        ans+=1LL*max(0,min(x2,a)-max(x1,0))*max(0,min(y2,b)-max(y1,0));
    }
    cout << ans << '\n';
}
