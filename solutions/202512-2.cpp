#include <bits/stdc++.h>
using namespace std;
int f(int x,int k) {return ((x*x+k*k)&7)^k;}
int g(int x,int k) {
    int a=x>>6,b=(x>>3)&7,c=x&7;
    return (b<<6)|((c^f(b,k))<<3)|(a^f(c,k));
}
int main() {
    int n,m;cin >> n >> m;vector<int>k(m);for(int &x:k)cin >> x;
    vector<int> inv(512);
    for(int x=0;x<512;++x) {int y=x;for(int v:k)y=g(y,v);inv[y]=x;}
    for(int i=0,x;i<n;++i) {cin >> x;cout << inv[x] << (i+1==n?'\n':' ');}
}
