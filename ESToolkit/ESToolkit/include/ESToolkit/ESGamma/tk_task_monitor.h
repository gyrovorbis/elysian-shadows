#ifndef TK_TASK_MONITOR_H
#define TK_TASK_MONITOR_H

#include <ESGamma/utility/elysian_task_monitor_base.hpp>
#include <QESTkUtils/model/tk_property_stack_model.h>

class TkTaskMonitor:
        public QStandardItem,
        public elysian::TaskMonitorBase

{
public:
                    TkTaskMonitor(const char*const name);
                    ~TkTaskMonitor(void) = default;

        virtual QVariant data(int role) const override {

            switch((Qt::ItemDataRole)role) {
            case Qt::DisplayRole:
                return QVariant(getAvgTime());
            case Qt::EditRole:
                return QVariant();
            default:
                return QStandardItem::data(role);
            }
        }

    void startNewFrame(void) {
        elysian::TaskMonitorBase::startNewFrame();
        emitDataChanged();
    }


    bool addChild(TaskMonitor*const child) {
        if(elysian::TaskMonitorBase::addChild(child)) {
            QList<QStandardItem*> items;
            items.push_back(new QStandardItem(child->getName()));
            items.push_back(child);
            //appendRow(items);
            //emitDataChanged();
            return true;
        }
        return false;
    }


};








// ===== INLINEZ =======

inline TkTaskMonitor::TkTaskMonitor(const char * const name): QStandardItem(), elysian::TaskMonitorBase(name) {
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::NoItemFlags);
}


#endif // TK_TASK_MONITOR_H

