#include "clusterizer.h"
#include <QDebug>
#include <math.h>

Clusterizer::Clusterizer(QObject *parent) : QObject(parent)
{}

QList<Repositories> Clusterizer::clusterize(Repositories &repositories, QString & text)
{
    QList<Repositories> clusters;
    text = "";
    int numberOfCluster = 0;
    forever {
        if (repositories.count() == 0)
            return clusters;

        // if 2 or less repositories remains
        if (repositories.count() <= 2) {
            Repositories clusterReps;
            text += "CLUSTER " + QString::number(++numberOfCluster) + ": {  ";
            for (auto & r: repositories) {
                text += r.name() + "  ";
                clusterReps.append(r);
            }
            text += "}\n";
            clusters.append(clusterReps);
            return clusters;
        }

        // generate indexes
        QList<Floats> indexMatrix, similarityMatrix;
        for (int i = 0; i < repositories.count(); i++) {
            indexMatrix.append(Floats()
                               << repositories.at(i).language()
                               << repositories.at(i).support()
                               << repositories.at(i).size()
                               << repositories.at(i).quality());
        }

        // show indexes
        text += "Matrix obtained from index generation:\n";
        int number = 0;
        for (auto & i: indexMatrix) {
            text += repositories[number++].name() + "\t";
            for (auto & ii: i) {
                text += QString::number(ii) + " ";
            }
            text += "\n";
        }
        text += "\n";

        //build similarity matrix
        int column = 1;
        int count = 0;
        Floats similarities;
        for (int i = 0; i < indexMatrix.count(); i++) {
            similarities.clear();
            for (int j = 0; j < column; j++) {
                similarities.append(0);
            }
            for (int j = column; j < indexMatrix.count(); j++) {
                for (int o = 0; o < indexMatrix.at(0).count(); o++) {
                    float fir =  indexMatrix.at(i).at(o);
                    float sec = indexMatrix.at(j).at(o);
                    float similarWithDifference = 0.1;
                    if (fabs(fir - sec) < similarWithDifference) {
                        ++count;
                    }
                }
                similarities.append(count);
                count = 0;
            }
            ++column;
            similarityMatrix.append(similarities);
        }

        QList<NamedList> indexesByRepositories;
        for (int i = 0; i < similarityMatrix.count(); i++) {
            NamedList l;
            l.list = similarityMatrix.at(i);
            l.name = repositories[i].name();
            indexesByRepositories.append(l);
        }

        // show similarity matrix
        similarityMatrix.clear();
        text += "Similarity matrix:\n";
        for (auto & i: indexesByRepositories) {
            text += i.name + "\t";
            for (auto & ii: i.list) {
                text += QString::number(ii) + " ";
            }
            similarityMatrix.append(i.list);
            text += "\n";
        }

        // max index in similarity matrix
        int maxSimilarityIndex = -1;
        for (int i = 0; i < similarityMatrix.count(); i++) {
            for (int j = 0; j < similarityMatrix.count(); j++) {
                if (maxSimilarityIndex < similarityMatrix.at(i).at(j)) {
                    maxSimilarityIndex = similarityMatrix.at(i).at(j);
                }
            }
        }

        // find repositories with this index
        Floats clusterIndexes;
        for (int i = 0; i < similarityMatrix.count(); i++) {
            for (int j = 0; j < similarityMatrix.at(0).count(); j++) {
                if (similarityMatrix.at(i).at(j) == maxSimilarityIndex) {
                    if (!clusterIndexes.contains(i)) {
                        clusterIndexes.append(i);
                    }
                    if (!clusterIndexes.contains(j)) {
                        clusterIndexes.append(j);
                    }
                }
            }
        }

        // show cluster
        Repositories clusterReps;
        text += "\nCLUSTER " + QString::number(++numberOfCluster) + ": {  ";
        for (int i = 0; i < clusterIndexes.count(); i++) {
            text += indexesByRepositories.at(clusterIndexes.at(i)).name + "  ";
            for (auto & r: repositories) {
                if (r.name() == indexesByRepositories.at(clusterIndexes.at(i)).name) {
                    clusterReps.append(r);
                    break;
                }
            }
        }
        clusters.append(clusterReps);

        // remove clusterized
        QList<QString> repositoriesToRemove;
        for (int i = 0; i < clusterIndexes.count(); i++) {
            repositoriesToRemove.append(indexesByRepositories.at(clusterIndexes.at(i)).name);
        }

        for (auto & toRemove: repositoriesToRemove) {
            for (int j = 0; j < repositories.count(); j++) {
                if (repositories[j].name() == toRemove) {
                    repositories.removeAt(j);
                    break;
                }
            }
        }
        text += "}\n\n";
    }
}

