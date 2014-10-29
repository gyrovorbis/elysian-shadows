#ifndef PROJECT_H
#define PROJECT_H
//QT
#include <QString>
#include <QVector>

#include "treenode.h"
#include "propertyStackModel.h"

class TkLevel;
class Area;
class Assets;
class TkEntity;
typedef TkEntity Entity;
class Component;

class Project: public PropertyStackModel, public TreeNode {
    Q_OBJECT
    friend class Assets;
private:

    QVector<TkLevel *> _level;
    QString _location;
    bool _active;
	virtual bool deactivate(void);
    void connectSlots(TkLevel *const level);

public:
	Project(void);
	~Project(void); //delete all levels

	virtual void populateModel(void) override;

    void setName(const QString name);
    void setLocation(const QString loc);
	QString getLocation(void) const;
	virtual bool activate(void);
	bool isActive(void);

    TkLevel *newLevel(void);
    TkLevel *newLevel(const QString n);
    TkLevel *loadLevel(const QString name); //assumed to be within current project, so directory is not necessary
	bool saveLevel(const QString name);
	bool removeLevel(const QString name);
    unsigned int getLevelAmount() const;
    TkLevel *getLevel(const QString name);
    TkLevel *getLevelByIndex(const unsigned int index);
    TkLevel *getFirstLevel(void) const;

	bool load(const QString loc);
	bool save(void);

    TkLevel *importLevel(const QString dir);
	bool saveAs(const QString dir);

	void clearUndoStacks(void);

	virtual Selectable::TYPE getType(void) const { return Selectable::PROJECT; }
	virtual TreeNode::ITEM_TYPE getTreeWidgetItemType(void) { return ITEM_PROJECT; }

private slots:
      bool activateByLevel(Level *const level, Area *const area);
	  void onAreaAdded(Level *const level, Area *const area);
	  void onAreaRenamed(const QString oldName, const QString newName);
	  void onAreaRemoved(Level *const level, Area *const area);
	  void onLevelRenamed(const QString oldName, const QString newName);
	  void onAddEntity(Level *const level, Area *const area, Entity *const entity);
	  void onRemoveEntity(Level *const level, Area *const area, Entity *const entity);
	  void onComponentAttached(Level *const level, Area *const area, Entity *const entity, Component *const comp);
	  void onComponentRemoved(Level *const level, Area *const area, Entity *const entity, Component *const comp);

signals:
	void activating(Project *const project, Level *const level, Area *const area);
	void areaAdded(Project *const project, Level *const level, Area *const area);
	void areaRenamed(const QString oldName, const QString newName);
	void areaRemoved(Project *const project, Level *const level, Area *const area);
	void levelAdded(Project *const project, Level *const level);
	void levelRemoved(Project *const project, Level *const level);
	void levelRenamed(const QString oldName, const QString newName);
	void entityAdded(Project *const project, Level *const level, Area *const area, Entity *const entity);
	void entityRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity);
	void componentAttached(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);
	void componentRemoved(Project *const project, Level *const level, Area *const area, Entity *const entity, Component *const comp);

};

#endif // PROJECT_H
