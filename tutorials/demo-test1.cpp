#include "Snap.h"

#include <memory>
#include <sstream>

////////////////////////////////////////////////////////////////

// 无向图
template <typename Graph> void CountTriangle(const Graph &g) {
  printf("CountTriangle:\n");
  int count = TSnap::GetTriangleCnt(g);
  printf("count=%d\n", count);

  for (int v = 0; v < 10; v++) {
    int count = TSnap::GetNodeTriads(g, v);
    printf("vid=%d, count=%d\n", v, count);
  }
}

template <typename Graph> void ClustCfLocal(const Graph &g) {
  printf("ClustCfLocal:\n");
  for (int i = 0; i < 10; i++) {
    double cf = TSnap::GetNodeClustCf(g, i);
    printf("vid=%d, cf=%f\n", i, cf);
  }
}

template <typename Graph> void ClustCfGlobal(const Graph &g) {
  double d1 = TSnap::GetClustCf(g);
  printf("clustering coefficient:%f\n", d1);
}

template <typename Graph> void wcc(const Graph &g) {
  printf("wcc:\n");
  TCnComV CnComV;
  TSnap::GetWccs(g, CnComV);
  printf("size: %d\n", CnComV.GetMemSize());

  // 输出太多了
  for (int i = 0; i < 10; i++) {
    TCnCom Com = CnComV[i];
    int limit = 10;
    for (int j = 0; j < Com.Len() && limit > 0; j++, limit--) {
      printf("%d,", Com[j]);
    }
    printf("\n");
  }
}

// 比较耗时
template <typename Graph> void kcore(const Graph &g) {
  printf("kcore:\n");
  TKCore<Graph> KCore(g);
  TIntPrV KCoreV;
  int count = 0;
  while (KCore.GetNextCore() != 0 & count < 3) {
    count++;
    KCoreV.Add(TIntPr(KCore.GetCurK(), KCore.GetCoreNodes()));
    TSnap::SaveEdgeList(KCore.GetCoreG(),
                        TStr::Fmt("core%02d.txt", KCore.GetCurK()),
                        TStr::Fmt("%d-core of graph", KCore.GetCurK()));
  }
  TGnuPlot::PlotValV(KCoreV, "kcore-",
                     TStr::Fmt("k-core decomposition of graph (%d, %d)",
                               g->GetNodes(), g->GetEdges()),
                     "k (min node degree in the k-core)",
                     "Number of nodes in the k-core", gpsLog);
}

template <typename Graph> void page_rank(const Graph &g) {
  printf("pagerank:\n");
  TIntFltH PRankH;
  TSnap::GetPageRank(g, PRankH);
  for (int i = 0; i < 10; i++) {
    printf("vid=%d,pagerank=%f\n", i, PRankH.GetDat(i).Val);
  }
}

// 中介中心性
template <typename Graph> void bnc(const Graph &g) {
  printf("BNC:\n");
  TIntFltH nodeBtwH;
  TIntPrFltH edgeBtwH;

  TSnap::GetBetweennessCentr<Graph>(g, nodeBtwH, edgeBtwH, true);

  int count = 10;
  for (TIntFltH::TIter It = nodeBtwH.BegI(); It < nodeBtwH.EndI() && count > 0;
       It++) {
    int node_id = It.GetKey();
    double centr = It.GetDat();
    printf("vid: %d, bnc: %f \n", node_id, centr);
    count--;
  }
}

// 接近中心性
template <typename Graph> void cnc(const Graph &g) {
  printf("CNC:\n");
  for (int id = 0; id < 10; id++) {
    double centr = TSnap::GetClosenessCentr<Graph>(g, id, true);
    printf("vid: %d, cnc: %f \n", id, centr);
  }
}

template <typename Graph> void shortest_path(const Graph &g) {
  printf("shortest_path:\n");
  int src = 12345;
  for (int dst = 0; dst < 10; dst++) {
    int Length = TSnap::GetShortPath(g, src, dst, true);
    printf("%d to %d,len=%d\n", src, dst, Length);
  }
}

