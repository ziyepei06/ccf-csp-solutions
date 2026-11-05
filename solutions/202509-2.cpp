#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,L;cin >> n >> L;vector<vector<int>>a(n,vector<int>(n));
    for(auto &r:a)for(int &x:r)cin >> x;
    vector<string> pat={"111111111","100100101","100111110","100001100","111111100"};
    vector<int>d(L+1);
    for(int i=0;i+5<=n;++i)for(int j=0;j+9<=n;++j) {
        int lo=0,hi=L-1;
        for(int r=0;r<5;++r)for(int c=0;c<9;++c)
            if(pat[r][c]=='1')hi=min(hi,a[i+r][j+c]);else lo=max(lo,a[i+r][j+c]+1);
        if(lo<=hi) {++d[lo];--d[hi+1];}
    }
    int cur=0;for(int k=0;k<L;++k) {cur+=d[k];if(cur)cout << k << '\n';}
}
