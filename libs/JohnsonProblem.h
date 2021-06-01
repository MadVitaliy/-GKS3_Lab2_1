//
// Created by Vitalik on 23.03.2021.
//

#ifndef LABA2_JOHNSONPROBLEM_H
#define LABA2_JOHNSONPROBLEM_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>

#include "Matrix.h"


template <typename A, typename B>
void zip(
        const std::vector<A> &a,
        const std::vector<B> &b,
        std::vector<std::pair<A,B>> &zipped)
{
    for(size_t i=0; i<a.size(); ++i)
    {
        zipped.push_back(std::make_pair(a[i], b[i]));
    }
}

// Write the first and second element of the pairs in
// the given zipped vector into a and b. (This assumes
// that the vectors have equal length)
template <typename A, typename B>
void unzip(
        const std::vector<std::pair<A, B>> &zipped,
        std::vector<A> &a,
        std::vector<B> &b)
{
    for(size_t i=0; i<a.size(); i++)
    {
        a[i] = zipped[i].first;
        b[i] = zipped[i].second;
    }
}

class JohnsonProblem {
private:
    unsigned int _numOfDet = 0;
    unsigned int _numOfMach = 0;

    Matrix<int> _processTime;
    Matrix<int> _startTime;
    Matrix<int> _endTime;
    Matrix<int> _downtime;
    Matrix<int> _waitingTime;
    Matrix<int> _localReserveTime;

    int* _t1t2;
    int* _t2t3;

    std::vector<int> _firstGroup;
    std::vector<int> _secondGroup;
    int* _processOrder;
    int _totalProcessTime;

    std::map<int, float> _criterions;




    void sort(bool dec = true) {
        std::vector<int> firstGroupTime;
        std::vector<int> secondGroupTime;
        firstGroupTime.reserve(_firstGroup.size());
        secondGroupTime.reserve(_secondGroup.size());

        for (int detail: _firstGroup){
            firstGroupTime.push_back(_t1t2[detail]);
        }
        for (int detail: _secondGroup){
            secondGroupTime.push_back(_t2t3[detail]);
        }

        {
            std::vector<std::pair<int,int>> zipped;
            zip(_firstGroup, firstGroupTime, zipped);
            // Sort the vector of pairs
            std::sort(std::begin(zipped), std::end(zipped),
                      [&](const auto& a, const auto& b)
                      {
                          return a.second <= b.second;
                      });
            // Write the sorted pairs back to the original vectors
            unzip(zipped, _firstGroup, firstGroupTime);
        }

        {
            std::vector<std::pair<int,int>> zipped;
            zip(_secondGroup, secondGroupTime, zipped);
            std::sort(std::begin(zipped), std::end(zipped),
                      [&](const auto& a, const auto& b)
                      {
                          return a.second > b.second;
                      });
            unzip(zipped, _secondGroup, secondGroupTime);
        }
    }
    //can be tamplated later
    int findLastTime(int machineNumber){
        return  _endTime.maxInColumn(machineNumber);

    }
public:
    JohnsonProblem( ) = default;
    JohnsonProblem( Matrix<int> processTime) : _processTime(processTime) {
        _numOfDet = _processTime.getHeight();
        _numOfMach = _processTime.getWidth();
        _startTime = Matrix<int>(_numOfDet,_numOfMach);
        _endTime = Matrix<int>(_numOfDet,_numOfMach);
        _downtime = Matrix<int>(_numOfDet + 1,_numOfMach);
        _waitingTime = Matrix<int>(_numOfDet,_numOfMach);
        _localReserveTime = Matrix<int>(_numOfDet,_numOfMach);

        _t1t2 = new int[this->_numOfDet];
        _t2t3 = new int[this->_numOfDet];
        _processOrder = new int[this->_numOfDet];


    }

    ~JohnsonProblem() {
        delete [] _t1t2;
        delete [] _t2t3;
        delete [] _processOrder;
    }



