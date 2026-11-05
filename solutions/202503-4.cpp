#include <bits/stdc++.h>
using namespace std;
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int n;cin >> n;const long long P=998244353;long long ans=0;
    vector<pair<int,long long>>prev,cur;
    for(int r=1,x;r<=n;++r){
        cin >> x;cur.clear();cur.push_back({x,r});
        for(auto [g,s]:prev){int h=gcd(g,x);if(cur.back().first==h)cur.back().second=(cur.back().second+s)%P;else cur.push_back({h,s});}
        for(auto [g,s]:cur)ans=(ans+(1LL*r*g%P)*s)%P;prev.swap(cur);
    }
    cout << ans << '\n';
}
