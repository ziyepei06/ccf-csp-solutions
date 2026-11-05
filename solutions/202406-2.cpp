#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,m,t;cin >> n >> m >> t;vector<int>a(n*m);
    for(int &x:a)cin >> x;
    while(t--) {
        int op,x,y;cin >> op >> x >> y;
        if(op==1) {n=x;m=y;}
        else if(op==2) {
            vector<int>b(a.size());
            for(int i=0;i<n;++i)for(int j=0;j<m;++j)b[j*n+i]=a[i*m+j];
            a.swap(b);swap(n,m);
        } else cout << a[x*m+y] << '\n';
    }
}
