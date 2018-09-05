#include <QESTkUtils/ui/widget/tk_line_chart.h>


namespace QESTkUtils {

LineChartView::LineChartView(void):
    QChartView(nullptr, nullptr)
{
    _chart = new QChart();
    _chart->legend()->hide();

    _chart->setTitle("Performance Profiler");
    setChart(_chart);
    setRenderHint(QPainter::Antialiasing);
}

unsigned LineChartView::addSeries(AbstractFrameUpdatedValueSeries* series) {
    unsigned pos = _series.size();
    _chart->addSeries(series);
    _chart->createDefaultAxes();
    _chart->axisX()->setRange(0,1000);
    _chart->axisX()->setTitleText("Frame");
    _chart->axisY()->setRange(0,2000000);
    _chart->axisY()->setTitleText("Nanoseconds");
    _series.push_back(series);
    return pos;
}

void LineChartView::onEngineUpdated(void) {
    advanceFrame();
}

void LineChartView::advanceFrame(void) {
    for(auto& it: _series)
        it->update(_curFrame);
    _chart->update();
    repaint();
    if(++_curFrame >= 1000) {
        for(auto& it: _series) it->clear();
        _curFrame = 0;
    }
}




};