QString Clusterizer::lingua(QList<Repositories> &clusters, int reposCount)
{
    QString text;
    text += "\n====== LINGUA =======\n";
    text += "Clusters count " + QString::number(clusters.count()) + ":\n";

    // show clusters
    int clusterNumber = 0;
    for (auto & c: clusters) {
        ++clusterNumber;
        text += "CLUSTER " + QString::number(clusterNumber) + ": {  ";
        for (auto & e: c) {
            text += e.name() + "  ";
        }
        text += "}\n";
    }
    text += "\n";

    // percentage
    int max = -1;
    int min = INT32_MAX;
    int idMax = 0;
    int idMin = 0;
    int id = 0;
    float perc = 0.f;
    for (auto & c: clusters) {
        ++id;
        if (c.count() > max) {
            max = c.count();
            idMax = id;
        }
        if (c.count() < min) {
            min = c.count();
            idMin = id;
        }
        perc = (float)c.count() / (float)reposCount;
        text += "CLUSTER " + QString::number(id) + " contains " + QString::number(perc * 100, 'f', 2) + "% of all repositories\n";
    }
    text += "\n";
    text += "Biggest cluster\t" + QString::number(idMax) + "\n";
    text += "Smallest cluster\t" + QString::number(idMin) + "\n";

    return text;
    // cluster is
    //    clusterNumber = 0;
    //    for (auto & c: clusters) {
    //        ++clusterNumber;
    //        int l = 0;
    //        int a = 0;
    //        int s = 0;
    //        int paa = 0;
    //        foreach (auto p, c) {
    //            foreach (auto pp, projects) {
    //                if (pp.name == p)
    //                {
    //                    l += (int)pp.language;
    //                    a += (int)pp.as;
    //                    s += (int)pp.sloc;
    //                    paa += (int)pp.platform;
    //                }
    //            }
    //        }

    //        QString la, pa, sa, aa;
    //        switch(l / c.count())
    //        {
    //            case CPP: la = "CPP"; break;
    //            case BASH: la = "BASH"; break;
    //            case CSHARP: la = "CSHARP"; break;
    //            case PYTHON: la = "PYTHON"; break;
    //        }
    //        switch(paa / c.count())
    //        {
    //            case PC: pa = "PC"; break;
    //            case MOBILE: pa = "MOBILE"; break;
    //        }
    //        switch(a / c.count())
    //        {
    //            case APPLICATION: aa = "APP"; break;
    //            case SCRIPT: aa = "SCRIPT"; break;
    //        }
    //        switch(s / c.count())
    //        {
    //            case SMALL: sa = "SMALL"; break;
    //            case MEDIUM: sa = "MEDIUM"; break;
    //            case LARGE: sa = "LARGE"; break;
    //        }

    //        text += "\nCLUSTER " + QString::number(clusterNumber)
    //                + ":\nMost:\n"
    //                + "Language\t" + la + "\n"
    //                + "Platform\t" + pa + "\n"
    //                + "SLOC    \t" + sa + "\n"
    //                + "App/Scr \t" + aa + "\n";
    //    }
}
