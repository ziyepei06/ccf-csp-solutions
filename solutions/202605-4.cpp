#include <bits/stdc++.h>
using namespace std;
struct Hash{
    static uint64_t mix(uint64_t x){
        x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);
    }
    size_t operator()(uint64_t x)const{
        static const uint64_t seed=chrono::steady_clock::now().time_since_epoch().count();
        return mix(x+seed);
    }
};
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n,q;cin>>n>>q;vector<int> odd(n+1),even(n+1);
    for(int i=1,x;i<=n;i++){
        cin>>x;odd[i]=odd[i-1];even[i]=even[i-1];
        ((i&1)?odd[i]:even[i])^=x;
    }
    unordered_map<int,int,Hash> last[2];
    for(auto &h:last)h.reserve(n/2+1);
    int LOG=1;while((1<<LOG)<=n)++LOG;
    vector<vector<int>> jump(LOG,vector<int>(n+3,n+2));
    int best=n+1;
    for(int l=n;l>=1;l--){
        auto &h=last[l&1];auto &prefix=(l&1)?odd:even;
        h[prefix[l]]=l;
        auto it=h.find(prefix[l-1]);
        if(it!=h.end())best=min(best,it->second);
        jump[0][l]=best+1;
    }
    // 哈希表只用于预处理，提前释放降低峰值内存。
    for(auto &h:last)unordered_map<int,int,Hash>().swap(h);
    for(int j=1;j<LOG;j++)for(int l=1;l<=n;l++)
        jump[j][l]=jump[j-1][jump[j-1][l]];
    while(q--){
        int l,r,answer=0;cin>>l>>r;
        for(int j=LOG-1;j>=0;j--)if(jump[j][l]<=r+1){
            answer+=1<<j;l=jump[j][l];
        }
        cout<<answer<<'\n';
    }
}
