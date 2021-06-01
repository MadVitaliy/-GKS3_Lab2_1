#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <conio.h>
#include <vector>
#include <algorithm>

#include <windows.h>
#include <fstream>
#include <clocale>
#include "libs/GantDiagram.h"
#include "libs/Matrix.h"
#include "libs/JohnsonProblem.h"

void swap(int *a, int i, int j) {
    int s = a[i];
    a[i] = a[j];
    a[j] = s;
}
bool NextSet(int *a, int n) {
    int j = n - 2;
    while (j != -1 && a[j] >= a[j + 1]) j--;
    if (j == -1)
        return false; // больше перестановок нет
    int k = n - 1;
    while (a[j] >= a[k]) k--;
    swap(a, j, k);
    int l = j + 1, r = n - 1; // сортируем оставшуюся часть последовательности
    while (l<r)
        swap(a, l++, r--);
    return true;
}
void Print(int *a, int n){
    static int num = 1; // номер перестановки
   // std::cout.width(3); // ширина поля вывода номера перестановки
   // std::cout << num++ << ": ";
    for (int i = 0; i < n; i++)
        std::cout << a[i] << " ";
   // std::cout << std::endl;
}


static void show_usage()
{
    std::cerr << "Use: " << "progname.exe " << " <option(s)>" << std::endl;
    std::cout << "Options:\n"
              << "-h,--help\tShow this help message\n\n"
              << "-i PATH\t\tset path to input data. Default \"./input.txt\"\n\n"
              << "-o PATH\t\tset path for output data. Default \"./output.txt\"\n"
              << "\t\tNOTICE: diagrams will be put in the console anyway \n\n"
              << "-c\t\toutput result in the console\n"
              << "\t\tDON\'T use with \'-o\' \n\n"
              << "\t\t25.03.2021\tby @mad_skipper"
              << std::endl;
}

static bool isReserved(std::string arg) {
    if(arg == "-h" ||
       arg == "--help" ||
       arg == "-i" ||
       arg == "-o" ||
       arg == "-c")
        return false;
    return true;
}

static void printNotThroughCout(std::string str) {
    for(char sym: str) {
        putchar(sym);
    }
    putchar('\n');
}

static int findIndexOfOrder(std::vector<std::vector<int>> orders, std::vector<int> order) {
    int result = 0;
    bool same = true;
    bool found = false;
    for (int i = 0; i < orders.size(); ++i) {
        same = true;
        for (int j = 0; j < orders[i].size(); ++j) {
            if(orders[i][j] !=  order[j])
                same = false;
            if(!same)
                break;
        }
        if(same)
            return i;
    }
    return -1;
}

