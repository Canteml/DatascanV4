/*============================================================================
#
#   Copyright Roboplanet 2021
#
# ----------------------------------------------------------------------------
#
# File name   : ComLinkManager.h
# Author      : Thierry Lechevallier
# Date        : 24/05/2021
#
# ==========================================================================*/

#pragma once

// -- Internal includes
#include <xml/XmlSettingsLoggable.h>
#include <v2/com/ComLinkManager.h>

/**
 * @brief The ComLinkModel class
 *
 * Contain the list of links, accessible from QML
 *
 */
class ComLinkModel : public QAbstractListModel, public Loggable {
    Q_OBJECT
    QML_ELEMENT
public :

    enum ComLinkRoles {
        ROLE_LINK_TYPE = Qt::UserRole,    ///< TYPE : value in GenComLink::LINK_TYPE
        ROLE_LINK_NAME,
        ROLE_LINK_STATE,
        ROLE_SERIAL_PORT,
        ROLE_SERIAL_PORT_LIST,
        ROLE_SERIAL_BAUDRATE,
        ROLE_IP_ADDRESS,
        ROLE_IP_PORT
     };

    ComLinkModel(ComLinkManager * parent, QList<GenComLink *> * p_plLink);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addLink();
    void removeLink(int p_row);
    void resetModel();
    void updateLinks();

private :
    QList<GenComLink *> * m_plLink; ///< list of data, owner is ComLinkManager
    ComLinkManager * m_pComLinkManager;
};
