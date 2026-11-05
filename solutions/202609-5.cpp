#include <bits/stdc++.h>
using namespace std;
using U=uint64_t;
struct Counter{
    struct Cell{U key=~U(0);array<int,4> count{};};
    vector<Cell> table;size_t mask=0;U seed;
    static U hash(U x){
        x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;
        x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);
    }
    Counter(const vector<U>&keys,U seed):seed(seed){
        size_t cap=1;while(cap<2*keys.size()+1)cap*=2;
        table.resize(cap);mask=cap-1;
        for(U key:keys){
            size_t at=hash(key+seed)&mask;
            while(table[at].key!=~U(0)&&table[at].key!=key)at=(at+1)&mask;
            table[at].key=key;
        }
    }
    int get(U key,int r)const{
        size_t at=hash(key+seed)&mask;
        while(table[at].key!=~U(0)){
            if(table[at].key==key)return table[at].count[r];at=(at+1)&mask;
        }
        return 0;
    }
    void add(U key,int r,int delta){
        size_t at=hash(key+seed)&mask;
        while(table[at].key!=key)at=(at+1)&mask;
        table[at].count[r]+=delta;
    }
};
struct Entry{U mask;int center,sub,dist;};
struct Solver{
    int n,m,k;U seed;
    vector<vector<pair<int,int>>> graph;
    vector<int> color,size,parent;vector<bool> removed;
    vector<vector<Entry>> path;
    vector<Counter> counts;
    vector<int> all;
    Solver(int n,int m,int k):n(n),m(m),k(k),graph(n),color(n),size(n),parent(n),removed(n),path(n),all(n){
        seed=chrono::steady_clock::now().time_since_epoch().count();
        counts.reserve(2*n);
    }
    int newCounter(const vector<U>&keys){
        int id=counts.size();counts.emplace_back(keys,Counter::hash(seed+id));return id;
    }
    void build(int start){
        vector<int> order{start};parent[start]=-1;
        for(size_t i=0;i<order.size();i++){
            int u=order[i];
            for(auto [v,w]:graph[u])if(!removed[v]&&v!=parent[u]){
                parent[v]=u;order.push_back(v);
            }
        }
        int total=order.size(),c=start,balance=total;
        for(int i=total-1;i>=0;i--){
            int u=order[i],largest=0;size[u]=1;
            for(auto [v,w]:graph[u])if(!removed[v]&&parent[v]==u){size[u]+=size[v];largest=max(largest,size[v]);}
            largest=max(largest,total-size[u]);if(largest<balance){balance=largest;c=u;}
        }
        removed[c]=true;
        U bit=U(1)<<color[c];vector<U> keys{bit};
        path[c].push_back({bit,c,-1,0});
        for(auto [v,w]:graph[c])if(!removed[v]){
            struct Visit{int u,p,d;U mask;};
            vector<Visit> stack{{v,c,w%k,bit^(U(1)<<color[v])}};
            vector<pair<int,Entry>> entries;vector<U> subKeys;
            while(!stack.empty()){
                auto z=stack.back();stack.pop_back();
                entries.push_back({z.u,{z.mask,c,-1,z.d}});subKeys.push_back(z.mask);keys.push_back(z.mask);
                for(auto [next,len]:graph[z.u])if(next!=z.p&&!removed[next])
                    stack.push_back({next,z.u,(z.d+len)%k,z.mask^(U(1)<<color[next])});
            }
            int sub=newCounter(subKeys);
            for(auto [u,e]:entries){e.sub=sub;path[u].push_back(e);}
        }
        all[c]=newCounter(keys);
        for(auto [v,w]:graph[c])if(!removed[v])build(v);
    }
    long long query(int u)const{
        long long answer=0;
        for(auto e:path[u]){
            int r=(k-e.dist)%k;U base=e.mask^(U(1)<<color[e.center]);
            for(int c=0;c<m;c++){
                U target=base^(U(1)<<c);
                answer+=counts[all[e.center]].get(target,r);
                if(e.sub!=-1)answer-=counts[e.sub].get(target,r);
            }
        }
        return answer;
    }
    void update(int u,int delta){
        for(auto e:path[u]){
            counts[all[e.center]].add(e.mask,e.dist,delta);
            if(e.sub!=-1)counts[e.sub].add(e.mask,e.dist,delta);
        }
    }
};
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n,m,k,q;cin>>n>>m>>k>>q;Solver s(n,m,k);string initial;cin>>initial;
    for(int &c:s.color){cin>>c;--c;}
    for(int i=1;i<n;i++){int u,v,w;cin>>u>>v>>w;--u;--v;s.graph[u].push_back({v,w});s.graph[v].push_back({u,w});}
    s.build(0);long long answer=0;vector<bool> on(n);
    for(int u=0;u<n;u++)if(initial[u]=='1'){
        answer+=s.query(u)+1;s.update(u,1);on[u]=true;
    }
    cout<<answer<<'\n';
    while(q--){
        int u;cin>>u;--u;
        if(on[u]){s.update(u,-1);answer-=s.query(u)+1;}
        else{answer+=s.query(u)+1;s.update(u,1);}
        on[u]=!on[u];cout<<answer<<'\n';
    }
}
