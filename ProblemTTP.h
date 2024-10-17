// *******************************************************************
//      file with specific functions to solve the TTP
//                  (Travelling Thief Problem)
// *******************************************************************
#ifndef _PROBLEM_H
#define _PROBLEM_H

#include "../Main/Data.h"

// Global Variables
extern int n;                               // size of the vector solution

//---------------------- DEFINITION OF TYPES OF PROBLEM SPECIFIC --------------------

struct Item
{
    int Index;               // Item's index
    double Profit;           // Item's profit
    double Weight;           // Item's weight
    int City;                // Items's city
};

struct City
{
    int Index;                           // Index of the city
    std::vector<int> Items;              // Set of index items assigned to the city
    double PositionX;                    // Position X of the city
    double PositionY;                    // Position Y of the city
};

//------ DEFINITION OF GLOBAL CONSTANTS AND VARIABLES OF SPECIFIC PROBLEM  ----------

static std::vector <City> Cities;            // Set of cities in the instance
static std::vector <Item> Items;             // Set of items in the instance
static int nCities;                          // Number of cities in the instance
static int mItems;                           // Total number of items in the instance
static double Rate;                          // Renting rate
static double MinSpeed;                      // Minimal speed
static double MaxSpeed;                      // Maximal speed
static double MaxWeight;                     // Maximal knapsack's weight
static double MaxProfit;                     // Maximal knapsack's profit
static double bestPW;                        // Best relation between profit and weight
static double averagePW;                     // Average relation between profit and weight
static double medianPW;                      // Median relation between profit and weight


static std::vector <std::vector <double> > dist;	    // matrix with Euclidean distance
static std::vector <std::vector <int> > KPvector;	    // matrix with possible solutions of the Knapsak problem
static int KP;                                          // number of KP possibilities


//----------------------- IMPLEMENTATION OF FUNCTIONS  -------------------------------


