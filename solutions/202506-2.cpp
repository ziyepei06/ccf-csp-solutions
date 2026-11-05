#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,k,x,y;cin >> n >> k >> x >> y;
    vector<vector<int>>d(n+1,vector<int>(n+1,-1));queue<pair<int,int>>q;
    d[x][y]=0;q.push({x,y});int ans=1;
    int dx[]={1,1,-1,-1,2,2,-2,-2},dy[]={2,-2,2,-2,1,-1,1,-1};
    while(!q.empty()) {
        auto [a,b]=q.front();q.pop();if(d[a][b]==k)continue;
        for(int j=0;j<8;++j) {
            int u=a+dx[j],v=b+dy[j];
            if(u<1||u>n||v<1||v>n||d[u][v]!=-1)continue;
            d[u][v]=d[a][b]+1;q.push({u,v});++ans;
        }
    }
    cout << ans << '\n';
}
