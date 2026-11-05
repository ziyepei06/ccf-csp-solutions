#include <bits/stdc++.h>
using namespace std;
int main(){
    int n,m;string qs;cin>>n>>m>>qs;qs.erase(qs.find('.'),1);
    long long Q=stoll(qs);vector<int> value(n),weight(n);int sum=0;
    for(int &x:value){cin>>x;sum+=x;}
    for(int &w:weight){string s;cin>>s;s.erase(s.find('.'),1);int x=stoi(s);w=x*x;}
    const int INF=1000000000;vector<int> dp(sum+1,INF);dp[0]=0;
    for(int i=0;i<n;i++)for(int v=sum;v>=value[i];v--)
        dp[v]=min(dp[v],dp[v-value[i]]+weight[i]);
    for(int v=sum;v>=0;v--)if(1LL*dp[v]*Q*Q<=100000000LL*m*m){
        cout<<v<<'\n';break;
    }
}