    void setOrder() {
        for (int i = 0; i < _numOfDet; ++i) {
            _t1t2[i] = _processTime[i][0] +_processTime[i][1];
            _t2t3[i] = _processTime[i][1] +_processTime[i][2];
            if(_t1t2[i] < _t2t3[i])
                _firstGroup.push_back(i);
            else
                _secondGroup.push_back(i);
        }
        sort();

        if(_firstGroup.size()+_secondGroup.size() != _numOfDet) {
            std::cout << "asshole";
            exit(2);
        }

        {
            int i = 0;
            for (int detail : _firstGroup) {
                _processOrder[i] = detail;
                i++;
            }
            for (int detail : _secondGroup) {
                _processOrder[i] = detail;
                i++;
            }
        }
    }

    void setOrder(std::vector<int> order) {
        for (int i = 0; i < _numOfDet; ++i) {
            _processOrder[i] = order[i];
        }
    }

    void setOrder(const int* order) {
        for (int i = 0; i < _numOfDet; ++i) {
            _processOrder[i] = order[i];
        }
    }

    void createProcessPlan() {
        int currrentDetail;
        int endTime = 0;        //end time on current machine
        int prevEndTime = 0;    //ent time on prev machine

        for (int i = 0; i < _numOfDet; ++i) {
            currrentDetail = _processOrder[i];

            for (int j = 0; j < _numOfMach; ++j) {
                if(j == 0)
                    prevEndTime = 0;
                else
                    prevEndTime = findLastTime(j - 1);

                endTime = findLastTime(j);
                _startTime[currrentDetail][j] = (endTime < prevEndTime)? prevEndTime : endTime;
                _endTime[currrentDetail][j] = _startTime[currrentDetail][j] + _processTime[currrentDetail][j];
            }
        }
        _totalProcessTime = _endTime.max();
    }

    void findDowntime() {
        int currrentDetail;
        int nextDetail;
        for (int i = 0; i < _numOfDet; ++i) {
            currrentDetail = _processOrder[i];
            if(i+1 == _numOfDet)
                nextDetail = -1;
            else
                nextDetail = _processOrder[i+1];

            for (int j = 0; j < _numOfMach; ++j) {
                if(nextDetail != -1) {
                    _downtime[currrentDetail][j] = _startTime[nextDetail][j] - _endTime[currrentDetail][j];
                } else {
                    _downtime[currrentDetail][j] = _totalProcessTime - _endTime[currrentDetail][j];
                }

            }
        }

        for (int j = 0; j < _numOfMach; ++j) {
            _downtime[_numOfDet][j] = _startTime[_processOrder[0]][j];
        }
    }

    void findWaitingtime(){
        int currrentDetail;
        for (int i = 0; i < _numOfDet; ++i) {
            currrentDetail = _processOrder[i];
            for (int j = 0; j < _numOfMach; ++j) {
                if(j == 0) {
                    _waitingTime[currrentDetail][j] = _startTime[currrentDetail][j];

                } else {
                    _waitingTime[currrentDetail][j] = _startTime[currrentDetail][j]-_endTime[currrentDetail][j-1];

                }
            }
        }
    }

    void findLocalReserveTime() {
        int currrentDetail;
        for (int i = 0; i < _numOfDet; ++i) {
            currrentDetail = _processOrder[i];
            for (int j = 0; j < _numOfMach; ++j) {
                if(j == _numOfMach - 1) {
                    if (i == _numOfDet - 1)
                        _localReserveTime[currrentDetail][j] = _totalProcessTime - _endTime[currrentDetail][j];
                    else {
                        int nextDetail = _processOrder[i+1];

                        _localReserveTime[currrentDetail][j] = _startTime[nextDetail][j] - _endTime[currrentDetail][j];
                    }

                } else {
                    _localReserveTime[currrentDetail][j] = _startTime[currrentDetail][j+1] - _endTime[currrentDetail][j];

                }
            }
        }
    }

