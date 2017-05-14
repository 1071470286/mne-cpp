//=============================================================================================================
/**
* @file     geometryinfo.cpp
* @author   Lars Debor <lars.debor@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     Mai, 2017
*
* @section  LICENSE
*
* Copyright (C) 2017, Lars Debor and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    GeometryInfo class definition.
*
*/


//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================
#include "geometryinfo.h"
#include<mne/mne_bem_surface.h>


//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include <cmath>
#include <fstream>
#include <limits>
#include <set>
#include <utility>

//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QFile>
#include <QDateTime>

//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace GEOMETRYINFO;
using namespace Eigen;
using namespace MNELIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE GLOBAL METHODS
//=============================================================================================================


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

QSharedPointer<MatrixXd> GeometryInfo::scdc(const MNEBemSurface &inSurface, const QVector<qint32> &vertSubSet)
{
    //start timer
    qint64 startTimeSecs = QDateTime::currentSecsSinceEpoch();
    qint64 startTimeMsecs = QDateTime::currentMSecsSinceEpoch();

    size_t matColumns;
    if(!vertSubSet.empty()) {
        matColumns = vertSubSet.size();
    }
    else {
        matColumns = inSurface.rr.rows();
    }
    // convention: first dimension in distance table is "from", second dimension "to"
    QSharedPointer<MatrixXd> ptr = QSharedPointer<MatrixXd>::create(inSurface.rr.rows(), matColumns);

    //QPair<int, QVector<int> > tempPair;
    //int tempID;
    std::cout << inSurface.rr.rows() <<std::endl;
    std::cout << inSurface.rr.cols() <<std::endl;
    for (size_t i = 0; i < matColumns; ++i)
    {
        //ToDo bessere Lösung mit und ohne subset
        size_t index = i;
        if(!vertSubSet.empty())
        {
            index = vertSubSet[i];
        }

        //std::cout << inSurface.rr.rows() <<std::endl;
        float xFrom = inSurface.rr(index, 0);
        float yFrom = inSurface.rr(index, 1);
        float zFrom = inSurface.rr(index, 2);
        //Vector3f currentVertex = inSurface.rr(i)
        for (size_t j = 0; j < inSurface.rr.rows(); ++j)
        {


            float xTo = inSurface.rr(j, 0);
            float yTo = inSurface.rr(j, 1);
            float zTo = inSurface.rr(j, 2);
            (*ptr)(  j, i) = sqrt(pow(xTo - xFrom, 2) + pow(yTo - yFrom, 2) + pow(zTo - zFrom, 2));
        }
    }
    std::cout << QDateTime::currentMSecsSinceEpoch()- startTimeMsecs <<" ms " << std::endl;
    std::cout << "start writing to file" << std::endl;
    std::ofstream file;
    file.open("./matrixDump.txt");
    file << *ptr;
    std::cout << "writing to file ended!\n";
    std::cout << QDateTime::currentSecsSinceEpoch()- startTimeSecs <<" s " << std::endl;
    return ptr;
}
//*************************************************************************************************************

QSharedPointer<MatrixXd> GeometryInfo::scdc(const MNEBemSurface  &inSurface, double cancelDistance, const QVector<qint32> &vertSubSet)
{
    QSharedPointer<MatrixXd> outputMat = QSharedPointer<MatrixXd>::create();
    return outputMat;
}
//*************************************************************************************************************

QSharedPointer<QVector<qint32>> GeometryInfo::projectSensor(const MNEBemSurface &inSurface, const QVector<Vector3d> &sensorPositions)
{
    QSharedPointer<QVector<qint32>> outputArray = QSharedPointer<QVector<qint32>>::create();
    return outputArray;
}
//*************************************************************************************************************

void GeometryInfo::iterativeDijkstra(QSharedPointer<MatrixXd> ptr, const MNEBemSurface &inSurface, const QVector<qint32> &vertSubSet) {
    // Initialization
    // @todo if this copies neighbor_vert, a pointer might be the more efficient option
    QList<QPair<int, QVector<int> > > adjacency = inSurface.neighbor_vert;

    qint32 n = adjacency.size();
    // have to use std::vector because QVector.resize takes only one argument
    std::vector<double> minDists(n);
    std::set< std::pair< double, qint32> > vertexQ;
    double MAX_WEIGHT = std::numeric_limits<double>::infinity();

    for (qint32 i = 0; i < vertSubSet.size(); ++i) {
        qint32 root = vertSubSet[i];

        minDists.clear();
        minDists.resize(n, MAX_WEIGHT);
        minDists[root] = 0;
        vertexQ.clear();

        vertexQ.insert(std::make_pair(minDists[root], root));

        while (vertexQ.empty() == false) {
            // remove next vertex from queue
            double dist = vertexQ.begin()->first;
            qint32 u = vertexQ.begin()->second;
            vertexQ.erase(vertexQ.begin());
            // visit each neighbour of u
            // @todo check whether the neighbours for vertexID x are stored at adjacency[x]
            // or if a linear search for adjacency[i].first == x is necessary
            QVector<int> neighbours = adjacency[u].second;
            for (qint32 ne = 0; ne < neighbours.length(); ++ne) {
                qint32 v = neighbours[ne];
                // distance from source to v, using u as its predecessor
                double distWithU = dist + distanceBetween(inSurface.rr, u, v);
                if (distWithU < minDists[v]) {
                    // this is a combination of insert and decreaseKey
                    // @todo need an effort estimate for this (complexity ?)
                    vertexQ.erase(std::make_pair(minDists[v], v));
                    minDists[v] = distWithU;
                    vertexQ.insert(std::make_pair(minDists[v], v));
                }
            }
        }
        for (qint32 m = 0; m < minDists.size(); ++m) {
            (*ptr)(m , root) = minDists[m];
        }
    }
}
//*************************************************************************************************************

inline double GeometryInfo::distanceBetween(MatrixX3f nodes, qint32 u, qint32 v) {
    return sqrt(pow(nodes(u, 0) - nodes(v, 0), 2) +  pow(nodes(u, 1) - nodes(v, 1), 2) +  pow(nodes(u, 2) - nodes(v, 2), 2));
}