/************************************************************************************
 Method: ReadData
 Description: read input data of the problem
*************************************************************************************/
void ReadData(char nameTable[])
{ 
    char name[200] = "../Instances/";
    strcat(name,nameTable);

    FILE *file = fopen(name, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", name);
        exit(1);
    }

    // => read data
    char line[256];

    fgets(line, 256, file); // skip problem name 
    fgets(line, 256, file); // skip kanpsack data

    fgets(line, sizeof(line), file); // read DIMENSION line
    sscanf(line, "%*s %d", &nCities);
    
    fgets(line, sizeof(line), file); // read NUMBER OF ITEMS line
    sscanf(line, "%*s %*s %*s %d", &mItems);

    fgets(line, sizeof(line), file); // read CAPACITY OF KNAPSACK line
    sscanf(line, "%*s %*s %*s %lf", &MaxWeight);

    fgets(line, sizeof(line), file); // read MIN SPEED line
    sscanf(line, "%*s %*s %lf", &MinSpeed);

    fgets(line, sizeof(line), file); // read MAX SPEED line
    sscanf(line, "%*s %*s %lf", &MaxSpeed);

    fgets(line, sizeof(line), file); // read RENTING RATIO line
    sscanf(line, "%*s %*s %lf", &Rate);

    fgets(line, 256, file); // skip edge
    
    // NODE_COORD_SECTION	(INDEX, X, Y): 
    fgets(line, 256, file); // skip node_coord

    Cities.clear();
    Cities.resize(nCities);
    for (int i = 0; i < nCities; i++) {
        fscanf(file, "%d %lf %lf", &Cities[i].Index, &Cities[i].PositionX, &Cities[i].PositionY);
    }

    // ITEMS SECTION	(INDEX, PROFIT, WEIGHT, ASSIGNED NODE NUMBER): 
    fgets(line, 256, file); // skip items section
    fgets(line, 256, file); // skip items section
    MaxProfit = 0;
    bestPW = -1;
    averagePW = 0;
    std::vector <double> ItemsPW;
    for (int i = 0; i < mItems; i++) {
        Item item;

        fscanf(file, "%d %lf %lf %d", &item.Index, &item.Profit, &item.Weight, &item.City);

        // correct the index of city
        item.City = item.City - 1;
        item.Index = item.Index - 1;

        Cities[item.City].Items.push_back(item.Index);
        MaxProfit += item.Profit;

        Items.push_back(item);

        if (item.Profit/item.Weight > bestPW){
            bestPW = item.Profit/item.Weight;
        }
        averagePW += item.Profit/item.Weight;
        ItemsPW.push_back(item.Profit/item.Weight);
    }
    averagePW = averagePW / mItems;

    std::sort(ItemsPW.begin(), ItemsPW.end());

    if (mItems % 2 == 0) {
        // Para tamanho par, a mediana é a média dos dois elementos centrais
        medianPW = (ItemsPW[mItems/2 - 1] + ItemsPW[mItems/2]) / 2.0;
    } else {
        // Para tamanho ímpar, a mediana é o elemento central
        medianPW = ItemsPW[mItems/2];
    }

    fclose(file);

    // print input data
    // if (debug){
    //     printf("\n\n");
    //     printf("\n%d \n%d \n%.0lf \n%.2lf \n%.2lf \n%.2lf \n", nCities, mItems, MaxWeight, MinSpeed, MaxSpeed, Rate);
    //     for (int i = 0; i < nCities; i++) {
    //         printf("\n%d \t %.0lf \t %.0lf", Cities[i].Index, Cities[i].PositionX, Cities[i].PositionY);
    //         for (unsigned int j=0; j<Cities[i].Items.size(); j++)
    //         {
    //             printf("\t[ %d \t %.0lf \t %.0lf ] ", Items[Cities[i].Items[j]].Index, Items[Cities[i].Items[j]].Profit, Items[Cities[i].Items[j]].Weight);
    //         }
    //     }
    //     getchar();
    // }

    // ***** gerar os padroes possiveis considerando o numero de itens por cidade ****
    KP = std::ceil((double)mItems/nCities);
    // printf("\nItens: %d, cities: %d, KP: %d", mItems, nCities, KP);

    // Número total de combinações possíveis para KP bits é 2^k
    int totalCombinations = std::pow(2, KP);

    KPvector.clear();
    KPvector.resize(totalCombinations, std::vector<int>(KP));

    // Iterar sobre todas as combinações
    for (int i = 0; i < totalCombinations; ++i) {
        // Converter o número atual para um vetor binário
        for (int j = 0; j < KP; ++j) {
            if (i & (1 << j)) {
                KPvector[i][KP - j - 1] = 1;  // Definir bit na posição correta
            }
        }

        // Imprimir o vetor binário
        // printf("\n");
        // for (unsigned int j=0; j<KPvector[i].size(); j++) {
        //     printf("%d ", KPvector[i][j]);
        // }
    }
    

    // calculate the euclidean distance
    dist.clear();
    dist.resize(nCities, std::vector<double>(nCities));

    for (int i=0; i<nCities; i++)
    {
    	for (int j=i; j<nCities; j++)
    	{
            if (i == j)
                dist[i][j] = 0.0;
            else
                dist[i][j] = dist[j][i] = ceil (sqrt( (Cities[j].PositionX - Cities[i].PositionX) * (Cities[j].PositionX - Cities[i].PositionX) +
    										          (Cities[j].PositionY - Cities[i].PositionY) * (Cities[j].PositionY - Cities[i].PositionY) ) );
    	}
    }

    // printf("\nDistancias:\n");
    // for (int i=0; i<nCities; i++)
    // {
    // 	for (int j=0; j<nCities; j++)
    // 	{
    // 		printf("%.2lf\t", dist[i][j]);
    // 	}
    //     printf("\n");
    // }
    // getchar();

    n = (nCities-1) + mItems + 1; // decoder02
    // n = (nCities-1)+(nCities-1); //decoder01
}

void TwoOPT(std::vector <int> &route) 
{
    int t = nCities, // use a circular list (preciso voltar o 0 para a primeira posicao)
        i = 0,
        j = 0,
        Mi1= 0,
        Mj = 0;

    double foOpt = 0;

    int melhorou = 1;
    while (melhorou)
    {
        melhorou = 0;
    
        if (t > 4)
        {
            for (i=0; i < t; i++)
            {
                j = i+2;
                while (((j+1)%t) != i)
                {
                    int vi  = route[i];
                    int vi1 = route[(i+1)%t];
                    int vj  = route[j%t];
                    int vj1 = route[(j+1)%t];

                    foOpt = - dist[vi][vi1]
                            - dist[vj][vj1]
                            + dist[vi][vj]
                            + dist[vi1][vj1];

                    if (foOpt < 0)
                    {
                        melhorou = 0;

                        // first improvement strategy
                        Mi1 = (i+1)%t;
                        Mj  = j%t;

                        int inicio = Mi1,
                        fim = Mj;

                        int tam, p1, p2, aux;

                        if(inicio > fim)
                            tam = t - inicio + fim + 1;
                        else
                            tam = fim - inicio + 1;

                        p1=inicio;
                        p2=fim;

                        for(int k=0; k < tam/2; k++)
                        {
                            aux = route[p1%t];
                            route[p1%t] = route[p2%t];
                            route[p2%t] = aux;

                            p1 = (p1==t-1)?0:p1+1;
                            p2 = (p2 == 0)?t-1:p2-1;
                        }
                    }
                    j++;
                }//while
            }//for
        }//if t > 4
    }
}

