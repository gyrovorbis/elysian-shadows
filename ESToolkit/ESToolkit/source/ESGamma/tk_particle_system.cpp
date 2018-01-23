#include <ESGamma/tk_particle_system.h>
#include <QESTkUtils/model/tk_lua_table_stack_model.h>
#include <proxy/elysian_instance_transform_proxy.hpp>
#include <ESToolkit/ui/dock/tk_particle_attrib_preview_dock_widget.h>
#include <QESTkUtils/ui/widget/tk_2d_vector_widget.h>
#include <ESToolkit/ui//tk_estoolkit.h>
#include <QObject>

TkParticleSystem::TkParticleSystem(void):
    elysian::ParticleSystemBase(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_DEFAULT_COUNT+1),
    _model(new PropertyStackModel),
    _previewWidget(new ParticleAttribPreviewDockWidget(ESToolkit::getInstance())),
    _tkEmitterIndex(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_DEFAULT_COUNT)
{
    ESToolkit::getInstance()->addDockWidget(Qt::BottomDockWidgetArea, _previewWidget);
    _previewWidget->setFloating(true);
    _previewWidget->setVisible(false);

    LuaVariant parentTable = {
        { "position", &_previewProperties.getPos() },
        { "direction", &_previewProperties.getDir() }
    };

    QStandardItem* label = new QStandardItem("position");
    auto* luaItem = new LuaTableFieldItem(LuaFieldRef(parentTable, "position"), label);
    luaItem->insertingModelItem();
    _model->addRow(label, luaItem);

    label = new QStandardItem("direction");
    luaItem = new LuaTableFieldItem(LuaFieldRef(parentTable, "direction"), label);
    luaItem->insertingModelItem();
    _model->addRow(label, luaItem);


    auto* item = new PropertyStandardItem<unsigned, PreviewProperties>(&_previewProperties,
                                                                       &PreviewProperties::getIntensity,
                                                                       &PreviewProperties::setIntensity);
    _model->addProperty("intensity", item);

    auto* item2 = new PropertyStandardItem<float, PreviewProperties>(&_previewProperties,
                                                              &PreviewProperties::getSpreadAngle,
                                                              &PreviewProperties::setSpreadAngle);
    _model->addProperty("spreadAngle", item2);

    _previewWidget->setModel(_model);

    QObject::connect(_previewWidget->getVector2DWidget(), &Vector2DWidget::vectorChanged, [&](elysian::Vector2 vec, float mag) {
        vec.normalize();
        _previewProperties.dir.x = vec.x;
        _previewProperties.dir.y = vec.y;

        _emitters[_tkEmitterIndex].emitFromPointInDir(_previewProperties.pos,
                                                      _previewProperties.dir,
                                                      _previewProperties.intensity * mag,
                                                      _previewProperties.spreadAngle);

    });

}


TkParticleSystem::~TkParticleSystem(void) {
    delete _previewWidget;
}

unsigned TkParticleSystem::getPreviewPoolSize(void) const {
    return _emitters[_tkEmitterIndex].getPoolSize();
}

void TkParticleSystem::resizePreviewPool(unsigned size) {
    _emitters[_tkEmitterIndex].resizePool(size);
}

unsigned TkParticleSystem::getPreviewActiveParticleCount(void) const {
    return _emitters[_tkEmitterIndex].getActiveCount();
}

std::pair<QStandardItem*, QStandardItem*> TkParticleSystem::getStdItems(void) {
    if(!_labelItem || !_parentItem)
        _createItems();

    GY_ASSERT(_labelItem && _parentItem);
    return std::pair<QStandardItem*, QStandardItem*>(_labelItem, _parentItem);
}

void  TkParticleSystem::emitPreview(ParticleEmitterAttrib* attrib) {
    if(attrib) setAttrib(_tkEmitterIndex, attrib); //otherwise use whatever-the-fuck we were using!

    _emitters[_tkEmitterIndex].emitFromPointInDir(_previewProperties.pos,
                                                  _previewProperties.dir,
                                                  _previewProperties.intensity,
                                                  _previewProperties.spreadAngle);
}

void TkParticleSystem::_createItems(void) {
    _labelItem  = new QStandardItem("Preview Properties");
    _parentItem = new ESTkStandardItem("Test");



}

void TkParticleSystem::popDockWidget(ParticleEmitterAttrib *attrib) {
   _previewWidget->show();
   _previewWidget->setVisible(true);
   //_previewWidget->getVector2DWidget()->show();
}
