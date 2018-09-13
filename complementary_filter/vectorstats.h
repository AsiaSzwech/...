#ifndef VECTORSTATS_H
#define VECTORSTATS_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include <numeric>

using namespace std;

typedef double ld;
typedef unsigned int uint;
typedef std::vector<ld>::iterator vec_iter_ld;


class VectorStats : public QObject
{
    Q_OBJECT
public:
    explicit VectorStats(QObject *parent = nullptr);

    /**
        * Constructor for VectorStats class.
        *
        * @param start - This is the iterator position of the start of the window,
        * @param end   - This is the iterator position of the end of the window,
        */
       VectorStats(vec_iter_ld start, vec_iter_ld end) {
           this->start = start;
           this->end = end;
           this->compute();
       }
       void compute() {
               ld sum = std::accumulate(start, end, 0.0);
               uint slice_size = std::distance(start, end);
               ld mean = sum / slice_size;

                this->m1 = mean;
       }
       //vector<ld> filtered_mean;
       ld mean() {
               return m1;
           }
signals:

public slots:


private:
       vec_iter_ld start;
       vec_iter_ld end;
       ld m1;
       ld m2;
};

#endif // VECTORSTATS_H
