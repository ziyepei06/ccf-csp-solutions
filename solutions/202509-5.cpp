#include <bits/stdc++.h>
using namespace std;
using ll=long long;using Score=pair<ll,ll>;
Score plusScore(Score a,Score b){return {a.first+b.first,a.second+b.second};}
Score minusScore(Score a,Score b){return {a.first-b.first,a.second-b.second};}
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int n;ll budget;cin >> n >> budget;vector<ll>a(n),b(n);for(ll &x:a)cin >> x;for(ll &x:b)cin >> x;
    auto evaluate=[&](ll reward){
        priority_queue<Score,vector<Score>,greater<Score>>breaks;Score value{0,0};
        for(int i=0;i<n;++i){
            Score immediate=min(Score{0,0},Score{2*(a[i]+b[i])-reward,-1});
            Score lower=minusScore(immediate,{2*b[i]-reward,-1});
            Score upper=minusScore({2*a[i],0},immediate);value=plusScore(value,immediate);
            if(!breaks.empty()&&breaks.top()<lower){value=plusScore(value,minusScore(breaks.top(),lower));breaks.pop();breaks.push(lower);}
            breaks.push(upper);
        }
        ll k=-value.second;return Score{k,(value.first+reward*k)/2};
    };
    ll low=0,high=4000000001LL;
    while(low<high){ll mid=(low+high+1)/2;if(evaluate(mid).second<=budget)low=mid;else high=mid-1;}
    auto x=evaluate(low);if(x.first==n){cout << n << '\n';return 0;}
    auto y=evaluate(low+1);ll answer=x.first;
    if(y.first>x.first){ll marginal=(y.second-x.second)/(y.first-x.first);if(marginal>0)answer+=min(y.first-x.first,(budget-x.second)/marginal);}
    cout << answer << '\n';
}