/************************************************************************************
 Method: Encode
 Description: encode a solution s into a random key vector
*************************************************************************************/
void Encode(TSol &s, std::vector <int> &route)
{
    // encode solution sol into random key
    double delta = 1.0 / (nCities);         // Size of each chunk
    double X_bar = delta / 2.0;     // Center of the first chunk

    // Loop through the input sequence π
    for (int i = 1; i < nCities; i++) {
        // Assign center values to appropriate positions in s
        s.rk[route[i]-1] = X_bar;

        // Move to the center of the next chunk
        X_bar += delta;
    }

    // Randomize the protocol by adding uniform noise to each element in s
    for (int i = 0; i < nCities-1; i++) {
        double delta_i = (double)rand() / RAND_MAX * delta - delta / 2.0;
        s.rk[i] += delta_i;
    }
}

double Decoder01(TSol &s) 
{
    // create a TSP solution
    std::vector <int> C(nCities-1);  
    for (int j = 0; j < nCities-1; j++){ 
		    C[j] = j+1;
    }

    // sort the problem vector with nCities based on the values in the rk vector
    std::sort(C.begin(), C.begin() + nCities - 1, [&s](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });

    // insert the 3 first nodes in the route
    std::vector <int> route;
    route.resize(3);
    route[0] = 0;
    route[1] = C[0];
    route[2] = C[1];

    // construct a solution with cheapest insertion
    int sizeAtual = 3;
    double costRoute = 0.0;
    for (int i = 2; i<nCities-1; i++)
    {
        // find the cheapest position to insert the i-th point of C
        int bestPosition = 0;
        float costBest = INFINITY;
        float costInsertion = 0;
        for (int j = 1; j<=sizeAtual; j++)
        {
            if (j == sizeAtual)
            {
                // cost to insert between j-1 and 0
                costInsertion = dist[route[j-1]][C[i]] + dist[C[i]][route[0]] - dist[route[j-1]][route[0]];
                if (costInsertion < costBest)
                {
                    costBest = costInsertion;
                    bestPosition = j;
                }
            }
            else
            {
                // cost to insert between j-1 and j
                costInsertion = dist[route[j-1]][C[i]] + dist[C[i]][route[j]] - dist[route[j-1]][route[j]];
                if (costInsertion < costBest)
                {
                    costBest = costInsertion;
                    bestPosition = j;
                }
            }
        }

        // insert the i-th point in the cheapest position
        if (bestPosition == sizeAtual)
            route.push_back(C[i]);
        else
            route.insert(route.begin()+bestPosition,C[i]);   

        costRoute += costBest;

        // increase the number of points in the route
        sizeAtual++;
    }
    
    // printf("\n[ ");
    // for (unsigned int j = 0; j < route.size(); j++){ printf("%d ",route[j]);}
    // printf(" ]");
    // getchar();

    // create a KP solution
    std::vector<int> packCity(nCities-1,0);                     // vector with the packing planing index in each city
    std::vector<int> packing(mItems,0);                         // vector with the packing planing
    double CollectedWeight = 0;                                 // Collected weight 

    // insert the items of the last cities without exceed the capacity
    for (int i=nCities-1; i>0; i--)
    {
        // i-th city in the route
        int city = route[i];
        
        // assign the KP combination to each city
        packCity[city-1] = floor(s.rk[nCities+city-2] * (int)KPvector.size());

        // create the packing
        for (unsigned int j=0; j<KPvector[packCity[city-1]].size(); j++)
        {
            int id = Cities[city].Items[j];

            // if item is selected
            if (KPvector[packCity[city-1]][j] == 1 || (Items[id].Profit/Items[id].Weight > averagePW))
            // if (KPvector[packCity[city-1]][j] == 1 || (Items[id].Profit/Items[id].Weight > 0.3*bestPW))
            {
                if (Items[id].Weight + CollectedWeight <= MaxWeight)
                {
                    // current weight
                    CollectedWeight += Items[id].Weight;
                    packing[id] = 1;
                }
            }
        }
    }

    // percorrer as cidades da ultima para a primeira e adicionar o item se ele tiver entre os melhores

    // Calculate objective function value
    double CurrentCollectedWeight = 0;                          // Collected weight while traveling along Tour
    double SpeedCoef = (MaxSpeed - MinSpeed) / MaxWeight;       // constant
    double velocity = 0;

    double fp = 0;
    double ft = 0;

    // foreach (City in Tour)
    for (int i=0; i<nCities; i++)
    {
        int city = route[i];
        if (city > 0)
        {
            // foreach item in city
            for (unsigned int j = 0; j<Cities[city].Items.size(); j++)
            {
                int item = Cities[city].Items[j];
                if (packing[item] == 1)
                {
                    // current weight
                    CurrentCollectedWeight += Items[item].Weight;

                    // fp: final profit gained form the picked items
                    fp += Items[item].Profit;
                }
            }
        }

        // calculate the current velocity
        if (CurrentCollectedWeight > MaxWeight)
            velocity = MinSpeed;
        else
            velocity = (MaxSpeed - CurrentCollectedWeight * SpeedCoef);

        // ft: the time takes to finish the tour (including changes of the speed)
        if (i == nCities-1)
            ft += (dist[city][route[0]]) / velocity;
        else
            ft += (dist[city][route[i+1]]) / velocity;
    }

    // objective function value
    s.ofv =  ((fp - Rate * ft) * -1);

    // print the solution in the screen
    if (debug && print)
    {
        double totalProfitItems = 0;
        double totalWeightItems = 0;

        // route
        printf("\n\n[");
        for (unsigned int i = 0; i < route.size(); i++){
            if (i < route.size()-1)
                printf("%d, ", route[i]);
            else
                printf("%d", route[i]);
        }
        printf("]");

        // selected items
        printf("\n[");
        for (unsigned int i = 0; i < Items.size(); i++){
            if (packing[i] == 1){
                printf("%d ", Items[i].Index);

                totalProfitItems += Items[i].Profit;
                totalWeightItems += Items[i].Weight;
            }
        }
        printf("]\n\n");
        printf("Cities: %d \nProfit: %lf \nWeigth: %lf \nCapacity: %lf \nFT: %lf \nRatio*FT: %lf \nOFV: %.0lf (%lf)\n\n", 
                (int)route.size(), totalProfitItems, totalWeightItems, MaxWeight, ft, Rate*ft, totalProfitItems - Rate*ft, totalProfitItems - Rate*ft);
    }

    // print the solution in a file
    if (!debug && print)
    {
        // route
        for (int i=0; i<nCities; i++){
            fprintf(arqSol,"%d ", route[i]);
        }
        // packing
        for (int i=0; i<nCities; i++){
            if (packing[i] == 1)
                fprintf(arqSol,"%d ", Items[i].Index);
        }
    }


    return s.ofv;
}

