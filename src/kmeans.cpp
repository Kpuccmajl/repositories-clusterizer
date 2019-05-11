#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <QString>
#include <QDebug>

using namespace std;

struct Point
{
    int pointId, clusterId, dimensions;
    vector<double> values;

    Point(int id, string line)
    {
        dimensions = 0;
        pointId = id;
        stringstream is(line);
        double val;
        while (is >> val) {
            values.push_back(val);
            dimensions++;
        }
        clusterId = 0; //Initially not assigned to any cluster
    }

    void setCluster(int val)
    {
        clusterId = val;
    }

    double getVal(int pos)
    {
        return values[pos];
    }

    int getDimensions(){
        return dimensions;
    }

    int getCluster(){
        return clusterId;
    }

    int getID(){
        return pointId;
    }
};

struct Cluster
{
    int clusterId;
    vector<double> centroid;
    vector<Point> points;

    Cluster(int clusterId, Point centroid)
    {
        this->clusterId = clusterId;
        for (int i = 0; i < centroid.dimensions; i++){
            this->centroid.push_back(centroid.getVal(i));
        }
        this->addPoint(centroid);
    }

    void addPoint(Point p)
    {
        p.setCluster(this->clusterId);
        points.push_back(p);
    }

    bool removePoint(int pointId){
        int size = points.size();
        for (int i = 0; i < size; i++) {
            if (points[i].getID() == pointId) {
                points.erase(points.begin() + i);
                return true;
            }
        }
        return false;
    }

    int getId(){
        return clusterId;
    }

    Point getPoint(int pos){
        return points[pos];
    }

    int getSize(){
        return points.size();
    }

    double getCentroidByPos(int pos) {
        return centroid[pos];
    }

    void setCentroidByPos(int pos, double val){
        this->centroid[pos] = val;
    }
};

struct KMeans
{
    int K, iters, dimensions, total_points;
    vector<Cluster> clusters;
    QString info;

    int getNearestClusterId(Point point)
    {
        double sum = 0.0;
        for (int i = 0; i < dimensions; i++) {
            sum += pow(clusters[0].getCentroidByPos(i) - point.getVal(i), 2.0);
        }
        double min_dist = sqrt(sum);
        int nearestClusterId = clusters[0].getId();
        for (int i = 1; i < K; i++) {
            double dist;
            sum = 0.0;
            for (int j = 0; j < dimensions; j++) {
                sum += pow(clusters[i].getCentroidByPos(j) - point.getVal(j), 2.0);
            }
            dist = sqrt(sum);
            if (dist < min_dist) {
                min_dist = dist;
                nearestClusterId = clusters[i].getId();
            }
        }
        return nearestClusterId;
    }

    KMeans(int K, int iterations)
    {
        this->K = K;
        this->iters = iterations;
    }

    void clusterize(vector<Point> points)
    {
        stringstream ssinfo;
        clusters.clear();
        info.clear();
        if (points.size() < K){
            ssinfo << "Error: Number of clusters greater than number of points." << endl;
        }

        total_points = points.size();
        dimensions = points[0].dimensions;

        //Initializing Clusters
        vector<int> usedpPointsIds;
        for (int i = 1; i <= K; i++) {
            forever {
                int index = rand() % total_points;
                if (find(usedpPointsIds.begin(), usedpPointsIds.end(), index) == usedpPointsIds.end()) {
                    usedpPointsIds.push_back(index);
                    points[index].setCluster(i);
                    Cluster cluster(i, points[index]);
                    clusters.push_back(cluster);
                    break;
                }
            }
        }
        ssinfo << "> " << clusters.size() << " clusters initialized" << endl;
        ssinfo << "> running clustering..." << endl;
        int iteration = 1;
        forever {
            bool done = true;
            // Add all points to their nearest cluster
            for (int i = 0; i < total_points; i++) {
                int currentClusterId = points[i].getCluster();
                int nearestClusterId = getNearestClusterId(points[i]);
                if (currentClusterId != nearestClusterId) {
                    if (currentClusterId != 0) {
                        for (int j=0; j<K; j++) {
                            if (clusters[j].getId() == currentClusterId) {
                                clusters[j].removePoint(points[i].getID());
                            }
                        }
                    }
                    for (int j=0; j<K; j++) {
                        if (clusters[j].getId() == nearestClusterId) {
                            clusters[j].addPoint(points[i]);
                        }
                    }
                    points[i].setCluster(nearestClusterId);
                    done = false;
                }
            }
            // Recalculating the center of each cluster
            for (int i = 0; i < K; i++) {
                int clusterSize = clusters[i].getSize();
                for (int j = 0; j < dimensions; j++) {
                    double sum = 0.0;
                    if (clusterSize > 0) {
                        for (int p = 0; p < clusterSize; p++) {
                            sum += clusters[i].getPoint(p).getVal(j);
                        }
                        clusters[i].setCentroidByPos(j, sum / clusterSize);
                    }
                }
            }
            if (done || iteration >= iters) {
                ssinfo << "> clustering completed in " << iteration << " iterations"<< endl << endl;
                break;
            }
            iteration++;
        }
        //Print pointIds in each cluster
        for (int i = 0; i < K; i++){
            ssinfo << "> points in cluster " << clusters[i].getId() << " : ";
            for (int j = 0; j < clusters[i].getSize(); j++){
                ssinfo << clusters[i].getPoint(j).getID() << " ";
            }
            ssinfo << endl;
        }
        ssinfo << endl;
        info = QString::fromStdString(ssinfo.str());
    }
};
