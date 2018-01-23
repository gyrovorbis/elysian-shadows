#ifndef TK_PERFORMANCE_MONITOR_HPP
#define TK_PERFORMANCE_MONITOR_HPP

#include <QStandardItem>
#include <utility/elysian_performance_monitor.hpp>

class TkPerformanceMonitor:
        public QStandardItem,
        public elysian::PerformanceMonitor {

public:
                    TkPerformanceMonitor(const char*const name);
                    ~TkPerformanceMonitor(void) = default;

        virtual QVariant data(int role) const override {

            switch((Qt::ItemDataRole)role) {
            case Qt::DisplayRole:
                return QVariant(_deltaTicks);
            case Qt::EditRole:
                return QVariant();
            default:
                return QStandardItem::data(role);
            }
        }


    bool addChild(TkPerformanceMonitor* child) {
            elysian::PerformanceMonitor::addChild(child);

            QList<QStandardItem*> items;
            items.push_back(new QStandardItem(child->_taskName));
            items.push_back(child);
            appendRow(items);
            //emitDataChanged();
            return true;
    }


};








// ===== INLINEZ =======

inline TkTaskMonitor::TkTaskMonitor(const char * const name): QStandardItem(), elysian::TaskMonitorBase(name) {
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::NoItemFlags);
}



#endif // TK_PERFORMANCE_MONITOR_HPP