double Decoder02(TSol &s) 
{
    // create a TSP solution
    std::vector <int> C(nCities-1);  
    for (int j = 0; j < nCities-1; j++){ 
		    C[j] = j+1;
    }

    // sort the problem vector with nCities based on the values in the rk vector
    std::sort(C.begin(), C.begin() + nCities-1, [&s](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });

    // insert the 3 first nodes in the route
    std::vector <int> route;
    route.resize(3);
    route[0] = 0;
    route[1] = C[0];
    route[2] = C[1];

    // // construct a solution with cheapest insertion
    int sizeAtual = 3;
    for (int i = 2; i<nCities-1; i++)
    {
        // find the cheapest position to insert the i-th point of C
        int bestPosition = 0;
        float costBest = INFINITY;
        float costInsertion = 0;
        for (int j = 1; j<=sizeAtual; j++)
        {
            if (j == sizeAtual)
            {
                // cost to insert between j and 0
                costInsertion = dist[route[j-1]][C[i]] + dist[C[i]][route[0]] - dist[route[j-1]][route[0]];
                if (costInsertion < costBest)
                {
                    costBest = costInsertion;
                    bestPosition = j;
                }
            }
            else
            {
                // cost to insert between j-1 and j
                costInsertion = dist[route[j-1]][C[i]] + dist[C[i]][route[j]] - dist[route[j-1]][route[j]];
                if (costInsertion < costBest)
                {
                    costBest = costInsertion;
                    bestPosition = j;
                }
            }
        }

        // insert the i-th point in the cheapest position
        if (bestPosition == sizeAtual)
            route.push_back(C[i]);
        else
            route.insert(route.begin()+bestPosition,C[i]);   

        // increase the number of points in the route
        sizeAtual++;
    }

    // printf("\n");
    // for (int j = 0; j < route.size(); j++){ printf("%d \n",route[j]);}
    // getchar();


    // create a KP solution
    double CollectedWeight = 0;                                 // Collected weight 

    // insert the items of the last cities without exceed the capacity
    for (int i=nCities-1; i>0; i--)
    {
        // i-th city in the route
        int city = route[i];

        // create the packing
        for (unsigned int j=0; j<Cities[city].Items.size(); j++)
        {
            // if (item.Selected)
            int id = Cities[city].Items[j];
            if (s.rk[nCities+id] >= 0.5)
            {
                if (Items[id].Weight + CollectedWeight <= MaxWeight)
                {
                    // current weight
                    CollectedWeight += Items[id].Weight;
                }
                else
                {
                    // correct the random-key solution
                    s.rk[nCities+id] = 0;
                }
            }
        }
    }

    // Calculate objective function value
    double CurrentCollectedWeight = 0;                          // Collected weight while traveling along Tour
    double SpeedCoef = (MaxSpeed - MinSpeed) / MaxWeight;       // constant
    double velocity = 0;

    double fp = 0;
    double ft = 0;

    // foreach (City in Tour)
    for (int i=0; i<nCities; i++)
    {
        int city = route[i];
        if (city > 0)
        {
            // foreach item in city
            for (unsigned int j = 0; j<Cities[city].Items.size(); j++)
            {
                int id = Cities[city].Items[j];
                if (s.rk[nCities+id] >= 0.5)
                {
                    // current weight
                    CurrentCollectedWeight += Items[id].Weight;

                    // fp: final profit gained form the picked items
                    fp += Items[id].Profit;
                }
            }
        }

        // calculate the current velocity
        if (CurrentCollectedWeight > MaxWeight)
            velocity = MinSpeed;
        else
            velocity = (MaxSpeed - CurrentCollectedWeight * SpeedCoef);

        // ft: the time takes to finish the tour (including changes of the speed)
        if (i == nCities-1)
            ft += (dist[city][route[0]]) / velocity;
        else
            ft += (dist[city][route[i+1]]) / velocity;
    }

    // objective function value
    s.ofv =  ((fp - Rate * ft) * -1);

    // print the solution in the screen
    if (debug && print)
    {
        double totalProfitItems = 0;
        double totalWeightItems = 0;

        // route
        printf("\n\n[");
        for (unsigned int i = 0; i < route.size(); i++){
            if (i < route.size()-1)
                printf("%d, ", route[i]);
            else
                printf("%d", route[i]);
        }
        printf("]");

        // selected items
        printf("\n[");
        for (unsigned int i = 0; i < Items.size(); i++){
            if (s.rk[i+nCities] >= 0.5){
                printf("%d ", Items[i].Index);

                totalProfitItems += Items[i].Profit;
                totalWeightItems += Items[i].Weight;
            }
        }
        printf("]\n\n");
        printf("Cities: %d \nProfit: %lf \nWeigth: %lf \nCapacity: %lf \nFT: %lf \nRatio*FT: %lf \nOFV: %lf\n\n", 
                (int)route.size(), totalProfitItems, totalWeightItems, MaxWeight, ft, Rate*ft, totalProfitItems - Rate*ft);
    }

    // print the solution in a file
    if (!debug && print)
    {
        for (int i=0; i<nCities; i++){
            fprintf(arqSol,"%d ", route[i]);
        }
        for (int i=0; i<nCities; i++){
            if (s.rk[i+nCities] >= 0.5)
                fprintf(arqSol,"%d ", 1);
            else
                fprintf(arqSol,"%d ", 0);
        }
    }

    return s.ofv;
}

