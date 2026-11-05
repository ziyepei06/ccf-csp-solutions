#include <bits/stdc++.h>
using namespace std;
int main() {
    long long b,c,l,r;cin >> b >> c >> l >> r;long long s=0;
    for(long long x=l+(l%2);x<=r;x+=2)s+=2*(x*x+b*x+c);
    cout << s << '\n';
}
