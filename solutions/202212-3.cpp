#include <bits/stdc++.h>
using namespace std;
int main() {
    int Q[8][8],M[8][8]={},n,T;
    for(auto &r:Q)for(int &x:r)cin >> x;cin >> n >> T;
    vector<pair<int,int>>order;
    for(int s=0;s<=14;++s) {
        vector<pair<int,int>>v;
        for(int i=max(0,s-7);i<=min(7,s);++i)v.push_back({i,s-i});
        if(s%2==0)reverse(v.begin(),v.end());
        order.insert(order.end(),v.begin(),v.end());
    }
    for(int k=0;k<n;++k){auto [i,j]=order[k];cin >> M[i][j];}
    if(T)for(int i=0;i<8;++i)for(int j=0;j<8;++j)M[i][j]*=Q[i][j];
    double pi=acos(-1.0),C[8][8];
    for(int i=0;i<8;++i)for(int u=0;u<8;++u)C[i][u]=cos((2*i+1)*u*pi/16)*(u?1:1/sqrt(2.0));
    for(int i=0;i<8;++i)for(int j=0;j<8;++j) {
        int ans=M[i][j];
        if(T==2) {
            double x=0;for(int u=0;u<8;++u)for(int v=0;v<8;++v)x+=C[i][u]*C[j][v]*M[u][v]/4;
            ans=clamp((int)floor(x+128+0.5),0,255);
        }
        cout << ans << (j==7?'\n':' ');
    }
}