double Decoder03(TSol &s) 
{
    // create a TSP solution
    std::vector <int> route(nCities);  
    for (int j = 0; j < nCities; j++){ 
		    route[j] = j;
    }

    // sort the problem vector with nCities based on the values in the rk vector
    std::sort(route.begin()+1, route.begin() + nCities, [&s](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });

    TwoOPT(route);

    // Encontrar a posição do valor 1 no vetor
    int pos = -1;
    for (int i = 0; i < nCities; i++) {
        if (route[i] == 0) {
            pos = i;
            break;
        }
    }

    if (pos > 0){
        // printf("\n Route size = %d (%d)", (int)route.size(), pos);

        // Cria um vetor temporário para armazenar a nova ordem
        std::vector <int> temp(nCities);
        int j = 0;

        // Copia os elementos a partir da posição do 1 até o final
        for (int i = pos; i < nCities; i++) {
            temp[j++] = route[i];
        }

        // Copia os elementos do início até a posição anterior ao 1
        for (int i = 0; i < pos; i++) {
            temp[j++] = route[i];
        }

        // Transfere o conteúdo do vetor temporário de volta para o original
        for (int i = 0; i < nCities; i++) {
            route[i] = temp[i];
        }
    }

    Encode(s,route);

    // printf("\n");
    // for (int j = 0; j < route.size(); j++){ printf("%d \n",route[j]);}
    // getchar();


    // create a KP solution
    double CollectedWeight = 0;                                 // Collected weight 

    // insert the items of the last cities without exceed the capacity
    for (int i=nCities-1; i>0; i--)
    {
        // i-th city in the route
        int city = route[i];

        // create the packing
        for (unsigned int j=0; j<Cities[city].Items.size(); j++)
        {
            // if (item.Selected)
            int id = Cities[city].Items[j];
            if (s.rk[nCities+id] >= 0.5)
            {
                if (Items[id].Weight + CollectedWeight <= MaxWeight)
                {
                    // current weight
                    CollectedWeight += Items[id].Weight;
                }
                else
                {
                    // correct the random-key solution
                    s.rk[nCities+id] = 0;
                }
            }
            // item nao foi selecionado mas esta entre os melhores
            else
            {
                if ( (Items[id].Weight + CollectedWeight <= MaxWeight) && (Items[id].Profit/Items[id].Weight > averagePW) )
                {
                    // current weight
                    CollectedWeight += Items[id].Weight;
                    s.rk[nCities+id] = 0.99;
                }
            }
        }
    }

    // Calculate objective function value
    double CurrentCollectedWeight = 0;                          // Collected weight while traveling along Tour
    double SpeedCoef = (MaxSpeed - MinSpeed) / MaxWeight;       // constant
    double velocity = 0;

    double fp = 0;
    double ft = 0;

    // foreach (City in Tour)
    for (int i=0; i<nCities; i++)
    {
        int city = route[i];
        if (city > 0)
        {
            // foreach item in city
            for (unsigned int j = 0; j<Cities[city].Items.size(); j++)
            {
                int id = Cities[city].Items[j];
                if (s.rk[nCities+id] >= 0.5)
                {
                    // current weight
                    CurrentCollectedWeight += Items[id].Weight;

                    // fp: final profit gained form the picked items
                    fp += Items[id].Profit;
                }
            }
        }

        // calculate the current velocity
        if (CurrentCollectedWeight > MaxWeight)
            velocity = MinSpeed;
        else
            velocity = (MaxSpeed - CurrentCollectedWeight * SpeedCoef);

        // ft: the time takes to finish the tour (including changes of the speed)
        if (i == nCities-1)
            ft += (dist[city][route[0]]) / velocity;
        else
            ft += (dist[city][route[i+1]]) / velocity;
    }

    // objective function value
    s.ofv =  ((fp - Rate * ft) * -1);

    // print the solution in the screen
    if (debug && print)
    {
        double totalProfitItems = 0;
        double totalWeightItems = 0;

        // route
        printf("\n\n[");
        for (unsigned int i = 0; i < route.size(); i++){
            if (i < route.size()-1)
                printf("%d, ", route[i]);
            else
                printf("%d", route[i]);
        }
        printf("]");

        // selected items
        printf("\n[");
        for (unsigned int i = 0; i < Items.size(); i++){
            if (s.rk[i+nCities] >= 0.5){
                printf("%d ", Items[i].Index);

                totalProfitItems += Items[i].Profit;
                totalWeightItems += Items[i].Weight;
            }
        }
        printf("]\n\n");
        printf("Cities: %d \nProfit: %lf \nWeigth: %lf \nCapacity: %lf \nFT: %lf \nRatio*FT: %lf \nOFV: %lf\n\n", 
                (int)route.size(), totalProfitItems, totalWeightItems, MaxWeight, ft, Rate*ft, totalProfitItems - Rate*ft);
    }

    // print the solution in a file
    if (!debug && print)
    {
        for (int i=0; i<nCities; i++){
            fprintf(arqSol,"%d ", route[i]);
        }
        for (int i=0; i<nCities; i++){
            if (s.rk[i+nCities] >= 0.5)
                fprintf(arqSol,"%d ", 1);
            else
                fprintf(arqSol,"%d ", 0);
        }
    }

    return s.ofv;
}