int main(int argc, char **argv) {
    std::string inputPath = "./input.txt";
    std::string outputPath = "./output.txt";


    bool outInConcole = false;
    bool outInFile = false;


    if(argc > 1) {
        int i = 1;
        do {
            std::string arg = argv[i];
            if ((arg == "-h") || (arg == "--help")) {
                show_usage();
                return 0;
            }
            else if (arg == "-c"){
                if(outInFile) {
                    std::cerr << "You are not allowed to use -c and -o simultaneously\n";
                    return 3;
                }
                outInConcole = true;
            }
            else if (arg == "-o"){
                if(outInConcole) {
                    std::cerr << "You are not allowed to use -c and -o simultaneously\n";
                    return 3;
                }
                outInFile = true;

                if(i+1 != argc)
                    if(isReserved(argv[i+1]))
                        outputPath = argv[i+1];
            }
            else if (arg == "-i"){
                if(i+1 != argc)
                    if(isReserved(argv[i+1]))
                        outputPath = argv[i+1];
            }
            i++;
        }while( i < argc);
    } else {
        outInFile = true;
    }


    Matrix<int> processTime(1,1);
    unsigned int numOfDet = 0;
    unsigned int numOfMach = 0;
    int numOfCrit = 0;
    std::vector<int> critNums;
    std::map<int, float> weights;
    int compromiseCriterionType = 1;


    //read input data;


        std::string line;
        std::ifstream in(inputPath, std::ifstream::in);
        int tempVar;
        if(!in.is_open()) {
            std::cout << "Cannot open input file: "<< inputPath << std::endl;
            exit(1);
        }

        getline(in, line);
        in >> numOfDet;
        getline(in, line);
        getline(in, line);
        in >> numOfMach;
        getline(in, line);
        getline(in, line);
        processTime = Matrix<int>(numOfDet,numOfMach);

        for (int i = 0; i < numOfDet; ++i)
            for (int j = 0; j < numOfMach; ++j)
                in >> processTime[i][j];

        getline(in, line);
        getline(in, line);
        in >> numOfCrit;
        getline(in, line);
        getline(in, line);
        critNums.reserve(numOfCrit);
        for (int i = 0; i < numOfCrit; ++i) {
            in >> tempVar;
            critNums.push_back(tempVar);
        }
        getline(in, line);
        getline(in, line);
        for (int i = 0; i < numOfCrit; ++i) {
            in >> tempVar;
            weights[critNums[i]] = tempVar;
        }
        getline(in, line);
        getline(in, line);
        in >> compromiseCriterionType;

        in.close();

    if(numOfMach != 3) {
        std::cout << "This application do Johnson problem only with 3 machine";
        exit(4);
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    std::ofstream out;
    if(outInFile){
        out.open(outputPath);
        if(!out.is_open()){
            std::cout << "Cannot open output file: "<< outputPath << std::endl;
            exit(3);
        }
        std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
    }


    std::cout << std::fixed;
    std::cout << std::setprecision(3);


    ////////////////////////////////////////////////////////////////////////////////////////////
    int numOfOrders = 1;
    for (int j = 1; j <= numOfDet; ++j) {
        numOfOrders *= j;
    }

    Matrix<float> criterions(numOfOrders,numOfCrit);

    int n, *processOrder;
    n = numOfDet;
    processOrder = new int[n];
    for (int i = 0; i < n; i++)
        processOrder[i] = i ;

    std::vector<std::vector<int>> orders;

    orders.reserve(static_cast<unsigned int>(numOfOrders));
    {
        int i = 0;
        do {
            std::vector<int> temporder;
            for (int i = 0; i < n; i++)
                temporder.push_back(processOrder[i]);
            orders.push_back(temporder);

            JohnsonProblem jp2(processTime);
            jp2.setOrder(processOrder);
          /*  Print(processOrder,n);
            std::cout<<std::endl;*/

            jp2.createProcessPlan();
            jp2.findDowntime();
            jp2.findWaitingtime();
            jp2.findLocalReserveTime();
            std::map<int, float> crit = jp2.findCriterions(critNums);

            for (int j = 0; j < numOfCrit; ++j) {
                criterions[i][j] = crit[critNums[j]];
            }
            i++;
        } while (NextSet(processOrder, n));
    }

    std::map<int, float> extremeCriterions;
    for (int k = 0; k < numOfCrit; ++k) {
        int critNum = critNums[k];
        if(critNum == 21 | critNum == 22)
            extremeCriterions[critNum] = criterions.maxInColumn(k);
        else
            extremeCriterions[critNum] = criterions.minInColumn(k);
    }




    Matrix<float>compromiseCriterions(static_cast<unsigned int>(numOfOrders),1);

////////////////////////////////////////////////
    float tempCompCrit=0;
    if(compromiseCriterionType == 1) {
        for (int k = 0; k < numOfOrders; ++k) {
            int critNum = 0;
            tempCompCrit = 0;
            for (int j = 0; j < numOfCrit; ++j) {

                critNum = critNums[j];

                if(critNum == 21 | critNum == 22)
                    tempCompCrit +=weights[critNum]*(extremeCriterions[critNum] - criterions[k][j]);
                else
                    tempCompCrit +=weights[critNum]*(criterions[k][j] - extremeCriterions[critNum]);
            }
            compromiseCriterions[k][0] = tempCompCrit;
        }
    } else {
        for (int k = 0; k < numOfOrders; ++k) {
            tempCompCrit = 0;
            int critNum = 0;
            for (int j = 0; j < numOfCrit; ++j) {
                critNum = critNums[j];

                if(critNum == 21 | critNum == 22)
                    tempCompCrit +=weights[critNum]*(extremeCriterions[critNum] - criterions[k][j])/extremeCriterions[critNum];
                else
                    tempCompCrit +=weights[critNum]*(criterions[k][j] - extremeCriterions[critNum])/extremeCriterions[critNum];
            }
            compromiseCriterions[k][0] = tempCompCrit;
        }
    }
/////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////
    JohnsonProblem jp(processTime);

    jp.setOrder();
    jp.createProcessPlan();
    jp.findDowntime();
    jp.findWaitingtime();
    jp.findLocalReserveTime();
    jp.findCriterions(critNums);



    std::cout << "Doing Johnson problem:" << std::endl << std::endl;
    std::cout << jp.out(critNums) << std::endl;

    std::cout << "Compromise criterion: "
    << compromiseCriterions[findIndexOfOrder(orders,jp.getProcessOrder())][0]
    << std::endl;

    std::cout << "Gant\'s diagram for this order:" << std::endl;
    drawGantDiagram(jp.getStartTime(), jp.getEndTime());
    if(outInFile){
        std::cout << std::endl << "DIAGRAM 1" << std::endl << std::endl;
        printNotThroughCout("DIAGRAM 1");
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    std::cout << std::endl << "Now will be created diagram for reverse order:" << std::endl << std::endl;

    std::vector<int> reverseOrder = jp.getProcessOrder();
    std::reverse(reverseOrder.begin(), reverseOrder.end());

    {
        JohnsonProblem jp2(processTime);


        jp2.setOrder(reverseOrder);
        jp2.createProcessPlan();
        jp2.findDowntime();
        jp2.findWaitingtime();
        jp2.findLocalReserveTime();
        jp2.findCriterions(critNums);

        std::cout << jp2.outCriterions(critNums) << std::endl;

        std::cout << "Compromise criterion: "
                  << compromiseCriterions[findIndexOfOrder(orders,jp2.getProcessOrder())][0]
                  << std::endl;

        drawGantDiagram(jp2.getStartTime(), jp2.getEndTime());
        if(outInFile){
            std::cout << std::endl << "DIAGRAM 2" << std::endl << std::endl;
            printNotThroughCout("DIAGRAM 2");
        }
        std::cout << std::endl;
    }


    std::cout << std::endl << "Criterions for all orders:" << std::endl;

    for (int j = 0; j < numOfCrit; ++j){
        std::cout << critNums[j] << '\t';
    }
    std::cout << std::endl;
    for (int j = 0; j < numOfCrit*8; ++j)
        std::cout << '-';

    std::cout <<  std::endl;
    std::cout << std::setprecision(2);
    std::cout << criterions.out() << std::endl;

    std::cout << std::endl <<  "Extreme value of criterions:" <<  std::endl;
    for (int j = 0; j < numOfCrit; ++j)
        std::cout << critNums[j] << '\t';
    std::cout <<  std::endl;
    for (int j = 0; j < numOfCrit*8; ++j)
        std::cout << '_';
    std::cout <<  std::endl;
    for (int j = 0; j < numOfCrit; ++j)
        std::cout << extremeCriterions[critNums[j]] << '\t';
    std::cout <<  std::endl;

    ///////
/*    std::cout << "CompromiseCriterions: " << std::endl;
    for (int k = 0; k < numOfOrders; ++k) {
        std::cout << compromiseCriterions[k][0] << std::endl;
    }
    std::cout <<  std::endl;*/


    float bestCriterion = compromiseCriterions.min();
    int numOfBestOrder = compromiseCriterions.minIndex()[0][0];

    std::cout << std::endl<< "Minimal compromise criterions = " << bestCriterion << std::endl;
    std::cout << " , which matches ";
    for(int order : orders[numOfBestOrder])
        std::cout << order+1 << '-';
    std::cout << " order" <<  std::endl;



    JohnsonProblem jp2(processTime);

    jp2.setOrder(orders[numOfBestOrder]);
    jp2.createProcessPlan();
    jp2.findDowntime();
    jp2.findWaitingtime();
    jp2.findLocalReserveTime();
    jp2.findCriterions(critNums);

    std::cout << jp2.outProcessTime() <<  std::endl;
    std::cout << jp2.outOrder() << std::endl;
    std::cout << jp2.outTotalProcessTime() << std::endl;
    std::cout << jp2.outStartTime() << std::endl;
    std::cout << jp2.outEndTime() << std::endl;
    std::cout << jp2.outDowntime() << std::endl;
    std::cout << jp2.outWaitingTime() << std::endl;
    std::cout << jp2.outLocalReservTime() << std::endl;
    std::cout << jp2.outCriterions(critNums) << std::endl;

    std::cout << "Gant\'s diagram for this order:" << std::endl;
    drawGantDiagram(jp2.getStartTime(), jp2.getEndTime());
    if(outInFile){
        std::cout << std::endl << "DIAGRAM 3" << std::endl << std::endl;
        printNotThroughCout("DIAGRAM 3");
    }
    //std::ofstream fout;
    //fout.open("output.txt");
    std::cout << std::endl;




                  // Matrix




    //fout.close();
    delete [] processOrder;
    return 0;
}