    float findK11() {
        _criterions[11] = _totalProcessTime;
        return _criterions[11];
    }
    float findK12() {
        Matrix<int> temp = _processTime.addRaws();
        Matrix<int> temp2 = _downtime.addRaws();
        Matrix<int> temp3 = temp + temp2;
        //temp.out();
        //temp2.out();
        //temp3.out();
        _criterions[12] = temp3.max();
        return _criterions[12];
    }
    float findK13() {
        Matrix<int> temp = _processTime + _waitingTime;
        Matrix<int> temp2 = _downtime.addRaws();
        //temp.out();
        //temp2.out();
        _criterions[13] = temp2.max();
        return _criterions[13];
    }
    float findK21() {
        Matrix<int> temp = _processTime.addRaws();
        Matrix<int> temp2 = _downtime.addRaws();
        Matrix<float> temp3 = temp/(temp+temp2);
        //temp.out();
        //temp2.out();
        //temp3.out();
        _criterions[21] = temp3.min();
        return _criterions[21];
    }
    float findK22() {
        Matrix<int> temp = _processTime.addRaws();
        Matrix<int> temp2 = _downtime.addRaws();
        Matrix<float> temp3 = temp/(temp+temp2);
        //temp.out();
        //temp2.out();
        //temp3.out();
        _criterions[22] = temp3.fullSum();
        return _criterions[22];
    }
    float findK23() {
        Matrix<int> temp = _downtime.addRaws();
       // temp.out();
        _criterions[23] = temp.max();
        return _criterions[23];
    }
    float findK24() {
        Matrix<int> temp = _downtime.subMatrix(_numOfDet,_numOfMach);
        //temp.out();
        _criterions[24] = temp.max();
        return _criterions[24];
    }
    float findK25() {
        Matrix<int> temp = _downtime.addRaws();
       // temp.out();
        _criterions[25] = temp.fullSum();
        return _criterions[25];
    }
    float findK26() {
        Matrix<int> temp = _downtime.addRaws();
        Matrix<float> temp2(temp.getHeight(), temp.getWidth());

        for (int j = 0; j < _numOfMach; ++j) {
            int numOfDowntimes = 0;
            for (int i = 0; i < _numOfDet+1; ++i) {
                if(_downtime[i][j] > 0 )
                    numOfDowntimes++;
            }
            temp2[0][j] = 1.0f*temp[0][j]/numOfDowntimes;
        }

        //temp.out();
        //temp2.out();
        _criterions[26] = temp2.max();
        return _criterions[26];
    }
    float findK27() {

        int numOfDowntimes = 0;
        for (int j = 0; j < _numOfMach; ++j)
            for (int i = 0; i < _numOfDet+1; ++i)
                if(_downtime[i][j] > 0 )
                    numOfDowntimes++;
        _criterions[27] = 1.0f*_downtime.fullSum()/numOfDowntimes;
        return _criterions[27];
    }
    float findK31() {
        _criterions[31] = _waitingTime.max();
        return _criterions[31];
    }
    float findK32() {
        _criterions[32] = _waitingTime.addRaws().max();
        return _criterions[32];
    }
    float findK33() {
        _criterions[33] = _waitingTime.fullSum();
        return _criterions[33];
        return 0;
    }
    float findK34() {

        Matrix<int> temp = _waitingTime.addRaws();
        Matrix<float> temp2(temp.getHeight(), temp.getWidth());

        for (int j = 0; j < _numOfMach; ++j) {
            int numOfDowntimes = 0;
            for (int i = 0; i < _numOfDet+1; ++i) {
                if( _waitingTime[i][j] > 0 )
                    numOfDowntimes++;
            }
            temp2[0][j] = 1.0f*temp[0][j]/numOfDowntimes;
        }

      //  temp.out();
       // temp2.out();

        _criterions[34] = temp2.max();
        return _criterions[34];
    }
    float findK35() {
        _criterions[35] = findK34();
        return _criterions[35];
    }
    float findK36() {
        int numOfWaitingtimes = 0;
        for (int j = 0; j < _numOfMach; ++j)
            for (int i = 0; i < _numOfDet+1; ++i)
                if(_waitingTime[i][j] > 0 )
                    numOfWaitingtimes++;
        _criterions[36] = 1.0f*_waitingTime.fullSum()/numOfWaitingtimes;
        return _criterions[36];
    }
    float findK37() {
        _criterions[37] = findK36();
        return _criterions[37];
    }

    std::map<int, float> findCriterions(std::vector<int> criter){
        findK11(); findK12(); findK13();

        findK21(); findK22(); findK23(); findK24(); findK25(); findK26(); findK27();

        findK31(); findK32(); findK33(); findK34(); findK35(); findK36(); findK37();

        std::map<int, float> result;

        for (int crit: criter) {
            result[crit] = _criterions[crit];
        }
        return result;
    }