double Decoder(TSol &s) 
{
    // create a TSP solution
    std::vector <int> C(nCities-1);  
    for (int j = 0; j < nCities-1; j++){ 
		    C[j] = j+1;
    }

    // sort the problem vector with nCities based on the values in the rk vector
    std::sort(C.begin(), C.begin() + nCities - 1, [&s](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });

    // insert the 3 first nodes in the route
    std::vector <int> route;
    route.resize(3);
    route[0] = 0;
    route[1] = C[0];
    route[2] = C[1];

    // construct a solution with cheapest insertion
    int sizeAtual = 3;
    double costRoute = 0.0;
    for (int i = 2; i<nCities-1; i++) // candidates
    {
        // find the cheapest position to insert the i-th point of C
        int bestPosition = 0;
        float costBest = INFINITY;
        float costInsertion = 0;
        for (int j = 1; j<=sizeAtual; j++)
        {
            if (j == sizeAtual)
            {
                // cost to insert between j-1 and 0
                costInsertion = dist[route[j-1]][C[i]] + dist[C[i]][route[0]] - dist[route[j-1]][route[0]];
                if (costInsertion < costBest)
                {
                    costBest = costInsertion;
                    bestPosition = j;
                }
            }
            else
            {
                // cost to insert between j-1 and j
                costInsertion = dist[route[j-1]][C[i]] + dist[C[i]][route[j]] - dist[route[j-1]][route[j]];
                if (costInsertion < costBest)
                {
                    costBest = costInsertion;
                    bestPosition = j;
                }
            }
        }

        // insert the i-th point in the cheapest position
        if (bestPosition == sizeAtual)
            route.push_back(C[i]);
        else
            route.insert(route.begin()+bestPosition,C[i]);   

        costRoute += costBest;

        // increase the number of points in the route
        sizeAtual++;
    }

    //aplicar um 2-Opt aqui ????
    // TwoOPT(route);

    // // Encontrar a posição do ponto 0 no vetor
    // int pos = -1;
    // for (int i = 0; i < nCities; i++) {
    //     if (route[i] == 0) {
    //         pos = i;
    //         break;
    //     }
    // }

    // if (pos > 0){
    //     // printf("\n Route size = %d (%d)", (int)route.size(), pos);

    //     // Cria um vetor temporário para armazenar a nova ordem
    //     std::vector <int> temp(nCities);
    //     int j = 0;

    //     // Copia os elementos a partir da posição do 1 até o final
    //     for (int i = pos; i < nCities; i++) {
    //         temp[j++] = route[i];
    //     }

    //     // Copia os elementos do início até a posição anterior ao 1
    //     for (int i = 0; i < pos; i++) {
    //         temp[j++] = route[i];
    //     }

    //     // Transfere o conteúdo do vetor temporário de volta para o original
    //     for (int i = 0; i < nCities; i++) {
    //         route[i] = temp[i];
    //     }
    // }
    
    // create a KP solution
    double CollectedWeight = 0;                                 // Collected weight 

    // insert the items of the last cities without exceed the capacity
    for (int i=nCities-1; i>=1; i--)
    {
        // i-th city in the route
        int city = route[i];

        // create the packing
        for (unsigned int j=0; j<Cities[city].Items.size(); j++)
        {
            // if (item.Selected) Cities[i].Items[j]
            int id = Cities[city].Items[j];
            if (s.rk[nCities+id-1] >= 0.5)
            {
                if (Items[id].Weight + CollectedWeight <= MaxWeight)
                {
                    // current weight
                    CollectedWeight += Items[id].Weight;
                }
                else
                {
                    // correct the random-key solution
                    s.rk[nCities+id-1] = 0.0;
                }
            }
            // item nao foi selecionado mas esta entre os melhores
            else
            {
                // if ( (Items[id].Weight + CollectedWeight <= MaxWeight) && (Items[id].Profit/Items[id].Weight > 0.15*averagePW) )
                // if ( (Items[id].Weight + CollectedWeight <= MaxWeight) && (Items[id].Profit/Items[id].Weight > 0.5*bestPW) )
                if ( (Items[id].Weight + CollectedWeight <= MaxWeight) && ((Items[id].Profit/Items[id].Weight) > ((1.0 + s.rk[n-1])*medianPW)) )
                {
                    // current weight
                    CollectedWeight += Items[id].Weight;
                    s.rk[nCities+id-1] = 0.99;
                }
            }
        }
    }

    // Calculate objective function value
    double CurrentCollectedWeight = 0;                          // Collected weight while traveling along Tour
    double SpeedCoef = (MaxSpeed - MinSpeed) / MaxWeight;       // constant
    double velocity = 0;

    double fp = 0;
    double ft = 0;

    // foreach (City in Tour)
    for (int i=0; i<nCities; i++)
    {
        int city = route[i];
        if (city > 0)
        {
            // foreach item in city
            for (unsigned int j = 0; j<Cities[city].Items.size(); j++)
            {
                int id = Cities[city].Items[j];
                if (s.rk[nCities+id-1] >= 0.5)
                {
                    // current weight
                    CurrentCollectedWeight += Items[id].Weight;

                    // fp: final profit gained form the picked items
                    fp += Items[id].Profit;
                }
            }
        }

        // calculate the current velocity
        if (CurrentCollectedWeight > MaxWeight)
            velocity = MinSpeed;
        else
            velocity = (MaxSpeed - CurrentCollectedWeight * SpeedCoef);

        // ft: the time takes to finish the tour (including changes of the speed)
        if (i == nCities-1)
            ft += (dist[city][route[0]]) / velocity;
        else
            ft += (dist[city][route[i+1]]) / velocity;
    }

    // objective function value
    s.ofv =  ((fp - Rate * ft) * -1);

    // print the solution in the screen
    if (debug && print)
    {
        double totalProfitItems = 0;
        double totalWeightItems = 0;

        // route
        printf("\n\n[");
        for (unsigned int i = 0; i < route.size(); i++){
            if (i < route.size()-1)
                printf("%d, ", route[i]);
            else
                printf("%d", route[i]);
        }
        printf("]");

        // selected items
        printf("\n[");
        for (unsigned int i = 0; i < Items.size(); i++){
            if (s.rk[i+nCities] >= 0.5){
                printf("%d ", Items[i].Index+1);

                totalProfitItems += Items[i].Profit;
                totalWeightItems += Items[i].Weight;
            }
        }
        printf("]\n\n");
        printf("Cities: %d \nProfit: %lf \nWeigth: %lf \nCapacity: %lf \nCollected Weight: %lf \nFT: %lf \nRatio*FT: %lf \nOFV: %lf\n\n", 
                (int)route.size(), totalProfitItems, totalWeightItems, MaxWeight, CurrentCollectedWeight, ft, Rate*ft, totalProfitItems - Rate*ft);
    }

    // print the solution in a file
    if (!debug && print)
    {
        for (int i=0; i<nCities; i++){
            fprintf(arqSol,"%d ", route[i]);
        }
        for (int i=0; i<nCities; i++){
            if (s.rk[i+nCities] >= 0.5)
                fprintf(arqSol,"%d ", 1);
            else
                fprintf(arqSol,"%d ", 0);
        }
    }


    return s.ofv;
}


