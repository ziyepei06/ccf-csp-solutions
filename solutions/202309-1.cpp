#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,m;cin >> n >> m;long long dx=0,dy=0,x,y;
    while(n--) {cin >> x >> y;dx+=x;dy+=y;}
    while(m--) {cin >> x >> y;cout << x+dx << ' ' << y+dy << '\n';}
}
