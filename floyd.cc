#include <bits/stdc++.h>
using namespace std;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M, Q; 
    cin >> N >> M >> Q; 
    // N个景点，M条路，Q次查询
    
    // 使用一个很大的值来表示不可达
    const long long INF = LLONG_MAX / 3; 
    // 为避免可能的加法溢出，这里除以3，给它留出空间

    // 定义邻接矩阵 dist，大小 (N+1)*(N+1)
    // 因为节点下标从 1 到 N，所以额外开1便于直接使用
    vector<vector<long long>> dist(N+1, vector<long long>(N+1, INF));

    // 1) 初始化：自己到自己距离为 0
    for(int i = 1; i <= N; i++){
        dist[i][i] = 0;
    }

    // 2) 读入边，更新 dist
    //   若图是无向，则同时设置 dist[u][v] 和 dist[v][u]
    for(int i = 0; i < M; i++){
        int u, v;
        long long w;
        cin >> u >> v >> w;
        // 处理多重边的情况，保留最小边权
        dist[u][v] = min(dist[u][v], w);
        dist[v][u] = min(dist[v][u], w);
    }

    // 3) Floyd-Warshall 核心：三重循环
    for(int k = 1; k <= N; k++){
        for(int i = 1; i <= N; i++){
            for(int j = 1; j <= N; j++){
                // 需要先判断 dist[i][k] 与 dist[k][j] 不为 INF，防止溢出
                if(dist[i][k] < INF && dist[k][j] < INF){
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }

    // 4) 根据 Q 次查询输出结果
    //   dist[start][end] 若仍是 INF 表示不可达
    while(Q--){
        int start, end;
        cin >> start >> end;
        if(dist[start][end] == INF) {
            cout << -1 << "\n";
        } else {
            cout << dist[start][end] << "\n";
        }
    }

    return 0;
}
