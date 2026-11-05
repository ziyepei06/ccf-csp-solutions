#include <bits/stdc++.h>
using namespace std;
int main() {
    int n;double i;cin >> n >> i;
    double factor=1,ans=0,x;
    for(int k=0;k<=n;++k) {cin >> x;ans+=x*factor;factor/=1+i;}
    cout << fixed << setprecision(3) << ans << '\n';
}