    Matrix<int> getStartTime(){
        return _startTime;
    }

    Matrix<int> getEndTime() {
        return _endTime;
    }

    std::vector<int> getProcessOrder(){
        std::vector<int> result;
        result.reserve(_numOfDet);
        for (int i = 0; i < _numOfDet; ++i) {
            result.push_back(_processOrder[i]);
        }
        return result;
    }


    int getDetailNumber() {
        return _numOfDet;
    }
    int getMachineNumber() {
        return _numOfMach;
    }


    std::string outTimesSum() {
        std::stringstream ss;
        ss << '\t' << "T1+T2" << '\t' << "T2+T3" << std::endl;

        for (int i = 0; i < 5*4; ++i)
            ss << '-';

        ss << '\t'<< std::endl;
        for (int i = 0; i < _numOfDet; ++i)
            ss << i+1 << "|" << '\t' << _t1t2[i] << '\t' << _t2t3[i] << std::endl;
        return ss.str();
    }
    std::string outGroups() {
        std::stringstream ss;
        ss << "Groups:" << std::endl;
        ss << "First T1+T2<T2+T3:" << '\t';
        for (int i : _firstGroup)
            ss << i+1 << ", ";
        ss << std::endl;
        ss << "Second T1+T2=>T2+T3:" << '\t';
        for (int i : _secondGroup)
            ss << i+1 << ", ";
        ss << std::endl;
        return ss.str();
    }
    std::string outOrder() {
        std::stringstream ss;
        ss << "Process order:" << '\t';

        for (int j = 0; j < _numOfDet; ++j)
            ss << _processOrder[j]+1 << ", ";

        ss << std::endl;
        return ss.str();
    }
    std::string outProcessTime() {
        std::stringstream ss;
        ss << "Process time:" << std::endl;
        ss << _processTime.outWithNumerating();
        return ss.str();
    }
    std::string outTotalProcessTime() {
        std::stringstream ss;
        ss << "Total process time:" << '\t' << _totalProcessTime << std::endl;
        return ss.str();
    }
    std::string outStartTime() {
        std::stringstream ss;
        ss << "Start time:" << std::endl;
        ss << _startTime.outWithNumerating();
        return ss.str();
    }
    std::string outEndTime() {
        std::stringstream ss;
        ss << "End time:" << std::endl;
        ss <<_endTime.outWithNumerating();
        return ss.str();
    }
    std::string outDowntime() {
        std::stringstream ss;
        ss << "Downtime:" << std::endl;
        ss <<_downtime.outWithNumerating();
        return ss.str();
    }
    std::string outWaitingTime() {
        std::stringstream ss;
        ss << "Waiting time:" << std::endl;
        ss <<_waitingTime.outWithNumerating();
        return ss.str();
    }
    std::string outLocalReservTime() {
        std::stringstream ss;
        ss << "Local reserve time:" << std::endl;
        ss <<_localReserveTime.outWithNumerating();
        return ss.str();
    }
    std::string outCriterions(std::vector<int> critNums) {
        std::stringstream ss;

        ss.precision(3);
        ss << "Criterions:" << std::endl;
        for(int cr: critNums) {
            ss << cr << '\t';
        }
        ss << std::endl;
        int temp = static_cast<unsigned int>(critNums.size());
        for (int i = 0; i < temp + (temp-1)*7+5; ++i) {
            ss <<  '-';
        }
        ss << std::endl;
        for(int cr: critNums) {
            ss << _criterions[cr] << '\t';
        }

        ss << std::endl;
        return ss.str();
    }


    std::string  out(std::vector<int> critNums) {
        std::stringstream ss;

        ss << outProcessTime() <<  std::endl;
        ss << outTimesSum() <<  std::endl;
        ss << outGroups() <<  std::endl;
        ss << outOrder() << std::endl;
        ss << outTotalProcessTime() << std::endl;
        ss << outStartTime() << std::endl;
        ss << outEndTime() << std::endl;
        ss << outDowntime() << std::endl;
        ss << outWaitingTime() << std::endl;
        ss << outLocalReservTime() << std::endl;
        ss << outCriterions(critNums) << std::endl;

        return ss.str();
    }
};


#endif //LABA2_JOHNSONPROBLEM_H
