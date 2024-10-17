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

    n = (nCities-1) + mItems + 1; 
}

/************************************************************************************
 Method: Decoder
 Description: Mapping a random-key solution in a problem solution
*************************************************************************************/
double Decoder(TSol &s) 
{
    

    return s.ofv;
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