void TwoOPT(TSol &s) 
{
    TSol sBest = s;

    // create a TSP solution
    std::vector <int> route(nCities);  
    for (int j = 0; j < nCities; j++){ 
		    route[j] = j;
    }

    // sort the problem vector with nCities based on the values in the rk vector
    std::sort(route.begin()+1, route.begin() + nCities, [&s](int i1, int i2) {
        return s.rk[i1] < s.rk[i2];
    });

    int t = nCities, // use a circular list (preciso voltar o 0 para a primeira posicao)
        i = 0,
        j = 0,
        Mi1= 0,
        Mj = 0;

    double foOpt = 0;

    int melhorou = 1;
    while (melhorou)
    {
        melhorou = 0;
    
        if (t > 4)
        {
            for (i=0; i < t; i++)
            {
                j = i+2;
                while (((j+1)%t) != i)
                {
                    int vi  = route[i];
                    int vi1 = route[(i+1)%t];
                    int vj  = route[j%t];
                    int vj1 = route[(j+1)%t];

                    foOpt = - dist[vi][vi1]
                            - dist[vj][vj1]
                            + dist[vi][vj]
                            + dist[vi1][vj1];

                    if (foOpt < 0)
                    {
                        melhorou = 0;

                        // first improvement strategy
                        Mi1 = (i+1)%t;
                        Mj  = j%t;

                        int inicio = Mi1,
                        fim = Mj;

                        int tam, p1, p2, aux;

                        if(inicio > fim)
                            tam = t - inicio + fim + 1;
                        else
                            tam = fim - inicio + 1;

                        p1=inicio;
                        p2=fim;

                        for(int k=0; k < tam/2; k++)
                        {
                            aux = route[p1%t];
                            route[p1%t] = route[p2%t];
                            route[p2%t] = aux;

                            p1 = (p1==t-1)?0:p1+1;
                            p2 = (p2 == 0)?t-1:p2-1;
                        }
                    }
                    j++;
                }//while
            }//for
        }//if t > 4
    }

    // Encontrar a posição do valor 1 no vetor
    int pos = -1;
    for (int i = 0; i < nCities; i++) {
        if (route[i] == 0) {
            pos = i;
            break;
        }
    }

    if (pos > 0){
        // printf("\n Route size = %d (%d)", (int)route.size(), pos);

        // Cria um vetor temporário para armazenar a nova ordem
        std::vector <int> temp(nCities);
        int j = 0;

        // Copia os elementos a partir da posição do 1 até o final
        for (int i = pos; i < nCities; i++) {
            temp[j++] = route[i];
        }

        // Copia os elementos do início até a posição anterior ao 1
        for (int i = 0; i < pos; i++) {
            temp[j++] = route[i];
        }

        // Transfere o conteúdo do vetor temporário de volta para o original
        for (int i = 0; i < nCities; i++) {
            route[i] = temp[i];
        }
    }

    Encode(s,route);
    Decoder(s);

    if (s.ofv < sBest.ofv){
        sBest = s;
    }
    else{
        s = sBest;
    }
}

/************************************************************************************
 Method: FreeMemoryProblem
 Description: Free local memory allocate by Problem
*************************************************************************************/
void FreeMemoryProblem()
{
    //specific problem
    dist.clear();
    KPvector.clear();
    Cities.clear();
    Items.clear();
}

#endif