template <typename Graph> void jaccard_sim(const Graph &g) {
  printf("Cannot support PUNGraph.");
}
template <> void jaccard_sim<PNGraph>(const PNGraph &g) {
  printf("jaccard_sim:\n");
  TNGraph::TNodeI NI1 = g->GetNI(100);
  for (int ind = 100; ind < 110; ind++) {
    TNGraph::TNodeI NI2 = g->GetNI(ind);
    double d = JaccardSim(NI1, NI2);
    printf("vid1=%d, vid2=%d, sim=%f\n", NI1.GetId(), NI2.GetId(), d);
  }
}

void split(const std::string &s, std::vector<std::string> &sv,
           const char delim = ' ') {
  sv.clear();
  std::istringstream iss(s);
  std::string temp;

  while (std::getline(iss, temp, delim)) {
    sv.emplace_back(std::move(temp));
  }
}

int main(int argc, char const *argv[]) {
  if (argc != 4) {
    printf("demo-test1 <filepath> [undirected|directed] "
           "[triangle|clust_cf|wcc|kcore|pagerank|bnc|cnc|shortest_"
           "path|jaccard_sim]\n");
    return 0;
  }

  // print argv
  for (int i = 0; i <= 3; i++) {
    printf("argv[%d]=%s\n", i, argv[i]);
  }

  PNGraph graph;
  PUNGraph und_graphd;
  TStr data_file = argv[1];
  std::string direction = argv[2];
  // std::string algo_str = argv[3];
  if (direction == "directed") {
    graph = TSnap::LoadEdgeList<PNGraph>(data_file, 0, 1);
    printf("Nodes:%d, Edges:%d\n", graph->GetNodes(), graph->GetEdges());

  } else {
    und_graphd = TSnap::LoadEdgeList<PUNGraph>(data_file, 0, 1);
    printf("Nodes:%d, Edges:%d\n", und_graphd->GetNodes(),
           und_graphd->GetEdges());
  }

  std::vector<std::string> algos;
  split(argv[3], algos, ',');

  for (const auto &algo : algos) {
    if (direction == "directed") {
      if (algo == "triangle") {
        CountTriangle(graph);
      } else if (algo == "clust_cf") {
        ClustCfGlobal(graph);
        ClustCfLocal(graph);
      } else if (algo == "wcc") {
        wcc(graph);
      } else if (algo == "kcore") {
        // kcore(graph);
      } else if (algo == "pagerank") {
        page_rank(graph);
      } else if (algo == "bnc") {
        bnc(graph);
      } else if (algo == "cnc") {
        cnc(graph);
      } else if (algo == "shortest_path") {
        shortest_path(graph);
      } else if (algo == "jaccard_sim") {
        jaccard_sim(graph);
      } else if (algo == "all") {
        CountTriangle(graph);
        ClustCfGlobal(graph);
        ClustCfLocal(graph);
        wcc(graph);
        // kcore(graph);
        page_rank(graph);
        bnc(graph);
        cnc(graph);
        shortest_path(graph);
        jaccard_sim(graph);
      } else {
        printf("Cannot support %s\n", algo.c_str());
      }
    } else {
      if (algo == "triangle") {
        CountTriangle(und_graphd);
      } else if (algo == "clust_cf") {
        ClustCfGlobal(und_graphd);
      } else if (algo == "wcc") {
        wcc(und_graphd);
      } else if (algo == "kcore") {
        kcore(und_graphd);
      } else if (algo == "pagerank") {
        page_rank(und_graphd);
      } else if (algo == "bnc") {
        bnc(und_graphd);
      } else if (algo == "cnc") {
        cnc(und_graphd);
      } else if (algo == "shortest_path") {
        shortest_path(und_graphd);
      } else if (algo == "jaccard_sim") {
        jaccard_sim(und_graphd);
      } else if (algo == "all") {
        CountTriangle(und_graphd);
        ClustCfGlobal(und_graphd);
        ClustCfLocal(und_graphd);
        wcc(und_graphd);
        // kcore(und_graphd);
        page_rank(und_graphd);
        bnc(und_graphd);
        cnc(und_graphd);
        shortest_path(und_graphd);
        jaccard_sim(und_graphd);
      } else {
        printf("Cannot support %s\n", algo.c_str());
      }
    }
  }

  return 0;
}
