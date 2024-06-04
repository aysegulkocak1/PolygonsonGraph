#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    char u, v;
    int w;
} Edge;

typedef struct {
    int numEdges;
    Edge* edges;
} Graph;


void addEdge(Graph* g, char u, char v, int w);
char* getCanonicalCycle(char* cycle, int len);
int containsCycle(char** detectedCycles, int detectedCount, char* cycle);
int calculateCycleWeight(Graph* g, char* cycle, int len);
void DFS(Graph* g, int currentVertex, int parent, int* visited, int* path, int* pathIndex, int startVertex, char** detectedCycles, int* detectedCount, int* cycleLengths, int* cycleWeights);
void findAndPrintShapes(Graph* g, int vertices);



int main() {
    int N, M;
    printf("N degeri giriniz:\n");
    scanf("%d", &N);
    printf("M degeri giriniz:\n");
    scanf("%d", &M);

    Graph g;
    g.numEdges = 0;
    g.edges = (Edge*)malloc(M * sizeof(Edge));

    char u, v;
    int w;
    printf("dosya adini giriniz:\n");
    char fileName[100];
    scanf("%s", fileName);
    FILE* fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("Dosya acilamadi.\n");
        exit(1);
    }
	int i;
	printf("Graf:\n");
    for (i = 0; i < M; i++) {
        fscanf(fp, " %c %c %d", &u, &v, &w);
        printf("%c %c %d\n", u, v, w);
        addEdge(&g, u, v, w);
    }
	printf("\n");
    fclose(fp);

    findAndPrintShapes(&g, N);

    free(g.edges);
    return 0;
}

/*
@brief Grafa bir kenar ekler.
@param g Graf yapisi.
@param u Ýlk dugum.
@param v Ýkinci dugum.
@param w Kenarin agirligi.
*/
void addEdge(Graph* g, char u, char v, int w) {
    g->edges[g->numEdges].u = u;
    g->edges[g->numEdges].v = v;
    g->edges[g->numEdges].w = w;
    g->numEdges++;
}

/*
@brief Donguyu saat yununde veya tersinde olacak sekilde kanonik hale getirir
@param first cycle Donguyu temsil eden karakter dizisi.
@param  second len dongunun uzunlugu.
@return Kanonik hale getirilmis donguyu dondurur.
*/
char* getCanonicalCycle(char* cycle, int len) {
    int minIndex = 0;
    char minNode = cycle[0]; 

    // Baslangic dugumunu alfabetik olarak en kucuk olanla degistir
    int i;
	for (i = 1; i < len; i++) {
        if (cycle[i] < minNode) {
            minNode = cycle[i];
            minIndex = i;
        }
    }

    //  saat yonunde bir siralama yap
    char* result = (char*)malloc((len + 1) * sizeof(char));
    for (i = 0; i < len; i++) {
        result[i] = cycle[(minIndex + i) % len];
    }
    result[len] = '\0';
    return result;
}

/*
@brief Dongu saat yonunde ya da saat yonunun tersinde daha once bulunmus mu diye kontrol eder.
@param first detectedCycles Tespit edilen dongulerin listesi.
@param  second detectedCount Tespit edilen dongulerin sayisi.
@param  third cycle Kontrol edilecek dongu.
@return Dongu detectedCycle da varsa  1, yoksa 0 doner.
*/
int containsCycle(char** detectedCycles, int detectedCount, char* cycle) {
    int i;
    int len = strlen(cycle);
    char* reverseCycle = (char*)malloc((len + 1) * sizeof(char));
    reverseCycle[0] = cycle[0];
    for (i = 1; i < len; i++) {
        reverseCycle[i] = cycle[len - i];
    }
    reverseCycle[len] = '\0';

    for (i = 0; i < detectedCount; i++) {
        if (strcmp(detectedCycles[i], cycle) == 0 || strcmp(detectedCycles[i], reverseCycle) == 0) {
            free(reverseCycle);
            return 1;
        }
    }

    free(reverseCycle);
    return 0;
}

/*
@brief Dongunun  uzunlugunu hesaplar.
@param  first g Graf yapisi.
@param second cycle Donguyu temsil eden karakter dizisi.
@param third len Dongunun uzunluðu.
@return Dongunun uzunlugunu dodurur.
*/

int calculateCycleWeight(Graph* g, char* cycle, int len) {
    int weight = 0;
    int i,j;
	for (i = 0; i < len; i++) {
        char u = cycle[i];
        char v = cycle[(i + 1) % len];
        for (j = 0; j < g->numEdges; j++) {
            if ((g->edges[j].u == u && g->edges[j].v == v) ||
                (g->edges[j].u == v && g->edges[j].v == u)) {
                weight += g->edges[j].w;
            }
        }
    }
    return weight;
}

