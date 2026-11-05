#include <bits/stdc++.h>
using namespace std;
using ull=unsigned long long;const int MOD=1000000007,H=12;
struct Big{
    struct Node{int l=-1,r=-1,mod=0;unsigned val=0;bool full=false;};
    vector<Node>v;unordered_map<ull,int>pairs;unordered_map<unsigned,int>leaves;int zero[H+1];long long power[H+1];
    int leaf(unsigned x){auto it=leaves.find(x);if(it!=leaves.end())return it->second;int id=v.size();v.push_back({-1,-1,(int)(x%MOD),x,x==UINT_MAX});leaves[x]=id;return id;}
    int join(int l,int r,int h){ull key=(ull)(unsigned)l<<32|(unsigned)r;auto it=pairs.find(key);if(it!=pairs.end())return it->second;
        int id=v.size();Node a=v[l],b=v[r];v.push_back({l,r,(int)((a.mod+power[h-1]*b.mod)%MOD),0,a.full&&b.full});pairs.emplace(key,id);return id;}
    Big(){power[0]=1;for(int i=0;i<32;++i)power[0]=power[0]*2%MOD;for(int h=1;h<=H;++h)power[h]=power[h-1]*power[h-1]%MOD;
        zero[0]=leaf(0);for(int h=1;h<=H;++h)zero[h]=join(zero[h-1],zero[h-1],h);}
    unsigned get(int u,int p,int h=H){if(!h)return v[u].val;int half=1<<(h-1);return p<half?get(v[u].l,p,h-1):get(v[u].r,p-half,h-1);}
    int set(int u,int p,unsigned x,int h=H){if(!h)return leaf(x);int half=1<<(h-1),l=v[u].l,r=v[u].r;
        if(p<half)l=set(l,p,x,h-1);else r=set(r,p-half,x,h-1);return join(l,r,h);}
    int clear(int u,int L,int R,int h=H,int start=0){int end=start+(1<<h)-1;if(R<start||end<L)return u;if(L<=start&&end<=R)return zero[h];
        int l=v[u].l,r=v[u].r,half=1<<(h-1);l=clear(l,L,R,h-1,start);r=clear(r,L,R,h-1,start+half);return join(l,r,h);}
    int nonFull(int u,int from,int h=H,int start=0){if(v[u].full||start+(1<<h)<=from)return -1;if(!h)return start;
        int l=v[u].l,r=v[u].r;int p=nonFull(l,from,h-1,start);return p>=0?p:nonFull(r,from,h-1,start+(1<<(h-1)));}
    int add(int u,unsigned a,int b){
        int p=b/32;ull shifted=(ull)a<<(b%32),s=(ull)get(u,p)+(unsigned)shifted;
        u=set(u,p,(unsigned)s);ull t=(ull)get(u,p+1)+(shifted>>32)+(s>>32);u=set(u,p+1,(unsigned)t);
        if(t>>32){int z=nonFull(u,p+2);if(z>p+2)u=clear(u,p+2,z-1);u=set(u,z,get(u,z)+1);}return u;
    }
    int cmp(int a,int b,int h=H)const{if(a==b)return 0;if(!h)return v[a].val<v[b].val?-1:1;
        if(v[a].r!=v[b].r)return cmp(v[a].r,v[b].r,h-1);return cmp(v[a].l,v[b].l,h-1);}
};
struct Edge{int to,a,b;};using Graph=vector<vector<Edge>>;
struct HeapCmp{const Big *big;bool operator()(pair<int,int>a,pair<int,int>b)const{return big->cmp(a.first,b.first)>0;}};
vector<int> shortest(const Graph&g,int n){
    Big big;vector<int>d(g.size(),-1),ans(n,-1);vector<char>done(g.size());
    priority_queue<pair<int,int>,vector<pair<int,int>>,HeapCmp>q(HeapCmp{&big});d[0]=big.zero[H];q.push({d[0],0});
    while(!q.empty()){
        auto [dist,u]=q.top();q.pop();if(done[u]||d[u]!=dist)continue;done[u]=1;
        for(auto e:g[u]){int nd=e.a?big.add(dist,e.a,e.b):dist;if(d[e.to]<0||big.cmp(nd,d[e.to])<0){d[e.to]=nd;q.push({nd,e.to});}}
    }
    for(int i=0;i<n;++i)if(d[i]>=0)ans[i]=big.v[d[i]].mod;return ans;
}
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int n,m;cin >> n >> m;Graph g(n),rev(n);vector<int>in(n*4),out(n*4);
    auto node=[&](){int id=g.size();g.emplace_back();rev.emplace_back();return id;};
    auto edge=[&](int u,int v,int a=0,int b=0){g[u].push_back({v,a,b});rev[v].push_back({u,a,b});};
    function<void(int,int,int)>build=[&](int u,int l,int r){if(l==r){in[u]=out[u]=l-1;return;}int mid=(l+r)/2;build(u*2,l,mid);build(u*2+1,mid+1,r);in[u]=node();out[u]=node();edge(in[u*2],in[u]);edge(in[u*2+1],in[u]);edge(out[u],out[u*2]);edge(out[u],out[u*2+1]);};build(1,1,n);
    function<void(int,int,int,int,int,int,bool)>link=[&](int u,int l,int r,int L,int R,int sat,bool input){
        if(L<=l&&r<=R){if(input)edge(in[u],sat);else edge(sat,out[u]);return;}int mid=(l+r)/2;if(L<=mid)link(u*2,l,mid,L,R,sat,input);if(R>mid)link(u*2+1,mid+1,r,L,R,sat,input);
    };
    for(int i=0;i<m;++i){int l1,r1,l2,r2,a,b;cin >> l1 >> r1 >> l2 >> r2 >> a >> b;int x=node(),y=node();edge(x,y,a,b);link(1,1,n,l1,r1,x,true);link(1,1,n,l2,r2,y,false);}
    auto a=shortest(g,n),b=shortest(rev,n);for(int i=1;i<n;++i)cout << (a[i]<0||b[i]<0?-1:(a[i]+(long long)b[i])%MOD) << (i+1==n?'\n':' ');
}
