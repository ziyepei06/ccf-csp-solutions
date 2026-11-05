#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,k;long long m;cin >> n >> m >> k;
    vector<pair<int,int>> a(n);int hi=k;
    for(auto &[t,c]:a) {cin >> t >> c;hi=max(hi,t);}
    int lo=k;
    while(lo<hi) {
        int mid=(lo+hi)/2;long long cost=0;
        for(auto [t,c]:a)if(t>mid)cost+=1LL*(t-mid)*c;
        if(cost<=m)hi=mid;else lo=mid+1;
    }
    cout << lo << '\n';
}
