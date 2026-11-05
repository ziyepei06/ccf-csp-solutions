#include <bits/stdc++.h>
using namespace std;
struct Process{
    char type;int start,w=0,pos=0,remaining=0,fail=0;
    vector<pair<int,int>> task;
    long long gain=0,duration=-1;
    bool done=false;
};
int main(){
    int n,m;cin>>n>>m;vector<Process> p(n);
    for(auto &x:p){
        int k;cin>>x.type>>x.start;
        if(x.type!='X')cin>>x.w;
        cin>>k;x.task.resize(k);
        for(auto &job:x.task)cin>>job.first>>job.second;
    }
    vector<int> owner(m+1,-1);
    for(int now=1;;now++){
        vector<vector<int>> request(m+1),force(m+1);
        vector<bool> release(n,false);
        for(int i=0;i<n;i++){
            auto &x=p[i];
            if(x.done||now<x.start||x.remaining)continue;
            int r=x.task[x.pos].first;
            if(x.type=='C'&&x.fail>=x.w)force[r].push_back(i);
            else request[r].push_back(i);
        }
        auto run=[&](int i){
            auto &x=p[i];x.remaining=x.task[x.pos].second;x.fail=0;
        };
        for(int r=1;r<=m;r++){
            int winner=-1;
            if(!force[r].empty()){
                winner=force[r].back();owner[r]=winner;
                // 强夺失败者也开始任务，只是不持有该资源。
                for(int i:force[r])run(i);
            }else if(owner[r]==-1&&!request[r].empty()){
                winner=request[r][0];owner[r]=winner;run(winner);
            }
            for(int i:request[r])if(i!=winner){
                auto &x=p[i];++x.fail;
                if(x.type=='A'&&x.fail==x.w)release[i]=true;
                if(x.type=='B'&&x.fail==x.w)run(i);
            }
        }
        vector<int> held(n);
        for(int r=1;r<=m;r++)if(owner[r]!=-1)++held[owner[r]];
        for(int i=0;i<n;i++){
            auto &x=p[i];if(!x.remaining)continue;
            x.gain+=held[i];--x.remaining;
            if(x.remaining==0){
                ++x.pos;
                if(x.pos==(int)x.task.size()){
                    x.done=true;x.duration=now-x.start+1;release[i]=true;
                }
            }
        }
        for(int r=1;r<=m;r++)if(owner[r]!=-1&&release[owner[r]])owner[r]=-1;
        // 只剩永久等待时，未来不会再改变状态，可直接判死锁。
        bool future=false;
        for(auto &x:p)if(!x.done){
            if(now<x.start||x.remaining||x.type=='B'||x.type=='C')future=true;
            else if(x.type=='A'&&x.fail<x.w)future=true;
        }
        // 刚释放资源或完成任务后，下一段仍可能成功申请。
        if(!future){
            for(int i=0;i<n;i++)if(!p[i].done&&now>=p[i].start){
                int r=p[i].task[p[i].pos].first;
                if(owner[r]==-1)future=true;
            }
        }
        if(!future)break;
    }
    for(auto &x:p)cout<<x.gain<<' '<<x.duration<<'\n';
}
