#include <bits/stdc++.h>
using namespace std;
struct Job{int a,b;};
int main(){
    int n,m;cin>>n>>m; long long original=0;
    vector<Job> flexible,whole;
    for(int i=0;i<n;i++){
        int type,t,a,b;cin>>type>>t>>a>>b;original+=t;
        (type?whole:flexible).push_back({a,b});
    }
    // 普通任务是 0/1 背包，dp[c] 为恰用 c 杯的最大减时。
    vector<int> dp(m+1,-1000000000);dp[0]=0;
    for(auto j:whole)
        for(int c=m;c>=j.a;c--)
            dp[c]=max(dp[c],dp[c-j.a]+j.b);
    sort(flexible.begin(),flexible.end(),[](Job x,Job y){
        return 1LL*x.b*y.a>1LL*y.b*x.a;
    });
    vector<long double> gain(m+1);
    for(int c=0;c<=m;c++){
        int left=c;long double value=0;
        for(auto j:flexible){
            int used=min(left,j.a);value+=(long double)used*j.b/j.a;
            left-=used;if(!left)break;
        }
        gain[c]=value;
    }
    long double best=0;
    for(int c=0;c<=m;c++)if(dp[c]>=0)
        best=max(best,dp[c]+gain[m-c]);
    cout<<fixed<<setprecision(8)<<original-best<<'\n';
}
