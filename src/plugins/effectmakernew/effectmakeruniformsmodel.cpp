// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "effectmakeruniformsmodel.h"

#include "propertyhandler.h"
#include "uniform.h"

#include <utils/qtcassert.h>

namespace EffectMaker {

EffectMakerUniformsModel::EffectMakerUniformsModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

QHash<int, QByteArray> EffectMakerUniformsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "uniformName";
    roles[DescriptionRole] = "uniformDescription";
    roles[ValueRole] = "uniformValue";
    roles[BackendValueRole] = "uniformBackendValue";
    roles[DefaultValueRole] = "uniformDefaultValue";
    roles[MinValueRole] = "uniformMinValue";
    roles[MaxValueRole] = "uniformMaxValue";
    roles[TypeRole] = "uniformType";
    roles[UseCustomValueRole] = "uniformUseCustomValue";
    return roles;
}

int EffectMakerUniformsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_uniforms.size();
}

QVariant EffectMakerUniformsModel::data(const QModelIndex &index, int role) const
{
    QTC_ASSERT(index.isValid() && index.row() < m_uniforms.size(), return {});
    QTC_ASSERT(roleNames().contains(role), return {});

    return m_uniforms.at(index.row())->property(roleNames().value(role));
}

bool EffectMakerUniformsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !roleNames().contains(role))
        return false;

    auto uniform = m_uniforms.at(index.row());

    if (uniform->type() == Uniform::Type::Sampler) {
        QString updatedValue = value.toString();
        int idx = value.toString().indexOf("file:");

        QString path = idx > 0 ? updatedValue.right(updatedValue.size() - idx - 5) : updatedValue;
        updatedValue = QUrl::fromLocalFile(path).toString();

        uniform->setValue(updatedValue);
        g_propertyData.insert(uniform->name(), updatedValue);
    } else {
        uniform->setValue(value);
        g_propertyData.insert(uniform->name(), value);
    }

    emit dataChanged(index, index, {role});

    return true;
}

void EffectMakerUniformsModel::resetModel()
{
    beginResetModel();
    endResetModel();
}

void EffectMakerUniformsModel::addUniform(Uniform *uniform)
{
    beginInsertRows({}, m_uniforms.size(), m_uniforms.size());
    m_uniforms.append(uniform);
    endInsertRows();
}

QList<Uniform *> EffectMakerUniformsModel::uniforms() const
{
    return m_uniforms;
}

} // namespace EffectMaker