/*
@brief DFS algoritmasi ile graftaki donguleri bulur.
@param  first g Graf yapisi.
@param  second currentVertex o anki dugum.
@param third parent parent dugum.
@param fourth visited Ziyaret edilen dugumler dizisi.
@param fifth path DFS de gezilen dugumlerin tutuldugu dizi.
@param pathIndex path indeksi.
@param startVertex baslangic dugumu.
@param detectedCycles Tespit edilen donguler.
@param detectedCount Tespit edilen dongu sayisi.
@param cycleLengths Dongulerin uzunluklari.
@param cycleWeights Dongulerin agirliklari.
*/

void DFS(Graph* g, int currentVertex, int parent, int* visited, int* path, int* pathIndex, int startVertex, char** detectedCycles, int* detectedCount, int* cycleLengths, int* cycleWeights) {
    visited[currentVertex] = 1;
    path[(*pathIndex)++] = currentVertex;
    int i, j;
    for (i = 0; i < g->numEdges; i++) {
        if (g->edges[i].u == currentVertex + 'A' || g->edges[i].v == currentVertex + 'A') {
            int adjVertex;
            if (g->edges[i].u == currentVertex + 'A') {
                adjVertex = g->edges[i].v - 'A';
            } else {
                adjVertex = g->edges[i].u - 'A';
            }

            if (!visited[adjVertex]) {
                DFS(g, adjVertex, currentVertex, visited, path, pathIndex, startVertex, detectedCycles, detectedCount, cycleLengths, cycleWeights);
            } else if (adjVertex != parent && adjVertex == startVertex) {
                char* cycle = (char*)malloc((*pathIndex + 1) * sizeof(char));
                // dugumler char olarak temsil edildigi icin path dizisini char dizisine cevir 
                for (j = 0; j < *pathIndex; j++) {
                    cycle[j] = path[j] + 'A';
                }
                cycle[*pathIndex + 1] = '\0';

                char* canonicalCycle = getCanonicalCycle(cycle, *(pathIndex));

                if (!containsCycle(detectedCycles, *detectedCount, canonicalCycle)) {
                    detectedCycles[*detectedCount] = canonicalCycle;
                    cycleLengths[*detectedCount] = *pathIndex ; 
                    cycleWeights[*detectedCount] = calculateCycleWeight(g, canonicalCycle, *pathIndex ); 
                    (*detectedCount)++;
                } else {
                    free(canonicalCycle);
                }
                free(cycle);
            }
        }
    }

    (*pathIndex)--;
    visited[currentVertex] = 0; // Backtracking için sýfýrla
}


/*
@brief DFS'yi cagirarak graftaki sekilleri tespit eder ve yazdirir.
@param  first g Graf yapisi.
@param second vertices Dugum sayisi.
*/

void findAndPrintShapes(Graph* g, int vertices) {
    int* visited = (int*)calloc(vertices, sizeof(int));
    int* path = (int*)calloc(vertices, sizeof(int));
    int pathIndex = 0;

    char** detectedCycles = (char**)malloc(vertices * vertices * sizeof(char*));
    int* cycleLengths = (int*)malloc(vertices * vertices * sizeof(int));
    int* cycleWeights = (int*)malloc(vertices * vertices * sizeof(int));
    int detectedCount = 0;
    int i,j;
    for (i = 0; i < vertices; i++) {
        if (!visited[i]) {
            DFS(g, i, -1, visited, path, &pathIndex, i, detectedCycles, &detectedCount, cycleLengths, cycleWeights);
        }
    }

    printf("Sekil sayisi: %d\n", detectedCount);
    printf("\n");
    
    int polygonCount[100] = {0};
    for (i = 0; i < detectedCount; i++) {
        polygonCount[cycleLengths[i]]+=1;
    }
    
    for(i=0;i< (sizeof(polygonCount)/sizeof(polygonCount[0]));i++){
    	if(polygonCount[i] != 0){
		
    		printf("%d'gen sayisi:%d\n",i,polygonCount[i]);
    		
    	}
	}
	printf("\n");

    int maxCycleLength = 0;
    for (i = 0; i < detectedCount; i++) {
        if (cycleLengths[i] > maxCycleLength) {
            maxCycleLength = cycleLengths[i];
        }
    }
	int length;
    for (length = 3; length <= maxCycleLength; length++) {
        int count = 1;
        for (i = 0; i < detectedCount; i++) {
            if (cycleLengths[i] == length) {
                printf("%d. %d'gen: ", count, length);
                for (j = 0; j < length; j++) {
                    printf("%c ", detectedCycles[i][j]);
                }
                printf("%c Uzunluk: %d\n", detectedCycles[i][0], cycleWeights[i]);
                count++;
            }
        }
        if (count > 1) {
            printf("\n");
        }
    }

    for (i = 0; i < detectedCount; i++) {
        free(detectedCycles[i]);
    }
    free(detectedCycles);
    free(cycleLengths);
    free(cycleWeights);
    free(visited);
    free(path);
}


