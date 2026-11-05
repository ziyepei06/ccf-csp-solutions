#include <bits/stdc++.h>
using namespace std;
int main(){
    long long n;int k,m;cin>>n>>k>>m;
    auto enough=[&](long long count){
        long long left=n;
        for(int day=0;day<m;day++){
            // 先分解再乘，避免 n*k 接近 64 位边界。
            long long bad=(left/100)*k+(left%100*k+99)/100;
            left-=bad;if(left<count)return false;left-=count;
        }
        return true;
    };
    long long lo=0,hi=1000000000;
    while(lo<hi){long long mid=(lo+hi+1)/2;
        if(enough(mid))lo=mid;else hi=mid-1;
    }
    cout<<lo<<'\n';
}
